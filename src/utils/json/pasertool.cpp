#include "pasertool.h"

namespace share_me_utils {
namespace json_inner {

StateMachine::StateMachine() {}
StateMachine::~StateMachine() {}
Value *StateMachine::Next(const char &c) { return nullptr; }

// ----------------------
CharMap::CharMap() {}
CharMap::~CharMap() {}

bool CharMap::operator[](char c) { return true; }
}
}