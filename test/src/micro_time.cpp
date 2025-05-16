#include "main.h"

long micro_time(){
    struct timeval ts;
    gettimeofday(&ts, NULL); 
    return ts.tv_sec*1000000 + ts.tv_usec; 
}