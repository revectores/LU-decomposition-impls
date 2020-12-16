#include <stddef.h>
#include <sys/time.h>

double now();
double timeit(void (*func_ptr)(size_t thread_count, void* args), size_t thread_count, void* args);
