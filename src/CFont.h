#ifndef _CFONT_H
#define _CFONT_H

#include <map>
#include <string>

#ifdef GCC
#include <SDL3_ttf/SDL_ttf.h>
#else
#include <SDL3_ttf/SDL_ttf.h>
#endif

#include "CDisplay.h"

class CFont {
public:
  CFont();
  ~CFont();

  int fontSize;

  int getFontHeight();
  int getStringWidth(char c);
  int getStringWidth(char* str);
  int getStringWidth(std::string str);
  int getStringWidthN(int num);
  int getStringWidthN(const char* str);
  int getStringWidthN(std::string str);
  bool loadFont(const char* fname);
  void setDisplay(CDisplay* d);
  void setFontSize(int sz);
  bool setText(char c, SDL_Texture*& dest);
  void render(int x, int y, char* str, int color = 0, bool rotate = false);
  void render(int x, int y, std::string s, int color = 0, bool rotate = false);
  void renderInt(int x, int y, int num, int color = 0, bool rotate = false);
  int  renderWrap(int x, int y, std::string s, int wrap, int lineHeight, int color = 0); //width of box in pixels. Word wrap past that point. Returns number of lines rendered.

private:
  CDisplay*   display;
  TTF_Font*   font;
  std::string fontPath;

  //Glyphs are cached lazily per point size instead of eagerly pre-rendering a fixed range (6-64) -
  //UI text on a high-resolution/high-DPI screen can legitimately need a native size well past 64,
  //and pre-rendering every size up front for every mod would be wasteful. See ensureSize().
  struct GlyphSet{
    SDL_Texture* texture[128]; //[character]; one glyph per char - color is applied at render time via SDL_SetTextureColorMod
    SDL_Rect     rect[128];
    GlyphSet(){ for(int i=0;i<128;i++) texture[i]=NULL; }
  };
  std::map<int, GlyphSet> glyphCache; //keyed by point size

  void ensureSize(int sz); //renders and caches every glyph at this point size, if not already cached

  int height;
  int width;

};

#endif
