#include "../time.h"

#include <time.h>

DateTime::DateTime():m_t(0){
    struct timeval t;
    if (gettimeofday(&t) == 0)
        m_t = t.tv_nsec + t.tv_sec*1000;
}

DateTime::DateTime(unsigned long t) {
    m_t = t;
}

DateTime::~DateTime(){}

unsigned long DateTime::Get() {
    return m_t;
}