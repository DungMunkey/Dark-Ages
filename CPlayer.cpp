#include "CPlayer.h"

CPlayer::CPlayer(){
  level=1;
  str=5;
  dex=3;
  wis=2;
  hp=10;
  maxHP=10;
  gold=50;
  mp=2;
  maxMP=2;
  expr=0;
  medallion=0;

  wStr=0;
  aStr=0;
  hStr=0;
  sStr=0;

  for(int i=0; i < 11; i++) spells[i]=false;

  strcpy(name,"The Hero");
  strcpy(weapon,"None");
  strcpy(armor, "None");
  strcpy(helm, "None");
  strcpy(shield, "None");
}