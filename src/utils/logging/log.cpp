#include "log.h"

namespace share_me_utils {

Log::Log() {}

bool Log::Set(const char* prefix) {
    if (!prefix) return false;
    if (*prefix == '\0') return false;
    char* s = prefix;
    size_t i = 0;
    while (s[i] != '\0' && s[i+1] != '\0') {
        if (s[i] == '%') {
            if (i >= 1 && s[i-1] == '\\') continue;
            if (s[i+1] == '\0') break;
            // TODO complete this log prefix
        }
    }
    return true;
}

void LogContent(
    const char* date,
    const char* time,
    const char* filename,
    const char* funcname,
    const int lineno,
    int level,
    const char* format,
    ...
) {
    ;
}

void Log::Trace(const char *format, ...) {
    ;
}
void Log::Debug(const char *format, ...) {
    ;
}
void Log::Info(const char *format, ...) {
    ;
}
void Log::Error(const char *format, ...) {
    ;
}
void Log::Fatal(const char *format, ...) {
    ;
}

void formatString(const char *format, ...) {
    printf("%s %s ", __DATE__, __TIME__);  
    va_list args;       //定义一个va_list类型的变量，用来储存单个参数  
    va_start(args,cmd); //使args指向可变参数的第一个参数  
    snprintf(m_logBuffer, LOG_BUFFER_LENGTH, cmd,args);  //必须用vprintf等带V的  
    va_end(args);
}

} //namespace share_me_utils