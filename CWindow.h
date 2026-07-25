#ifndef _CWINDOW_H
#define _CWINDOW_H

#include "CDisplay.h"
#include "CFont.h"
#include "CGfxCollection.h"

//BevelSimple: today's 3-band look (most dialog boxes). BevelRich: today's 5-band look
//(the NPC dialogue/shop box). Both are generated from the same ramp-based drawing code
//in CWindow::renderBox() rather than hand-duplicated per class.
enum eBevelStyle{
  BevelSimple,
  BevelRich
};

class CWindow{
public:
  CWindow();
  CWindow(CDisplay* d, CFont* f, CGfxCollection* g);
  ~CWindow();

  CDisplay* display;
  CFont* font;
  CGfxCollection* gfx;

  virtual void render();
  int renderBox(int x, int y, int w, int h); //instance form for CWindow-derived classes; forwards to the static form using this->display

  //the single shared implementation - callable by any class holding a CDisplay*, no inheritance required.
  //baseColor defaults to today's silver/gray; style picks the band pattern; per-band pixel thickness and
  //band-count detail multiplier are read from display->modSettings (HighResBorders/BevelDetail in mod.cfg).
  //Returns the total border inset in pixels (from the outer edge to the innermost stroke), so callers that
  //need to position content precisely inside the drawn border don't have to duplicate the band/thickness math.
  static int renderBox(CDisplay* display, int x, int y, int w, int h, eBevelStyle style = BevelSimple, SDL_Color baseColor = SDL_Color{128,128,128,255});
};

#endif
