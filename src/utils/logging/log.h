#ifndef SHARE_ME_LOG_LOG_H
#define SHARE_ME_LOG_LOG_H

namespace share_me_utils {

#define LOG_BUFFER_LENGTH 1024

class Log {
    public:
        enum LEVEL {
            TRACE,
            DEBUG,
            INFO,
            ERROR,
            FATAL,
            INVALID,
        };
        
    public:
        static Log* Instance();
        bool Set(const char* prefix);
        
        void Trace(const char *format, ...);
        void Debug(const char *format, ...);
        void Info(const char *format, ...);
        void Error(const char *format, ...);
        void Fatal(const char *format, ...);

        void LogContent(
            const char* date,
            const char* time,
            const char* filename,
            const char* funcname,
            const int lineno,
            int level,
            const char* format,
            ...
        );

    private:
        Log();

        void formatString(const char *format, ...);

        char* m_logBuffer[2*LOG_BUFFER_LENGTH];
        char* m_prefixBuffer[LOG_BUFFER_LENGTH];

        enum PREFIX_CELL_TYPE {
            eDate = 0, //%d
            eTime,     //%t
            eFile,     //%F
            eFunc,     //%f
            eLine,     //%l
            eTop,
        };
        char m_prefixSymbols[eTop];
        int m_prefixSwitchs[eTop];
};

} //namespace share_me_utils


#endif //SHARE_ME_LOG_LOG_H