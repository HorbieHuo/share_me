#include "json.h"
#include "log.h"
#include <assert.h>
#include <iostream>
#include <memory.h>


namespace share_me_utils {

Json::Json()
    : m_text(nullptr), m_textLength(0), m_root(nullptr),
      m_currentValue(nullptr), m_expectNextValueType(0) {}
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

void Json::Set(const char *text, const int length) {
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
    LOG_INFO("text char = %c, text offset = %d, textPos - beginPos = %d",
             *textPos, textPos - m_text, textPos - beginPos);
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
      if (m_stateMachine.ActionComplete() == false) {
        continue;
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
  if (m_root) {
    m_root->Print(0);
  }
  return true;
}

bool Json::onAction(int action) {
  if (m_root) {
    m_root->Print();
  }
  if (m_currentValue) {
    m_currentValue->Print();
  }
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
  case json_inner::StateMachine::NEXT_KEY_ELEM: {
    return onNextKeyElement();
  }
  case json_inner::StateMachine::NEXT_VALUE_ELEM: {
    return onNextValueElement();
  }
  case json_inner::StateMachine::NEXT_ARRAY_ELEM: {
    return onNextArrayElement();
  }
  case json_inner::StateMachine::NEXT_OBJECT: {
    return onNextObject();
  }
  default: {
    LOG_ERROR("Error action = %d", action);
    assert(0);
    return false;
  }
  }
  return true;
}

bool Json::onIntoObject() {
  m_expectNextValueType = Value::KEY;
  if (!m_currentValue) {
    m_currentValue = new Value(Value::OBJECT);
    m_root = m_currentValue;
    return true;
  }
  Value *currentValue = new Value(Value::OBJECT);
  m_currentValue->AddChild(currentValue);
  m_currentValue = currentValue;
  LOG_INFO("m_currentValue move to child");
  return true;
}

bool Json::onGetOutObject() {
  if (!m_currentValue)
    return false;
  // if (m_currentValue->GetRole() == Value::VALUE) {
  //   m_currentValue = m_currentValue->GetParent();
  //   assert(m_currentValue->GetRole() == Value::KEY);
  //   m_currentValue = m_currentValue->GetParent();
  //   LOG_INFO("m_currentValue move to parent");
  //   if (!m_currentValue)
  //     return false;
  // }
  assert(m_currentValue->GetRole() == Value::OBJECT);
  m_currentValue = m_currentValue->GetParent();
  LOG_INFO("m_currentValue move to parent");
  return true;
}

bool Json::onIntoArray() {
  m_expectNextValueType = Value::VALUE;
  Value *currentValue = new Value(Value::ARRAY);
  m_currentValue->AddChild(currentValue);
  m_currentValue = currentValue;
  LOG_INFO("m_currentValue move to child");
  return true;
}

bool Json::onGetOutArray() {
  // if (m_currentValue->GetRole() == Value::VALUE) {
  //   m_currentValue = m_currentValue->GetParent();
  //   LOG_INFO("m_currentValue move to parent");
  //   assert(m_currentValue->GetRole() == Value::ARRAY);
  // }
  // m_expectNextValueType = Value::KEY;
  assert(m_currentValue->GetRole() == Value::ARRAY);
  m_currentValue = m_currentValue->GetParent();
  if (m_currentValue->GetRole() == Value::KEY) {
    m_currentValue = m_currentValue->GetParent();
  }
  LOG_INFO("m_currentValue move to parent");
  return true;
}
bool Json::onIntoElement() {
  Value *currentValue = new Value(m_expectNextValueType);
  LOG_INFO("into elem");
  ASSERT_NOT_NULL(m_currentValue);
  m_currentValue->AddChild(currentValue);
  LOG_INFO("into elem");
  m_currentValue = currentValue;
  LOG_INFO("m_currentValue move to child");
  LOG_INFO("into elem");
  return true;
}
bool Json::onGetOutElement() {
  if (!m_currentValue)
    return false;
  if (m_currentValue->GetRole() == Value::KEY)
    return true;
  m_currentValue = m_currentValue->GetParent();
  if (m_currentValue->GetRole() == Value::KEY)
    m_currentValue = m_currentValue->GetParent();
  LOG_INFO("m_currentValue move to parent");
  return true;
}

bool Json::onNextElement() {
  switch (m_currentValue->GetRole()) {
    case Value::KEY : {
      m_expectNextValueType = Value::VALUE;
      break;
    }
    case Value::VALUE : {
      m_expectNextValueType = Value::KEY;
      break;
    }
    case Value::ARRAY : {
      m_expectNextValueType = Value::VALUE;
      break;
    }
    case Value::OBJECT : {
      m_expectNextValueType = Value::KEY;
      break;
    }
    default: {
      assert(0);
    }
  }
  return true;}

bool Json::onNextValueElement() {
  m_expectNextValueType = Value::VALUE;
  ASSERT_NOT_NULL(m_currentValue);
  return true;
}
bool Json::onNextKeyElement() {
  Value *v = m_currentValue->GetParent();
  if (!v)
    return false;
  m_expectNextValueType = Value::KEY;
  m_currentValue = v;
  LOG_INFO("m_currentValue move to parent");
  return true;
}
bool Json::onNextArrayElement() {
  m_expectNextValueType = Value::VALUE;
  return true;
}
bool Json::onNextObject() {
  // Value *v = m_currentValue->GetParent();
  // if (!v)
  //   return false;
  // Value *currentValue = new Value(Value::OBJECT);
  // m_currentValue->AddChild(currentValue);
  // m_currentValue = currentValue;
  m_expectNextValueType = Value::OBJECT;
  LOG_INFO("m_currentValue move to child");
  return true;
}

// ----------------------------------------

Value::Value(const int &role)
    : m_children(nullptr), m_parent(nullptr), m_data(nullptr), m_dataLength(0),
      m_childCount(0), m_role(role) {}
Value::Value(const Value &other) { Set(other.m_data, other.m_dataLength); }
Value::~Value() {
  // TODO delete child node
  if (m_data) {
    delete[] m_data;
    m_data = nullptr;
  }
  m_dataLength = 0;
  if (m_children) {
    for (int i = 0; i < m_childCount; ++i) {
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
  LOG_INFO("data length = %d, 0x%X", length, this);
  memcpy(m_data, data, length);
  m_dataLength = length;
  m_data[m_dataLength] = '\0';
  LOG_INFO("set value %s, 0x%X", m_data, this);
  return true;
}

bool Value::AddChild(Value *v) {
  LOG_INFO("child value 0x%X", v);
  LOG_INFO("this 0x%X", this);
  if (!m_children) {
    LOG_INFO("m_children 0x%X", m_children);
    m_children = new Value *[1];
    LOG_INFO("m_children 0x%X", m_children);
    m_children[0] = v;
    m_childCount = 1;
    v->SetParent(this);
    return true;
  }
  LOG_INFO("old count child %d, byte = %d", m_childCount, sizeof(Value*) * m_childCount);
  Value **newSpace = new Value *[m_childCount + 1];
  memcpy(newSpace, m_children, sizeof(Value*) * m_childCount);
  delete[] m_children;
  m_children = newSpace;
  LOG_INFO("add child finish");
  m_children[m_childCount] = v;
  v->SetParent(this);
  ++m_childCount;
  return true;
}
void Value::SetParent(Value *v) { m_parent = v; }
Value *Value::GetParent() { return m_parent; }
int Value::GetRole() { return m_role; }

void Value::Print(int indent) {
  for (int i = 0; i < indent; ++i) {
    std::cout << " ";
  }
  std::cout << this << " ";
  switch (m_role) {
  case KEY:
  case VALUE: {
    std::cout << m_data << "  " << m_role;
    break;
  }
  case ARRAY: {
    std::cout << "[]";
    break;
  }
  case OBJECT: {
    std::cout << "{}";
    break;
  }
  default: {
    LOG_ERROR("invalid value role %d", m_role);
    assert(0);
  }
  }
  std::cout << std::endl;
  for (int i = 0; i < m_childCount; ++i) {
    m_children[i]->Print(indent + 2);
  }
}
}
