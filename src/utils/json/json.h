#ifndef SHARE_ME_JSON_JSON_H_
#define SHARE_ME_JSON_JSON_H_

#include "pasertool.h"

namespace share_me_utils {

class Value {
public:
  Value();
  Value(const Value &other);
  ~Value();
  Value &operator=(const Value &other);

  bool Set(const char *data, const int length);
  bool AddChild(Value *v);
  Value *GetParent();

private:
  Value **m_children;
  Value *m_parent;

  char *m_data;
  int m_dataLength;
};

class Json {
public:
  Json();
  Json(const Json &other);
  ~Json();
  Json &operator=(const Json &other);

  bool Paser();
  void Set(const char* text, const int length);
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

private:
  char *m_text;
  Value *m_root;
  Value *m_currentValue;
  size_t m_textLength;
  json_inner::StateMachine m_stateMachine;
};
}

#endif // SHARE_ME_JSON_JSON_H_