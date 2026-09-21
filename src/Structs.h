#ifndef _DASTRUCTS_H
#define _DASTRUCTS_H

#include <cstring>
#include <vector>

typedef struct sConf{
  int w;
  int h;
  int vol;
  bool fullScreen;
  bool vSync;
  char modName[32];
  sConf(){
    memset(this, 0, sizeof(*this)); //this struct is written to darkages.cfg as raw bytes: zero the padding and the unused tail of modName so the file is identical every time
    w=1280;
    h=200;
    vol=5;
    fullScreen=false;
    vSync=true;
    strcpy(modName, "None");
  }
}sConf;

typedef struct sModSettings{
  int tileSize;
  int monsterSize;
  bool highResBorders; //true = 1px per bevel band stroke instead of 2px (thinner bevel overall)
  int bevelDetail;     //multiplies each bevel style's band count for finer gradation (1 = today's look)
  int heroWalkFrames;  //walk-cycle frames per direction in Gfx/DA1HeroL.bmp (2 = today's two-frame gait)
  std::vector<int> solidTiles; //extra impassable map-tile values (mod.cfg's SolidTiles), on top of CDarkages::checkTile()'s built-in table - same numbering as the raw values stored in .map files
  std::vector<int> heroIdleAnimations; //mod.cfg's HeroIdleAnimations - frame count per idle animation, e.g. "12,16,8" = 3 animations. Empty = feature off (base game has none). See Gfx/DA1HeroIdle.bmp convention: one row per animation, frames left-to-right within that row.
  sModSettings(){
    tileSize=40;
    monsterSize=100;
    highResBorders=false;
    bevelDetail=1;
    heroWalkFrames=2;
  }
}sModSettings;

typedef struct da1saves{
  int day;
  int month;
  int year;
  int level;
  int gold;
  char name[32];
} da1save;

#endif