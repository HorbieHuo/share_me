#ifndef SHARE_ME_JSON_JSON_H_
#define SHARE_ME_JSON_JSON_H_

namespace share_me_utils {
Class Json {
public:
  Json();
  Json(const Json &other);
  ~Json();
  Json &operator=(const Json &other);
};

class Value {
public:
  Value();
  Value(const Value &other);
  ~Value();
  Value &operator=(const Value &other);
}
}

#endif // SHARE_ME_JSON_JSON_H_