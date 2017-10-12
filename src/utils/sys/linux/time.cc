#include "../time.h"

#include <time.h>

Time::Time():m_t(0){}

Time::~Time(){}

unsigned long Time::GetTime() {
    return m_t;
}