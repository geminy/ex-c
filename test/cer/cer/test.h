#ifndef TEST_H
#define TEST_H

static int var = 1;
int var2;
extern int var2;

void test();
void test2();
static void test3();
inline void test4();
static void test5() {}
inline void test6() {}

#endif // !TEST_H