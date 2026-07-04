#ifndef _DASTRUCTS_H
#define _DASTRUCTS_H

#include <cstring>

typedef struct sConf{
  int w;
  int h;
  int vol;
  bool fullScreen;
  bool vSync;
  char modName[32];
  sConf(){
    w=1280;
    h=200;
    vol=5;
    fullScreen=false;
    vSync=true;
    strcpy(modName, "None");
  }
}sConf;

typedef struct da1saves{
  int day;
  int month;
  int year;
  int level;
  int gold;
  char name[32];
} da1save;

#endif