#ifndef _CDISPLAY_H
#define _CDISPLAY_H

#include "Structs.h"
#include <SDL3/SDL.h>
#include <stdio.h>

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
  int           screenHeight;
  int           screenWidth;
  SDL_Color     txtColors[20];
  sModSettings  modSettings;
  double        scale;

  //The range Options offers for sConf::scaleN, computed once in init() from canvasW/H and the display's
  //usable area (so it never changes mid-session: the canvas size is fixed once the mod is loaded, and
  //the game doesn't react to the display changing while it runs). minScale is the smallest whole number
  //that makes the window at least 640x400 for the UI layer; maxScale is the largest that still fits the
  //display's usable area (excluding the taskbar etc.), never below minScale even on a display too small
  //to truly fit it.
  int           minScale;
  int           maxScale;

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

  //Brackets UI-space drawing (text, bevel boxes, selection rects, and - since the display-scaling
  //redesign moved them here, see decision 7 in display-scaling-plan.md - the battle monster sprite and
  //the new-character hero preview) directly onto the current render target (expected to be the
  //backbuffer): temporarily repoints S() at uiScale instead of the mod's native scale, and restricts
  //drawing to uiRect so it lands in the right letterboxed position.
  void beginUIPass();
  void endUIPass();

private:
  double savedScale;
  CFont* font;

};

#endif