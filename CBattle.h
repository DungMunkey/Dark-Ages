#ifndef _CBATTLE_H
#define _CBATTLE_H

#include "CDisplay.h"
#include "CFont.h"
#include "CGfxCollection.h"
#include "CPlayer.h"
#include <vector>

using namespace std;

typedef struct da1monster{
  int hp;
  int maxHP;
  int ap;
  int hitp;
  int expr;
  int gold;
  int gfx;
  char name[32];
} da1monster;

class CBattle{
public:

  CBattle();
  ~CBattle();

  bool noRun;

  int fight(int index);
  void init(CDisplay* d, CFont* f, CGfxCollection* g, CPlayer* p);

private:

  CDisplay* display;
  CFont* font;
  CGfxCollection* gfx;
  CPlayer* hero;

  da1monster monsters[20];
  da1monster curMon;
  string spellNames[11];

  bool showSpell;
  bool showText;

  int run;
  int win;

  int level[100];
  int selection;
  int spellSelection;
  vector<string> text;
  vector<int> spells;

  void actionCursorDown();
  void actionCursorUp();
  void actionEnter();
  void castSpell(int index);
  void getReward();
  void monsterAttack();
  void playerAttack();
  void render();
  void renderBox(int x, int y, int w, int h);
  void renderSpell();
  void renderText();
  void setHeroSpells();

};

#endif