#ifndef _DASTRUCTS_H
#define _DASTRUCTS_H

typedef struct sConf{
  int w;
  int h;
  int vol;
  bool fullScreen;
  sConf(){
    w=1280;
    h=200;
    vol=5;
    fullScreen=false;
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