#ifndef SHARE_ME_MANAGER_MANAGER_H_
#define SHARE_ME_MANAGER_MANAGER_H_

namespace manager {

class Manager {
private:
  explicit Manager();
  explicit Manager(const Manager &other);
  ~Manager();
  Manager &operator=(const Manager &other);

  static Manager *inst;

public:
  Manager *Instance();
  void Release();
};
}


#endif // SHARE_ME_MANAGER_MANAGER_H_