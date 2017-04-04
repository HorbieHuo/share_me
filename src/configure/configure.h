#ifndef SHARE_ME_CONFIGURE_CONFIGURE_H_
#define SHARE_ME_CONFIGURE_CONFIGURE_H_

namespace configure {
class Configure {
private:
  Configure();
  Configure(const Configure &other);
  ~Configure();
  Configure &operator=(const Configure &other);

  static Configure *inst;

public:
  Configure *Instance();
  void Release();

  bool Init();
  bool Save();
  

};
}

#endif // SHARE_ME_CONFIGURE_CONFIGURE_H_