#ifndef SHARE_ME_SYS_DATETIME_H_
#define SHARE_ME_SYS_DATETIME_H_

class DateTime {
public:
    DateTime();
    DateTime(unsigned long);
    ~DateTime();

    unsigned long Get();

private:
    unsigned long m_t;
}

#endif //SHARE_ME_SYS_DATETIME_H_