#ifndef _CWORLD_H
#define _CWORLD_H

#include <vector>

#include "CMap.h"

class CWorld{
public:

  CMap& operator[](const int& index);
  
  //int  checkTile(int map, int x, int y);
  void loadMaps();

private:

  vector<CMap> maps;
};

#endif