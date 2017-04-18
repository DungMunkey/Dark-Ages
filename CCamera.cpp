#include "CCamera.h"
#include <cstdio>

CCamera::CCamera(){
  moveX=0;
  moveY=0;
  xPos=0;
  yPos=0;
  maxX=4000;
  maxY=4000;
  lastTime=SDL_GetTicks();

  keyBuf = NULL;
  bKeys[0]=false;
  bKeys[1]=false;
  bKeys[2]=false;
  bKeys[3]=false;

  freq=SDL_GetPerformanceFrequency();
  t0=SDL_GetPerformanceCounter();
  speed=0;
}

CCamera::~CCamera(){
  clearMovement();
}

void CCamera::blockMovement(){
  moveX=0;
  moveY=0;
}

void CCamera::bumpCamera(int x, int y){
  if(moveX != 0 || moveY != 0) return;

  moveX=x;
  moveY=y;

  /*
  xPos+=x;
  yPos+=y;
  if(xPos > maxX) xPos=maxX;
  if(xPos < 0) xPos=0;
  if(yPos > maxY) yPos=maxY;
  if(yPos < 0) yPos=0;
  */


  
  if(xPos+x > maxX) moveX=0;
  if(xPos+x < 0) moveX=0;
  if(yPos+y > maxY) moveY=0;
  if(yPos+y < 0) moveY=0;
  

}

void CCamera::clearMovement(){
  bKeys[0]=false;
  bKeys[1]=false;
  bKeys[2]=false;
  bKeys[3]=false;
  while(keyBuf != NULL){
    sKeypress* s=keyBuf;
    keyBuf=keyBuf->next;
    delete s;
  }
}

bool CCamera::getKeyBuf(){
  if(keyBuf == NULL) return false;
  return true;
}

void CCamera::getTileUnderMouse(int mx, int my, int& x, int& y){
  x=(xPos+mx)/40;
  y=(yPos+my)/40;
}

int CCamera::getTileX(){
  return xPos / 40;
}

int CCamera::getTileY(){
  return yPos / 40;
}

int CCamera::getX(){
  return xPos;
}

int CCamera::getY(){
  return yPos;
}

void CCamera::keyDown(int dir){
  bKeys[dir]=true;
  if(keyBuf==NULL){
    keyBuf=new sKeypress;
    keyBuf->dir=dir;
  } else if(keyBuf->dir!=dir){
    sKeypress* s=keyBuf;
    keyBuf=new sKeypress;
    keyBuf->dir=dir;
    keyBuf->next=s;
    s=NULL;
  }
  //setMovement();
}

void CCamera::keyUp(int dir){
  bKeys[dir]=false; 
  while(keyBuf != NULL){
    if(bKeys[keyBuf->dir] == false){
      sKeypress* s=keyBuf;
      keyBuf=keyBuf->next;
      delete s;
    } else {
      break;
    }
  }
}

bool CCamera::scrollCamera(int& count){
  if(moveX == 0 && moveY == 0) {
    //t0=SDL_GetPerformanceCounter();
    return false;
  }

  //printf("%d,%d\n", moveX, moveY);
  //deltaTime=(SDL_GetPerformanceCounter() - t0)/(double)freq*1000.0;
  //t0=SDL_GetPerformanceCounter();
  //speed+=32 * deltaTime;
  //if(speed < 1) return true;
  //else speed-=1;
    //if(count < 24) return true;
    //count =0;
  //printf("%.8lf\n", deltaTime);
  //if(deltaTime < 8) return true;
  //else t0=SDL_GetPerformanceCounter();
  //curTime=SDL_GetTicks();
  //if(curTime - lastTime < 16) return true;
  //else lastTime=curTime;
  if(moveX < 0){
    xPos--;
    moveX++;
    //if(moveX == 0) setMovement();
    return true;
  }
  if(moveX>0){
    xPos++;
    moveX--;
    //if(moveX==0) setMovement();
    return true;
  }
  if(moveY < 0){
    yPos--;
    moveY++;
    //if(moveY == 0) setMovement();
    return true;
  }
  if(moveY>0){
    yPos++;
    moveY--;
    //if(moveY==0) setMovement();
    return true;
  }
  return false;
}

void CCamera::setMax(int x, int y){
  maxX=x*40;
  maxY=y*40;
}

int CCamera::setMovement(){
  if(moveX != 0 || moveY != 0) return -1;
  if(keyBuf!=NULL){
    switch(keyBuf->dir){
    case 0: moveY=-40; return 0;
    case 1: moveX=40; return 1;
    case 2: moveY=40; return 2;
    case 3: moveX=-40; return 3;
    default: break;
    }
  }
  return -1;
}

void CCamera::setPos(int x, int y){
  xPos = x * 40;
  yPos = y * 40;
}


