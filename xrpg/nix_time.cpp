#include "time.h"

void waitcputime(unsigned v) {
    timespec req = {};
    req.tv_sec = v / 1000;
    req.tv_nsec = (v%1000)*10000000;
    nanosleep(&req, 0);
}

unsigned long getcputime() {
	return clock()*1000/CLOCKS_PER_SEC;
}

unsigned randomseed() {
    return time(0);
}