#ifndef SHARE_ME_LOG_LOG_H
#define SHARE_ME_LOG_LOG_H

namespace share_me_utils {

#define LOG_BUFFER_LENGTH 1024

class Log {
    public:
        static Log* Instance();
        bool Set(const char* prefix);
        
        void Trace();
        void Debug();
        void Info();
        void Error();
        void Fatal();

    private:
        Log();

        void formatString(const char *format, ...);

        char* m_logBuffer[LOG_BUFFER_LENGTH];
};

} //namespace share_me_utils


#endif //SHARE_ME_LOG_LOG_H