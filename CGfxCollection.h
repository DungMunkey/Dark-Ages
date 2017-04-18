#ifndef _CGFXCOLLECTION_H
#define _CGFXCOLLECTION_H

#include "CGraphic.h"

class CGfxCollection{
public:
  CGfxCollection();
  ~CGfxCollection();

  CGraphic* tiles;
  CGraphic* player;
  CGraphic* extra;
  CGraphic* monster;
  CGraphic* death;
  CGraphic* endgame1;
  CGraphic* endgame2;
  CGraphic* endgame3;
  CGraphic* title;

  bool loadGfx(SDL_Renderer* rend);

private:
  //CGraphic* bldgs[4];
};

#endif