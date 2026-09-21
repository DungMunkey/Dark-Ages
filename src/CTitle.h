#ifndef _CTITLE_H
#define _CTITLE_H

#include "CDisplay.h"
#include "CMusic.h"
#include "Structs.h"
#include "CWindow.h"


class CTitle:public CWindow{
public:
  CTitle(CDisplay* d, CFont* f, CGfxCollection* g);
  ~CTitle();

  int selection;
  bool active;

  void actionCursorDown();
  void actionCursorUp();
  int actionEnter();
  void render();
  int run();
};

#endif
