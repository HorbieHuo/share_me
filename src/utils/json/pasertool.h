#ifndef SHARE_ME_JSON_PASERTOOL_H_
#define SHARE_ME_JSON_PASERTOOL_H_

namespace share_me_utils {
namespace json_inner {
#define STATE_DIVIDE_ACTION
class StateMachine {
public:
  enum STATE {
    OBJECT = 1 << 0,
    ARRAY = 1 << 1,
    KEY_ELEM = 1 << 2,
    VALUE_ELEM = 1 << 3,
    STR_ELEM = 1 << 4,
    NUM_ELEM = 1 << 5,
    IN_ELEM = 1 << 6,
    OUT_ELEM = 1 << 7,
    TOP_STATE = 1 << 8,
  };

  enum ACTION {
    INTO_OBJECT = TOP_STATE << 0,
    OUT_OBJECT = TOP_STATE << 1,
    INTO_ARRAY = TOP_STATE << 2,
    OUT_ARRAY = TOP_STATE << 3,
    NEXT_ELEM = TOP_STATE << 4,
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
  int onNextElement();

private:
  size_t m_deep;
  CharMap m_charMap;
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