#ifndef SHARE_ME_LOG_LOG_H
#define SHARE_ME_LOG_LOG_H

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace share_me_utils {

#ifdef _WIN32
#define localtime localtime_s
#define sprintf sprintf_s
#endif // _WIN32
#define COLOR unsigned short

#define LOG_BUFFER_LENGTH 1024

class Log {
public:
  enum LEVEL {
    S_TRACE = 0,
    S_DEBUG,
    S_WARN,
    S_INFO,
    S_ERROR,
    S_FATAL,
    S_INVALID,
  };

public:
  static Log *Instance();
  bool SetPrefix(const char *prefix);

  void LogContent(const char *filename, const int lineno, const char *funcname,
                  int level, const char *format, ...);

private:
  Log();

  void formatString(const char *format, ...);
  int generatePrefix(const char *filename, const char *funcname,
                     const int lineno, int level);
  void setColor(int level);
  void resetColor();
  bool initColor();
 
  char m_logBuffer[2 * LOG_BUFFER_LENGTH];
  char m_prefixBuffer[LOG_BUFFER_LENGTH];

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
  COLOR m_levelColor[S_INVALID];
  COLOR m_oldColorAttr;
  char *m_levelString[S_INVALID];
};

class LogClient {
  private:
  LogClient();
};

#define LOG_TRACE(formart, ...)                                                \
  (Log::Instance()->LogContent(__FILE__, __LINE__, __func__, Log::S_TRACE,     \
                               formart, __VA_ARGS__))
#define LOG_DEBUG(formart, ...)                                                \
  (Log::Instance()->LogContent(__FILE__, __LINE__, __func__, Log::S_DEBUG,     \
                               formart, __VA_ARGS__))
#define LOG_INFO(formart, ...)                                                 \
  (Log::Instance()->LogContent(__FILE__, __LINE__, __func__, Log::S_INFO,      \
                               formart, __VA_ARGS__))
#define LOG_WARN(formart, ...)                                                 \
  (Log::Instance()->LogContent(__FILE__, __LINE__, __func__, Log::S_DEBUG,     \
                               formart, __VA_ARGS__))
#define LOG_ERROR(formart, ...)                                                \
  (Log::Instance()->LogContent(__FILE__, __LINE__, __func__, Log::S_ERROR,     \
                               formart, __VA_ARGS__))
#define LOG_FATAL(formart, ...)                                                \
  (Log::Instance()->LogContent(__FILE__, __LINE__, __func__, Log::S_FATAL,     \
                               formart, __VA_ARGS__))

} // namespace share_me_utils

#endif // SHARE_ME_LOG_LOG_H