/**
 * @file RefBase.cpp
 * Copyright: Android Open Source Project
 * License: Apache 2.0
 */

#define LOG_TAG "RefBase"
// #define LOG_NDEBUG 0

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <typeinfo>
#include <unistd.h>

#include <utils/RefBase.h>

#include <utils/CallStack.h>
#include <utils/Log.h>
#include <utils/threads.h>

// gcc用法表示函数参数未使用否则有编译警告
#ifndef __unused
#define __unused __attribute__((__unused__))
#endif

// 引用计数是否开启debug模式
#define DEBUG_REFS                      1

// 是否追踪引用计数信息
#define DEBUG_REFS_ENABLED_BY_DEFAULT   1

// 是否收集函数调用堆栈信息
#define DEBUG_REFS_CALLSTACK_ENABLED    1

// debug信息保存位置
#define DEBUG_REFS_CALLSTACK_PATH       "/data/debug"

// 是否打印引用计数操作
#define PRINT_REFS                      1

// ---------------------------------------------------------------------------

namespace android {

/********************************************************************************
1. 默认情况下，当最后一个强引用消失时对象被销毁，如果没有强引用，就在最后一个弱引用
   消失时销毁对象。但这种行为受OBJECT_LIFETIME_WEAK的影响，在最后一个引用消失前将
   无条件地一直拥有对象，而不管是强引用还是弱引用。使用extendObjectLifetime时，我
   们需要保证在用于对象释放相关的减少引用计数动作前调用，或者在可能与之依赖的
   attemptIncStrong前调用，且支持并发地改变对象的生命期。

2. 对象有强引用时AttemptIncStrong将会成功，或者对象有弱引用且从来没有过强引用时
   AttemptIncStrong也会成功。而AttemptIncWeak的成功只有在对象已经有弱引用时才会
   发生，但attemptIncStrong成功后AttemptIncWeak可能会失败。

3. mStrong为强引用计数的变量，mWeak为弱引用计数的变量，在函数调用期间，忽略内存有序性
   （memory ordering）的影响，mWeak将包括强引用，也就是说mWeak的值大于等于mStrong。
   weakref_impl在RefBase对象构造时初始化为mRefs，保存了许多信息，包括两种引用计数，
   需要执行wp<>的操作，这样在RefBase对象销毁后仍可以继续执行。

4. 在OBJECT_LIFETIME_STRONG的条件下，weakref_impl在decWeak时释放，因此可以生存到
   最后一个弱引用消失为之，如果强引用从没有增加且弱引用为0也可以在RefBase析构中释放，
   这时需要在decStrong中调用RefBase析构，若RefBase对象被显式销毁却没有减少强引用计数
   的话，应该避免这种操作。在OBJECT_LIFETIME_WEAK的条件下，decWeak总是会调用RefBase
   析构，而weakref_impl总是在RefBase析构中释放，显式decStrong则避免了这种销毁策略。

5. 关于内存有序性（memory ordering），我们必须保证访问对象时有inc()操作，之后是对应的
   dec()操作。由于内存防护（memory fence）或有序的内存访问可能会损失性能，所以需要限制
   内存有序性的原子操作。对mStrong、mWeak、mFlags的显式访问会在某种方式上relax-meory-
   order，唯一不会导致memory_order_relaxed的是减少引用计数，对应于释放操作。另外，最后
   一个减引用会导致内存释放，随后acquire fence。
********************************************************************************/

// 强引用计数初始值
#define INITIAL_STRONG_VALUE (1<<28)

// 引用计数最大值
#define MAX_COUNT 0xfffff

// 检查强引用计数是否有效（上下限）
#define BAD_STRONG(c) \
        ((c) == 0 || ((c) & (~(MAX_COUNT | INITIAL_STRONG_VALUE))) != 0)

// 检查弱引用计数是否有效（上下限）
#define BAD_WEAK(c) ((c) == 0 || ((c) & (~MAX_COUNT)) != 0)

// ---------------------------------------------------------------------------

/**
 * @class RefBase::weakref_impl
 * 弱引用计数类型implementation
 * 句柄类
 */
class RefBase::weakref_impl : public RefBase::weakref_type
{
public:
    // 使用原子模板类型atomic<int32_t>
    // 普通类型int32_t修改时包括read、modify、write
    std::atomic<int32_t>    mStrong;
    std::atomic<int32_t>    mWeak;
    RefBase* const          mBase;
    std::atomic<int32_t>    mFlags;

#if !DEBUG_REFS

