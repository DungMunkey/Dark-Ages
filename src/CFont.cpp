#include "CFont.h"
#include <cmath>
#include <cstdio>
#include <cstring>

using namespace std;

CFont::CFont() {
  fontSize = 16;
  font = NULL;
  display = NULL;

  TTF_Init();
}

CFont::~CFont() {
  display = NULL;

  for(std::map<int, GlyphSet>::iterator it = glyphCache.begin(); it != glyphCache.end(); ++it){
    for(int k = 0; k<128; k++){
      if(it->second.texture[k] != NULL) SDL_DestroyTexture(it->second.texture[k]);
    }
  }
  glyphCache.clear();

  if (font != NULL){
    TTF_CloseFont(font);
    font = NULL;
  }
  TTF_Quit();
}

int CFont::getFontHeight() {
  return glyphCache.at(fontSize).rect[106].h;
}

int CFont::getStringWidth(char c) {
  return glyphCache.at(fontSize).rect[(int)c].w;
}

int CFont::getStringWidth(char* str) {
  int i = 0;
  int index;
  const SDL_Rect* r = glyphCache.at(fontSize).rect;
  for (size_t j = 0; j<strlen(str); j++){
    index = (int)str[j];
    i += r[index].w;
  }
  return i;
}

int CFont::getStringWidth(string str) {
  return getStringWidth(&str[0]);
}

int CFont::getStringWidthN(const char* str) {
  const SDL_Rect* r = glyphCache.at(fontSize).rect;
  int i = r['8'].w*(int)strlen(str); //digits are treated as monospaced so numeric fields align without jitter
  i += r[','].w*(((int)strlen(str) - 1) / 3); //account for grouping commas
  return i;
}

int CFont::getStringWidthN(int num) {
  const SDL_Rect* r = glyphCache.at(fontSize).rect;
  if (num == 0) return r['8'].w;
  int i = 0;
  if (num<0){
    i += r['-'].w;
    num = -num;
  }
  int n = (int)log10((double)num) + 1;
  i += n*r['8'].w;
  i += r[','].w*((n - 1) / 3);
  return i;
}

int CFont::getStringWidthN(string str) {
  return getStringWidthN(str.c_str());
}

bool CFont::loadFont(const char* fname) {
  fontPath = fname;
  ensureSize(fontSize);
  return glyphCache.find(fontSize) != glyphCache.end();
}

void CFont::ensureSize(int sz) {
  if (glyphCache.find(sz) != glyphCache.end()) return; //already cached

  font = TTF_OpenFont(fontPath.c_str(), (float)sz);
  if (font == NULL) return;

  GlyphSet& gs = glyphCache[sz];
  for (int k = 32; k<127; k++){
    if (!setText((char)k, gs.texture[k])) {
      TTF_CloseFont(font);
      font = NULL;
      glyphCache.erase(sz);
      return;
    }
    float glyphW, glyphH;
    SDL_GetTextureSize(gs.texture[k], &glyphW, &glyphH);
    gs.rect[k].w = (int)glyphW;
    gs.rect[k].h = (int)glyphH;
  }
  TTF_CloseFont(font);
  font = NULL;
}

void CFont::render(int x, int y, char* str, int color, bool rotate) {
  size_t i;
  int index;
  int posX = x;
  int posY = y;
  SDL_FRect r;
  SDL_FPoint p;
  GlyphSet& gs = glyphCache.at(fontSize);

  for (i = 0; i<strlen(str); i++){
    index = (int)str[i];

    SDL_SetTextureColorMod(gs.texture[index], display->txtColors[color].r, display->txtColors[color].g, display->txtColors[color].b);

    r.w = (float)gs.rect[index].w;
    r.h = (float)gs.rect[index].h;
    r.x = (float)posX;
    r.y = (float)posY;
    if (rotate){
      p.x = 0;
      p.y = 0;
      SDL_RenderTextureRotated(display->renderer, gs.texture[index], NULL, &r, -90.0, &p, SDL_FLIP_NONE);
      posY -= gs.rect[index].w;
    }
    else {
      SDL_RenderTexture(display->renderer, gs.texture[index], NULL, &r);
      posX += gs.rect[index].w;
    }
  }

}

void CFont::render(int x, int y, string s, int color, bool rotate) {
  render(x, y, &s[0], color, rotate);
}

