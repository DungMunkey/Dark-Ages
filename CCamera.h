#ifndef _CCAMERA_H
#define _CCAMERA_H

#include "SDL.h"

typedef struct sKeypress{
  sKeypress* next;
  int dir;
  sKeypress(){
    next=NULL;
    dir=-1;
  }
  ~sKeypress(){
    next=NULL;
  }
}sKeypress;

class CCamera{
public:
  CCamera();
  ~CCamera();

  void  blockMovement();
  void  bumpCamera(int x, int y);
  void  clearMovement();
  bool  getKeyBuf();
  void  getTileUnderMouse(int mx, int my, int& x, int& y);
  int   getTileX();
  int   getTileY();
  int   getX();
  int   getY();
  void  keyDown(int dir);
  void  keyUp(int dir);
  bool  scrollCamera(int& count);
  void  setMax(int x, int y); //in tiles
  int   setMovement();
  void  setPos(int x, int y); //in tiles

private: 
  int moveX;
  int moveY;
  int xPos;
  int yPos;
  int maxX;
  int maxY;
  unsigned int lastTime;
  unsigned int curTime;
  bool bKeys[4];

  Uint64 freq;
  Uint64 t0;
  Uint64 t1;
  double deltaTime;
  double speed;

  sKeypress* keyBuf;

};


#endif