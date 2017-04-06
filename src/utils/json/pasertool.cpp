#include "pasertool.h"

namespace share_me_utils {
namespace json_inner {

StateMachine::StateMachine() { init(); }
StateMachine::~StateMachine() {}
bool StateMachine::init() {
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
    default: {
        return 0;
    }
}
  return 0;
}

int StateMachine::onIntoObject() { return 0; }
int StateMachine::onOutObject() { return 0; }
int StateMachine::onIntoArray() { return 0; }
int StateMachine::onOutArray() { return 0; }
int StateMachine::onNextElement() { return 0; }

// ----------------------
CharMap::CharMap() {}
CharMap::~CharMap() {}
void CharMap::Clear() { memset(m_map, 0, sizeof(m_map)); }

bool CharMap::operator[](const char &c) { return true; }
}
}