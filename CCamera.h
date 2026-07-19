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
  bool  bumpCamera(double& deltaMs);
  void  clearMovement();
  bool  getKeyBuf();
  void  getTileUnderMouse(int mx, int my, int& x, int& y);
  int   getTileX();
  int   getTileY();
  int   getX();
  int   getY();
  void  keyDown(int dir);
  void  keyUp(int dir);
  void  setMax(int x, int y); //in tiles
  int   setMovement();
  void  setPos(int x, int y); //in tiles

private:
  double xPos;
  double yPos;
  double dXPos;
  double dYPos;
  int maxX;
  int maxY;
  bool bKeys[4];

  sKeypress* keyBuf;

};


#endif