    // 非debug模式时全部为空实现
    explicit weakref_impl(RefBase* base)
        : mStrong(INITIAL_STRONG_VALUE)
        , mWeak(0)
        , mBase(base)
        , mFlags(0)
    {
    }

    void addStrongRef(const void* /*id*/) {}
    void removeStrongRef(const void* /*id*/) {}
    void renameStrongRefId(const void* /*old_id*/, const void* /*new_id*/) {}
    void addWeakRef(const void* /*id*/) {}
    void removeWeakRef(const void* /*id*/) {}
    void renameWeakRefId(const void* /*old_id*/, const void* /*new_id*/) {}
    void printRefs() const {}
    void trackMe(bool, bool) {}

#else

    // debug模式
    weakref_impl(RefBase* base)
        : mStrong(INITIAL_STRONG_VALUE) // 强引用计数初始化为INITIAL_STRONG_VALUE
        , mWeak(0) // 弱引用计数初始化为0
        , mBase(base)
        , mFlags(0)
        , mStrongRefs(NULL)
        , mWeakRefs(NULL)
        , mTrackEnabled(!!DEBUG_REFS_ENABLED_BY_DEFAULT)
        , mRetain(false)
    {
    }
    
    ~weakref_impl()
    {
        bool dumpStack = false;
        // 打印mStrongRefs堆栈
        if (!mRetain && mStrongRefs != NULL) {
            dumpStack = true;
            ALOGE("Strong references remain:");
            ref_entry* refs = mStrongRefs;
            while (refs) {
                char inc = refs->ref >= 0 ? '+' : '-';
                ALOGD("\t%c ID %p (ref %d):", inc, refs->id, refs->ref);
#if DEBUG_REFS_CALLSTACK_ENABLED
                refs->stack.log(LOG_TAG);
#endif
                refs = refs->next;
            }
        }

        // 打印mWeakRefs堆栈
        if (!mRetain && mWeakRefs != NULL) {
            dumpStack = true;
            ALOGE("Weak references remain!");
            ref_entry* refs = mWeakRefs;
            while (refs) {
                char inc = refs->ref >= 0 ? '+' : '-';
                ALOGD("\t%c ID %p (ref %d):", inc, refs->id, refs->ref);
#if DEBUG_REFS_CALLSTACK_ENABLED
                refs->stack.log(LOG_TAG);
#endif
                refs = refs->next;
            }
        }
        // dumpStack为true说明有内存问题并打印堆栈信息
        if (dumpStack) {
            ALOGE("above errors at:");
            CallStack stack(LOG_TAG);
        }
    }

    // 增加强引用
    void addStrongRef(const void* id) {
        addRef(&mStrongRefs, id, mStrong.load(std::memory_order_relaxed));
    }

    // 移除强引用
    void removeStrongRef(const void* id) {
        //ALOGD_IF(mTrackEnabled,
        //        "removeStrongRef: RefBase=%p, id=%p", mBase, id);
        if (!mRetain) {
            removeRef(&mStrongRefs, id);
        } else {
            addRef(&mStrongRefs, id, -mStrong.load(std::memory_order_relaxed));
        }
    }

    // 修改强引用Id
    void renameStrongRefId(const void* old_id, const void* new_id) {
        renameRefsId(mStrongRefs, old_id, new_id);
    }

    // 增加弱引用
    void addWeakRef(const void* id) {
        addRef(&mWeakRefs, id, mWeak.load(std::memory_order_relaxed));
    }

    // 移除弱引用
    void removeWeakRef(const void* id) {
        if (!mRetain) {
            removeRef(&mWeakRefs, id);
        } else {
            addRef(&mWeakRefs, id, -mWeak.load(std::memory_order_relaxed));
        }
    }

    // 修改弱引用Id
    void renameWeakRefId(const void* old_id, const void* new_id) {
        renameRefsId(mWeakRefs, old_id, new_id);
    }

    // 跟踪引用计数信息开关
    void trackMe(bool track, bool retain)
    { 
        mTrackEnabled = track;
        mRetain = retain;
    }

