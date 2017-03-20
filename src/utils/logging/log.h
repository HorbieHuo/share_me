#ifndef SHARE_ME_LOG_LOG_H
#define SHARE_ME_LOG_LOG_H

namespace share_me_utils {

#define LOG_BUFFER_LENGTH 1024

class Log {
    public:
        static Log* Instance();
        bool Set(const char* prefix);
        
        void Trace(const char *format, ...);
        void Debug(const char *format, ...);
        void Info(const char *format, ...);
        void Error(const char *format, ...);
        void Fatal(const char *format, ...);

    private:
        Log();

        void formatString(const char *format, ...);

        char* m_logBuffer[2*LOG_BUFFER_LENGTH];
        char* m_prefixBuffer[LOG_BUFFER_LENGTH];

        enum PREFIX_CELL_TYPE {
            eDate = 0,
            eDime,
            eFile,
            eFunc,
            eLine,
        };
};

} //namespace share_me_utils


#endif //SHARE_ME_LOG_LOG_H