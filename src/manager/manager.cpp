#include <log.h>
#include "Manager.h"

namespace manager {

Manager* Manager::inst = nullptr;

Manager::Manager() {}
Manager::Manager(const Manager& other) {}
Manager::~Manager() {}
Manager& Manager::operator= (const Manager& other) {
    return *this;
}

Manager* Manager::Instance() {
    if (!inst) {
        inst = new Manager();
    }
    return inst;
}

void Manager::Release() {
    if (inst) {
        delete inst;
        inst = nullptr;
    }
}
}
