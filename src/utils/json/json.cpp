#include "json.h"

namespace share_me_utils {

Json::Json() : m_text(nullptr), m_textLength(0) {}
Json::Json(const Json &other) {
  if (this == &other)
    return;
  if (m_text) {
    delete m_text;
    m_text = nullptr;
  }
  m_textLength = other.m_textLength;
  m_text = new char[m_textLength];
  memcpy(m_text, other.m_text, m_textLength);
}
Json::~Json() {
  if (m_text) {
    delete m_text;
    m_text = nullptr;
  }
  m_textLength = 0;
}
Json &Json::operator=(const Json &other) {
  if (this == &other)
    return *this;
  Json tmp(other);
  char *text = tmp.m_text;
  tmp.m_text = m_text;
  m_text = text;
  size_t textLength = other.m_textLength;
  other.m_textLength = m_textLength;
  m_textLength = textLength;
  return *this;
}

bool Json::Paser() {
  if (!m_text || m_textLength <= 0) {
    return false;
  }
  return true;
}

// ----------------------------------------

Value::Value() {}
Value::Value(const Value &other) {}
Value::~Value() {}
Value &Value::operator=(const Value &other) { return *this; }
}
