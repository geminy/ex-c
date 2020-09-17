#define BEGIN __attribute__((constructor))
#define END __attribute__((destructor))

void BEGIN init();
void END final();
void doing();
