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

  //Canonical UI-pass font size, computed alongside uiScale - see computeLayout(). beginUIPass() forces
  //the font to it every time a UI pass starts, so leftover state from whichever *other* context last
  //changed the font size (e.g. Options' or Title's temporary footnote-text downsizing, which each run
  //under a different scale) can never leak into the wrong context.
  int      uiFontPx;
  void setFont(CFont* f); //called once, after both CDisplay and the (single, shared) CFont exist

  void setCanvasSize(int w, int h); //stores the world canvas's native pixel size and computes the layout
  void computeLayout(); //(re)computes worldScale/worldRect/uiScale/uiRect/uiFontPx from canvasW/H + current screenWidth/Height - call again after any runtime resolution/fullscreen change

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

  //Converts a rect expressed in "compat-local" coordinates - mod-native pixels, the same space the
  //world canvas and its S(32) font size use - into real, absolute screen pixels: worldRect's origin
  //plus the rect scaled by worldScale. Used for legacy screens (the new-character hero preview, the
  //battle monster sprite and its frame) that draw mod-native-scaled bitmap content, or a border that
  //must exactly frame it, straight onto the backbuffer instead of through the world canvas texture.
  //Do the scaling here rather than with SDL's own SDL_SetRenderScale plus a non-default
  //SDL_SetRenderViewport: SDL3 3.4.16 was found to multiply the viewport's own offset by the render
  //scale when both are set together (the same bug the display-scaling-plan.md notes SDL 2.32 had for
  //the viewport rect itself), which pushed content drawn that way further from the screen origin than
  //intended - see the SDL3 migration plan for how this was found and the two call sites it broke.
  //Draw the resulting FRect (SDL_RectToFRect) with the default viewport and scale left neutral.
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