    // 打印引用信息（写文件）
    void printRefs() const
    {
        String8 text;

        {
            Mutex::Autolock _l(mMutex);
            char buf[128];
            snprintf(buf, sizeof(buf),
                     "Strong references on RefBase %p (weakref_type %p):\n",
                     mBase, this);
            text.append(buf);
            printRefsLocked(&text, mStrongRefs);
            snprintf(buf, sizeof(buf),
                     "Weak references on RefBase %p (weakref_type %p):\n",
                     mBase, this);
            text.append(buf);
            printRefsLocked(&text, mWeakRefs);
        }

        {
            char name[100];
            snprintf(name, sizeof(name), DEBUG_REFS_CALLSTACK_PATH "/%p.stack",
                     this);
            int rc = open(name, O_RDWR | O_CREAT | O_APPEND, 644);
            if (rc >= 0) {
                write(rc, text.string(), text.length());
                close(rc);
                ALOGD("STACK TRACE for %p saved in %s", this, name);
            }
            else ALOGE("FAILED TO PRINT STACK TRACE for %p in %s: %s", this,
                      name, strerror(errno));
        }
    }

private:
    // 这是一个重要的结构体
    // 用于统计引用计数信息
    struct ref_entry
    {
        ref_entry* next;
        const void* id;
#if DEBUG_REFS_CALLSTACK_ENABLED
        CallStack stack;
#endif
        int32_t ref;
    };

    // 增加引用
    void addRef(ref_entry** refs, const void* id, int32_t mRef)
    {
        if (mTrackEnabled) {
            AutoMutex _l(mMutex);

            ref_entry* ref = new ref_entry;
            ref->ref = mRef;
            ref->id = id;
#if DEBUG_REFS_CALLSTACK_ENABLED
            ref->stack.update(2);
#endif
            ref->next = *refs;
            *refs = ref;
        }
    }

    // 移除引用
    void removeRef(ref_entry** refs, const void* id)
    {
        if (mTrackEnabled) {
            AutoMutex _l(mMutex);
            
            ref_entry* const head = *refs;
            ref_entry* ref = head;
            while (ref != NULL) {
                if (ref->id == id) {
                    *refs = ref->next;
                    delete ref;
                    return;
                }
                refs = &ref->next;
                ref = *refs;
            }

            ALOGE("RefBase: removing id %p on RefBase %p"
                    "(weakref_type %p) that doesn't exist!",
                    id, mBase, this);

            ref = head;
            while (ref) {
                char inc = ref->ref >= 0 ? '+' : '-';
                ALOGD("\t%c ID %p (ref %d):", inc, ref->id, ref->ref);
                ref = ref->next;
            }

            CallStack stack(LOG_TAG);
        }
    }

    // 修改引用计数Id
    void renameRefsId(ref_entry* r, const void* old_id, const void* new_id)
    {
        if (mTrackEnabled) {
            AutoMutex _l(mMutex);
            ref_entry* ref = r;
            while (ref != NULL) {
                if (ref->id == old_id) {
                    ref->id = new_id;
                }
                ref = ref->next;
            }
        }
    }

    // 打印引用计数信息
    // 增加引用计数用+表示
    // 减少引用计数用-表示
    void printRefsLocked(String8* out, const ref_entry* refs) const
    {
        char buf[128];
        while (refs) {
            char inc = refs->ref >= 0 ? '+' : '-';
            snprintf(buf, sizeof(buf), "\t%c ID %p (ref %d):\n",
                     inc, refs->id, refs->ref);
            out->append(buf);
#if DEBUG_REFS_CALLSTACK_ENABLED
            out->append(refs->stack.toString("\t\t"));
#else
            out->append("\t\t(call stacks disabled)");
#endif
            refs = refs->next;
        }
    }

