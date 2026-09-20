#ifndef _CDARKAGES_H
#define _CDARKAGES_H

#include "CBattle.h"
#include "CCamera.h"
#include "CDisplay.h"
#include "CFont.h"
#include "CGfxCollection.h"
#include "CLoadSave.h"
#include "CMusic.h"
#include "COptions.h"
#include "CPlayer.h"
#include "CTitle.h"
#include "CWorld.h"
#include "Version.h"
#include "time.h"
#include <string>
#include <vector>

typedef struct da1response {
  std::string text;
  int result;
} da1response;

typedef struct da1script {
  std::vector<std::string>* text;
  std::vector<da1response>* choices;
  size_t selection;
  size_t offset;
  da1script() {
    text = new std::vector<std::string>;
    choices = new std::vector<da1response>;
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
    text = new std::vector<std::string>;
    choices = new std::vector<da1response>;
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
      text = new std::vector<std::string>;
      choices = new std::vector<da1response>;
      for(i = 0; i < s.text->size(); i++) text->push_back(s.text->at(i));
      for(i = 0; i < s.choices->size(); i++) choices->push_back(s.choices->at(i));
    }
    return *this;
  }
  void addChoice(std::string s, int result){
    da1response res;
    res.result=result;
    res.text=s;
    choices->push_back(res);
  }
  void addText(std::string s){
    text->push_back(s);
  }
  void clear(){
    delete text;
    delete choices;
    text = new std::vector<std::string>;
    choices = new std::vector<da1response>;
    selection=0;
    offset=0;
  }
} da1script;

typedef struct da1item{
  char name[32];
  int cost;
  int value;
} da1item;

enum eShopItemType{
  ShopWeapon,
  ShopArmor,
  ShopHelm,
  ShopShield
};

typedef struct sShopItem{
  eShopItemType type;
  int index;
  sShopItem(eShopItemType t, int i):type(t),index(i){}
} sShopItem;

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
  CLoadSave* loadSave;

  da1script script;
  std::vector<int> spellList;

  int fps;
  int actionTile[150]; //tile IDs 0-149 are reserved for vanilla; mod-defined tiles use 150+ and are decorative/walkable-only (see checkAction())

  int curMap;
  int multiFight;
  int playerDir;
  int playerAnim;
  bool idlePlaying;          //true while an idle animation (mod-only, see HeroIdleAnimations) is playing in place of normal walk-frame rendering
  int idleAnimIndex;         //which entry of modSettings.heroIdleAnimations is currently playing
  int idleFrame;             //current frame within that animation
  unsigned int idleTicks;    //ms since last input activity; triggers an idle animation once it crosses idleTimeoutMs
  unsigned int idleFrameTicks; //ms accumulator for advancing idleFrame at idleFrameMs
  bool pendingMapChange;     //a story scene has queued a map change to happen once its dialogue box is dismissed (see queueMapChange())
  int pendingMap;
  int pendingX;
  int pendingY;
  int pendingDir;
  int pendingBattleNum;
  int selection;
  bool showCredits;
  bool showMenu;
  bool showLoad;
  bool showSave;
  bool showShop;
  bool showEquipShop;
  bool showSpellShop;
  bool showSpell;
  bool showStats;
  bool showText;
  bool showTextInput;
  bool showTravel;
  bool stop;
  int curShopCase;
  std::vector<sShopItem> curShopItems;
  std::vector<int> curSpellItems;
  int renderCount;
  std::string userText;

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

  SDL_Texture* canvas;
  int fadeIn;
  sConf* conf;
  sModSettings modSettings;
  unsigned int ticks;

  void actionCursorDown();
  void actionCursorDownRel();
  void actionCursorLeft();
  void actionCursorLeftRel();
  void actionCursorRight();
  void actionCursorRightRel();
  void actionCursorUp();
  void actionCursorUpRel();
  void actionEnter(bool bSpace=false);
  void actionESC();
  void actionSpell();
  void actionStats();
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
  int  currentEquipValue(eShopItemType type);
  void credits();
  da1item* shopItemData(sShopItem item);
  std::string shopItemTypeName(eShopItemType type);
  void death();
  int  doBattle(int index);
  int heroTile(int dir, int frame); //maps a direction (0-3) + walk-cycle frame index into gfx.player's tile index - see CDarkages.cpp for the sheet layout convention
  int idleTile(int animIndex, int frame); //maps an idle-animation index + frame index into gfx.heroIdle's tile index
  void updateIdleAnimation(unsigned int aTicks, bool blockingUIOpen); //advances the idle countdown/animation by aTicks ms; blockingUIOpen suppresses idle entirely while a menu/dialogue/credits screen is up
  CGraphic* currentEndgameImage(); //endgame image for the current eGreyor stage, or NULL
  bool fitsCanvasInWholeScale(CGraphic* g); //true if the world canvas is an exact whole-number multiple of g, so stretching g over the canvas keeps its pixels uniform
  void renderEndgameFullWindow(CGraphic* g); //aspect-preserving, centered, full-window draw for endgame images that aren't 16:10
  void init();
  void loadGame(int index);
  void openLoadMenu(); //shows the load-game menu: sets both this class's showLoad and the CLoadSave layout flag, and resets its cursor to Cancel
  bool newGame();
  void render();
  bool renderCredits();
  void renderEquipShop(int shopCase, std::string greeting, std::vector<sShopItem> items);
  void renderMenu();
  void renderNew();
  //void renderSaves();
  void renderSpell();
  void renderSpellShop(int shopCase, std::string greeting, std::vector<int> spellIndices);
  void renderStats();
  void renderText();
  //void renderTitle();
  void renderTravelSpell();
  void reset();
  void saveGame(int index);
  void setRandomBanter();
  void setText(int i);
  int  titleLoad();
  void travel(int index);
  void updateMapMusic();
  void queueMapChange(int map, int x, int y, int dir, int battleNum); //like a direct curMap change, but deferred until the current dialogue is dismissed
  void applyPendingMapChange(); //performs the queued map change, if any (called when a dialogue box closes)
};

#endif
