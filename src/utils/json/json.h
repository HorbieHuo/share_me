#ifndef SHARE_ME_JSON_JSON_H_
#define SHARE_ME_JSON_JSON_H_

#include "pasertool.h"

namespace share_me_utils {

#define ASSERT_NOT_NULL(v) do {\
if (!v) {\
LOG_ERROR(#v" is nullptr");\
assert(0);\
}\
} while(0)

class Value {
public:
  enum ROLE {
    KEY,
    VALUE,
    ARRAY,
    OBJECT,
  };

  Value(const int &role);
  Value(const Value &other);
  ~Value();
  Value &operator=(const Value &other);

  bool Set(const char *data, const int length);
  bool AddChild(Value *v);
  Value *GetParent();
  void SetParent(Value *v);
  int GetRole();
  void Print(int indent=0);

private:
  Value **m_children;
  int m_childCount;
  Value *m_parent;

  char *m_data;
  int m_dataLength;
  int m_role;
};

class Json {
public:
  Json();
  Json(const Json &other);
  ~Json();
  Json &operator=(const Json &other);

  bool Paser();
  void Set(const char *text, const int length);

private:
  bool onAction(int action);
  bool onIntoObject();
  bool onGetOutObject();
  bool onIntoArray();
  bool onGetOutArray();
  bool onIntoElement();
  bool onGetOutElement();
  bool onNextElement();
  bool onNextObject();
  bool onNextArrayElement();
  bool onNextKeyElement();
  bool onNextValueElement();

private:
  char *m_text;
  Value *m_root;
  Value *m_currentValue;
  size_t m_textLength;
  json_inner::StateMachine m_stateMachine;
  int m_expectNextValueType;
};
}

#endif // SHARE_ME_JSON_JSON_H_