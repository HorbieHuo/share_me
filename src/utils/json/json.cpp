#include "json.h"
#include <memory.h>

namespace share_me_utils {

Json::Json() : m_text(nullptr), m_textLength(0), m_root(nullptr) {}
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

bool Json::Paser() {
  if (!m_text || m_textLength <= 0) {
    return false;
  }
  char *textPos = m_text;
  char prevChar = '\0';
  const json_inner::CharMap &specialCharMap = m_stateMachine.GetSpecialCharMap();
  while (*textPos != '\0') {
    if (*textPos == '{')
      break;
    ++textPos;
  }
  if (*textPos != '{')
    return false;
  int currentAction = 0;
  while (*textPos != '\0') {
    if (specialCharMap[*textPos]) {
      currentAction = m_stateMachine.Next(*textPos);
      //TODO dispatch action
    }
    //TODO build value
  }
  return true;
}

bool Json::onAction(int action) {
  switch(action) {
    case json_inner::StateMachine::INTO_OBJECT: {
      break;
    }
    case json_inner::StateMachine::GET_OUT_OBJECT: {
      break;
    }
    case json_inner::StateMachine::INTO_ARRAY: {
      break;
    }
    case json_inner::StateMachine::GET_OUT_ARRAY: {
      break;
    }
    case json_inner::StateMachine::INTO_ELEM: {
      break;
    }
    case json_inner::StateMachine::GET_OUT_ELEM: {
      break;
    }
    case json_inner::StateMachine::NEXT_ELEM: {
      break;
    }
    case json_inner::StateMachine::NEXT_OBJECT: {
      break;
    }
    default: {
      assert(0);
    }
    return true;
  }
}

bool Json::onIntoObject() {
  if (!m_currentValue) {
    m_currentValue = new Value();
    m_root = m_currentValue;
    return true;
  }
  Value* currentValue = new Value();
  m_currentValue.addChild(currentValue);
  m_currentValue = currentValue;
  return true;
}

bool onGetOutObject() {
  if (!m_currentValue) return false;
  m_currentValue = m_currentValue.GetParent();
  return true;
}

// ----------------------------------------

Value::Value() {}
Value::Value(const Value &other) {}
Value::~Value() {}
Value &Value::operator=(const Value &other) { return *this; }
}
