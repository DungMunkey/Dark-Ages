#ifndef _CLOADSAVE_H
#define _CLOADSAVE_H

#include "CDisplay.h"
#include "Structs.h"
#include "CWindow.h"


class CLoadSave:public CWindow{
public:
  CLoadSave(CDisplay* d, CFont* f, da1save* s);
  ~CLoadSave();

  int selection;
  bool showLoad;
  da1save* saves;

  void render();

};

#endif