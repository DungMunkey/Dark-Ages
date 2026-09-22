#ifndef _DASTRUCTS_H
#define _DASTRUCTS_H

#include <cstring>
#include <vector>

//darkages.cfg's very first field. A pre-display-scaling-redesign file's first field (sConfLegacy::w) was
//always a positive window width, so a negative value here is how Darkages.cpp tells the two formats
//apart on read and migrates the old one - see the read logic there. Bump this (to another negative
//value) if sConf's layout ever changes again in a way old readers would misinterpret.
static const int DA_CFG_VERSION = -1;

typedef struct sConf{
  int cfgVersion;
  int scaleN;       //windowed client size = canvas x scaleN (a whole number); ignored in fullscreen - see CDisplay::init()
  int vol;
  bool fullScreen;
  bool vSync;
  char modName[32];
  sConf(){
    memset(this, 0, sizeof(*this)); //this struct is written to darkages.cfg as raw bytes: zero the padding and the unused tail of modName so the file is identical every time
    cfgVersion=DA_CFG_VERSION;
    scaleN=2;
    vol=5;
    fullScreen=false;
    vSync=true;
    strcpy(modName, "None");
  }
}sConf;

//The pre-display-scaling-redesign layout (no version field; darkages.cfg saved a resolution instead of a
//scale). Kept only so Darkages.cpp can migrate an old file the first time it's read - never written.
typedef struct sConfLegacy{
  int w;
  int h;
  int vol;
  bool fullScreen;
  bool vSync;
  char modName[32];
}sConfLegacy;

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