    mutable Mutex mMutex;
    ref_entry* mStrongRefs;
    ref_entry* mWeakRefs;
    bool mTrackEnabled;
    bool mRetain;

#endif
};

// ---------------------------------------------------------------------------

/**
 * @fn RefBase::incStrong
 */
void RefBase::incStrong(const void* id) const
{
    weakref_impl* const refs = mRefs;
    // 因为弱引用包括强引用所以首先增加弱引用
    refs->incWeak(id);
    
    // 然后添加强引用
    refs->addStrongRef(id);
    // 强引用计数原子性地加1并返回原来的值
    // 至此任务基本完成
    // 下面对强引用计数原来的值进行判断（必须大于0）
    // 强引用计数原来的值不同于初始值INITIAL_STRONG_VALUE时操作结束
    // 否则说明是第一次访问
    const int32_t c = refs->mStrong.fetch_add(1, std::memory_order_relaxed);
    ALOG_ASSERT(c > 0, "incStrong() called on %p after last strong ref", refs);
#if PRINT_REFS
    ALOGD("incStrong of %p from %p: cnt=%d\n", this, id, c);
#endif
    if (c != INITIAL_STRONG_VALUE)  {
        return;
    }

    // 第一次访问时直接把强引用计数减INITIAL_STRONG_VALUE变成刚才增加的1
    // old应该为INITIAL_STRONG_VALUE+1
    int32_t old = refs->mStrong.fetch_sub(INITIAL_STRONG_VALUE,
            std::memory_order_relaxed);
    ALOG_ASSERT(old > INITIAL_STRONG_VALUE, "0x%x too small", old);
    // 第一次访问时回调onFirstRef
    // 子类可以重载onFirstRef做想做的事情
    refs->mBase->onFirstRef();
}

/**
 * @fn RefBase::decStrong
 */
void RefBase::decStrong(const void* id) const
{
    weakref_impl* const refs = mRefs;
    // 首先移除强引用
    refs->removeStrongRef(id);
    // 然后给强引用计数原子性地减1并通过BAD_STRONG检查强引用是否合法
    const int32_t c = refs->mStrong.fetch_sub(1, std::memory_order_release);
#if PRINT_REFS
    ALOGD("decStrong of %p from %p: cnt=%d\n", this, id, c);
#endif
    LOG_ALWAYS_FATAL_IF(BAD_STRONG(c), "decStrong() called on %p too many times",
            refs);
    if (c == 1) {
        // 强引用计数原来的值为1说明这是最后一个强引用
        // 所以回调onLastStrongRef
        // 接着判断对象生命期为strong时delete this
        std::atomic_thread_fence(std::memory_order_acquire);
        refs->mBase->onLastStrongRef(id);
        int32_t flags = refs->mFlags.load(std::memory_order_relaxed);
        if ((flags & OBJECT_LIFETIME_MASK) == OBJECT_LIFETIME_STRONG) {
            // 这种情况下析构不会删除refs
            // refs是在最后一个弱引用时删除的
            // 所以最后通过refs减少弱引用
            delete this;
        }
    }
    // 最后通过refs减少弱引用
    refs->decWeak(id);
}

/**
 * @fn RefBase::forceIncStrong
 * forceIncStrong与incStrong的唯一区别是允许强引用计数原来的值为0
 */
void RefBase::forceIncStrong(const void* id) const
{
    weakref_impl* const refs = mRefs;
    refs->incWeak(id);
    
    refs->addStrongRef(id);
    const int32_t c = refs->mStrong.fetch_add(1, std::memory_order_relaxed);
    ALOG_ASSERT(c >= 0, "forceIncStrong called on %p after ref count underflow",
               refs);
#if PRINT_REFS
    ALOGD("forceIncStrong of %p from %p: cnt=%d\n", this, id, c);
#endif

    switch (c) {
    case INITIAL_STRONG_VALUE:
        refs->mStrong.fetch_sub(INITIAL_STRONG_VALUE,
                std::memory_order_relaxed);
    case 0:
        refs->mBase->onFirstRef();
    }
}

/**
 * @fn RefBase::getStrongCount
 * 获取强引用计数（只用于debug且不能保证memory order）
 */
int32_t RefBase::getStrongCount() const
{
    // Debugging only; No memory ordering guarantees.
    return mRefs->mStrong.load(std::memory_order_relaxed);
}

/**
 * @fn RefBase::weakref_type::refBase
 * 获取RefBase对象指针
 */
RefBase* RefBase::weakref_type::refBase() const
{
    return static_cast<const weakref_impl*>(this)->mBase;
}

/**
 * @fn RefBase::weakref_type::incWeak
 */
void RefBase::weakref_type::incWeak(const void* id)
{
    weakref_impl* const impl = static_cast<weakref_impl*>(this);
    // 先增加弱引用
    impl->addWeakRef(id);
    // 再原子地给弱引用计数加1
    const int32_t c __unused = impl->mWeak.fetch_add(1,
            std::memory_order_relaxed);
    ALOG_ASSERT(c >= 0, "incWeak called on %p after last weak ref", this);
}

/**
 * @fn RefBase::weakref_type::decWeak
 */
void RefBase::weakref_type::decWeak(const void* id)
{
    weakref_impl* const impl = static_cast<weakref_impl*>(this);
    // 首先移除弱引用
    impl->removeWeakRef(id);
    // 然后原子地给弱引用计数减1并检查弱引用计数原来的值是否有效
    const int32_t c = impl->mWeak.fetch_sub(1, std::memory_order_release);
    LOG_ALWAYS_FATAL_IF(BAD_WEAK(c), "decWeak called on %p too many times",
            this);
    // 弱引用计数原来的值不为1时直接return
    // 否则说明是最后一个弱引用        
    if (c != 1) return;
    atomic_thread_fence(std::memory_order_acquire);

    int32_t flags = impl->mFlags.load(std::memory_order_relaxed);
    if ((flags & OBJECT_LIFETIME_MASK) == OBJECT_LIFETIME_STRONG) {
        // 对象生命期为strong
        // 在这里删除weakref_impl
        if (impl->mStrong.load(std::memory_order_relaxed)
                == INITIAL_STRONG_VALUE) {
            ALOGW("RefBase: Object at %p lost last weak reference "
                    "before it had a strong reference", impl->mBase);
        } else {
            delete impl;
        }
    } else {
        // 对象生命期为weak
        // 所以回调onLastWeakRef并delete RefBase
        impl->mBase->onLastWeakRef(id);
        delete impl->mBase;
    }
}

/**
 * @fn RefBase::weakref_type::attemptIncStrong
 */
bool RefBase::weakref_type::attemptIncStrong(const void* id)
{
    // 增加强引用时先增加弱引用
    incWeak(id);
    
    weakref_impl* const impl = static_cast<weakref_impl*>(this);
    // 获取当前的强引用计数
    int32_t curCount = impl->mStrong.load(std::memory_order_relaxed);

    ALOG_ASSERT(curCount >= 0,
            "attemptIncStrong called on %p after underflow", this);

    // 修改强引用计数
    while (curCount > 0 && curCount != INITIAL_STRONG_VALUE) {
        if (impl->mStrong.compare_exchange_weak(curCount, curCount+1,
                std::memory_order_relaxed)) {
            break;
        }
    }
    
    // 所有强引用都被释放或者从来没有过强引用的情况（比较少见）
    if (curCount <= 0 || curCount == INITIAL_STRONG_VALUE) {
        // 获取对象生命期的标记
        int32_t flags = impl->mFlags.load(std::memory_order_relaxed);
        // strong
        if ((flags & OBJECT_LIFETIME_MASK) == OBJECT_LIFETIME_STRONG) {
            // 这个对象的生命期是正常的
            // 对象在最后一个强引用消失时已销毁
            // 所以减少弱引用并return false
            if (curCount <= 0) {
                // the last strong-reference got released, the object cannot
                // be revived.
                decWeak(id);
                return false;
            }

            // 修改强引用计数
            // 方法同上
            while (curCount > 0) {
                if (impl->mStrong.compare_exchange_weak(curCount, curCount+1,
                        std::memory_order_relaxed)) {
                    break;
                }
            }

            if (curCount <= 0) {
                decWeak(id);
                return false;
            }
        // weak
        } else {
            // 这个对象的生命期进行了扩展
            // 也就是说对象只能从弱引用中复活
            // 所以请求对象是否允许复活
            // 不允许时减少弱引用并return false
            if (!impl->mBase->onIncStrongAttempted(FIRST_INC_STRONG, id)) {
                decWeak(id);
                return false;
            }
            // 强引用计数加1
            curCount = impl->mStrong.fetch_add(1, std::memory_order_relaxed);
            // onIncStrongAttempted()获取了不必要的引用所以通过onLastStrongRef()移除
            if (curCount != 0 && curCount != INITIAL_STRONG_VALUE) {
                impl->mBase->onLastStrongRef(id);
            }
        }
    }
    
    // 增加强引用
    impl->addStrongRef(id);

#if PRINT_REFS
    ALOGD("attemptIncStrong of %p from %p: cnt=%d\n", this, id, curCount);
#endif

    // curCount为INITIAL_STRONG_VALUE作特殊处理
    if (curCount == INITIAL_STRONG_VALUE) {
        impl->mStrong.fetch_sub(INITIAL_STRONG_VALUE,
                std::memory_order_relaxed);
    }

    return true;
}

/**
 * @fn RefBase::weakref_type::attemptIncWeak
 * attemptIncWeak类似于attemptIncStrong
 */
bool RefBase::weakref_type::attemptIncWeak(const void* id)
{
    weakref_impl* const impl = static_cast<weakref_impl*>(this);

    int32_t curCount = impl->mWeak.load(std::memory_order_relaxed);
    ALOG_ASSERT(curCount >= 0, "attemptIncWeak called on %p after underflow",
               this);
    while (curCount > 0) {
        if (impl->mWeak.compare_exchange_weak(curCount, curCount+1,
                std::memory_order_relaxed)) {
            break;
        }
    }

    if (curCount > 0) {
        impl->addWeakRef(id);
    }

    return curCount > 0;
}

/**
 * @fn RefBase::weakref_type::getWeakCount
 * 获取弱引用计数（只用于debug）
 */
int32_t RefBase::weakref_type::getWeakCount() const
{
    return static_cast<const weakref_impl*>(this)->mWeak
            .load(std::memory_order_relaxed);
}

/**
 * @fn RefBase::weakref_type::printRefs
 * 打印弱引用信息
 */
void RefBase::weakref_type::printRefs() const
{
    static_cast<const weakref_impl*>(this)->printRefs();
}

/**
 * @fn RefBase::weakref_type::trackMe
 * 追踪弱引用信息
 */
void RefBase::weakref_type::trackMe(bool enable, bool retain)
{
    static_cast<weakref_impl*>(this)->trackMe(enable, retain);
}

/**
 * @fn RefBase::weakref_type::createWeak
 * 创建弱引用
 */
RefBase::weakref_type* RefBase::createWeak(const void* id) const
{
    mRefs->incWeak(id);
    return mRefs;
}

/**
 * @fn RefBase::weakref_type::getWeakRefs
 * 获取弱引用对象指针
 */
RefBase::weakref_type* RefBase::getWeakRefs() const
{
    return mRefs;
}

/**
 * @fn RefBase::RefBase
 * 构造函数创建weakref_impl
 */
RefBase::RefBase()
    : mRefs(new weakref_impl(this))
{
}

/**
 * @fn RefBase::~RefBase
 */
RefBase::~RefBase()
{
    // 获取对象生命期标记
    int32_t flags = mRefs->mFlags.load(std::memory_order_relaxed);
    // weak
    // 弱引用计数为0时销毁weakref_impl
    if ((flags & OBJECT_LIFETIME_MASK) == OBJECT_LIFETIME_WEAK) {
        if (mRefs->mWeak.load(std::memory_order_relaxed) == 0) {
            delete mRefs;
        }
    } else if (mRefs->mStrong.load(std::memory_order_relaxed)
            == INITIAL_STRONG_VALUE) {
        // 从来没有过强引用
        // 所以弱引用计数也要为0且销毁weakref_impl
        LOG_ALWAYS_FATAL_IF(mRefs->mWeak.load() != 0,
                "RefBase: Explicit destruction with non-zero weak "
                "reference count");
        delete mRefs;
    }

    const_cast<weakref_impl*&>(mRefs) = NULL;
}

/**
 * @fn RefBase::extendObjectLifetime
 * 扩展对象生命期
 */
void RefBase::extendObjectLifetime(int32_t mode)
{
    mRefs->mFlags.fetch_or(mode, std::memory_order_relaxed);
}

/**
 * @fn RefBase::onFirstRef
 */
void RefBase::onFirstRef()
{
}

/**
 * @fn RefBase::onLastStrongRef
 */
void RefBase::onLastStrongRef(const void* /*id*/)
{
}

/**
 * @fn RefBase::onIncStrongAttempted
 */
bool RefBase::onIncStrongAttempted(uint32_t flags, const void* /*id*/)
{
    return (flags & FIRST_INC_STRONG) ? true : false;
}

/**
 * @fn RefBase::onLastWeakRef
 */
void RefBase::onLastWeakRef(const void* /*id*/)
{
}

// ---------------------------------------------------------------------------

#if DEBUG_REFS
// debug模式才修改引用计数（通过ReferenceRenamer重载的圆括号完成）
// 否则为空实现
void RefBase::renameRefs(size_t n, const ReferenceRenamer& renamer) {
    for (size_t i = 0 ; i < n ; i++) {
        renamer(i);
    }
}
#else
void RefBase::renameRefs(size_t /*n*/, const ReferenceRenamer& /*renamer*/) {}
#endif

// 同时修改强引用和弱引用的Id
void RefBase::renameRefId(weakref_type* ref,
        const void* old_id, const void* new_id) {
    weakref_impl* const impl = static_cast<weakref_impl*>(ref);
    impl->renameStrongRefId(old_id, new_id);
    impl->renameWeakRefId(old_id, new_id);
}

// 同时修改强引用和弱引用的Id
void RefBase::renameRefId(RefBase* ref,
        const void* old_id, const void* new_id) {
    ref->mRefs->renameStrongRefId(old_id, new_id);
    ref->mRefs->renameWeakRefId(old_id, new_id);
}

VirtualLightRefBase::~VirtualLightRefBase() {}

}; // namespace android
