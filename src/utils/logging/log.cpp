#include "log.h"

namespace share_me_utils {

Log::Log() {
    memset(m_prefixSwitchs, -1, sizeof(m_prefixSwitchs));
    m_prefixSymbols[eDate] = 'd';
    m_prefixSymbols[eTime] = 't';
    m_prefixSymbols[eFile] = 'F';
    m_prefixSymbols[eFunc] = 'f';
    m_prefixSymbols[eLine] = 'l';

    m_levelString[TRACE] = "TRACE";
    m_levelString[DEBUG] = "DEBUG";
    m_levelString[INFO] = "INFO";
    m_levelString[ERROR] = "ERROR";
    m_levelString[FATAL] = "FATAL";
}

bool Log::Set(const char* prefix) {
    if (!prefix) return false;
    if (*prefix == '\0') return false;

    char* s = prefix;
    int switchOffset = 0;
    bool switchConfiged[eTop] = {false};

    for (int i = -1; s[i] != '\0'; ++i) {
        if (s[i] == '%') {
            if (i >= 1 && s[i-1] == '\\') continue;
            if (s[i+1] == '\0') break;
            if (switchOffset >= eTop) break;
            for (int j = eDate; j < eTop; ++j) {
                if (m_prefixSymbols[j] == s[i+1]) {
                    if (!switchConfiged[j]) {
                        for (int k = eDate; k < eTop; ++k) {
                            if (m_prefixSwitchs[k][0] == -1) {
                                m_prefixSwitchs = j;
                                break;
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
    return true;
}

void LogContent(
    const char* filename,
    const char* funcname,
    const int lineno,
    int level,
    const char* format,
    ...
) {
    int offset = 0;
    offset = generatePrefix(filename, funcname, lineno, level)
    if (offset > 0) {
        m_prefixBuffer[offset] = '\0';
    } else offset = 0;
    va_list args;       //定义一个va_list类型的变量，用来储存单个参数  
    va_start(args,format); //使args指向可变参数的第一个参数  
    offset = snprintf(m_logBuffer, 2*LOG_BUFFER_LENGTH, format,args);  //必须用vprintf等带V的  
    va_end(args);
    if (offset > 0) {
        offset = offset >= 2*LOG_BUFFER_LENGTH-2 ? (2*LOG_BUFFER_LENGTH-2) : offset;
    } else {
        offset = 0;
    }
    m_logBuffer[offset] = '\n';
    m_logBuffer[offset+1] = '\0';
    fprintf(stdout, "%s", m_logBuffer);
}

int generatePrefix(
    const char* filename,
    const char* funcname,
    const int lineno,
    int level
) {
    memset(m_prefixBuffer, 0, LOG_BUFFER_LENGTH);
    size_t offset = 0;
    for (int i = eDate; i < eTop; ++i) {
        if (m_prefixSwitchs == -1) break;
        switch (m_prefixSwitchs[i]) {
            case eDate: {
                offset += sprintf(m_prefixBuffer + offset, "%s", date);
                break;
            }
            case eTime: {
                offset += sprintf(m_prefixBuffer + offset, "%s", time);
                break;
            }
            case eFile: {
                offset += sprintf(m_prefixBuffer + offset, "%s", filename);
                break;
            }
            case eFunc: {
                offset += sprintf(m_prefixBuffer + offset, "%s", funcname);
                break;
            }
            case eLine: {
                offset += sprintf(m_prefixBuffer + offset, "%d", lineno);
                break;
            }
            default: {
                break;
            }
        }
    }
    offset += sprintf(m_prefixBuffer + offset, "%s", m_levelString[level]);
    return (offset > 0 && offset < LOG_BUFFER_LENGTH) ? offset : -1;
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