#ifndef _CMODS_H
#define _CMODS_H

#include "Structs.h"
#include <string>
#include <vector>

class CMods{
public:
  static std::vector<std::string> listMods();
  static sModSettings loadModSettings(const std::string& modName);
  static std::string resolve(const std::string& modName, const std::string& defaultPath);
};

#endif
