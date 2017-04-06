#ifndef SHARE_ME_JSON_PASERTOOL_H_
#define SHARE_ME_JSON_PASERTOOL_H_

namespace share_me_utils {
namespace json_inner {
class StateMachine {
public:
  enum STATE {
    OBJECT = 1 << 0,
    ARRAY = 1 << 1,

    IN_ELEM = 1 << 2,
    OUT_ELEM = 1 << 3,
  };

public:
  StateMachine();
  ~StateMachine();
  Value *Next(const char &c);

private:
  size_t m_deep;
};

class CharMap {
public:
  CharMap();
  ~CharMap();

  bool operator[](char c);

private:
  char m_map[32];
};
}
}

#endif // SHARE_ME_JSON_PASERTOOL_H_