void CFont::renderInt(int x, int y, int num, int color, bool rotate) {
  size_t i;
  int index;
  int posX = x;
  int posY = y;
  SDL_FRect r;
  SDL_FPoint p;

  string s;
  char str[64];
  bool neg = false;
  if (num<0){
    neg = true;
    num = -num;
  }
  sprintf(str, "%d", num);
  size_t len = strlen(str);
  for (i = 0; i < len; i++){
    s += str[i];
    if (i<(len - 1) && (len - i - 1) % 3 == 0) s += ',';
  }
  if (neg) s = "-" + s;

  GlyphSet& gs = glyphCache.at(fontSize);
  int bigSpace = gs.rect['8'].w;
  int littleSpace = gs.rect[','].w;
  int offset;
  for (i = 0; i<s.size(); i++){
    index = (int)s[i];

    SDL_SetTextureColorMod(gs.texture[index], display->txtColors[color].r, display->txtColors[color].g, display->txtColors[color].b);

    r.w = (float)gs.rect[index].w;
    r.h = (float)gs.rect[index].h;
    offset = (bigSpace - gs.rect[index].w) / 2;
    r.x = (float)posX;
    r.y = (float)posY;
    if (s[i] != ',') r.x += (float)offset;
    if (rotate){
      p.x = 0;
      p.y = 0;
      SDL_RenderTextureRotated(display->renderer, gs.texture[index], NULL, &r, -90.0, &p, SDL_FLIP_NONE);
      if (s[i] != ',') posY -= bigSpace;
      else posY += littleSpace;
    }
    else {
      SDL_RenderTexture(display->renderer, gs.texture[index], NULL, &r);
      if (s[i] != ',') posX += bigSpace;
      else posX += littleSpace;
    }
  }

}

int CFont::renderWrap(int x, int y, string str, int wrap, int lineHeight, int color) {
  size_t i = 0;
  int posX = x;
  int posY = y;
  string word;
  string line;
  int szWord = 0;
  int szLine = 0;
  int szSpace = glyphCache.at(fontSize).rect[' '].w;
  int lineCount = 0;

  while (i < str.size()) {

    //grab each word
    if (str[i] == ' ') { //identify word was finished. Note: other acceptable break points should be hyphens and carriage returns.
      if (szLine + szSpace + szWord > wrap) { //too large. Render current line then wrap
        render(posX, posY + lineHeight*lineCount++, line, color);
        line = word;
        szLine = szWord;
      } else { //otherwise add word to line
        if (szLine > 0) {
          szLine += szSpace;
          line += " ";
        }
        szLine += szWord;
        line += word;
      }
      word.clear();
      szWord = 0;
    } else { //extend the word by this character
      word += str[i];
      szWord += glyphCache.at(fontSize).rect[(int)str[i]].w;
    }
    i++;
  }

  //done reading all words, so export last of buffer
  if (szLine + szSpace + szWord > wrap) { //too large. Render current line then wrap
    render(posX, posY + lineHeight*lineCount++, line, color);
    line = word;
    szLine = szWord;
  } else {
    if (szLine > 0) {
      szLine += szSpace;
      line += " ";
    }
    szLine += szWord;
    line += word;
  }
  render(posX, posY + lineHeight*lineCount++, line, color);

  return lineCount;
}

void CFont::setDisplay(CDisplay* d) {
  display = d;
}

void CFont::setFontSize(int sz) {
  if (sz<6) sz = 6;
  ensureSize(sz);
  fontSize = sz;
}

bool CFont::setText(char c, SDL_Texture*& dest) {
  SDL_Color col;
  char str[2];
  if (c == 30) str[0] = 'a';
  else if (c == 31) str[0] = 'b';
  else  str[0] = c;
  str[1] = '\0';
  if (dest != NULL) SDL_DestroyTexture(dest);
  col.r = 255;
  col.g = 255;
  col.b = 255;
  col.a = 255;
  SDL_Surface* surf;
  surf = TTF_RenderText_Blended(font, str, 0, col);
  if (surf == NULL) return false;
  dest = SDL_CreateTextureFromSurface(display->renderer, surf);
  if (dest == NULL) return false;
  SDL_SetTextureBlendMode(dest, SDL_BLENDMODE_BLEND);
  SDL_DestroySurface(surf);
  return true;
}
