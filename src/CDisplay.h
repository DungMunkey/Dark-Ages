#ifndef _CDISPLAY_H
#define _CDISPLAY_H

#include "Structs.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <string>
#include <vector>

typedef struct sDAVidMode{
  int w;
  int h;
  std::string name;
}sDAVidMode;

class CFont; //forward declaration only - CFont.h includes CDisplay.h, so this avoids a circular include

class CDisplay {
public:
  CDisplay();
  ~CDisplay();

  //SDL_Surface*  screenSurface;
  SDL_Renderer* renderer;
  SDL_Window*   window;
  
  bool init(sConf& conf);
  int  S(int refValue); //scales a 640x400-reference-resolution value to the active mod's tile size
  std::vector<sDAVidMode> screenModes;
  int           screenHeight;
  int           screenWidth;
  size_t        currentScreenMode;
  SDL_Color     txtColors[20];
  sModSettings  modSettings;
  double        scale;

  //World canvas (tile-art, native TileSize-scaled) placement, and the separate UI layer's placement -
  //see computeLayout(). Both are integer multiples of their own reference size, letterboxed to fit
  //the real screen, so tile art keeps its own best-fit magnification while UI text/borders always use
  //the same multiplier regardless of which mod's TileSize is loaded.
  int      canvasW, canvasH;
  int      worldScale;
  SDL_Rect worldRect;
  int      uiScale;
  SDL_Rect uiRect;

  //Canonical font sizes for each pass, computed alongside worldScale/uiScale - see computeLayout().
  //beginUIPass()/beginCompatPass() force the font to the right one every time a pass starts, so
  //leftover state from whichever *other* context last changed the font size (e.g. Options' or
  //Title's temporary footnote-text downsizing, which each run under a different scale) can never
  //leak into the wrong context.
  int      worldFontPx;
  int      uiFontPx;
  void setFont(CFont* f); //called once, after both CDisplay and the (single, shared) CFont exist

  void setCanvasSize(int w, int h); //stores the world canvas's native pixel size and computes the layout
  void computeLayout(); //(re)computes worldScale/worldRect/uiScale/uiRect/worldFontPx/uiFontPx from canvasW/H + current screenWidth/Height - call again after any runtime resolution/fullscreen change

  //Wipes the ENTIRE backbuffer to black. Use this instead of SDL_RenderClear() for full-screen wipes: with SDL
  //2.0.12's Direct3D renderer, SDL_RenderClear() only clears the region of whichever viewport was last drawn
  //into (e.g. uiRect after a UI pass), leaving everything outside it untouched - so anything drawn outside
  //uiRect, like a full-window endgame image or a wider world view, survived into later screens.
  void clearScreen();

  //Brackets UI-space drawing (text, bevel boxes, selection rects) directly onto the current render
  //target (expected to be the backbuffer): temporarily repoints S() at uiScale instead of the mod's
  //native scale, and restricts drawing to uiRect so it lands in the right letterboxed position.
  void beginUIPass();
  void endUIPass();

  //Brackets legacy screens (title splash, battle sprites) that still draw mod-native-scaled bitmap
  //content directly to the backbuffer, the way SDL_RenderSetLogicalSize used to do for every draw
  //call. Unlike beginUIPass(), this uses SDL's own render scale, since that content's positions
  //aren't pre-multiplied by worldScale the way S()-based UI coordinates are pre-multiplied by uiScale.
  void beginCompatPass();
  void endCompatPass();

  //Converts a rect expressed in compat-pass local coordinates (as if drawn during beginCompatPass())
  //into real, absolute screen pixels. Lets a procedural element (e.g. a bevel border) that must
  //exactly frame a piece of mod-native bitmap art be drawn instead during beginUIPass() - which
  //leaves SDL's own render scale neutral, so the border's stroke thickness matches every other UI
  //border's, rather than being magnified by the mod's own worldScale.
  SDL_Rect compatRectToScreenRect(SDL_Rect r);

  //worldRect and uiRect are each independently letterboxed to their own reference size, so they are
  //not guaranteed to be the same width/height - a worldRect wider than uiRect means a rect converted
  //by compatRectToScreenRect() can legitimately fall outside uiRect's bounds (e.g. content near the
  //world's own left/right edge). Since beginUIPass() clips drawing to uiRect, call this pair around
  //drawing such a rect to temporarily lift that clip back to the full screen - scale stays neutral
  //(so stroke thickness is unaffected), only the viewport widens.
  void beginUnclippedUI();
  void endUnclippedUI();

private:
  double savedScale;
  CFont* font;

};

#endif