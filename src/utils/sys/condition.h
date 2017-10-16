#ifndef SHARE_ME_SYS_CONDITON_H_
#define SHARE_ME_SYS_CONDITON_H_

#include <pthread.h>
#include "../datetime.h"

class Condition {
public:
    Condition();
    ~Condition();

    Get();
    Release();
    Wait();
    Wait(const DateTime& until);

private:
    pthread_cond_t m_event;
    pthread_mutex_t m_mutex;
};

#endif //SHARE_ME_SYS_CONDITON_H_