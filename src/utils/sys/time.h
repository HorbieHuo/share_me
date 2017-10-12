#ifndef SHARE_ME_SYS_TIME_H_
#define SHARE_ME_SYS_TIME_H_

class Time {
public:
    Time();
    Time(unsigned long);
    ~Time();

    unsigned long GetTime();

private:
    unsigned long m_t;
}

#endif //SHARE_ME_SYS_TIME_H_