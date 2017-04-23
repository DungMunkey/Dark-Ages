#ifndef _CWINDOW_H
#define _CWINDOW_H

#include "CDisplay.h"
#include "CFont.h"
#include "CGfxCollection.h"

class CWindow{
public:
  CWindow();
  CWindow(CDisplay* d, CFont* f, CGfxCollection* g);
  ~CWindow();

  CDisplay* display;
  CFont* font;
  CGfxCollection* gfx;

  virtual void render();
  void renderBox(int x, int y, int w, int h);
};

#endif