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

bool StateMachine::has(const STATE &s) { return m_currentState & s != 0; }

void StateMachine::addPosDeep(const STATE_POS &pos) {
  if (pos >= 0 && pos < TOP_STATE_POS) {
    ++m_currentStateDeep[pos];
    assert(m_currentStateDeep[pos] >= 0);
    assert(m_currentStateDeep[IN_ELEM] == 0 ||
           m_currentStateDeep[IN_ELEM] == 1);
    return;
  }
  assert(0);
}

void StateMachine::reducePosDeep(const STATE_POS &pos) {
  if (pos >= 0 && pos < TOP_STATE_POS) {
    --m_currentStateDeep[pos];
    assert(m_currentStateDeep[pos] >= 0);
    assert(m_currentStateDeep[IN_ELEM] == 0 ||
           m_currentStateDeep[IN_ELEM] == 1);
    return;
  }
  assert(0);
}

int StateMachine::Next(const char &c) {
  if (!m_charMap[c])
    return 0;
  int action = 0;
  switch (c) {
  case '{': {
    action = onIntoObject();
    break;
  }
  case '}': {
    action = onOutObject();
    break;
  }
  case '[': {
    action = onIntoArray();
    break;
  }
  case ']': {
    action = onOutArray();
    break;
  }
  case ',': {
    action = onNextElement();
    break;
  }
  case ':': {
    action = onNextElement();
    break;
  }
  case '"': {
    if (has(IN_ELEM)) {
      action = onOutElement();
    } else {
      action = onIntoElement();
    }
    break;
  }
  default: { return 0; }
  }
  return 0;
}

int StateMachine::onIntoObject() {
  if (has(OUT_ELEM)) {
    m_currentState |= OBJECT;
    addPosDeep(OBJECT_POS);
    return INTO_OBJECT;
  }
  return 0;
}
int StateMachine::onOutObject() {
  if (has(OUT_ELEM) && has(OBJECT)) {
    reducePosDeep(OBJECT_POS);
    if (m_currentStateDeep[OBJECT_POS] == 0) {
      m_currentState &= ~OBJECT;
    }
    return GET_OUT_OBJECT;
  }
  return 0;
}
int StateMachine::onIntoArray() {
  if (has(OUT_ELEM) && has(VALUE_ELEM) && has(OBJECT)) {
    m_currentState |= ARRAY;
    addPosDeep(ARRAY_POS);
    return INTO_ARRAY;
  }
  return 0;
}
int StateMachine::onOutArray() {
  if (m_currentState & (OUT_ELEM | ARRAY)) {
    reducePosDeep(ARRAY_POS);
    if (m_currentStateDeep[ARRAY_POS] == 0) {
      m_currentState &= ~ARRAY;
    }
    return GET_OUT_ARRAY;
  }
  return 0;
}
int StateMachine::onIntoElement() {
  if (has(OUT_ELEM) && (has(ARRAY) || has(OBJECT))) {
    m_currentState &= ~OUT_ELEM;
    m_currentState |= IN_ELEM;
    addPosDeep(IN_ELEM);
    return INTO_ELEM;
  }
  return 0;
}
int StateMachine::onOutElement() {
  if (has(IN_ELEM)) {
    m_currentState &= ~IN_ELEM;
    m_currentState |= OUT_ELEM;
    reducePosDeep(IN_ELEM);
    return GET_OUT_ELEM;
  }
  return 0;
}
int StateMachine::onNextElement() {
  if (!has(OUT_ELEM)) {
    onOutElement();
  }
  if (has(OUT_ELEM) && has(ARRAY) && has(OBJECT)) {
    return NEXT_ELEM;
  } else if (has(OUT_ELEM) && !has(ARRAY) && has(OBJECT)) {
    if (has(VALUE_ELEM)) {
      m_currentState &= ~VALUE_ELEM;
      m_currentState |= KEY_ELEM;
    } else if (has(KEY_ELEM)) {
      m_currentState &= ~KEY_ELEM;
      m_currentState |= VALUE_ELEM;
    } else {
      return NEXT_OBJECT;
    }
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