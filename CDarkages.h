#ifndef _CDARKAGES_H
#define _CDARKAGES_H

#include "CBattle.h"
#include "CCamera.h"
#include "CDisplay.h"
#include "CFont.h"
#include "CGfxCollection.h"
#include "CMusic.h"
#include "COptions.h"
#include "CPlayer.h"
#include "CWorld.h"
#include "time.h"

typedef struct da1response {
  string text;
  int result;
} da1response;

typedef struct da1script {
  vector<string>* text;
  vector<da1response>* choices;
  size_t selection;
  size_t offset;
  da1script() {
    text = new vector<string>;
    choices = new vector<da1response>;
    selection = 0;
    offset=0;
  }
  ~da1script(){
    delete text;
    delete choices;
  }
  da1script(const da1script& s){
    size_t i;
    offset=s.offset;
    selection = s.selection;
    text = new vector<string>;
    choices = new vector<da1response>;
    for(i = 0; i < s.text->size(); i++) text->push_back(s.text->at(i));
    for(i = 0; i < s.choices->size(); i++) choices->push_back(s.choices->at(i));
  }
  da1script& operator=(const da1script& s){
    if(this != &s){
      size_t i;
      offset=s.offset;
      selection = s.selection;
      delete text;
      delete choices;
      text = new vector<string>;
      choices = new vector<da1response>;
      for(i = 0; i < s.text->size(); i++) text->push_back(s.text->at(i));
      for(i = 0; i < s.choices->size(); i++) choices->push_back(s.choices->at(i));
    }
    return *this;
  }
  void addChoice(string s, int result){
    da1response res;
    res.result=result;
    res.text=s;
    choices->push_back(res);
  }
  void addText(string s){
    text->push_back(s);
  }
  void clear(){
    delete text;
    delete choices;
    text = new vector<string>;
    choices = new vector<da1response>;
    selection=0;
    offset=0;
  }
} da1script;

typedef struct da1item{
  char name[32];
  int cost;
  int value;
} da1item;

typedef struct da1saves{
  int day;
  int month;
  int year;
  int level;
  int gold;
  char name[32];
} da1save;

class CDarkages{
public:
  CDarkages(CDisplay* d, sConf* c);
  ~CDarkages();

  void run();
  void title();

private:
  CBattle battle;
  CCamera cam;
  CDisplay* display;
  CFont font;
  CGfxCollection gfx;
  CMusic music;
  CPlayer hero;
  CWorld world;

  da1script script;
  vector<int> spellList;

  int fps;
  int actionTile[150];

  int curMap;
  int multiFight;
  int musicVolume;
  int playerDir;
  int playerAnim;
  int selection;
  bool showCredits;
  bool showMenu;
  bool showLoad;
  bool showSave;
  bool showSpell;
  bool showStats;
  bool showText;
  bool showTextInput;
  bool showTravel;
  bool stop;
  int renderCount;
  string userText;

  //save game information
  da1save saves[6];

  //plain old data
  da1item spells[11];
  da1item weapons[11];
  da1item armors[9];
  da1item helms[4];
  da1item shields[4];

  //event variables
  int eAle;
  int eBattleCheck;
  int eBattleNum;
  int eBridge;
  int eCave;
  int eEmma;
  int eEndGame;
  int eFirewand;
  int eGreyor;
  int eHelpDwarf;
  int eHorn;

  size_t tmpScreen;
  SDL_Texture* canvas;
  int fadeIn;
  sConf* conf;

  void buyArmor(int index);
  void buyArmorB(int index);
  void buyHelm(int index);
  void buyHelmB(int index);
  void buyRoom(int cost);
  void buyRoomB(int cost);
  void buyShield(int index);
  void buyShieldB(int index);
  void buySpell(int index);
  void buySpellB(int index);
  void buyWeapon(int index);
  void buyWeaponB(int index);
  void castSpell(int index);
  void changeMap(int key);
  int  checkAction(int map, int x, int y);
  int  checkBattle();
  int  checkTile(int map, int x, int y);
  void credits();
  void death();
  void init();
  void loadGame(int index);
  bool newGame();
  void render();
  void renderBox(int x, int y, int w, int h);
  bool renderCredits();
  void renderMenu();
  void renderNew();
  void renderOptions();
  void renderSaves();
  void renderSpell();
  void renderStats();
  void renderText();
  void renderTitle();
  void renderTravelSpell();
  void saveGame(int index);
  void setRandomBanter();
  void setText(int i);
  void travel(int index);
};

#endif
