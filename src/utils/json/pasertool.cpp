#include "pasertool.h"
#include "log.h"
#include <assert.h>
#include <memory.h>

namespace share_me_utils {
namespace json_inner {

StateMachine::StateMachine() { init(); }
StateMachine::~StateMachine() {}
bool StateMachine::init() {
  m_currentState = OUT_ELEM;
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
  m_charMap.Set('-');
  for (char i = '0'; i <= '9'; ++i) {
    m_charMap.Set(i);
  }
  LOG_INFO("m_currentState = %d", m_currentState);
  return true;
}

const CharMap &StateMachine::GetSpecialCharMap() { return m_charMap; }

bool StateMachine::has(const STATE &s) { return (m_currentState & s) != 0; }

void StateMachine::addPosDeep(const int &pos) {
  if (pos >= 0 && pos < TOP_STATE_POS) {
    ++m_currentStateDeep[pos];
    LOG_INFO(
        "m_currentStateDeep[%d] = %d, m_currentStateDeep[IN_ELEM_POS] = %d",
        pos, m_currentStateDeep[pos], m_currentStateDeep[IN_ELEM_POS]);
    assert(m_currentStateDeep[pos] >= 0);
    assert(m_currentStateDeep[IN_ELEM_POS] == 0 ||
           m_currentStateDeep[IN_ELEM_POS] == 1);
    return;
  }
  LOG_ERROR("invalid pos = %d", pos);
  assert(0);
}

void StateMachine::reducePosDeep(const int &pos) {
  if (pos >= 0 && pos < TOP_STATE_POS) {
    --m_currentStateDeep[pos];
    assert(m_currentStateDeep[pos] >= 0);
    assert(m_currentStateDeep[IN_ELEM_POS] == 0 ||
           m_currentStateDeep[IN_ELEM_POS] == 1);
    return;
  }
  assert(0);
}

bool StateMachine::isSpecialChar(const char &prevChar, const char &curChar) {
  if (!m_charMap[curChar])
    return false;
  if (prevChar == '\\')
    return false;
  // if (has(IN_ELEM))
  //   return false;
  return true;
}

int StateMachine::Next(const char &c) {
  if (!m_charMap[c])
    return 0;
  LOG_INFO("m_currentState = %d", m_currentState);
  int action = 0;
  switch (c) {
  case '{': {
    m_currentState = OUT_ELEM;
    return INTO_OBJECT;
  }
  case '}': {
    m_currentState = OUT_ELEM;
    return GET_OUT_OBJECT;
  }
  case '[': {
    m_currentState = OUT_ELEM;
    return INTO_ARRAY;
  }
  case ']': {
    m_currentState = OUT_ELEM;
    return GET_OUT_ARRAY;
  }
  case ',': {
    m_currentState = OUT_ELEM;
    return NEXT_ELEM;
  }
  case ':': {
    m_currentState = OUT_ELEM;
    return NEXT_ELEM;
  }
  case '-':
  case '"': {
    if (m_currentState == IN_ELEM) {
      m_currentState = OUT_ELEM;
      return GET_OUT_ELEM;
    } else {
      m_currentState = IN_ELEM;
      return INTO_ELEM;
    }
  }
  default: {
    if (c >= '0' && c <= '9') {
      if (m_currentState == OUT_ELEM) {
        m_currentState = IN_ELEM;
      return INTO_ELEM;
      } else {
        assert(0);
        return 0;
      }
    } else {
      return 0;
    }
  }
  }
}

int StateMachine::onIntoObject() {
  LOG_INFO("m_currentState = %d", m_currentState);
  if (has(OUT_ELEM)) {
    m_currentState |= OBJECT;
    addPosDeep(OBJECT_POS);
    LOG_INFO("action INTO_OBJECT");
    LOG_INFO("m_currentState = %d", m_currentState);
    return INTO_OBJECT;
  }
  return 0;
}
int StateMachine::onOutObject() {
  LOG_INFO("m_currentState = %d", m_currentState);
  if (has(OUT_ELEM) && has(OBJECT)) {
    reducePosDeep(OBJECT_POS);
    if (m_currentStateDeep[OBJECT_POS] == 0) {
      m_currentState &= ~OBJECT;
    }
    m_currentState &= ~KEY_ELEM;
    m_currentState &= ~VALUE_ELEM;
    LOG_INFO("action GET_OUT_OBJECT");
    LOG_INFO("m_currentState = %d", m_currentState);
    return GET_OUT_OBJECT;
  }
  return 0;
}
int StateMachine::onIntoArray() {
  if (has(OUT_ELEM) && has(VALUE_ELEM) && has(OBJECT)) {
    m_currentState |= ARRAY;
    addPosDeep(ARRAY_POS);
    LOG_INFO("action INTO_ARRAY");
    return INTO_ARRAY;
  }
  return 0;
}
int StateMachine::onOutArray() {
  if (has(OUT_ELEM) && has(ARRAY)) {
    reducePosDeep(ARRAY_POS);
    if (m_currentStateDeep[ARRAY_POS] == 0) {
      m_currentState &= ~ARRAY;
      m_currentState &= ~KEY_ELEM;
      m_currentState &= ~VALUE_ELEM;
    }
    LOG_INFO("action GET_OUT_ARRAY");
    return GET_OUT_ARRAY;
  }
  return 0;
}
int StateMachine::onIntoElement() {
  LOG_INFO("m_currentState = %d", m_currentState);
  if (has(OUT_ELEM) && (has(ARRAY) || has(OBJECT))) {
    LOG_INFO("m_currentState = %d", m_currentState);
    m_currentState &= ~OUT_ELEM;
    LOG_INFO("m_currentState = %d", m_currentState);
    m_currentState |= IN_ELEM;
    addPosDeep(IN_ELEM_POS);
    LOG_INFO("action INTO_ELEM");
    LOG_INFO("m_currentState = %d", m_currentState);
    return INTO_ELEM;
  }
  return 0;
}
int StateMachine::onOutElement() {
  LOG_INFO("m_currentState = %d", m_currentState);
  if (has(IN_ELEM)) {
    m_currentState &= ~IN_ELEM;
    m_currentState |= OUT_ELEM;
    reducePosDeep(IN_ELEM_POS);
    LOG_INFO("action GET_OUT_ELEM");
    LOG_INFO("m_currentState = %d", m_currentState);
    return GET_OUT_ELEM;
  }
  return 0;
}
int StateMachine::onNextElement() {
  if (!has(OUT_ELEM)) {
    onOutElement();
  }
  if (has(OUT_ELEM) && has(ARRAY) && has(OBJECT)) {
    LOG_DEBUG("action = NEXT_ARRAY_ELEM");
    return NEXT_ARRAY_ELEM;
  } else if (has(OUT_ELEM) && !has(ARRAY) && has(OBJECT)) {
    if (has(VALUE_ELEM)) {
      m_currentState &= ~VALUE_ELEM;
      m_currentState |= KEY_ELEM;
      LOG_DEBUG("action = NEXT_KEY_ELEM");
      return NEXT_KEY_ELEM;
    } else if (has(KEY_ELEM)) {
      m_currentState &= ~KEY_ELEM;
      m_currentState |= VALUE_ELEM;
      LOG_DEBUG("action = NEXT_VALUE_ELEM");
      return NEXT_VALUE_ELEM;
    } else {
      LOG_DEBUG("action = NEXT_OBJECT");
      return NEXT_OBJECT;
    }
    // LOG_DEBUG("action = NEXT_ELEM");
    // return NEXT_ELEM;
  }
  return 0;
}

int StateMachine::onNextElementAfterColon() {
  if (has(OUT_ELEM) && has(OBJECT)) {
    m_currentState &= ~KEY_ELEM;
    m_currentState |= VALUE_ELEM;
    LOG_DEBUG("action = NEXT_VALUE_ELEM");
    return NEXT_VALUE_ELEM;
  }
  LOG_INFO("m_currentState = %d", m_currentState);
  return 0;
}

int StateMachine::onNextElementAfterComma() {
  if (!has(OUT_ELEM)) {
    onOutElement();
  }
  if (has(OUT_ELEM) && has(ARRAY) && has(OBJECT)) {
    LOG_DEBUG("action = NEXT_ARRAY_ELEM");
    return NEXT_ARRAY_ELEM;
  } else if (has(OUT_ELEM) && !has(ARRAY) && has(OBJECT)) {
    if (has(VALUE_ELEM)) {
      m_currentState &= ~VALUE_ELEM;
      m_currentState |= KEY_ELEM;
      LOG_DEBUG("action = NEXT_KEY_ELEM");
      return NEXT_KEY_ELEM;
    } else {
      LOG_DEBUG("action = NEXT_OBJECT");
      return NEXT_OBJECT;
    }
  }
  return 0;
}

// ----------------------
CharMap::CharMap() {}
CharMap::~CharMap() {}
void CharMap::Clear() { memset(m_map, 0, sizeof(m_map)); }

bool CharMap::operator[](const char &c) const {
  int charPos = c / 4;
  int offsetPos = c % 4;
  return (m_map[charPos] & 1 << offsetPos) != 0;
}

bool CharMap::Set(const char &c) {
  if (c >= 32 * 4) {
    LOG_ERROR("char too big %d, %c", c, c);
    assert(0);
  }
  int charPos = c / 4;
  int offsetPos = c % 4;
  m_map[charPos] |= 1 << offsetPos;
  return true;
}
}
}