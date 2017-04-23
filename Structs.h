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

#endif