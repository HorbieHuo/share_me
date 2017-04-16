#include "json.h"
#include <assert.h>
#include <memory.h>
#include "log.h"

namespace share_me_utils {

Json::Json()
    : m_text(nullptr), m_textLength(0), m_root(nullptr),
      m_currentValue(nullptr) {}
Json::Json(const Json &other) {
  if (this == &other)
    return;
  if (m_text) {
    delete m_text;
    m_text = nullptr;
  }
  m_textLength = other.m_textLength;
  m_text = new char[m_textLength];
  memcpy(m_text, other.m_text, m_textLength);
}
Json::~Json() {
  if (m_text) {
    delete m_text;
    m_text = nullptr;
  }
  m_textLength = 0;
  if (m_root) {
    delete m_root;
    m_root = nullptr;
  }
  m_currentValue = nullptr;
}
Json &Json::operator=(const Json &other) {
  if (this == &other)
    return *this;
  Json tmp(other);
  char *text = tmp.m_text;
  tmp.m_text = m_text;
  m_text = text;
  size_t textLength = other.m_textLength;
  tmp.m_textLength = m_textLength;
  m_textLength = textLength;
  return *this;
}

void Json::Set(const char* text, const int length) {
  if (!text || length <= 0) {
    return;
  }
  if (m_text) {
    delete[] m_text;
    m_text = nullptr;
  }
  if (m_root) {
    delete m_root;
    m_root = nullptr;
  }
  m_currentValue = nullptr;
  m_text = new char[length + 1];
  memcpy(m_text, text, length);
  m_textLength = length;
  m_text[m_textLength] = '\0';
}

bool Json::Paser() {
  if (!m_text || m_textLength <= 0) {
    return false;
  }
  char *textPos = m_text;
  char prevChar = '\0';
  const json_inner::CharMap &specialCharMap =
      m_stateMachine.GetSpecialCharMap();
  while (*textPos != '\0') {
    if (*textPos == '{')
      break;
    ++textPos;
  }
  if (*textPos != '{')
    return false;
  int currentAction = 0;
  char *beginPos = textPos;
  while (*textPos != '\0') {
    LOG_INFO("text char = %c, text offset = %d, textPos - beginPos = %d", *textPos, textPos - m_text, textPos - beginPos);
    if (m_stateMachine.isSpecialChar(prevChar, *textPos)) {
      LOG_INFO("%c is special char", *textPos);
      currentAction = m_stateMachine.Next(*textPos);
      if (json_inner::StateMachine::GET_OUT_ELEM == currentAction) {
        if (m_currentValue) {
          m_currentValue->Set(beginPos, (int)(textPos - beginPos));
        }
      }
      if (!onAction(currentAction)) {
        return false;
      }
      prevChar = *textPos;
      ++textPos;
      beginPos = textPos;
    } else {
      prevChar = *textPos;
      ++textPos;
    }
    // TODO build value
  }
  return true;
}

bool Json::onAction(int action) {
  switch (action) {
  case json_inner::StateMachine::INTO_OBJECT: {
    return onIntoObject();
  }
  case json_inner::StateMachine::GET_OUT_OBJECT: {
    return onGetOutObject();
  }
  case json_inner::StateMachine::INTO_ARRAY: {
    return onIntoArray();
  }
  case json_inner::StateMachine::GET_OUT_ARRAY: {
    return onGetOutArray();
  }
  case json_inner::StateMachine::INTO_ELEM: {
    return onIntoElement();
  }
  case json_inner::StateMachine::GET_OUT_ELEM: {
    return onGetOutElement();
  }
  case json_inner::StateMachine::NEXT_ELEM: {
    return onNextElement();
  }
  case json_inner::StateMachine::NEXT_OBJECT: {
    return onNextObject();
  }
  default: {
    assert(0);
    return false;
  }
  }
  return true;
}

bool Json::onIntoObject() {
  if (!m_currentValue) {
    m_currentValue = new Value();
    m_root = m_currentValue;
    return true;
  }
  Value *currentValue = new Value();
  m_currentValue->AddChild(currentValue);
  m_currentValue = currentValue;
  return true;
}

bool Json::onGetOutObject() {
  if (!m_currentValue)
    return false;
  m_currentValue = m_currentValue->GetParent();
  return true;
}

bool Json::onIntoArray() { return true; }

bool Json::onGetOutArray() { return true; }
bool Json::onIntoElement() {
  Value *currentValue = new Value();
  m_currentValue->AddChild(currentValue);
  m_currentValue = currentValue;
  return true;
}
bool Json::onGetOutElement() {
  if (!m_currentValue)
    return false;
  m_currentValue = m_currentValue->GetParent();
  return true;
}

bool Json::onNextElement() {
  Value *v = m_currentValue->GetParent();
  if (!v)
    return false;
  Value *currentValue = new Value();
  m_currentValue->AddChild(currentValue);
  m_currentValue = currentValue;
  return true;
}
bool Json::onNextObject() {
  Value *v = m_currentValue->GetParent();
  if (!v)
    return false;
  Value *currentValue = new Value();
  m_currentValue->AddChild(currentValue);
  m_currentValue = currentValue;
  return true;
}

// ----------------------------------------

Value::Value()
    : m_children(nullptr), m_parent(nullptr), m_data(nullptr), m_dataLength(0) {
}
Value::Value(const Value &other) { Set(other.m_data, other.m_dataLength); }
Value::~Value() {
  // TODO delete child node
  if (m_data) {
    delete[] m_data;
    m_data = nullptr;
  }
  m_dataLength = 0;
  if (m_children) {
    int childrenCount = sizeof(m_children) / sizeof( Value*);
    for (int i = 0; i < childrenCount; ++i) {
      delete m_children[i];
      m_children[i] = nullptr;
    }
    delete[] m_children;
    m_children = nullptr;
  }
  m_parent = nullptr;
}
Value &Value::operator=(const Value &other) {
  Set(other.m_data, other.m_dataLength);
  return *this;
}

bool Value::Set(const char *data, const int length) {
  if (!data || length <= 0)
    return false;
  if (m_data) {
    delete[] m_data;
    m_data = nullptr;
  }
  m_data = new char[length + 1];
  memcpy(m_data, data, length);
  m_dataLength = length;
  m_data[m_dataLength] = '\0';
  LOG_INFO("set value %s", m_data);
  return true;
}

bool Value::AddChild(Value *v) {
  if (!m_children) {
    m_children = new Value *[1];
    m_children[0] = v;
    return true;
  }
  int oldCount = sizeof(m_children) / sizeof(Value *);
  Value **newSpace = new Value *[oldCount + 1];
  memcpy(newSpace, m_children, sizeof(m_children));
  delete[] m_children;
  m_children = newSpace;
  m_children[oldCount] = v;
  return true;
}
Value *Value::GetParent() { return m_parent; }
}
