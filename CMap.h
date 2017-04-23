#ifndef _CMAP_H
#define _CMAP_H

#include <iostream>
#include <string.h>

using namespace std;

class CMap{
public:
  CMap();
  CMap(const CMap& m);
  ~CMap();

  int szX;
  int szY;
  
  CMap& operator=(const CMap& m);

  int getTile(int x, int y);
  void loadMap(char* fn);

private:
  
  int** map;

  void deleteMap();

};

#endif
