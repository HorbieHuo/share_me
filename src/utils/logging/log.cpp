#include "log.h"
#include <iostream>

namespace share_me_utils {

Log::Log() {
  memset(m_prefixSwitchs, -1, sizeof(m_prefixSwitchs));
  m_prefixSymbols[eDate] = 'd';
  m_prefixSymbols[eTime] = 't';
  m_prefixSymbols[eFile] = 'F';
  m_prefixSymbols[eFunc] = 'f';
  m_prefixSymbols[eLine] = 'l';

  m_levelString[S_TRACE] = "TRACE";
  m_levelString[S_DEBUG] = "DEBUG";
  m_levelString[S_INFO] = "INFO";
  m_levelString[S_ERROR] = "ERROR";
  m_levelString[S_FATAL] = "FATAL";
  SetPrefix("%d %t %F %f %l");
}

Log *Log::Instance() {
  static Log *inst = NULL;
  if (!inst)
    inst = new Log();
  return inst;
}

bool Log::SetPrefix(const char *prefix) {
  if (!prefix)
    return false;
  if (*prefix == '\0')
    return false;

  const char *s = prefix;
  bool switchConfiged[eTop] = {false};
  memset(m_prefixSwitchs, -1, sizeof(m_prefixSwitchs));
  for (int i = 0; s[i] != '\0'; ++i) {
    if (s[i] == '%') {
      if (i >= 1 && s[i - 1] == '\\')
        continue;
      for (int j = eDate; j < eTop; ++j) {
        if (m_prefixSymbols[j] == s[i + 1]) {
          if (!switchConfiged[j]) {
            for (int k = eDate; k < eTop; ++k) {
              if (m_prefixSwitchs[k] == -1) {
                m_prefixSwitchs[k] = j;
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

void Log::LogContent(const char *filename, const int lineno,
                     const char *funcname, int level, const char *format, ...) {
  int offset = 0;
  int prefixLen = 0;
  prefixLen = generatePrefix(filename, funcname, lineno, level);
  if (prefixLen > 0) {
    // m_prefixBuffer[offset] = '\0';
    memcpy(m_logBuffer, m_prefixBuffer, prefixLen);
  } else
    prefixLen = 0;
  va_list args; //定义一个va_list类型的变量，用来储存单个参数
  va_start(args, format); //使args指向可变参数的第一个参数
  offset = snprintf(m_logBuffer + prefixLen, 2 * LOG_BUFFER_LENGTH - prefixLen,
                    format, args); //必须用vprintf等带V的
  va_end(args);
  if (offset > 0) {
    offset = offset + prefixLen >= 2 * LOG_BUFFER_LENGTH - 2
                 ? (2 * LOG_BUFFER_LENGTH - 2)
                 : offset + prefixLen;
  } else {
    offset = 0;
  }
  m_logBuffer[offset] = '\n';
  m_logBuffer[offset + 1] = '\0';
  fprintf(stdout, "%s", m_logBuffer);
}

int Log::generatePrefix(const char *filename, const char *funcname,
                        const int lineno, int level) {
  memset(m_prefixBuffer, 0, LOG_BUFFER_LENGTH);
  size_t offset = 0;
  for (int i = eDate; i < eTop; ++i) {
    if (m_prefixSwitchs[i] == -1)
      break;
    switch (m_prefixSwitchs[i]) {
    case eDate: {
      time_t rawtime;
      struct tm timeinfo;
      time(&rawtime);
      localtime(&timeinfo, &rawtime);
      offset += strftime(m_prefixBuffer + offset, LOG_BUFFER_LENGTH - offset,
                         "[%Y-%m-%d]", &timeinfo);
      // offset += sprintf(m_prefixBuffer + offset, "%s", date);
      break;
    }
    case eTime: {
      time_t rawtime;
      struct tm timeinfo;
      time(&rawtime);
      localtime(&timeinfo, &rawtime);
      offset += strftime(m_prefixBuffer + offset, LOG_BUFFER_LENGTH - offset,
                         "[%H:%M:%S]", &timeinfo);
      // offset += sprintf(m_prefixBuffer + offset, "%s", time);
      break;
    }
    case eFile: {
      offset += snprintf(m_prefixBuffer + offset,
                         2 * LOG_BUFFER_LENGTH - offset, "[%s]", filename);
      break;
    }
    case eFunc: {
      offset += snprintf(m_prefixBuffer + offset,
                         2 * LOG_BUFFER_LENGTH - offset, "[%s]", funcname);
      break;
    }
    case eLine: {
      offset += snprintf(m_prefixBuffer + offset,
                         2 * LOG_BUFFER_LENGTH - offset, "[%d]", lineno);
      break;
    }
    default: { break; }
    }
  }
  offset += snprintf(m_prefixBuffer + offset, 2 * LOG_BUFFER_LENGTH - offset,
                     "[%s]", m_levelString[level]);
  return (offset > 0 && offset < LOG_BUFFER_LENGTH) ? (int)offset : -1;
}

void Log::formatString(const char *format, ...) { ; }

} // namespace share_me_utils