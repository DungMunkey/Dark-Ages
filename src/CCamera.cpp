#include "CCamera.h"
#include <cstdio>

using namespace std;

CCamera::CCamera(){
  xPos=0;
  yPos=0;
  dXPos=0;
  dYPos=0;
  maxX=4000;
  maxY=4000;

  keyBuf = NULL;
  bKeys[0]=false;
  bKeys[1]=false;
  bKeys[2]=false;
  bKeys[3]=false;
}

CCamera::~CCamera(){
  clearMovement();
}

void CCamera::blockMovement(){
  dXPos=xPos;
  dYPos=yPos;
}

bool CCamera::bumpCamera(double& deltaMs){
  const double speed = 0.2; //pixels per millisecond (200 px/sec), matches the previous fixed step rate

  double* pos;
  double* dest;
  if(xPos != dXPos){ pos=&xPos; dest=&dXPos; }
  else if(yPos != dYPos){ pos=&yPos; dest=&dYPos; }
  else return false; //already at rest

  double remaining = *dest - *pos;
  double absRemaining = remaining < 0 ? -remaining : remaining;
  double maxDist = speed * deltaMs;

  if(absRemaining <= maxDist){
    deltaMs -= absRemaining / speed; //only consume the time actually needed to arrive
    *pos = *dest;
    return false; //arrived - caller may spend the leftover deltaMs starting a new step this same frame
  }

  *pos += (remaining > 0 ? maxDist : -maxDist);
  deltaMs = 0;
  return true; //still moving; used up the whole remaining frame budget
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
  x=((int)xPos+mx)/40;
  y=((int)yPos+my)/40;
}

int CCamera::getTileX(){
  return (int)xPos / 40;
}

int CCamera::getTileY(){
  return (int)yPos / 40;
}

int CCamera::getX(){
  return (int)xPos;
}

int CCamera::getY(){
  return (int)yPos;
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

void CCamera::setMax(int x, int y){
  maxX=x*40;
  maxY=y*40;
}

int CCamera::setMovement(){
  if(keyBuf!=NULL){
    switch(keyBuf->dir){
    case 0: dYPos-=40; return 0;
    case 1: dXPos+=40; return 1;
    case 2: dYPos+=40; return 2;
    case 3: dXPos-=40; return 3;
    default: break;
    }
  }
  return -1;
}

void CCamera::setPos(int x, int y){
  xPos = x * 40;
  yPos = y * 40;
  dXPos=xPos;
  dYPos=yPos;
}
