#include <time.h>
#include "../condition.h"

Condition::Condition() {
    pthread_mutex_init(&m_mutex, NULL);
    pthread_cond_init(&m_event, NULL);
}

Condition::~Condition() {
    pthread_cond_destroy(&m_mutex);
}

bool Condition::Get() {}

bool Condition::Release() {}

bool Condition::Wait() {
    pthread_mutex_lock(&m_mutex);
    int iReturn = pthread_cond_wait(&m_event, &m_mutex);
    pthread_mutex_unlock(&m_mutex);
    return 0 == iReturn;
}

bool Condition::Wait(DateTime& until) {
    struct timespec outTime;
    outTime.tv_sec = (long int)(until.Get() / 1000);
    outTime.tv_nsec = (long int)((until.Get() % 1000) * 1000);
    // pthread_cond_timedwait()
    pthread_mutex_lock(&m_mutex);
    int iReturn = pthread_cond_timedwait(&m_event, &m_mutex, &outTime);
    pthread_mutex_unlock(&m_mutex);
    return 0 == iReturn;
}