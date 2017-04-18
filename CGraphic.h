#ifndef _CGRAPHIC_H
#define _CGRAPHIC_H

#include <SDL.h>
#include <stdio.h>
#include <string>

using namespace std;

class CGraphic {
public:
  CGraphic();
  CGraphic(char* fn, SDL_Renderer* screen, bool alpha = false, Uint8 r = 0, Uint8 g = 0, Uint8 b = 0);
  ~CGraphic();
  
  SDL_Texture* texture;
  SDL_Surface* surface;

  SDL_Rect* getTile(int index);
  bool      loadTexture(char* fn, SDL_Renderer* rend, bool surf=false, bool alpha=false, Uint8 r=0, Uint8 g=0, Uint8 b=0);
  bool      createTiles(int szX, int szY, int canX, int canY);
  bool      createTiles(char* fn);

private:

  int       tileCount;
  SDL_Rect* tiles;
  

};

#endif