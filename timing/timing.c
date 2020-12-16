#include "timing.h"

double now(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + (double)tv.tv_usec / 1000000;
}

double timeit(void (*func_ptr)(size_t thread_count, void* args), size_t thread_count, void* args){
    double start_at = now();
    (*func_ptr)(thread_count, args);
    double finish_at = now();
    return finish_at - start_at;
}
