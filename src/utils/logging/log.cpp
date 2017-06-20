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
  m_levelString[S_WARN] = "WARN";
  m_levelString[S_ERROR] = "ERROR";
  m_levelString[S_FATAL] = "FATAL";
  SetPrefix("%d %t %F %f %l");
  initColor();
#ifdef _WIN32
  m_logEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
#endif
}

bool Log::initColor() {
  memset(m_levelColor, 0, sizeof(m_levelColor));
  HANDLE stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
  GetConsoleScreenBufferInfo(stdHandle, &csbiInfo);
  m_oldColorAttr = csbiInfo.wAttributes;
  m_levelColor[S_TRACE] = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
  m_levelColor[S_DEBUG] = m_oldColorAttr | FOREGROUND_INTENSITY;
  m_levelColor[S_INFO] = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
  m_levelColor[S_WARN] =
      FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;
  m_levelColor[S_ERROR] = FOREGROUND_RED | FOREGROUND_INTENSITY;
  m_levelColor[S_FATAL] =
      FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
  return true;
}

void Log::setColor(int level) {
  static HANDLE stdHandle = NULL;
  if (level >= S_TRACE && level < S_INVALID) {
    if (!stdHandle) {
      stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    }
    SetConsoleTextAttribute(stdHandle, m_levelColor[level]);
  }
}

void Log::resetColor() {
  static HANDLE stdHandle = NULL;
  if (!stdHandle) {
    stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
  }
  SetConsoleTextAttribute(stdHandle, m_oldColorAttr);
}

Log *Log::Instance() {
  static Log *inst = NULL;
  if (!inst)
    inst = new Log();
  return inst;
}

bool Log::Start() {
  DWORD threadID;
  HANDLE threadHandle;
  threadHandle =
      CreateThread(NULL, 0, Log::Instance()->loop, nullptr, 0, &threadID);
  return threadHandle != NULL;
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
  offset = vsnprintf(m_logBuffer + prefixLen, 2 * LOG_BUFFER_LENGTH - prefixLen,
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
  setColor(level);
  fprintf(stdout, "%s", m_logBuffer);
  resetColor();
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

void Log::loop(THREAD_PARAM parma) {
  MsgNode* msgs = nullptr;
  MsgNode* msg = nullptr;
  while (true) {
    msgs = m_msgQueue.get();
    if (!msgs) {
      DWORD dReturn = WaitForSingleObject(m_logEvent, 100)
      switch (dReturn) {
        case WAIT_TIMEOUT:
        case WAIT_OBJECT_0: continue;
        case WAIT_ABANDONED: break;
        case WAIT_FAILED: break;
      }
    }
    while(msgs) {
      msg = msgs;
      out(msg->msg);
      msgs = msgs->next;
      DESTROY_MSG_NODE(msg);
    }
  }
}

bool Log::AppendMsg(LogMsg *msg) { return MsgQueue.Append(msg); }

void Log::out(LogMsg* msg) {
  int offset = 0;
  offset = generatePrefix(msg->fileName, msg->funcName, msg->lineno, msg->logLevel);
  if (offset > 0) {
    memcpy(m_logBuffer, m_prefixBuffer, offset);
  } else
    offset = 0;
  offset += sprintf(m_logBuffer+offset, 2 * LOG_BUFFER_LENGTH-offset, "%s", msg->msg);
  if (offset > 0) {
    offset = offset >= 2 * LOG_BUFFER_LENGTH - 2
                 ? (2 * LOG_BUFFER_LENGTH - 2)
                 : offset;
  } else {
    offset = 0;
  }
  m_logBuffer[offset] = '\n';
  m_logBuffer[offset + 1] = '\0';
  setColor(msg->logLevel);
  fprintf(stdout, "%s", m_logBuffer);
  resetColor();
}

void Log::Notify() {
  if (m_isRunning) return;
  SetEvent(m_logEvent);
}

Log::MsgQueue::MsgQueue() : m_head(nullptr), m_tail(nullptr), m_count(0) {}
Log::MsgQueue::~MsgQueue() {
  MsgNode* node = nullptr;
  while(m_head) {
    node = m_head;
    m_head = m_head->next;
    delete node->msg;
    delete node;
  }
  m_count = 0;
}
bool Log::MsgQueue::Append(LogMsg *msg) {
  if (m_count > MAX_COUNT) {
    return false;
  }
  MsgNode *node = new MsgNode;
  node->msg = msg;
  node->next = nullptr;
  if (!m_head) {
    m_head = node;
  } else {
    m_tail->next = node;
  }
  m_tail = node;
  ++m_count;
  return true;
}
MsgNode *Log::MsgQueue::get() {
  MsgNode *node = m_head;
  m_head = nullptr;
  m_tail = nullptr;
  m_count = 0;
  return node;
}

void Log::MsgQueue::Stop() {
  m_count = MAX_COUNT + 1;
}


void Log::formatString(const char *format, ...) { ; }

// Logger implement

Logger::Logger() {}
Logger::~Logger() {}
void Logger::SendLog(const char *filename, const int lineno,
                     const char *funcname, int level, const char *format, ...) {
  int offset = 0;
  char *logBuffer = new char[LOG_BUFFER_LENGTH];
  va_list args; //定义一个va_list类型的变量，用来储存单个参数
  va_start(args, format); //使args指向可变参数的第一个参数
  offset = vsnprintf(logBuffer, LOG_BUFFER_LENGTH, format,
                     args); //必须用vprintf等带V的
  va_end(args);
  if (offset > 0) {
    offset = offset + prefixLen >= LOG_BUFFER_LENGTH - 2
                 ? (LOG_BUFFER_LENGTH - 2)
                 : offset;
  } else {
    offset = 0;
  }
  logBuffer[offset] = '\n';
  logBuffer[offset + 1] = '\0';
  LogMsg msg = new LogMsg(filename, lineno, funcname, level, logBuffer);
  Log::Instance()->AppendMsg(msg);
}
} // namespace share_me_utils