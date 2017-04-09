#include "configure.h"
#include <log.h>

namespace configure {

Configure *Configure::inst = nullptr;
Configure::Configure() {}
Configure::Configure(const Configure &other) {}
Configure::~Configure() {}
Configure &Configure::operator=(const Configure &other) {}

Configure *Configure::Instance() {
  if (!inst) {
    inst = new Configure();
  }
  return inst;
}

void Configure::Release() {
  if (inst) {
    delete inst;
    inst = nullptr;
  }
}
}