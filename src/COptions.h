#ifndef _COPTIONS_H
#define _COPTIONS_H

#include "CDisplay.h"
#include "CMusic.h"
#include "Structs.h"
#include "CWindow.h"
#include "CMods.h"
#include <vector>
#include <string>

enum optAction{
  optNone,
  optPress,
  optUp,
  optDown,
  optLeft,
  optRight
};

class COptions:public CWindow{
public:
  COptions(CDisplay* d, CMusic* m, CFont* f, CGfxCollection* g, sConf* c);
  ~COptions();

  sConf* conf;
  CMusic* music;

  int selection;
  int tmpScreen;
  bool tmpFull;
  bool tmpVSync;
  int tmpVol;
  int tmpMod;
  std::vector<std::string> mods;
  bool active;

  bool logic(optAction a);
  void render();
  void run();
};

#endif