#ifndef SHARE_ME_LOG_LOG_H
#define SHARE_ME_LOG_LOG_H

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace share_me_utils
{

#ifdef _WIN32
#define localtime localtime_s
#define sprintf sprintf_s
#define THREAD_PARAM PVOID
#define INIT_NOTIFY_OBJECT CreateEvent(NULL, FALSE, FALSE, NULL)
#define WAIT_NOTIFY WaitForSingleObject
#define SEND_NOTIFY SetEvent
#define CLEAR_NOTIFY ResetEvent
#define DESTROY_NOTYFI CloseHandle
#endif // _WIN32
#define COLOR unsigned short

#define LOG_BUFFER_LENGTH 1024

struct LogMsg
{
  char *fileName;
  int lineno;
  char *funcName;
  int logLevel;
  char *msg;
  LogMsg(char *fileName_, int lineno_, char *funcName_, int logLevel_,
         char *msg_)
  {
    fileName = fileName_;
    lineno = lineno_;
    funcName = funcName_;
    logLevel = logLevel_;
    msg = msg_;
  }
};

struct MsgNode
{
  MsgNode *next;
  LogMsg *msg;
}

#define DESTROY_MSG_NODE(m) \
  do                        \
  {                         \
    delete[] m->msg->msg;
delete m->msg;
delete m;
m = nullptr;
}
while (0)

  class LogDef
  {
  public:
    enum LEVEL
    {
      S_TRACE = 0,
      S_DEBUG,
      S_WARN,
      S_INFO,
      S_ERROR,
      S_FATAL,
      S_INVALID,
    };

  protected:
    enum PREFIX_CELL_TYPE
    {
      eDate = 0, //%d
      eTime,     //%t
      eFile,     //%F
      eFunc,     //%f
      eLine,     //%l
      eTop,
    };
  };

class Log : public LogDef
{
public:
  static Log *Instance();
  bool Start();
  bool SetPrefix(const char *prefix);

  void LogContent(const char *filename, const int lineno, const char *funcname,
                  int level, const char *format, ...);

  bool AppendMsg(LogMsg *msg);
  void Notify();

private:
  Log();

  void formatString(const char *format, ...);
  int generatePrefix(const char *filename, const char *funcname,
                     const int lineno, int level);
  void setColor(int level);
  void resetColor();
  bool initColor();

  void out(LogMsg *msg);
  void loop(THREAD_PARAM parma);
  void waitForNotify();

  char m_logBuffer[2 * LOG_BUFFER_LENGTH];
  char m_prefixBuffer[LOG_BUFFER_LENGTH];

  char m_prefixSymbols[eTop];
  int m_prefixSwitchs[eTop];
  COLOR m_levelColor[S_INVALID];
  COLOR m_oldColorAttr;
  char *m_levelString[S_INVALID];
  bool m_isRunning;
  class MsgQueue
  {
  public:
    MsgQueue();
    bool Append(LogMsg *msg);
    MsgNode *get();
    void Stop();

  private:
    MsgNode *m_head;
    MsgNode *m_tail;
    int m_count;
    static const int MAX_COUNT;
  };
  MsgQueue m_msgQueue;

#ifdef _WIN32
  HANDLE m_logEvent;
#endif
};

class Logger : public LogDef
{
private:
  Logger();
  ~Logger();

public:
  void SendLog(const char *filename, const int lineno, const char *funcname,
               int level, const char *format, ...);

private:
  static thread_local Logger *inst;
};

#define LOG_TRACE(formart, ...)                                            \
  (Log::Instance()->LogContent(__FILE__, __LINE__, __func__, Log::S_TRACE, \
                               formart, __VA_ARGS__))
#define LOG_DEBUG(formart, ...)                                            \
  (Log::Instance()->LogContent(__FILE__, __LINE__, __func__, Log::S_DEBUG, \
                               formart, __VA_ARGS__))
#define LOG_INFO(formart, ...)                                            \
  (Log::Instance()->LogContent(__FILE__, __LINE__, __func__, Log::S_INFO, \
                               formart, __VA_ARGS__))
#define LOG_WARN(formart, ...)                                             \
  (Log::Instance()->LogContent(__FILE__, __LINE__, __func__, Log::S_DEBUG, \
                               formart, __VA_ARGS__))
#define LOG_ERROR(formart, ...)                                            \
  (Log::Instance()->LogContent(__FILE__, __LINE__, __func__, Log::S_ERROR, \
                               formart, __VA_ARGS__))
#define LOG_FATAL(formart, ...)                                            \
  (Log::Instance()->LogContent(__FILE__, __LINE__, __func__, Log::S_FATAL, \
                               formart, __VA_ARGS__))

#define ASYNC_LOG_TRACE(formart, ...)                                      \
  (Log::Instance()->LogContent(__FILE__, __LINE__, __func__, Log::S_TRACE, \
                               formart, __VA_ARGS__))
#define ASYNC_LOG_DEBUG(formart, ...)                                      \
  (Log::Instance()->LogContent(__FILE__, __LINE__, __func__, Log::S_DEBUG, \
                               formart, __VA_ARGS__))
#define ASYNC_LOG_INFO(formart, ...)                                      \
  (Log::Instance()->LogContent(__FILE__, __LINE__, __func__, Log::S_INFO, \
                               formart, __VA_ARGS__))
#define ASYNC_LOG_WARN(formart, ...)                                       \
  (Log::Instance()->LogContent(__FILE__, __LINE__, __func__, Log::S_DEBUG, \
                               formart, __VA_ARGS__))
#define ASYNC_LOG_ERROR(formart, ...)                                      \
  (Log::Instance()->LogContent(__FILE__, __LINE__, __func__, Log::S_ERROR, \
                               formart, __VA_ARGS__))
#define ASYNC_LOG_FATAL(formart, ...)                                      \
  (Log::Instance()->LogContent(__FILE__, __LINE__, __func__, Log::S_FATAL, \
                               formart, __VA_ARGS__))

} // namespace share_me_utils

#endif // SHARE_ME_LOG_LOG_H