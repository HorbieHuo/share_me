#ifndef SHARE_ME_JSON_PASERTOOL_H_
#define SHARE_ME_JSON_PASERTOOL_H_

namespace share_me_utils {
namespace json_inner {
#define STATE_DIVIDE_ACTION
class StateMachine {
public:

  enum STATE_POS {
    OBJECT_POS = 0,
    ARRAY_POS = 1,
    KEY_ELEM_POS = 2,
    VALUE_ELEM_POS = 3,
    STR_ELEM_POS = 4,
    NUM_ELEM_POS = 5,
    IN_ELEM_POS = 6,
    OUT_ELEM_POS = 7,
    TOP_STATE_POS = 8,
  }
  enum STATE {
    OBJECT = 1 << OBJECT_POS,
    ARRAY = 1 << ARRAY_POS,
    KEY_ELEM = 1 << KEY_ELEM_POS,
    VALUE_ELEM = 1 << VALUE_ELEM_POS,
    STR_ELEM = 1 << STR_ELEM_POS,
    NUM_ELEM = 1 << NUM_ELEM_POS,
    IN_ELEM = 1 << IN_ELEM_POS,
    OUT_ELEM = 1 << OUT_ELEM_POS,
    TOP_STATE = 1 << TOP_STATE_POS,
  };

  enum ACTION {
    INTO_OBJECT = TOP_STATE << 0,
    OUT_OBJECT = TOP_STATE << 1,
    INTO_ARRAY = TOP_STATE << 2,
    OUT_ARRAY = TOP_STATE << 3,
    INTO_ELEM = TOP_STATE << 4,
    OUT_ELEM = TOP_STATE << 5,
    NEXT_ELEM = TOP_STATE << 6,
  };

public:
  StateMachine();
  ~StateMachine();
  Value *Next(const char &c);

private:
  bool init();
  int onIntoObject();
  int onOutObject();
  int onIntoArray();
  int onOutArray();
  int onIntoElem();
  int onOutElem();
  int onNextElement();

private:
  size_t m_deep;
  CharMap m_charMap;
  int m_currentState;
  int m_currentStateDeep[TOP_STATE_POS];
};

class CharMap {
public:
  CharMap();
  ~CharMap();
  void Clear();
  bool Set(const char &c);

  bool operator[](const char &c);

private:
  char m_map[32];
};
}
}

#endif // SHARE_ME_JSON_PASERTOOL_H_