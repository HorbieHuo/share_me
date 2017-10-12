#ifndef SHARE_ME_SYS_CONDITON_H_
#define SHARE_ME_SYS_CONDITON_H_

#include "time.h"

class Condition {
public:
    Condition();
    ~Condition();

    Get();
    Release();
    Wait();
    Wait(Time until);
};

#endif //SHARE_ME_SYS_CONDITON_H_