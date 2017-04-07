#include "pasertool.h"

namespace share_me_utils {
namespace json_inner {

StateMachine::StateMachine() { init(); }
StateMachine::~StateMachine() {}
bool StateMachine::init() {
  m_currentState = 0;
  memset(m_currentStateDeep, 0, sizeof(m_currentStateDeep));
  m_deep = 0;
  m_charMap.Clear();
  m_charMap.Set('{');
  m_charMap.Set('}');
  m_charMap.Set('[');
  m_charMap.Set(']');
  m_charMap.Set(',');
  m_charMap.Set(':');
  m_charMap.Set('"');
  return true;
}
int StateMachine::Next(const char &c) {
  if (!m_charMap[c])
    return 0;
  switch (c) {
  case '{': {
    break;
  }
  case '}': {
    break;
  }
  case '[': {
    break;
  }
  case ']': {
    break;
  }
  case ',': {
    break;
  }
  case ':': {
    break;
  }
  case '"': {
    break;
  }
  default: { return 0; }
  }
  return 0;
}

int StateMachine::onIntoObject() {
  if (m_currentState & OUT_ELEM) {
    m_currentState |= OBJECT;
    ++m_currentStateDeep[OBJECT_POS];
    return INTO_OBJECT;
  }
  return 0;
}
int StateMachine::onOutObject() {
  if (m_currentState & (OUT_ELEM | OBJECT)) {
    --m_currentStateDeep[OBJECT_POS];
    if (m_currentStateDeep[OBJECT_POS] == 0) {
      m_currentState &= ~OBJECT;
    }
    return OUT_OBJECT;
  }
  return 0;
}
int StateMachine::onIntoArray() {
  if (m_currentState & OUT_ELEM) {
    m_currentState |= ARRAY;
    ++m_currentStateDeep[ARRAY_POS];
    return INTO_ARRAY;
  }
  return 0;
}
int StateMachine::onOutArray() {
  if (m_currentState & (OUT_ELEM | ARRAY)) {
    --m_currentStateDeep[ARRAY_POS];
    if (m_currentStateDeep[ARRAY_POS] == 0) {
      m_currentState &= ~ARRAY;
    }
    return OUT_ARRAY;
  }
  return 0;
}
int StateMachine::onNextElement() {
  if (m_currentState & (OUT_ELEM | ARRAY | OBJECT)) {
    return NEXT_ELEM;
  }
  return 0;
}

// ----------------------
CharMap::CharMap() {}
CharMap::~CharMap() {}
void CharMap::Clear() { memset(m_map, 0, sizeof(m_map)); }

bool CharMap::operator[](const char &c) { return true; }
}
}