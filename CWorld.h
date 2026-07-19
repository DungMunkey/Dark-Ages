#ifndef _CWORLD_H
#define _CWORLD_H

#include <string>
#include <vector>

#include "CMap.h"

class CWorld{
public:

  CMap& operator[](const int& index);

  //int  checkTile(int map, int x, int y);
  void loadMaps(const std::string& modName);

private:

  std::vector<CMap> maps;
};

#endif