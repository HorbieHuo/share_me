#ifndef SHARE_ME_LOG_LOG_H
#define SHARE_ME_LOG_LOG_H

namespace share_me_utils {

class Log {
    public:
        static Log* Instance();
        
        void Trace();
        void Debug();
        void Info();
        void Error();
        void Fatal();

    private:
        Log();
};

} //namespace share_me_utils


#endif //SHARE_ME_LOG_LOG_H