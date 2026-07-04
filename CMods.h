#ifndef _CMODS_H
#define _CMODS_H

#include <string>
#include <vector>

class CMods{
public:
  static std::vector<std::string> listMods();
  static std::string resolve(const std::string& modName, const std::string& defaultPath);
};

#endif
