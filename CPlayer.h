#ifndef _CPLAYER_H
#define _CPLAYER_H

#include <iostream>
#include <string.h>

using namespace std;

class CPlayer{
public:

  CPlayer();

  int level;
  int str;
  int dex;
  int wis;
  int hp;
  int maxHP;
  int gold;
  int mp;
  int maxMP;
  int expr;
  int medallion;

  int wStr;
  int aStr;
  int hStr;
  int sStr;

  bool spells[11];

  char name[32];
  char weapon[32];
  char armor[32];
  char helm[32];
  char shield[32];

  void clear();

private:
};

#endif
