#include "CGraphic.h"
#include <iostream>

CGraphic::CGraphic(){
  texture = NULL;
  surface = NULL;
  tiles = NULL;
}

CGraphic::CGraphic(char* fn, SDL_Renderer* rend, bool alpha, Uint8 r, Uint8 g, Uint8 b){
  texture = NULL;
  surface = NULL;
  tiles = NULL;
  loadTexture(fn, rend, false, alpha, r, g, b);
}

CGraphic::~CGraphic(){
  if(surface!=NULL) SDL_FreeSurface(surface);
  if(texture!=NULL) SDL_DestroyTexture(texture);
  surface=NULL;
  texture=NULL;
  if(tiles!=NULL) delete [] tiles;
  tiles = NULL;
}

SDL_Rect* CGraphic::getTile(int index){
  return &tiles[index];
}

bool CGraphic::loadTexture(char* fn, SDL_Renderer* rend, bool surf, bool alpha, Uint8 r, Uint8 g, Uint8 b) {

  if(texture!=NULL) SDL_DestroyTexture(texture);
  texture=NULL;

  if(surface!=NULL) SDL_FreeSurface(surface);
  surface=NULL;
  
  //Load image at specified path 
  surface = SDL_LoadBMP(fn);
  if( surface == NULL ) { 
    printf( "Unable to load image %s! SDL_image Error: %s\n", fn, SDL_GetError() );
    return false;
  } else {
    if(alpha) SDL_SetColorKey( surface, SDL_TRUE, SDL_MapRGB( surface->format, r, g, b ) );
    //Create texture from surface pixels 
    texture = SDL_CreateTextureFromSurface( rend, surface ); 
    if( texture == NULL ) { 
      printf( "Unable to create texture from %s! SDL Error: %s\n", fn, SDL_GetError() );
      return false;
    } 
    
    //Get rid of old loaded surface 
    if(!surf){
      SDL_FreeSurface( surface ); 
      surface=NULL;
    }
  } 
  
  return true; 
}

bool CGraphic::createTiles(int szX, int szY, int canX, int canY){
  if(texture==NULL) return false;

  //get number of tiles
  int x=canX/szX; //texture->w/szX;
  int y=canY/szY; //texture->h/szY;
  tileCount=x*y;

  //allocate memory
  if(tiles!=NULL) delete [] tiles;
  tiles=new SDL_Rect[tileCount];

  //generate tiles, left to right, top to bottom
  int count=0;
  for(int i=0;i<y;i++){
    for(int j=0;j<x;j++){
      tiles[count].x=j*szX;
      tiles[count].y=i*szY;
      tiles[count].w=szX;
      tiles[count].h=szY;
      count++;
    }
  }
  return true;
}

bool CGraphic::createTiles(char* fn){
  if(texture==NULL) return false;

  char str[256];
  char* tok;

  FILE* f=fopen(fn,"rt");
  if(f==NULL) return false;

  //get number of tiles
  fgets(str,256,f);
  tileCount=atoi(str);

  //allocate memory
  if(tiles!=NULL) delete [] tiles;
  tiles=new SDL_Rect[tileCount];

  //read tiles
  int count=0;
  for(int i=0;i<tileCount;i++){
    fgets(str,256,f);
    tok=strtok(str,",\r\n");
    tiles[count].x=atoi(tok);
    tok=strtok(NULL,",\r\n");
    tiles[count].y=atoi(tok);
    tok=strtok(NULL,",\r\n");
    tiles[count].w=atoi(tok);
    tok=strtok(NULL,",\r\n");
    tiles[count].h=atoi(tok);
    count++;
  }

  fclose(f);
  return true;
}
