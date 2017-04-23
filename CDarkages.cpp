#include "CDarkages.h"

CDarkages::CDarkages(CDisplay* d, sConf* c){
  display = d;
  showCredits=false;
  showLoad=false;
  showMenu=false;
  showSave=false;
  showSpell=false;
  showStats=false;
  showText = false;
  showTextInput = false;
  showTravel=false;
  curMap = 30;
  multiFight=0;
  playerDir = 0;
  playerAnim = 0;
  selection=0;
  init();
  renderCount=0;
  musicVolume=5;
  music.setVolume(musicVolume);
  conf=c;

  canvas = SDL_CreateTexture(display->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 640, 400);
}

CDarkages::~CDarkages(){
  SDL_DestroyTexture(canvas);
  display = NULL;
}

void CDarkages::buyArmor(int index){
  string s;
  da1response res;
  script.clear();
  if(hero.gold<armors[index].cost){
    s = "You don't have enough gold!";
    script.text->push_back(s);
    showText=true;
  } else if(hero.aStr >= armors[index].value){
    s = "You current armor is as good or better than this one. Are you sure?";
    script.text->push_back(s);
    res.text="No.";
    res.result=0;
    script.choices->push_back(res);
    res.text="Yes.";
    res.result=620 + index;
    script.choices->push_back(res);
    showText=true;
  } else {
    buyArmorB(index);
  }
}

void CDarkages::buyArmorB(int index){
  strcpy(hero.armor, armors[index].name);
  hero.aStr=armors[index].value;
  hero.gold-=armors[index].cost;
  script.clear();
  script.text->push_back("Enjoy your purchase!");
  showText=true;
}

void CDarkages::buyHelm(int index){
  string s;
  da1response res;
  script.clear();
  if(hero.gold<helms[index].cost){
    s = "You don't have enough gold!";
    script.text->push_back(s);
    showText=true;
  } else if(hero.hStr >= helms[index].value){
    s = "You current armor is as good or better than this one. Are you sure?";
    script.text->push_back(s);
    res.text="No.";
    res.result=0;
    script.choices->push_back(res);
    res.text="Yes.";
    res.result=630 + index;
    script.choices->push_back(res);
    showText=true;
  } else {
    buyHelmB(index);
  }
}

void CDarkages::buyHelmB(int index){
  strcpy(hero.helm, helms[index].name);
  hero.hStr=helms[index].value;
  hero.gold-=helms[index].cost;
  script.clear();
  script.text->push_back("Enjoy your purchase!");
  showText=true;
}

void CDarkages::buyRoom(int cost){
  string s;
  da1response res;
  script.clear();
  if(hero.gold<cost){
    s = "You don't have enough gold pieces!";
    script.text->push_back(s);
    showText=true;
  } else {
    buyRoomB(cost);
  }
}

void CDarkages::buyRoomB(int cost){
  hero.gold-=cost;
  hero.hp=hero.maxHP;
  hero.mp=hero.maxMP;
  script.clear();
  script.text->push_back("You awaken the next morning feeling refreshed!");
  showText=true;
}

void CDarkages::buySpell(int index){
  string s;
  script.clear();
  if(hero.gold<spells[index].cost){
    s = "You don't have enough gold pieces!";
    script.text->push_back(s);
    showText=true;
  } else if(hero.spells[index]){
    s = "You already know that spell!";
    script.text->push_back(s);
    showText=true;
  } else if(index>0 && !hero.spells[index-1]){
    s = "You haven't learned the lower spell levels yet! Come back when you know all the lower spell levels.";
    script.text->push_back(s);
    showText=true;
  } else {
    buySpellB(index);
  }
}

void CDarkages::buySpellB(int index){
  hero.spells[index]=true;
  hero.gold-=spells[index].cost;
  script.clear();
  script.text->push_back("You purchased a spell!");
  showText=true;
}

void CDarkages::buyShield(int index){
  string s;
  da1response res;
  script.clear();
  if(hero.gold<shields[index].cost){
    s = "You don't have enough gold!";
    script.text->push_back(s);
    showText=true;
  } else if(hero.sStr >= shields[index].value){
    s = "You current armor is as good or better than this one. Are you sure?";
    script.text->push_back(s);
    res.text="No.";
    res.result=0;
    script.choices->push_back(res);
    res.text="Yes.";
    res.result=640 + index;
    script.choices->push_back(res);
    showText=true;
  } else {
    buyShieldB(index);
  }
}

void CDarkages::buyShieldB(int index){
  strcpy(hero.shield, shields[index].name);
  hero.sStr=shields[index].value;
  hero.gold-=shields[index].cost;
  script.clear();
  script.text->push_back("Enjoy your purchase!");
  showText=true;
}

void CDarkages::buyWeapon(int index){
  string s;
  da1response res;
  script.clear();
  if(hero.gold<weapons[index].cost){
    s = "You don't have enough gold!";
    script.text->push_back(s);
    showText=true;
  } else if(hero.wStr >= weapons[index].value){
    s = "You current weapon is as good or better than this one. Are you sure?";
    script.text->push_back(s);
    res.text="No.";
    res.result=0;
    script.choices->push_back(res);
    res.text="Yes.";
    res.result=600+index;
    script.choices->push_back(res);
    showText=true;
  } else {
    buyWeaponB(index);
  }
}

void CDarkages::buyWeaponB(int index){
  script.clear();
  if(strcmp(hero.weapon,weapons[10].name)==0){
    script.text->push_back("(You relinquish your Magic Sword to the merchant, but it flies from his hand and sails out of the city through the sky. Phenomenal! But what can you do? You hand the merchant his money and receive your new weapon.)");
  }
  strcpy(hero.weapon, weapons[index].name);
  hero.wStr=weapons[index].value;
  hero.gold-=weapons[index].cost;
  script.text->push_back("Enjoy your purchase!");
  showText=true;
}

void CDarkages::castSpell(int index){
  bool lowMP=false;
  char str[128];

  if(index==-2){ //set spell list
    spellList.clear();
    spellList.push_back(-1);
    if(hero.spells[0]) spellList.push_back(0);
    if(hero.spells[2]) spellList.push_back(2);
    if(hero.spells[5]) spellList.push_back(5);
    if(hero.spells[8]) spellList.push_back(8);
    if(hero.spells[10]) spellList.push_back(10);
    showSpell=true;
    selection=0;
    return;
  } else if(index==-1) { //cancel
    showSpell=false;
    return;
  } else { //cast spell
    script.clear();
    switch(index){
    case 0:
      if(hero.mp < 2) {
        lowMP=true;
        break;
      }
      hero.hp+=5;
      hero.mp-=2;
      if(hero.hp > hero.maxHP) hero.hp=hero.maxHP;
      sprintf(str, "You gain 5 HP. Health is now %d of %d HP.", hero.hp,hero.maxHP);
      script.addText(str);
      break;
    case 2:
      if(hero.mp < 7) {
        lowMP=true;
        break;
      }
      hero.hp+=30;
      hero.mp-=7;
      if(hero.hp > hero.maxHP) hero.hp=hero.maxHP;
      sprintf(str, "You gain 30 HP. Health is now %d of %d HP.", hero.hp, hero.maxHP);
      script.addText(str);
      break;
    case 5:
      if(hero.mp < 12) {
        lowMP=true;
        break;
      }
      hero.hp+=60;
      hero.mp-=12;
      if(hero.hp > hero.maxHP) hero.hp=hero.maxHP;
      sprintf(str, "You gain 60 HP. Health is now %d of %d HP.", hero.hp, hero.maxHP);
      script.addText(str);
      break;
    case 8:
      if(hero.mp < 20) {
        lowMP=true;
        break;
      }
      hero.hp=hero.maxHP;
      hero.mp-=20;
      sprintf(str, "You gain full health.");
      script.addText(str);
      break;
    case 10:
      if(hero.mp < 15) {
        lowMP=true;
        break;
      }
      //travel
      showTravel=true;
      showSpell=false;
      selection=0;
      return;
    default:
      script.addText("Bad spell function. Please report.");
      break;
    }
    if(lowMP){
      script.addText("You don't have enought MP!");
    }
    showText=true;
  }
  showSpell=false;
}

void CDarkages::changeMap(int key){
  switch(key){
  case 0: curMap=0; cam.setPos(7, 19); playerDir=0; eBattleNum=0; break; //aaryak by ferry
  case 30:
    if(cam.getTileY() >= 43 && cam.getTileY() <= 45 && cam.getTileX() >= 112 && cam.getTileX() <= 114){
      curMap=29; cam.setPos(8, 16); playerDir = 2; eBattleNum=0; //northern post
    } else if(cam.getTileY() >= 46 && cam.getTileY() <= 48 && cam.getTileX() >= 28 && cam.getTileX() <= 30){
      curMap=41; cam.setPos(21, 39); playerDir = 3; eBattleNum=0; //trok
    } else if(cam.getTileY() >= 104 && cam.getTileY() <= 106 && cam.getTileX() >= 115 && cam.getTileX() <= 117){
      if(eFirewand<8 || eFirewand>13){
        script.clear();
        script.addText("You near the stronghold only to realize it's loaded with dark goblins! You quickly decide not to go in.");
        showText=true;
      } else {
        script.clear();
        script.addText("You slip into the goblin stronghold after the sun sets. With luck, the only problem you will run into is the occasional guard patrol.");
        showText=true;
        curMap=5; cam.setPos(19, 34); playerDir=3; eBattleNum=6;
      }
    } else {
      curMap = 27; cam.setPos(24, 24); playerDir = 3; eBattleNum=0;  //meadow
    }
    break;
  case 31: curMap=0; cam.setPos(47, 19); playerDir=1; eBattleNum=0; break; //aaryak by land
  case 34: curMap = 34; cam.setPos(34, 17); playerDir=0; eBattleNum=0; break;
  case 41:
    if(cam.getTileY()==8) {
      if(eCave<2){
        curMap=6; eBattleNum=2; cam.setPos(66, 33); playerDir = 1;
      } else if(eCave==2){
        curMap=7; eBattleNum=2; cam.setPos(66, 33); playerDir = 1;
      } else if(eCave==3){
        curMap=6; eBattleNum=0; cam.setPos(66, 33); playerDir = 1;
      } else {
        curMap=8; eBattleNum=0; cam.setPos(66, 33); playerDir = 1;
      }
    } else if(cam.getTileY()==98){
      if(eFirewand < 19){
        curMap=22; eBattleNum=5; cam.setPos(8, 5); playerDir = 0;
      } else {
        curMap=43; eBattleNum=5; cam.setPos(8, 5); playerDir = 0;
      }
    } 
    break;
  case 50: curMap=31; eBattleNum=1; cam.setPos(46,100); playerDir=0; break;
  case 51: curMap = 21; cam.setPos(32, 21); playerDir = 0; eBattleNum=0; break;
  case 58: 
    if(cam.getTileY() >= 84 && cam.getTileY() <= 86 && cam.getTileX() >= 125 && cam.getTileX() <= 127){
      if(hero.spells[6]){ //wizard tower. Needs magiccount>6
        script.clear();
        script.addText("You have achieved the proper magical strength to pass the Tower of Ascent. You may enter the Floating City.");
        showText=true;
        if(eFirewand<4) {
          curMap = 17; cam.setPos(24,35); playerDir = 0; eBattleNum=0;
        } else {
          curMap = 18; cam.setPos(24,35); playerDir = 0; eBattleNum=0;
        }
      } else {
        script.clear();
        script.addText("Your magic skill is insufficient. Permission to climb the Tower of Ascent to the Floating City is denied. Return when you have attained greater magical knowledge.");
        showText=true;
      }
    } else {
      curMap = 32; cam.setPos(16, 23); playerDir = 3; eBattleNum=0;
    }
    break;
  case 59: 
    //DATA 12, 9,  13, 16, 15, 18, 16, 17, 22, 26, 23, 21, 25, 24, 23, 23, 19, 16, 12, 19, 10, 9,  15, 13, 18, 16, 21, 21, 13, 20, 24, 24, 26, 22
    //DATA 12, 10, 6,  7,  9,  6,  11, 9,  9,  9,  11, 11, 14, 16, 15, 13, 13, 13, 14, 17, 12, 19, 19, 19, 20, 21, 15, 18, 23, 20, 23, 18, 18, 22
    if(curMap == 2){
      if(cam.getTileX() == 8 && cam.getTileY() == 8){
        cam.setPos(11, 11); playerDir=0;
      } else if(cam.getTileX() == 14 && cam.getTileY() == 6){
        cam.setPos(12, 5); playerDir=0;
      } else if(cam.getTileX() == 18 && cam.getTileY() == 5){
        cam.setPos(14, 8); playerDir=0;
      } else if(cam.getTileX() == 17 && cam.getTileY() == 8){
        cam.setPos(15, 10); playerDir=0;
      } else if(cam.getTileX() == 15 && cam.getTileY() == 13){
        cam.setPos(18, 12); playerDir=0;
      } else if(cam.getTileX() == 19 && cam.getTileY() == 10){
        cam.setPos(22, 10); playerDir=0;
      } else if(cam.getTileX() == 25 && cam.getTileY() == 7){
        cam.setPos(21, 8); playerDir=0;
      } else if(cam.getTileX() == 22 && cam.getTileY() == 15){
        cam.setPos(24, 13); playerDir=0;
      } else if(cam.getTileX() == 21 && cam.getTileY() == 17){
        cam.setPos(20, 14); playerDir=0;
      } else if(cam.getTileX() == 22 && cam.getTileY() == 17){
        cam.setPos(23, 22); playerDir=0;
      } else if(cam.getTileX() == 22 && cam.getTileY() == 21){
        cam.setPos(25, 17); playerDir=0;
      } else if(cam.getTileX() == 21 && cam.getTileY() == 12){
        cam.setPos(22, 14); playerDir=0;
      } else if(cam.getTileX() == 17 && cam.getTileY() == 16){
        cam.setPos(11, 13); playerDir=0;
      } else if(cam.getTileX() == 14 && cam.getTileY() == 20){
        cam.setPos(17, 19); playerDir=0;
      } else if(cam.getTileX() == 19 && cam.getTileY() == 20){
        cam.setPos(12, 22); playerDir=0;
      } else if((cam.getTileX() == 12 && cam.getTileY() == 19) || (cam.getTileX() == 11 && cam.getTileY() == 18)){
        cam.setPos(14, 18); playerDir=0;
      } else if((cam.getTileX() == 8 && cam.getTileY() == 19) || (cam.getTileX() == 9 && cam.getTileY() == 18)){
        cam.setPos(9, 11); playerDir=0;
      } else if(cam.getTileX() == 25 && cam.getTileY() == 21){
        curMap=3; cam.setPos(8, 5); playerDir=0; eBattleNum=4;
      }
    } else if(curMap==3){
      curMap=4; cam.setPos(12, 11); playerDir=0; eBattleNum=0;
    } else if(curMap == 9){ //leaving dwarfcave
      curMap=31; cam.setPos(71, 110); playerDir=0; eBattleNum=1;
    } else if(curMap == 10) {
      curMap=16; cam.setPos(8, 5); playerDir=0; eBattleNum=0; //dwarfB to dwarftown
    } else if(curMap == 11){
      curMap=10; cam.setPos(35, 5); playerDir=0; eBattleNum=0;
    } else if(curMap==12){
      curMap=11; eBattleNum=4; cam.setPos(8, 5); playerDir=0;
    } else if(curMap == 13){
      curMap=13; eBattleNum=4; cam.setPos(15, 12); playerDir=0;
    } else if(curMap == 14){
      curMap=12; eBattleNum=4; cam.setPos(8, 12); playerDir=0;
    } else if(curMap==15){
      curMap=16; cam.setPos(8, 5); playerDir=0; eBattleNum=0;
    } else if(curMap == 16){ //leaving dwarftown
      curMap=9; eBattleNum=3; cam.setPos(8, 5); playerDir=0;
    } else if(curMap==22 || curMap==43){ //leaving laendlich dungeon
      curMap=31; eBattleNum=1; cam.setPos(15, 98); playerDir=2;
    } else if(curMap == 23){
      curMap=43; cam.setPos(25, 22); playerDir=0; eBattleNum=5;
    } else if(curMap == 24){
      if(cam.getTileY()==14){
        curMap=23; cam.setPos(17, 13); playerDir=0; eBattleNum=5;
      } else {
        curMap=23; cam.setPos(8, 5); playerDir=0; eBattleNum=5;
      }
    } else if(curMap == 25){
      if(cam.getTileY() == 11){
        curMap=24; cam.setPos(12, 18); playerDir=0; eBattleNum=5;
      } else if(cam.getTileY() == 14){
        curMap=24; cam.setPos(17, 13); playerDir=0; eBattleNum=5;
      } else if(cam.getTileY() == 21){
        curMap=24; cam.setPos(25, 22); playerDir=0; eBattleNum=5;
      }
    } else if(curMap == 26){
      curMap=25; cam.setPos(8, 22); playerDir=0; eBattleNum=5;
    } else if(curMap==35){
      curMap=31; eBattleNum=1; cam.setPos(117,71); playerDir=0;
    } else if(curMap == 36){
      curMap=35; eBattleNum=7; cam.setPos(8, 22); playerDir=0;
    } else if(curMap == 37){
      curMap=36; eBattleNum=7; cam.setPos(14, 11); playerDir=0;
    } else {
      curMap = 33; cam.setPos(16, 5); playerDir=0; eBattleNum=0; //up Qtower
    }
    break;
  case 61:
    //DATA 12, 9,  13, 16, 15, 18, 16, 17, 22, 26, 23, 21, 25, 24, 23, 23, 19, 16, 12, 19, 10, 9,  15, 13, 18, 16, 21, 21, 13, 20, 24, 24, 26, 22
    //DATA 12, 10, 6,  7,  9,  6,  11, 9,  9,  9,  11, 11, 14, 16, 15, 13, 13, 13, 14, 17, 12, 19, 19, 19, 20, 21, 15, 18, 23, 20, 23, 18, 18, 22
    if(curMap == 2){
      if(cam.getTileX() == 11 && cam.getTileY() == 7){
        curMap = 31; cam.setPos(87, 99); playerDir=0; eBattleNum=1;
      } else if(cam.getTileX() == 11 && cam.getTileY() == 10){
        cam.setPos(8, 9); playerDir=0;
      } else if(cam.getTileX() == 8 && cam.getTileY() == 11){
        cam.setPos(8, 18); playerDir=0;
      } else if(cam.getTileX() == 12 && cam.getTileY() == 13){
        cam.setPos(18, 16); playerDir=0;
      } else if(cam.getTileX() == 14 && cam.getTileY() == 10){
        cam.setPos(16, 8); playerDir=0;
      } else if(cam.getTileX() == 13 && cam.getTileY() == 8){
        cam.setPos(17, 5); playerDir=0;
      } else if(cam.getTileX() == 11 && cam.getTileY() == 5){
        cam.setPos(15, 6); playerDir=0;
      } else if(cam.getTileX() == 19 && cam.getTileY() == 12){
        cam.setPos(15, 12); playerDir=0;
      } else if(cam.getTileX() == 21 && cam.getTileY() == 7){
        cam.setPos(25, 8); playerDir=0;
      } else if(cam.getTileX() == 19 && cam.getTileY() == 14){
        cam.setPos(20, 17); playerDir=0;
      } else if(cam.getTileX() == 23 && cam.getTileY() == 10){
        cam.setPos(20, 10); playerDir=0;
      } else if(cam.getTileX() == 24 && cam.getTileY() == 12){
        cam.setPos(23, 15); playerDir=0;
      } else if(cam.getTileX() == 22 && cam.getTileY() == 15){
        cam.setPos(22, 12); playerDir=0;
      } else if(cam.getTileX() == 17 && cam.getTileY() == 18){
        cam.setPos(15, 20); playerDir=0;
      } else if(cam.getTileX() == 15 && cam.getTileY() == 18){
        cam.setPos(12, 18); playerDir=0;
      } else if((cam.getTileX() == 12 && cam.getTileY() == 21) || (cam.getTileX() == 11 && cam.getTileY() == 22)){
        cam.setPos(19, 19); playerDir=0;
      } else if(cam.getTileX() == 25 && cam.getTileY() == 18){
        cam.setPos(21, 21); playerDir=0;
      } else if(cam.getTileX() == 23 && cam.getTileY() == 21){
        cam.setPos(23, 17); playerDir=0;
      }
    } else if(curMap==3){
      curMap=2; cam.setPos(25, 22); playerDir = 0; eBattleNum=4;
    } else if(curMap == 4){
      curMap=3; cam.setPos(18, 11); playerDir = 0; eBattleNum=4;
    } else if(curMap == 9){ //heading down into dwarf city
      curMap=16; eBattleNum=0; cam.setPos(33, 32); playerDir = 0;
    } else if(curMap == 10) { //heading down from dwarf2
      curMap=11; eBattleNum=4; cam.setPos(8, 5); playerDir=0;
    } else if(curMap == 11) { //down from dwarfA
      if(cam.getTileY() >= 4 && cam.getTileY() <= 6){
        curMap=12; eBattleNum=4; cam.setPos(8, 12); playerDir=0;
      } else if(cam.getTileY() >= 9 && cam.getTileY() <= 11){
        curMap=13; eBattleNum=4; cam.setPos(15, 12); playerDir=0;
      } else {
        curMap=11; eBattleNum=4; cam.setPos(8, 5); playerDir=0;
      }
    } else if(curMap==12){
      if(cam.getTileX() == 8){
        curMap=11; eBattleNum=4; cam.setPos(8, 5); playerDir=0;
      } else if(cam.getTileX() == 13){ 
        curMap=13; eBattleNum=4; cam.setPos(15, 12); playerDir=0;
      } else { 
        curMap=12; eBattleNum=4; cam.setPos(8, 12); playerDir=0;
      }
    } else if(curMap == 13){
      if(cam.getTileY() == 5){
        curMap=14; eBattleNum=4; cam.setPos(15, 5); playerDir=0;
      } else if(cam.getTileY() == 7){
        curMap=11; eBattleNum=4; cam.setPos(8, 5); playerDir=0;
      } else {
        curMap=12; eBattleNum=4; cam.setPos(8, 12); playerDir=0;
      }
    } else if(curMap == 14){
      if(cam.getTileX() == 8){
        curMap=11; eBattleNum=4; cam.setPos(8, 5); playerDir=0;
      } else if(cam.getTileX() == 10){
        curMap=11; eBattleNum=4; cam.setPos(8, 5); playerDir=0;
      } else { //12
        curMap=15; eBattleNum=4; cam.setPos(8, 9); playerDir=0;
      }
    } else if(curMap == 16){ //heading down from dwarf city
      if(eHelpDwarf > 1){
        script.clear();
        script.addText("You don't see any reason to go back down there.");
        showText=true;
      } else {
        curMap=10; eBattleNum=4; cam.setPos(8, 5); playerDir = 0;
      }
    } else if(curMap == 23){
      curMap=24; cam.setPos(8, 5); playerDir=0; eBattleNum=5;
    } else if(curMap == 24){
      if(cam.getTileY() == 14){
        curMap=25; cam.setPos(17, 13); playerDir=0; eBattleNum=5;
      } else if(cam.getTileY() == 17){
        curMap=25; cam.setPos(8, 11); playerDir=0; eBattleNum=5;
      } else if(cam.getTileY() == 21){
        curMap=25; cam.setPos(25, 22); playerDir=0; eBattleNum=5;
      }
    } else if(curMap == 25){
      curMap=26; cam.setPos(15, 9); playerDir=0; eBattleNum=5;
    } else if(curMap==33){
      curMap=32; cam.setPos(16, 5); playerDir=0; eBattleNum=0;
    } else if(curMap == 35){
      curMap=36; cam.setPos(8,17); playerDir=0; eBattleNum=7;
    } else if(curMap == 36){
      script.clear();
      script.addText("Before you is a dark alter. Eternal flames surround it. There is a slight depression in the center and you notice that it fits the medallion.");
      script.addText("This is it, you decide. Once you place that medallion, there is no going back.");
      showText=true;
      curMap=37; cam.setPos(11, 5); playerDir=0; eBattleNum=0;
    } else if(curMap==43){
      curMap=23; cam.setPos(25, 22); playerDir=0; eBattleNum=5;
    }
    break;
  case 80: curMap=31; cam.setPos(24, 8); playerDir=1; eBattleNum=1; break;
  case 81: 
    if(cam.getTileY() == 71){ //Rift cave
      if(eGreyor == 1){
        script.clear();
        script.addText("You heave a heavy sigh and stare into the black abyss that is the Rift Cave. It will all be over soon, you tell yourself. You rest a moment more then enter the murky darkness.");
        showText=true;
        curMap=35; eBattleNum=7; cam.setPos(16, 5); playerDir=0;
      } else {
        script.clear();
        script.addText("You have reached the Rift Cave, but you see no reason to enter.");
        showText=true;
      }
    } else if(cam.getTileY() == 99){
      if(eHelpDwarf > 2){
        curMap=2; cam.setPos(10, 7); eBattleNum=4; playerDir=0;
      } else {
        script.clear();
        script.addText("You see no reason to enter.");
        showText=true;
      }
    } else if(cam.getTileY() == 110) {
      if(eHorn<5){
        script.clear();
        script.addText("From a distance, you see a strange cave. But when you get closer, it mysteriously vanishes.");
        showText=true;
      } else if(eHorn==5){
        script.clear();
        script.addText("You see the Dwarven Caves in the distance. When you get close, they seem to disappear. You pull Bonebreaker's horn from your pack and belt a long, low note. The entrance appears!");
        showText=true;
        curMap=9; eBattleNum=3; cam.setPos(35,32); playerDir=0;
      }
    } else {
      curMap=8; eBattleNum=0; cam.setPos(8, 57); playerDir=2;
    }
    break;
  case 87:
    if(curMap == 0){ //leaving aaryak
      curMap=31; eBattleNum=1; cam.setPos(102, 100); playerDir=0;
    } else if(curMap==1) { //leaving amber
      curMap=31; eBattleNum=1; cam.setPos(94, 102); playerDir=0;
    } else if(curMap == 6 || curMap == 7){
      curMap=31; eBattleNum=1; cam.setPos(27, 8); playerDir=2;
    } else if(curMap==19){ //leaving garrison
      curMap=31; eBattleNum=1; cam.setPos(81, 87); playerDir=0;
    } else if (curMap==21){ //leaving laendlich
      curMap=31; eBattleNum=1;  cam.setPos(34, 100); playerDir=0;
    } else if(curMap == 27){
      if(eBridge == 3)curMap = 31;
      else curMap=30;
      eBattleNum=1; cam.setPos(42, 10);  playerDir = 0;
    } else if(curMap==29){ //leaving northern post
      curMap=31; eBattleNum=1; cam.setPos(113, 45); playerDir=0;
    } else if(curMap == 32){
      curMap=31; eBattleNum=1; cam.setPos(31, 7); playerDir=0;
    } else if(curMap==34){ //leaving rhoeyce
      curMap=31; eBattleNum=1; cam.setPos(93, 108); playerDir=0;
    } else if(curMap==38){ //leaving temple
      curMap=31; eBattleNum=1; cam.setPos(70, 56); playerDir=0;
    } else if(curMap==39 || curMap==40){ //leaving tristen
      curMap=31; eBattleNum=1; cam.setPos(42, 82); playerDir=0;
    } else if(curMap==41){ //leaving trok
      curMap=31; eBattleNum=1; cam.setPos(29, 48); playerDir=0;
    } else if(curMap==42){ //leaving wisp
      curMap=31; eBattleNum=1; cam.setPos(16, 13); playerDir=0;
    }
    break;
  case 89: 
    if(cam.getTileY() >= 106 && cam.getTileY() <= 108 && cam.getTileX() >= 92 && cam.getTileX() <= 94){
      curMap=34; cam.setPos(21, 28); playerDir = 2; eBattleNum=0; //rhoeyce
    } else {
      curMap = 21; cam.setPos(8, 22); playerDir = 2; eBattleNum=0; //laendlich
    }
    break;
  case 91: curMap=38; cam.setPos(13, 15); playerDir=3; eBattleNum=0; break;
  case 92: 
    if(cam.getTileY() >= 100 && cam.getTileY() <= 102 && cam.getTileX() >= 93 && cam.getTileX() <= 95){
      curMap=1; cam.setPos(17,29); playerDir=3; eBattleNum=0; //amber
    } else {
      curMap=42; cam.setPos(29, 33); playerDir=3; eBattleNum=0; //wisp
    }
    break;
  case 93: curMap=19; cam.setPos(27, 45); playerDir=3; eBattleNum=0; break;
  case 95: 
    //curMap=0; cam.setPos(47, 20); playerDir=1; eBattleNum=0; break;
    script.clear();
    script.addText("A short man steps out of the hut. His breath reeks of fish as he politely grunts, `Going to Laendlich? 50 gold pieces.'");
    script.addChoice("No", 0);
    script.addChoice("Yes", 221);
    showText=true;
    break;
  case 96:
    cam.setPos(23, 5); playerDir=0; eBattleNum=0;
    if(eFirewand < 4) curMap=39;
    else curMap=40;
    break;
  case 129:
    if(curMap==17 || curMap==18) { //loaving floating city
      curMap=31; eBattleNum=1; cam.setPos(126,86); playerDir=0;
    }
  default: break;
  }
  //always reset battle counter when changing maps
  eBattleCheck=0;
}

int CDarkages::checkAction(int map, int x, int y){
  int i, j,k;

  //special case magic sword
  if(map==31 && x==12 && y==46 && eFirewand>15){
    if(strcmp(hero.weapon,weapons[10].name)==0){
      return 130;
    } else {
      return 131;
    }
  }

  for (j = y - 1; j <= y + 1; j++){
    for (i = x - 1; i <= x + 1; i++){
      if (j != y && i != x) continue;  //do not check corners
      if (i == x && j == y) continue;  //do not check center
      k=world[map].getTile(i, j);
      if(actionTile[k] > 0) return actionTile[k];
    }
  }
  return 0; //nothing found
}

int CDarkages::checkBattle(){
  if(eBattleNum == 0) return -1;

  eBattleCheck++;
  if(eBattleCheck < 4) return -1;

  int chance = rand() % 100;
  if(eBattleNum==6) {
    if(chance > 2) return -1; //Dark goblin fortress has lower battle frequency
  } else {
    if(chance > 5) return -1;
  }

  eBattleCheck=0; //reset counter because battle is going to happen
  cam.clearMovement(); //reset key buffer for camera

  //special case battles

  //dungeon battles
  if(eBattleNum == 2) {
    return 4; //spiders
  } else if(eBattleNum==3){
    chance = rand() % 100;
    if(chance < 11) return 4;
    else if(chance < 66) return 6;
    else return 7;
  } else if(eBattleNum==4){
    chance = rand() % 100;
    if(chance < 51) return 7;
    else return 8;
  } else if(eBattleNum == 5){
    chance = rand() % 100;
    if(chance < 11) return 10;
    else if(chance < 76) return 8;
    else return 9;
  } else if(eBattleNum==6){
    return 11; //dark goblins
  } else if(eBattleNum == 7){ //Rift cave
    chance = rand() % 100;
    if(chance < 31) return 13;
    else if(chance < 71) return 14;
    else if(chance < 91) return 15;
    else return 16;
  }

  //wilderness battles
  chance = rand() % 100;
  switch((hero.level+1)/2){
  case 1:
    return 0;
  case 2:
    if(chance < 51) return 0;
    return 1;
  case 3:
    if(chance < 31) return 0;
    else if(chance > 69) return 1;
    return 2;
  case 4:
    if(chance < 21) return 0;
    else if(chance < 41) return 1;
    else if(chance < 66) return 2;
    return 3;
  case 5:
    if(chance < 11) return 0;
    else if(chance < 21) return 1;
    else if(chance < 41) return 2;
    else if(chance < 66) return 3;
    return 4;
  case 6:
    if(chance < 11) return 0;
    else if(chance < 21) return 1;
    else if(chance < 31) return 2;
    else if(chance < 56) return 3;
    else if(chance < 76) return 4;
    return 5;
  case 7:
    if(chance < 8) return 0;
    else if(chance < 16) return 1;
    else if(chance < 26) return 2;
    else if(chance < 41) return 3;
    else if(chance < 56) return 4;
    else if(chance < 76) return 5;
    return 6;
  case 8:
    if(chance < 6) return 1;
    else if(chance < 16) return 2;
    else if(chance < 26) return 3;
    else if(chance < 36) return 4;
    else if(chance < 51) return 5;
    else if(chance < 71) return 6;
    return 7;
  case 9:
    if(chance < 11) return 2;
    else if(chance < 21) return 3;
    else if(chance < 31) return 4;
    else if(chance < 41) return 5;
    else if(chance < 56) return 6;
    else if(chance < 76) return 7;
    return 8;
  case 10:
    if(chance < 8) return 3;
    else if(chance < 16) return 4;
    else if(chance < 26) return 5;
    else if(chance < 36) return 6;
    else if(chance < 56) return 7;
    else if(chance < 76) return 8;
    return 9;
  case 11:
    if(chance < 6) return 4;
    else if(chance < 16) return 5;
    else if(chance < 21) return 6;
    else if(chance < 36) return 7;
    else if(chance < 51) return 8;
    else if(chance < 76) return 9;
    return 10;
  case 12:
    if(chance < 6) return 5;
    else if(chance < 16) return 6;
    else if(chance < 21) return 7;
    else if(chance < 36) return 8;
    else if(chance < 51) return 9;
    else if(chance < 76) return 10;
    return 11;
  case 13:
    if(chance < 6) return 6;
    else if(chance < 16) return 7;
    else if(chance < 21) return 8;
    else if(chance < 36) return 9;
    else if(chance < 51) return 10;
    else if(chance < 76) return 11;
    return 12;
  case 14:
    if(chance < 6) return 7;
    else if(chance < 16) return 8;
    else if(chance < 21) return 9;
    else if(chance < 36) return 10;
    else if(chance < 51) return 11;
    else if(chance < 76) return 12;
    return 13;
  case 15:
    if(chance < 6) return 8;
    else if(chance < 16) return 9;
    else if(chance < 21) return 10;
    else if(chance < 36) return 11;
    else if(chance < 51) return 12;
    else if(chance < 76) return 13;
    return 14;
  case 16:
    if(chance < 6) return 9;
    else if(chance < 16) return 10;
    else if(chance < 21) return 11;
    else if(chance < 36) return 12;
    else if(chance < 51) return 13;
    else if(chance < 76) return 14;
    return 15;
  case 17:
    if(chance < 6) return 10;
    else if(chance < 16) return 11;
    else if(chance < 21) return 12;
    else if(chance < 36) return 13;
    else if(chance < 51) return 14;
    else if(chance < 76) return 15;
    return 16;
  case 18:
    if(chance < 6) return 11;
    else if(chance < 16) return 12;
    else if(chance < 21) return 13;
    else if(chance < 36) return 14;
    else if(chance < 51) return 15;
    else if(chance < 76) return 16;
    return 17;
  default:
    if(chance < 11) return 12;
    else if(chance < 16) return 13;
    else if(chance < 31) return 14;
    else if(chance < 51) return 15;
    else if(chance < 76) return 16;
    return 17;
  }

  return -1; //shouldn't ever get here.
}

int CDarkages::checkTile(int map, int x, int y){
  switch (world[map].getTile(x, y)){
  case 0: return -1;
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
  case 7:
  case 8:
  case 9:
  case 10:
  case 11: return -1;
  case 18: return -1;
  case 29: return -1;
  case 30: return 30;
  case 31: return 31;
  case 32: return -1;
  case 33: return -1;
  case 34: return -1;
  case 35: return -1;
  case 36: return -1;
  case 37: return -1;
  case 38: return -1;
  case 39: return -1;
  case 41: return 41;
  case 42: return -1;
  case 44: return -1;
  case 45: return -1;
  case 46: return -1;
  case 47: return -1;
  case 48: return -1;
  case 49: return -1;
  case 50: return -1;
  case 51: return -1;
  case 52: return -1;
  case 53: return -1;
  case 54: return -1;
  case 55: return -1;
  case 57: return -1;
  case 58: return 58;
  case 59: return 59;
  case 60: return 60;
  case 61: return 61;
  case 62: return -1;
  case 63: return -1;
  case 64: return -1;
  case 66: return -1;
  case 67: return -1;
  case 68: return -1;
  case 69: return -1;
  case 70: return -1;
  case 71: return -1;
  case 72: return -1;
  case 73: return -1;
  case 74: return -1;
  case 75: return -1;
  case 76: return -1;
  case 77: return -1;
  case 78: return -1;
  case 79: return -1;
  case 80: return 80;
  case 81: return 81;
  case 87: return 87;
  case 89: return 89;
  case 90: return -1;
  case 91: return 91;
  case 92: return 92;
  case 93: return 93;
  case 95: return 95;
  case 96: return 96;
  case 97: return -1;
  case 98: return -1;
  case 99: return -1;
  case 100: return -1;
  case 101: return -1;
  case 102: return -1;
  case 103: return -1;
  case 104: return -1;
  case 105: return -1;
  case 106: return -1;
  case 107: return -1;
  case 108: return -1;
  case 109: return -1;
  case 110: return -1;
  case 111: return 111;
  case 112: return -1;
  case 113: return -1;
  case 114: return -1;
  case 115: return -1;
  case 116: return -1;
  case 117: return -1;
  case 118: return -1;
  case 119: return -1;
  case 120: return -1;
  case 121: return -1;
  case 122: return -1;
  case 123: return -1;
  case 124: return -1;
  case 125: return -1;
  case 126: return -1;
  case 129: return 129;
  default: return 0;
  }
  return -1;
}

void CDarkages::credits(){
  script.clear();
  script.addText("   Dark Ages: The Continents");
  script.addText(" ");
  script.addText("Version 2.0 by Michael Hoopmann");
  script.addText("      Copyright 2017, DKS");
  script.addText("    http://dk-software.com");
  script.addText(" ");
  script.addText("Made with SDL and SFML");
  script.addText("Open source on GitHub");
  script.addText(" ");
  script.addText(" ");
  script.addText(" ");
  script.addText("Original QuickBasic Version 1.0");
  script.addText("Copyright 1997, Michael Hoopmann");
  script.addText(" ");
  script.addText("Created by:         Mike Hoopmann");
  script.addText(" ");
  script.addText("Story:              Mike Hoopmann");
  script.addText(" ");
  script.addText("Coding:             Mike Hoopmann");
  script.addText("  with help from:   Enigma Stargate");
  script.addText("                    Milo Sedlacek");
  script.addText("                       aka Gradius");
  script.addText("                    Tsugumo");
  script.addText(" ");
  script.addText("Graphics:           Mike Hoopmann");
  script.addText("                    Enigma Stargate");
  script.addText(" ");
  script.addText("Endgame Scenes:     Mike Hoopmann");
  script.addText("                    Enigma Stargate");
  script.addText(" ");
  script.addText("Midi:               Mike Hoopmann");
  script.addText(" ");
  script.addText("Midi Code:          Jesse Dorland");
  script.addText(" ");
  script.addText("Special thanks to:  The Almighty Kiwi");
  script.addText("                    Enigma Stargate");
  script.addText("                    HydroXide");
  script.addText("                    SiliconMeltdown");
  script.addText("                    Milo Sedlacek");
  script.addText(" ");
  script.addText("Beta Testers:       The Almighty Kiwi");
  script.addText("                    Enigma Stargate");
  script.addText("                    Anthracks");
  script.addText("                    Nekrophidius");
  script.addText("                    Tarak");
  script.addText("                    RATM");
  script.addText("                    HydroXide");
  script.addText("                    Navigator");
  script.addText("                    HoMeLeSS");
  script.addText("                    Olly");
  script.addText(" ");
  script.addText(" ");
  script.addText(" ");
  script.addText("Thank you for playing!");
  showCredits=true;
  selection=0;
}

void CDarkages::death(){
  script.clear();
  script.addText("Your vision turns red...then fades to black. You barely feel the ground as your body slams into it. The last thing you think of before death takes you is...");
  script.addText("I hope I saved my game...");
  script.addText("Load a saved game?");
  script.addChoice("Yes, soldier on!",700);
  script.addChoice("No, exit the game.", 701);
  showText=true;
}

void CDarkages::init(){ 
  font.setDisplay(display);
  font.loadFont("Font/DA1qb.ttf");
  font.setFontSize(32);
  gfx.loadGfx(display->renderer);
  world.loadMaps();
  cam.setPos(40, 12);
  cam.setMax(world[curMap].szX, world[curMap].szY);
  battle.init(display, &font, &gfx, &hero);
  //SDL_RenderSetIntegerScale(display->renderer, SDL_TRUE);
  SDL_RenderSetLogicalSize(display->renderer, 640, 400);

  //load save games (if any)
  FILE* f=fopen("Saves/master.sav","rb");
  if(f==NULL){ //no saves, so set to defaults
    saves[0].day=0;
    saves[0].gold=0;
    saves[0].level=0;
    saves[0].month=0;
    strcpy(saves[0].name, "<EMPTY>");
    saves[0].year=0;
    saves[1]=saves[0];
    saves[2]=saves[0];
    saves[3]=saves[0];
    saves[4]=saves[0];
    saves[5]=saves[0];
  } else { //load save list
    fread(&saves, sizeof(da1save), 6, f);
    fclose(f);
  }

  //spell names
  strcpy(spells[0].name, "Heal"); spells[0].cost=50; spells[0].value=2;
  strcpy(spells[1].name, "Burn"); spells[1].cost=150; spells[1].value=5;
  strcpy(spells[2].name, "Great Heal"); spells[2].cost=350; spells[2].value=7;
  strcpy(spells[3].name, "Blindness"); spells[3].cost=500; spells[3].value=5;
  strcpy(spells[4].name, "Firebomb"); spells[4].cost=750; spells[4].value=10;
  strcpy(spells[5].name, "Massive Heal"); spells[5].cost=1000; spells[5].value=12;
  strcpy(spells[6].name, "Acid Rain"); spells[6].cost=1500; spells[6].value=18;
  strcpy(spells[7].name, "Melt Flesh"); spells[7].cost=2000; spells[7].value=25;
  strcpy(spells[8].name, "Full Heal"); spells[8].cost=3000; spells[8].value=20;
  strcpy(spells[9].name, "Death"); spells[9].cost=3500; spells[9].value=30;
  strcpy(spells[10].name, "Travel"); spells[10].cost=4000; spells[10].value=15;

  //weapons
  strcpy(weapons[0].name, "Dagger"); weapons[0].cost=10; weapons[0].value=1;
  strcpy(weapons[1].name, "Club"); weapons[1].cost=25; weapons[1].value=2;
  strcpy(weapons[2].name, "Mace"); weapons[2].cost=40; weapons[2].value=3;
  strcpy(weapons[3].name, "Spear"); weapons[3].cost=60; weapons[3].value=5;
  strcpy(weapons[4].name, "Axe"); weapons[4].cost=300; weapons[4].value=6;
  strcpy(weapons[5].name, "Shortsword"); weapons[5].cost=800; weapons[5].value=8;
  strcpy(weapons[6].name, "Great Axe"); weapons[6].cost=1500; weapons[6].value=10;
  strcpy(weapons[7].name, "Longsword"); weapons[7].cost=2500; weapons[7].value=12;
  strcpy(weapons[8].name, "Halberd"); weapons[8].cost=3500; weapons[8].value=15;
  strcpy(weapons[9].name, "Magic Axe"); weapons[9].cost=5000; weapons[9].value=17;
  strcpy(weapons[10].name, "Magic Sword"); weapons[10].cost=0; weapons[10].value=20;

  //armors
  strcpy(armors[0].name, "Tunic"); armors[0].cost=25; armors[0].value=1;
  strcpy(armors[1].name, "Leather Vest"); armors[1].cost=100; armors[1].value=2;
  strcpy(armors[2].name, "Studded Leather"); armors[2].cost=250; armors[2].value=3;
  strcpy(armors[3].name, "Ring Mail"); armors[3].cost=450; armors[3].value=4;
  strcpy(armors[4].name, "Scale Mail"); armors[4].cost=1000; armors[4].value=5;
  strcpy(armors[5].name, "Chain Mail"); armors[5].cost=2200; armors[5].value=6;
  strcpy(armors[6].name, "Partial Plate Mail"); armors[6].cost=4000; armors[6].value=7;
  strcpy(armors[7].name, "Full Plate Mail"); armors[7].cost=6000; armors[7].value=8;
  strcpy(armors[8].name, "Magic Mail"); armors[8].cost=9000; armors[8].value=10;

  //helms
  strcpy(helms[0].name, "Leather Cap"); helms[0].cost=10; helms[0].value=1;
  strcpy(helms[1].name, "Chain Coif"); helms[1].cost=75; helms[1].value=2;
  strcpy(helms[2].name, "Plate Helm"); helms[2].cost=250; helms[2].value=3;
  strcpy(helms[3].name, "Magic Helm"); helms[3].cost=500; helms[3].value=5;

  //shields
  strcpy(shields[0].name, "Buckler"); shields[0].cost=15; shields[0].value=1;
  strcpy(shields[1].name, "Small Shield"); shields[1].cost=100; shields[1].value=2;
  strcpy(shields[2].name, "Large Shield"); shields[2].cost=500; shields[2].value=3;
  strcpy(shields[3].name, "Magic Shield"); shields[3].cost=2000; shields[3].value=5;

  //set event variables
  eAle=0;
  eBattleCheck=0;
  eBattleNum=0;
  eBridge=0;
  eCave=0;
  eEmma=0;
  eEndGame=0;
  eFirewand=0;
  eGreyor=0;
  eHelpDwarf=0;
  eHorn=0;

  //set tile actions
  int i;
  for(i=0;i<150;i++) actionTile[i]=0;
  actionTile[42]=42;
  for(i=45; i < 56; i++) actionTile[i]=i;
  actionTile[60]=60;
  for(i=62; i < 65; i++) actionTile[i]=i;
  for(i=66; i < 80; i++) actionTile[i]=i;
  actionTile[98]=98;
  for(i=101; i < 126; i++) actionTile[i]=i;
}

void CDarkages::loadGame(int index){
  FILE* f;
  char str[32];
  int version;
  int x, y;

  if(saves[index].level == 0) return;

  sprintf(str, "Saves/sv%d.da1", index);
  f=fopen(str, "rb");
  fread(&version, sizeof(int), 1, f);
  fread(&curMap, sizeof(int), 1, f);
  fread(&x, sizeof(int), 1, f);
  fread(&y, sizeof(int), 1, f);
  fread(&eAle, sizeof(int), 1, f);
  fread(&eBattleCheck, sizeof(int), 1, f);
  fread(&eBattleNum, sizeof(int), 1, f);
  fread(&eBridge, sizeof(int), 1, f);
  fread(&eCave, sizeof(int), 1, f);
  fread(&eEmma, sizeof(int), 1, f);
  fread(&eFirewand, sizeof(int), 1, f);
  fread(&eGreyor, sizeof(int), 1, f);
  fread(&eHelpDwarf, sizeof(int), 1, f);
  fread(&eHorn, sizeof(int), 1, f);
  fread(&hero, sizeof(CPlayer), 1, f);
  fclose(f);

  cam.setPos(x,y);
  playerDir = 0;
}

bool CDarkages::newGame(){
  userText.clear();
  SDL_StartTextInput();
  showTextInput=true;
  showText=true;

  script.clear();
  script.addText("Name your hero:");

  SDL_Event e;

  while(true){

    //printf("%d %d\n", world[30].szX, world[30].szY);
    while(SDL_PollEvent(&e) != 0) {
      if(e.type == SDL_TEXTINPUT){
        if(showTextInput && userText.size()<16) userText+=e.text.text;
      } else if(e.type == SDL_KEYUP){
        switch(e.key.keysym.sym)  {
        case SDLK_UP: cam.keyUp(0); break;
        case SDLK_DOWN: cam.keyUp(2); break;
        case SDLK_LEFT: cam.keyUp(3); break;
        case SDLK_RIGHT: cam.keyUp(1); break;
        default: break;
        }
      } else if(e.type == SDL_KEYDOWN)  {
        //Select surfaces based on key press
        switch(e.key.keysym.sym)  {
        case SDLK_ESCAPE:
          return false;
          break;
        case SDLK_BACKSPACE:
          if(showTextInput && userText.size()>0) userText.pop_back();
          break;
        case SDLK_RETURN:
          if(showText) {
            if(showTextInput){
              if(e.key.keysym.sym == SDLK_RETURN){
                strcpy(hero.name,&userText[0]);
                SDL_StopTextInput();
                showTextInput=false;
                showText=false;
                return true;
              }
            }
          }
          break;
        default: 
          break;
        }
      }
    }

    renderNew();

  }
  return false;

}

void CDarkages::run(){
  SDL_Event e;
  int i;
  int chMap = -1;
  bool anim = false;
  bool moving = false;
  stop = false;

  unsigned int ticks =0;
  unsigned int curTicks;
  unsigned int lastTicks=SDL_GetTicks();
  unsigned int lastFPS=SDL_GetTicks();

  while (true){

    curTicks=SDL_GetTicks();
    ticks+=curTicks - lastTicks;
    lastFPS+=curTicks - lastTicks;
    lastTicks=curTicks;
    if(lastFPS>1000){
      fps=renderCount;
      renderCount=0;
      lastFPS-=1000;
    }
    if(ticks<6) {
      if(showCredits) renderCredits();
      else render();
      renderCount++;
      continue;
    }
    
    if(fadeIn>0){
      if(ticks < 40) continue;
      ticks-=40;
      render();
      fadeIn-=5;
      continue;
    }
    ticks-=6;

    if(showCredits) {
      selection++;
      if(renderCredits()){
        selection=0;
        showCredits=false;
        return;
      }
      continue;
    }

    //if(cam.scrollCamera()){
    //  SDL_FlushEvents(SDL_USEREVENT, SDL_LASTEVENT);
    //  render();
    //  continue;
    //}

    //printf("%d %d\n", world[30].szX, world[30].szY);
    while (SDL_PollEvent(&e) != 0) {
      if(e.type == SDL_TEXTINPUT){
        if(showTextInput && userText.size() < 16 && e.text.text[0] != ' ') userText+=e.text.text;
      } else if(e.type==SDL_JOYAXISMOTION){
        cout << "joystick" << endl;
        if(e.jaxis.axis == 0) {
          //Left of dead zone
          if(e.jaxis.value < -12000){
            cam.keyDown(3);
            cam.keyUp(1);
          } else if(e.jaxis.value > 12000){
            cam.keyDown(1);
            cam.keyUp(3);
          } else {
            cam.keyUp(1);
            cam.keyUp(3);
          }
        } else if(e.jaxis.axis == 1){
          if(e.jaxis.value < -12000){
            cam.keyDown(0);
            cam.keyUp(2);
          } else if(e.jaxis.value > 12000){
            cam.keyDown(2);
            cam.keyUp(0);
          } else {
            cam.keyUp(0);
            cam.keyUp(2);
          }
        }
          
      } else if(e.type == SDL_KEYUP){
        switch(e.key.keysym.sym)  {
        case SDLK_UP: cam.keyUp(0); break;
        case SDLK_DOWN: cam.keyUp(2); break;
        case SDLK_LEFT: cam.keyUp(3); break;
        case SDLK_RIGHT: cam.keyUp(1); break;
        default: break;
        }
      }else if(e.type == SDL_KEYDOWN)  {
        //Select surfaces based on key press
        switch (e.key.keysym.sym)  {
        case SDLK_UP: 
          if(showLoad){
            if(selection == 0) selection=6;
            else selection--;
            break;
          }
          if(showMenu){
            if(selection == 0) selection=4;
            else selection--;
            break;
          }
          if(showSave){
            if(selection == 0) selection=5;
            else selection--;
            break;
          }
          if(showSpell){
            if(selection == 0) selection=spellList.size() - 1;
            else selection--;
            break;
          }
          if(showTravel){
            if(selection == 0) selection=12;
            else selection--;
            break;
          }
          if(showStats) break;
          if(showText) {
            if(script.selection > 0) script.selection--;
            if(script.selection < script.offset) script.offset--;
            break;
          }
          //if(battle.fight(checkBattle())==3) death();
          //i = checkTile(curMap, cam.getTileX(), cam.getTileY() - 1);
          //playerDir = 3; 
          //anim = true;
          //if (i == 0) {
            cam.keyDown(0);
            //cam.bumpCamera(0, -20);
          //} else if (i > 0){
          //  chMap = i;
          //}
          break;
        case SDLK_DOWN: 
          if(showLoad){
            if(selection == 6) selection=0;
            else selection++;
            break;
          }
          if(showMenu){
            if(selection == 4) selection=0;
            else selection++;
            break;
          }
          if(showSave){
            if(selection == 5) selection=0;
            else selection++;
            break;
          }
          if(showSpell){
            if(selection == spellList.size()-1) selection=0;
            else selection++;
            break;
          }
          if(showTravel){
            if(selection == 12) selection=0;
            else selection++;
            break;
          }
          if(showStats) break;
          if(showText) {
            if(script.selection<script.choices->size()-1) script.selection++;
            break;
          }
          //if(battle.fight(checkBattle())==3) death();
          //i = checkTile(curMap, cam.getTileX(), cam.getTileY() + 1);
          //playerDir = 0;  
          //anim = true;
          //if (i == 0) {
            cam.keyDown(2);
            //cam.bumpCamera(0, 20);
          //} else if (i > 0){
          //  chMap = i;
          //}
          break;
        case SDLK_LEFT: 
          if(showLoad) break;
          if(showMenu) break;
          if(showSave) break;
          if(showSpell) break;
          if(showStats) break;
          if(showTravel) break;
          if (showText) break;
          //if(battle.fight(checkBattle())==3) death();
          //i = checkTile(curMap, cam.getTileX()-1, cam.getTileY());
          //playerDir = 1; 
          //anim = true;
          //if (i == 0) {
            cam.keyDown(3);
            //cam.bumpCamera(-20, 0);
          //} else if (i > 0){
          //  chMap = i;
          //}
          break;
        case SDLK_RIGHT: 
          if(showLoad) break;
          if(showMenu) break;
          if(showSave) break;
          if(showSpell) break;
          if(showTravel) break;
          if(showStats) break;
          if (showText) break;
          //if(battle.fight(checkBattle())==3) death();
          //i = checkTile(curMap, cam.getTileX()+1, cam.getTileY());
          //playerDir = 2;
          //anim = true;
          //if (i == 0) {
            cam.keyDown(1);
            //cam.bumpCamera(20, 0);
          //} else if (i > 0){
          //  chMap = i;
          //}
          break; 
        case SDLK_ESCAPE:
          if(showLoad){
            showLoad=false;
            break;
          }
          if(showSave){
            showSave=false;
            break;
          }
          if(showSpell){
            showSpell=false;
            break;
          }
          if(showTravel){
            showTravel=false;
            break;
          }
          if(showStats) {
            showStats=false;
            break;
          }
          if(showText) break;
          if(showMenu) showMenu=false;
          else  showMenu=true;
          selection=0;
          break;
        //case SDLK_b: if(battle.fight(checkBattle())==3) death(); break;
        case SDLK_c: castSpell(-2); break;
        //case SDLK_q: stop = true; break;
        case SDLK_z: 
          if(showLoad) break;
          if(showMenu) break;
          if(showSave) break;
          if(showSpell) break;
          if(showTravel) break;
          if(showText) break;
          if(showStats) showStats=false;
          else showStats=true; 
          break;
        case SDLK_BACKSPACE:
          if(showTextInput && userText.size()>0) userText.pop_back();
          break;
        case SDLK_RETURN:
        case SDLK_SPACE:
          if(showMenu) {
            if(selection == 1){
              showMenu=false;
              showSave=true;
              selection=0;
            } else if(selection == 2){
              showMenu=false;
              showLoad=true;
              selection=0;
            } else if(selection == 3){
              COptions opt(display, &music, &font, &gfx, conf);
              opt.run();
            } else if(selection == 4){
              stop=true; break;
            } else{
              showMenu=false;
            }
          } else if(showLoad){
            if(selection > 0) loadGame(selection - 1);
            else if(hero.hp < 1) stop=true; //quit if player cancels load after death
            showLoad=false;
            break;
          } else if(showSave){
            if(selection > 0) saveGame(selection);
            showSave=false;
            break;
          } else if(showSpell){
            castSpell(spellList[selection]);
            break;
          } else if(showTravel){
            travel(selection);
            showTravel=false;
            break;
          } else if(showStats) {
            showStats=false;
          } else if(showText) {
            if(script.text->size()>1){
              script.text->erase(script.text->begin());
            } else if(script.choices->size() > 0){
              setText(script.choices->at(script.selection).result);
            } else if(showTextInput){
              if(e.key.keysym.sym == SDLK_RETURN){
                //check riddle answer
                setText(224);
                showTextInput=false;
                SDL_StopTextInput();
              }
            } else {
              showText = false;
            }
          } else {
            i = checkAction(curMap, cam.getTileX(), cam.getTileY());
            if (i > 0) setText(i);
          }
          break;
        default: break;
        }
      }
    }

    if (stop) break;

    if(eGreyor>1 && !showText){
      if(eGreyor == 2){
        eGreyor = 3;
        setText(150);
      } else if(eGreyor==4){
        eGreyor = 5;
        setText(151);
      } else if(eGreyor == 5){
        eGreyor = 6;
        setText(153);
      } else if(eGreyor == 7){
        eGreyor = 8;
        setText(154);
      } else if(eGreyor == 9){
        eGreyor = 10;
        setText(155);
      } else if(eGreyor == 11){
        credits();
        continue;
      }
    }
    
    moving=cam.scrollCamera(renderCount);
    if(!moving){
      
      switch(cam.setMovement()){
      case 0:
        i = checkTile(curMap, cam.getTileX(), cam.getTileY() - 1);
        playerDir = 3;
        anim = true;
        if(i > 0) chMap = i;
        else if(i < 0) cam.blockMovement();
        break;
      case 1:
        i = checkTile(curMap, cam.getTileX()+1, cam.getTileY());
        playerDir = 2;
        anim = true;
        if(i > 0) chMap=i;
        else if(i < 0) cam.blockMovement();
        break;
      case 2:
        i = checkTile(curMap, cam.getTileX(), cam.getTileY() + 1);
        playerDir = 0;
        anim = true;
        if(i > 0) chMap=i;
        else if(i < 0) cam.blockMovement();
        break;
      case 3:
        i = checkTile(curMap, cam.getTileX()-1, cam.getTileY());
        playerDir = 1;
        anim = true;
        if(i > 0) chMap=i;
        else if(i < 0) cam.blockMovement();
        break;  
      default:
        break;
      }
      //check to see if keypress is buffered. if so, check battle
      if(i==0 && cam.getKeyBuf()){
        if(battle.fight(checkBattle()) == 3) death();
      }
    }
       
    if (chMap > -1){
      cam.blockMovement();
      cam.clearMovement();
      changeMap(chMap);
      chMap = -1;
    }
    if (anim){
      if (playerAnim == 0) playerAnim = 4;
      else playerAnim = 0;
      anim = false;
    }
    if(!moving) cam.scrollCamera(renderCount);
    render();
    renderCount++;

  }

}

void CDarkages::render(){
  int x, y;
  int i, j, k;
  int a, b;
  int offX, offY;
  int lowX, lowY;
  int highX, highY;
  SDL_Rect r;
  //SDL_Rect vp;

  //set the viewport for the game
  //SDL_RenderGetViewport(display->renderer, &vp);
  //r.x=10; r.y=10; r.w=300; r.h=180;
  //SDL_RenderSetViewport(display->renderer, &r);

  x = cam.getX();
  y = cam.getY();

  offX = x % 40;
  offY = y % 40;

  a = x / 40;
  b = y / 40;

  lowX = a - 7;
  highX = a + 8+1;

  lowY = b - 4;
  highY = b + 5+1;

  r.h = 40;
  r.w = 40;

  //printf("A: %d, B: %d, lowX: %d, lowY: %d\n", a, b, lowX, lowY);

  SDL_SetRenderTarget(display->renderer, canvas);

  SDL_RenderClear(display->renderer);
  SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);
  for (j = lowY; j<highY; j++){
    r.y = 40 * (j - lowY) - offY +20;
    for (i = lowX; i<highX; i++){
      //printf("tile: %d,%d\n,", i, j);
      r.x = 40 * (i - lowX) - offX +20;
      if (i < 0 || i >= world[curMap].szX || j < 0 || j >= world[curMap].szY){
        SDL_RenderFillRect(display->renderer, &r);
      } else {
        k = world[curMap].getTile(i, j);
        if (k == 0) SDL_RenderFillRect(display->renderer, &r);
        else SDL_RenderCopy(display->renderer, gfx.tiles->texture, gfx.tiles->getTile(world[curMap].getTile(i, j) - 1), &r);
      }
    }
  }

  //special effect to begin game
  if(fadeIn>0){
    r.x=0; r.y=0; r.w=640; r.h=400;
    SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, fadeIn);
    SDL_RenderFillRect(display->renderer, &r);
    SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);
    r.h = 40;
    r.w = 40;
  }

  //draw player
  r.x = 300;
  r.y = 180;
  SDL_RenderCopy(display->renderer, gfx.player->texture, gfx.player->getTile(playerDir + playerAnim), &r);

  //open viewport back up
  //SDL_RenderSetViewport(display->renderer, &vp);

  //if dead, paste death image over tiles
  if(hero.hp<1){
    r.x=0; r.y=0; r.h=400; r.w=640;
    SDL_RenderCopy(display->renderer, gfx.death->texture, gfx.death->getTile(0), &r);
  }

  //show any endgame screens
  if(eGreyor==4){
    r.x=0; r.y=0; r.h=400; r.w=640;
    SDL_RenderCopy(display->renderer, gfx.endgame1->texture, gfx.endgame1->getTile(0), &r);
  } else if(eGreyor==7){
    r.x=0; r.y=0; r.h=400; r.w=640;
    SDL_RenderCopy(display->renderer, gfx.endgame3->texture, gfx.endgame3->getTile(0), &r);
  } else if(eGreyor == 11){
    r.x=0; r.y=0; r.h=400; r.w=640;
    SDL_RenderCopy(display->renderer, gfx.endgame2->texture, gfx.endgame2->getTile(0), &r);
  }

  //render blinds
  r.x=0; r.y=0; r.h=20; r.w=640;
  SDL_RenderFillRect(display->renderer, &r);
  r.x=0; r.y=0; r.h=400; r.w=20;
  SDL_RenderFillRect(display->renderer, &r);
  r.x=0; r.y=380; r.h=20; r.w=640;
  SDL_RenderFillRect(display->renderer, &r);
  r.x=620; r.y=0; r.h=400; r.w=20;
  SDL_RenderFillRect(display->renderer, &r);

  //draw any text
  if (showText) renderText();

  //draw any stats
  if(showStats) renderStats();

  //draw game menu
  if(showMenu) renderMenu();

  //draw load or save menu
  if(showLoad || showSave) renderSaves();

  //draw spell menu
  if(showSpell) renderSpell();

  //draw travel spell menu
  if(showTravel) renderTravelSpell();

  //for diagnostics
  char str[32];
  sprintf(str, "%d %d,%d:%d  %d,%d  %d", curMap, cam.getTileX(), cam.getTileY(), world[curMap].getTile(cam.getTileX(), cam.getTileY()), curMap,eBattleCheck, fps);
  font.render(10, 370, str);
  
  SDL_SetRenderTarget(display->renderer, NULL);
  SDL_RenderCopy(display->renderer, canvas, NULL, NULL);
  SDL_RenderPresent(display->renderer);

}

void CDarkages::renderBox(int x, int y, int w, int h){
  SDL_Rect r;

  //Draw Box
  r.w = w; r.h = h; r.x = x; r.y = y;
  SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 200);
  SDL_RenderFillRect(display->renderer, &r);
  //SDL_SetRenderDrawColor(display->renderer, 64, 64, 64, 255);
  //SDL_RenderDrawRect(display->renderer, &r);
  //r.w -= 2; r.h -= 2; r.x++; r.y++;
  SDL_SetRenderDrawColor(display->renderer, 96, 96, 96, 255);
  SDL_RenderDrawRect(display->renderer, &r);
  r.w -= 2; r.h -= 2; r.x++; r.y++;
  SDL_RenderDrawRect(display->renderer, &r);
  r.w -= 2; r.h -= 2; r.x++; r.y++;
  SDL_SetRenderDrawColor(display->renderer, 128, 128, 128, 255);
  SDL_RenderDrawRect(display->renderer, &r);
  r.w -= 2; r.h -= 2; r.x++; r.y++;
  SDL_RenderDrawRect(display->renderer, &r);
  r.w -= 2; r.h -= 2; r.x++; r.y++;
  SDL_SetRenderDrawColor(display->renderer, 96, 96, 96, 255);
  SDL_RenderDrawRect(display->renderer, &r);
  r.w -= 2; r.h -= 2; r.x++; r.y++;
  SDL_RenderDrawRect(display->renderer, &r);
  //r.w -= 2; r.h -= 2; r.x++; r.y++;
  //SDL_SetRenderDrawColor(display->renderer, 64, 64, 64, 255);
  //SDL_RenderDrawRect(display->renderer, &r);
  SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);

}

bool CDarkages::renderCredits(){

  SDL_RenderClear(display->renderer);
  SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);

  int max=(script.text->size()+25) * 16;
  if(selection/4 > max) return true;
  int y;
  for(size_t i=0; i < script.text->size(); i++){
    y=i * 16 - selection/4 + 400;
    if(y > 400) break;
    if(y > -16) font.render(20, y, script.text->at(i));
  }
  SDL_RenderPresent(display->renderer);
  return false;

}

void CDarkages::renderMenu(){
  SDL_Rect r;

  //Draw Menu
  renderBox(196, 146, 248, 108);

  //Draw selection
  r.x=202; r.y=160 + selection * 16; r.w=236; r.h=16;
  SDL_SetRenderDrawColor(display->renderer, 0, 0, 128, 255);
  SDL_RenderFillRect(display->renderer, &r);
  SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);

  //Draw Options
  font.render(210, 154, "Return To Game");
  font.render(210, 170, "Save Game");
  font.render(210, 186, "Load Game");
  font.render(210, 202, "Options");
  font.render(210, 218, "Quit");
}

void CDarkages::renderNew(){
  SDL_Rect r;

  SDL_RenderClear(display->renderer);

  //draw player
  r.w=40;
  r.h=40;
  r.x = 300;
  r.y = 180;
  SDL_RenderCopy(display->renderer, gfx.player->texture, gfx.player->getTile(playerDir + playerAnim), &r);

  //draw any text
  if(showText) renderText();

  SDL_RenderPresent(display->renderer);

}

void CDarkages::renderOptions(){
  SDL_Rect r;

  renderBox(20, 20, 600, 360);

  SDL_SetRenderDrawColor(display->renderer, 0, 0, 128, 255);
  switch(selection){
  case 0:
    r.x=36; r.y=40; r.w=260; r.h=26;
    break;
  case 1:
    r.x=36; r.y=80; r.w=260; r.h=26;
    break;
  case 2:
    r.x=36; r.y=120; r.w=260; r.h=26;
    break;
  case 3:
    r.x=36; r.y=160; r.w=260; r.h=26;
    break;
  default:
    break;
  }
  SDL_RenderFillRect(display->renderer, &r);

  //draw music indicators
  if(selection==1){
    r.w=16; r.h=16;
    if(musicVolume > 0) {
      r.x = 400; r.y = 84;
      SDL_RenderCopy(display->renderer, gfx.extra->texture, gfx.extra->getTile(3), &r);
    }
    if(musicVolume<10){
      r.x = 590; r.y = 84;
      SDL_RenderCopy(display->renderer, gfx.extra->texture, gfx.extra->getTile(2), &r);
    }
  }

  //draw resolution indicators
  if(selection == 2){
    r.w=16; r.h=16;
    if(tmpScreen > 0) {
      r.x = 400; r.y = 124;
      SDL_RenderCopy(display->renderer, gfx.extra->texture, gfx.extra->getTile(3), &r);
    }
    if(tmpScreen<display->screenModes.size()-1){
      r.x = 590; r.y = 124;
      SDL_RenderCopy(display->renderer, gfx.extra->texture, gfx.extra->getTile(2), &r);
    }
  }

  //draw fullscreen indicators
  if(selection==3){
    r.w=16; r.h=16;
    if(conf->fullScreen) {
      r.x = 400; r.y = 164;
      SDL_RenderCopy(display->renderer, gfx.extra->texture, gfx.extra->getTile(3), &r);
    } else {
      r.x = 590; r.y = 164;
      SDL_RenderCopy(display->renderer, gfx.extra->texture, gfx.extra->getTile(2), &r);
    }
  }

  font.render(40, 40, "Return to Game");
  font.render(40, 80, "Music");
  r.w = 186; r.h = 24; r.x = 410; r.y = 80;
  SDL_SetRenderDrawColor(display->renderer, 255,255,255,255);
  SDL_RenderDrawRect(display->renderer, &r);
  r.x++; r.y++; r.w-=2; r.h-=2;
  SDL_RenderDrawRect(display->renderer, &r);
  for(int i=1; i <= musicVolume;i++){
    r.w = 16; r.h = 16; r.x = 414+(i-1)*18; r.y = 84;
    SDL_RenderFillRect(display->renderer, &r);
  }
  font.render(40, 120, "Screen Res:");
  font.render(430, 120, display->screenModes[tmpScreen].name);

  font.render(40, 160, "Fullscreen:");
  if(conf->fullScreen) font.render(450, 160, "Yes");
  else font.render(460, 160, "No");

  font.render(40, 200, "Keys:");
  font.render(60, 230, "Cursors = Movement");
  font.render(60, 260, "SPACE/ENTER = Talk/Action");
  font.render(60, 290, "C = Cast Spell");
  font.render(60, 320, "Z = Player Stats");
  font.render(60, 350, "ESC = Game Menu");


  SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);


}

void CDarkages::renderSaves(){
  SDL_Rect r;
  int i;
  char str[64];

  //Draw Menu
  if(showLoad){
    renderBox(80, 52, 480, 296);

    //Draw selection
    r.x=86; r.y=64 + selection * 40; r.w=468; r.h=32;
    SDL_SetRenderDrawColor(display->renderer, 0, 0, 128, 255);
    SDL_RenderFillRect(display->renderer, &r);
    SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);

    font.render(94, 58, "Cancel");
    for(i=0; i < 6;i++){
      if(i == 0) sprintf(str, "QuickSave, %d-%d-%d", saves[i].year, saves[i].month+1, saves[i].day);
      else sprintf(str, "Save %d, %d-%d-%d", i, saves[i].year, saves[i].month+1, saves[i].day);
      font.render(94, 98 + i * 40, str);
      sprintf(str, "%s Lv=%d Gold=%d", saves[i].name, saves[i].level, saves[i].gold);
      font.render(94, 114 + i * 40, str);
    }

  } else {
    
    renderBox(80, 72, 480, 256);

    //Draw selection
    r.x=86; r.y=84 + selection * 40; r.w=468; r.h=32;
    SDL_SetRenderDrawColor(display->renderer, 0, 0, 128, 255);
    SDL_RenderFillRect(display->renderer, &r);
    SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);

    font.render(94, 78, "Cancel");
    for(i=1; i < 6; i++){
      sprintf(str, "Save %d, %d-%d-%d", i, saves[i].year, saves[i].month, saves[i].day);
      font.render(94, 78 + i * 40, str);
      sprintf(str, "%s Lv=%d Gold=%d", saves[i].name, saves[i].level, saves[i].gold);
      font.render(94, 94 + i * 40, str);
    }
  }

}

void CDarkages::renderSpell(){
  SDL_Rect r;
  int lineCount=0;
  size_t i;

  renderBox(150, 138, 340, 124);

  r.w=328; r.h=16; r.x=156; r.y=154 + 16 * selection;
  SDL_SetRenderDrawColor(display->renderer, 0, 0, 128, 255);
  SDL_RenderFillRect(display->renderer, &r);
  SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);

  for(i=0; i < spellList.size(); i++){
    switch(spellList[i]){
    case -1: font.render(164, 148 + 16 * lineCount++, "Cancel"); break;
    case 0: font.render(164, 148 + 16 * lineCount++, "Heal (2 MP)"); break;
    case 2: font.render(164, 148 + 16 * lineCount++, "Great Heal (7 MP)"); break;
    case 5: font.render(164, 148 + 16 * lineCount++, "Massive Heal (12 MP)"); break;
    case 8: font.render(164, 148 + 16 * lineCount++, "Full Heal (20 MP)"); break;
    case 10: font.render(164, 148 + 16 * lineCount++, "Travel (15 MP)"); break;
    default: break;
    }
  }

}

void CDarkages::renderStats(){
  char str[32];
  int i;
  int lineCount;

  //Draw Nameplate
  renderBox(0, 8, 640, 30);
  font.render((640 - 16 * strlen(hero.name)) / 2, 10, hero.name);

  //Draw Player Stats
  renderBox(0, 40, 370, 158);
  font.render(16, 42, "Level");
  sprintf(str, "= %d", hero.level);
  font.render(224, 42, str);
  font.render(16, 58, "Hit Points");
  sprintf(str, "= %d", hero.hp);
  font.render(224, 58, str);
  font.render(16, 72, "Max HP");
  sprintf(str, "= %d", hero.maxHP);
  font.render(224, 72, str);
  font.render(16, 90, "Magic Points");
  sprintf(str, "= %d", hero.mp);
  font.render(224, 90, str);
  font.render(16, 106, "Max MP");
  sprintf(str, "= %d", hero.maxMP);
  font.render(224, 106, str);
  font.render(16, 122, "Experience");
  sprintf(str, "= %d", hero.expr);
  font.render(224, 122, str);
  font.render(16, 138, "Strength");
  sprintf(str, "= %d", hero.str);
  font.render(224, 138, str);
  font.render(16, 154, "Dexterity");
  sprintf(str, "= %d", hero.dex);
  font.render(224, 154, str);
  font.render(16, 170, "Wisdom");
  sprintf(str, "= %d", hero.wis);
  font.render(224, 170, str);

  //Draw Battle Stats
  renderBox(0, 200, 370, 78);
  font.render(16, 202, "Attack Power");
  sprintf(str, "= %d", hero.str / 2 + hero.wStr);
  font.render(224, 202, str);
  font.render(16, 218, "Hit Percent");
  sprintf(str, "= %d%%", 60 + hero.dex / 2);
  font.render(224, 218, str);
  font.render(16, 234, "Armor Value");
  sprintf(str, "= %d", hero.aStr + hero.hStr + hero.sStr);
  font.render(224, 234, str);
  font.render(16, 250, "Gold");
  sprintf(str, "= %d", hero.gold);
  font.render(224, 250, str);

  //Draw Armaments
  renderBox(0, 280, 640, 94);
  font.render(16, 282, "Weapon");
  sprintf(str, "= %s", hero.weapon);
  font.render(288, 282, str);
  font.render(16, 298, "Armor");
  sprintf(str, "= %s", hero.armor);
  font.render(288, 298, str);
  font.render(16, 314, "Helmet");
  sprintf(str, "= %s", hero.helm);
  font.render(288, 314, str);
  font.render(16, 330, "Shield");
  sprintf(str, "= %s", hero.shield);
  font.render(288, 330, str);
  font.render(16, 346, "Medallion Pieces");
  sprintf(str, "= %d", hero.medallion);
  font.render(288, 346, str);

  //Draw Spells
  renderBox(372, 40, 268, 238);
  font.render(388, 42, "Spells:");
  lineCount=0;
  for(i=0; i<11; i++){
    if(hero.spells[i]) font.render(404, 58 + 16 * lineCount++, spells[i].name);
  }
  if(lineCount == 0) font.render(404, 58, "None");

}

void CDarkages::renderText(){
  if(script.text->at(0).size() < 2) return;

  SDL_Rect r;
  size_t i;
  string word;
  string line;
  string curText;
  int sz;
  int wsz;
  int lineNum;
  int choiceLine;
  bool bDownArrow=false;

  r.w = 640;
  r.h = 130;
  r.x = 0;
  r.y = 0;
  SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 200);
  SDL_RenderFillRect(display->renderer, &r);

  SDL_SetRenderDrawColor(display->renderer, 64, 64, 64, 255);
  SDL_RenderDrawRect(display->renderer, &r);
  r.w -= 2;  r.h -= 2;  r.x++;  r.y++;
  SDL_RenderDrawRect(display->renderer, &r);

  SDL_SetRenderDrawColor(display->renderer, 96, 96, 96, 255);
  r.w -= 2;  r.h -= 2;  r.x++;  r.y++;
  SDL_RenderDrawRect(display->renderer, &r);
  r.w -= 2;  r.h -= 2;  r.x++;  r.y++;
  SDL_RenderDrawRect(display->renderer, &r);

  SDL_SetRenderDrawColor(display->renderer, 128, 128, 128, 255);
  r.w -= 2;  r.h -= 2;  r.x++;  r.y++;
  SDL_RenderDrawRect(display->renderer, &r);
  r.w -= 2;  r.h -= 2;  r.x++;  r.y++;
  SDL_RenderDrawRect(display->renderer, &r);

  SDL_SetRenderDrawColor(display->renderer, 96, 96, 96, 255);
  r.w -= 2;  r.h -= 2;  r.x++;  r.y++;
  SDL_RenderDrawRect(display->renderer, &r);
  r.w -= 2;  r.h -= 2;  r.x++;  r.y++;
  SDL_RenderDrawRect(display->renderer, &r);

  SDL_SetRenderDrawColor(display->renderer, 64, 64, 64, 255);
  r.w -= 2;  r.h -= 2;  r.x++;  r.y++;
  SDL_RenderDrawRect(display->renderer, &r);
  r.w -= 2;  r.h -= 2;  r.x++;  r.y++;
  SDL_RenderDrawRect(display->renderer, &r);

  SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);

  //Render word by word, applying line breaks as needed.
  sz = 0;
  lineNum = 0;
  word.clear();
  curText=script.text->at(0);
  for (i = 0; i < curText.size(); i++){
    if (curText[i] == ' '){
      wsz=font.getStringWidth(word);
      if (sz + wsz>608){
        //print current line
        font.render(16, 8+16*lineNum++, line);
        line = word;
        line += " ";
        sz = wsz + font.getStringWidth(' ');
      } else {
        line += word;
        line += " ";
        sz += wsz+font.getStringWidth(' ');
      }
      word.clear();
      wsz = 0;
      continue;
    }
    word += curText[i];
    wsz += font.getStringWidth(curText[i]);
  }

  if (sz + wsz>608){
    font.render(16, 8+16 * lineNum++, line);
    line = word;
  } else {
    line += word;
  }
  font.render(16, 8+16 * lineNum++, line);

  //special case for user input
  if(showTextInput && script.text->size()==1) {
    font.render(16, 8 + 16 * lineNum, "You say: ");
    font.render(160, 8 + 16 * lineNum, userText);
    font.render(160 + 16 * userText.size(), 8 + 16 * lineNum, "_");
  }

  //render choices if necessary
  bDownArrow=false;
  choiceLine=lineNum;
  if(script.text->size()==1 && script.choices->size()>0){
    for(i=script.offset; i < script.choices->size();i++){
      if(script.selection==i){
        r.w -= 4;
        r.h = 16;
        r.x++;
        r.y = 18 + 16 * lineNum;
        SDL_SetRenderDrawColor(display->renderer, 0, 0, 128, 255);
        SDL_RenderFillRect(display->renderer, &r);
        SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);
      }
      font.render(36, 12 + 16 * lineNum++, script.choices->at(i).text);
      if(lineNum>5){
        if(i<script.choices->size()-1) bDownArrow=true;
        break;
      }
    }
    if(script.selection>i) script.offset++;
  }

  //Draw arrows if necessary
  if(script.offset>0) {
    r.w=16;
    r.h=16;
    r.x = 16;
    r.y = 18 + 16 * choiceLine;
    SDL_RenderCopy(display->renderer, gfx.extra->texture, gfx.extra->getTile(1), &r);
  }
  if(bDownArrow){
    r.w=16;
    r.h=16;
    r.x = 16;
    r.y = 96;
    SDL_RenderCopy(display->renderer, gfx.extra->texture, gfx.extra->getTile(0), &r);
  }

}

void CDarkages::renderTitle(){
  SDL_Rect r;

  SDL_RenderClear(display->renderer);
  SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);

  r.x=0; r.y=0; r.h=400; r.w=640;
  SDL_RenderCopy(display->renderer, gfx.title->texture, gfx.title->getTile(0), &r);

  font.setFontSize(16);
  font.render(10, 380, "Copyright (C) 1997-2017, Dark Knight Software");
  font.render(10, 370, "version 2.0 alpha 1");
  font.setFontSize(32);

  //draw load or save menu
  if(showLoad) {
    renderSaves();
  } else {

    //Menu options
    font.render(260, 240, "New Game");
    font.render(260, 270, "Load Game");
    font.render(260, 300, "Options");
    font.render(260, 330, "Exit");

    //draw indicator
    r.w=16;
    r.h=16;
    r.x = 240;
    r.y = 244 + selection * 30;
    SDL_RenderCopy(display->renderer, gfx.extra->texture, gfx.extra->getTile(2), &r);

  }

  SDL_RenderPresent(display->renderer);
  

}

void CDarkages::renderTravelSpell(){
  SDL_Rect r;

  renderBox(75, 39, 170, 116);

  r.w=164; r.h=8; r.x=78; r.y=47 + 8 * selection;
  SDL_SetRenderDrawColor(display->renderer, 0, 0, 128, 255);
  SDL_RenderFillRect(display->renderer, &r);
  SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);

  font.render(82, 44, "Cancel");
  font.render(82, 52, "Meadow");
  font.render(82, 60, "Quinine Tower");
  font.render(82, 68, "Wisp");
  font.render(82, 76, "Trok");
  font.render(82, 84, "Tristen");
  font.render(82, 92, "Northern Post");
  font.render(82, 100, "Castle Garrison");
  font.render(82, 108, "Amber");
  font.render(82, 116, "Laendlich");
  font.render(82, 124, "Rhoeyce");
  font.render(82, 132, "Aaryak");
  font.render(82, 140, "Floating City");

}

void CDarkages::saveGame(int index){
  time_t t;
  tm* curTime;
  FILE* f;
  char str[32];
  int version=1;
  int i;

  time(&t);
  curTime=localtime(&t);
  saves[index].day=curTime->tm_mday;
  saves[index].month=curTime->tm_mon;
  saves[index].year=curTime->tm_year+1900;
  curTime=NULL;
  strcpy(saves[index].name, hero.name);
  saves[index].level=hero.level;
  saves[index].gold=hero.gold;

  sprintf(str, "Saves/sv%d.da1", index);

  f=fopen(str, "wb");
  fwrite(&version, sizeof(int), 1, f);
  fwrite(&curMap, sizeof(int), 1, f);
  i=cam.getTileX();
  fwrite(&i, sizeof(int), 1, f);
  i=cam.getTileY();
  fwrite(&i, sizeof(int), 1, f);
  fwrite(&eAle,sizeof(int),1,f);
  fwrite(&eBattleCheck, sizeof(int), 1, f);
  fwrite(&eBattleNum, sizeof(int), 1, f);
  fwrite(&eBridge, sizeof(int), 1, f);
  fwrite(&eCave, sizeof(int), 1, f);
  fwrite(&eEmma, sizeof(int), 1, f);
  fwrite(&eFirewand, sizeof(int), 1, f);
  fwrite(&eGreyor, sizeof(int), 1, f);
  fwrite(&eHelpDwarf, sizeof(int), 1, f);
  fwrite(&eHorn, sizeof(int), 1, f);
  fwrite(&hero, sizeof(CPlayer), 1, f);
  fclose(f);

  f=fopen("Saves/master.sav", "wb");
  fwrite(&saves, sizeof(da1save), 6, f);
  fclose(f);
}

void CDarkages::setRandomBanter(){
  int r=rand() % 10;
  switch(r){
  case 0: script.addText("A wise man in Amber knows how to find the hidden city of dwarves."); break;
  case 1: script.addText("Only a person with vast knowledge of the magic arts may enter the Floating City."); break;
  case 2: script.addText("A jeweler in Wisp can fix any piece of jewelry."); break;
  case 3: script.addText("Legend says that the gods created a magic sword centuries ago. During The Breaking, however, the sword mysteriously disappeared."); break;
  case 4: script.addText("Good day, sir. Pleased to meet you."); break;
  case 5: script.addText("The Great Library in the Floating City has is the greatest source of information in the world. Be sure to read all the books contained there."); break;
  case 6: script.addText("Have you visited Castle Garrison? It is the capital of the main continent."); break;
  case 7: script.addText("If you ever get lost in your quest, just visit the towns and talk to everyone. Someone will always be able to help you on your quest."); break;
  case 8: script.addText("The dwarves stole a piece of the medallion from the museum in Wisp."); break;
  case 9: script.addText("There's a scientist in Trok who might be able to help you."); break;
  default:
    break;
  }
}

void CDarkages::setText(int i){
  script.clear();
  da1response res;
  string s;
  int r;

  switch(i){
  case 0:
    showText=false;
    return;
  case 42:
    if(curMap == 41){ //trok random banter
      setRandomBanter();
    }
    break;
  case 45:
    if(curMap == 19){ //garrison hint
      script.addText("Once you have the three medallion pieces, you must find some way to repair them. As three individual pieces, the medallion is worthless. It must be whole.");
    } else if(curMap == 42){ //jeweler in wisp
      script.addText("Hello and welcome to my humble shop. I deal in jewelry. I can make or repair.");
      if(hero.medallion > 2 && eGreyor == 0){
        script.addText("You have all the pieces of Greyor's medallion! Yes I can repair it, but it will be some time. This is a delicate situation.");
        script.addText("(After 3 days you return to the small shop.)                                It is done. I suggest you destroy it again when you are done. Such evil should not exist in this world, even if it is to be used for good.");
        eGreyor=1;
      } else if(hero.medallion > 2){
        script.addText("You have the repaired medallion. Use it wisely.");
      }
    }
    break;
  case 46: //tavernkeeper
    script.addText("Hey there adventurer! What can I do for you?");
    script.addChoice("Do you know of the Mystic Medallion?", 201);
    script.addChoice("How 'bout some ale?", 202);
    script.addChoice("Nothing. I'm just looking around.", 0);
    break;
  case 47: //aaryak tavernkeeper
    script.addText("Welcome to the Lion's Den! If you're looking for news, see the Miner's Guild. They govern this town.");
    break;
  case 48: //reyd
    if(curMap == 0 || curMap == 1) { //aaryak,amber banter
      setRandomBanter();
    } else if(curMap == 19) { //garrison gardener
      script.addText("(Senile Gardener)                     Hello Greybeard, you ol' turd, you. Why'd you shave that beard o' yours...Wait a sec, you're not Greybeard. What'd you do with Greybeard?");
      script.addText("(Without any noticeable transition...)See these trees here? I've been growing them since I was a boy. They're special trees, they're...err..um...special trees.");
      if(eFirewand == 5){
        script.addText("(`Are they Ironwood?' you ask.)       Yes! Yes! That's what they are. Strongest wood known to man. That's how they got their name.");
        script.addText("(You politely ask for some ironwood and the senile old man picks up a fallen branch.)                     Here you go, Greybeard. It's all yours. You looked better with the beard, in case you needed to know.");
        eFirewand=6;
      }
    } else if(curMap == 21) { //laendlich banter
      setRandomBanter();
    } else if(curMap == 41) {//trok welcome
      script.addText("Welcome to Trok!");
    } else if(curMap == 42) {//wisp random banter
      setRandomBanter();
    } else {
      if(eBridge == 0){
        script.addText("Hi, I'm Reyd the carpenter. Need a house built? Or maybe a chair?");
      } else if(eBridge == 1){
        script.addText("Hi, I'm Reyd the carpenter. Need a house built? Or maybe a chair?");
        script.addChoice("I need a bridge to cross the river.", 206);
        script.addChoice("Bye.", 0);
      } else if(eBridge == 2){
        script.addText("What can I do for you?");
        script.addChoice("I have your key.", 208);
        script.addChoice("Bye.", 0);
      } else if(eBridge == 3){
        script.addText("How's the bridge? Still there? Of course it is, I built it!");
      }
    }
    break;
  case 49: //emma
    if(curMap == 0) {//aaryak banter
      setRandomBanter();
    } else if(curMap == 1) { //amber girl
      script.addText("Hello mister.");
    } else if(curMap == 21) {
      script.addText("The dungeon to the east is dangerous!");
    } else {
      if(eEmma == 0){
        script.addText("My mommy says I can't talk to strangers.");
      } else if(eEmma == 1){
        script.addText("My mommy says I can't talk to strangers.");
        script.addChoice("But cousin Mert sent me.", 211);
        script.addChoice("Bye.", 0);
      } else if(eEmma == 2){
        script.addText("Hello agian. Tag! You're it!");
        script.addChoice("To whom did you give the key?", 212);
        script.addChoice("No I'm not. Bye.", 0);
      }
    }
    break;
  case 50: //mert
    if(curMap == 0) {//aaryak hint guy
      script.addText("Be sure to gather all the best items and spells before going after Greyor.");
    } else if(curMap == 1){ //amber welcome
      script.addText("Welcome to Amber!");
    } else if(curMap == 21) { //laendlich banter
      setRandomBanter();
    } else if(curMap == 39 || curMap == 40) { //tristen man
      script.addText("A priest resides in a temple near the northern coast between Trok and the Northern Post.");
    } else if(curMap == 41) { //trok dude 1
      script.addText("There's a ferryman in the southwest who can take you to Laendlich... for a fee.");
    } else if(curMap == 42) { //Wisp man
      script.addText("The Northern Post protects our northeastern coast from pirates.");
    } else {
      script.addText("Hello, I'm Mert. I am Reyd the carpenter's assistant.");
      if(eBridge == 1) {
        script.addChoice("What did you do with the key?", 209);
        script.addChoice("Bye.", 0);
      }
    }
    break;
  case 51: //lotar
    if(curMap == 0){ //aaryak drinker 1
      script.addText("Pull up a chair and we'll drink 'til we're drunk!");
    } else {
      script.addText("Vell helloo der! A new adventurur, I see. Joost like meself. Vhat can I do fur ye?");
      script.addChoice("Do you have any battle tips?", 215);
      if(eAle == 1) script.addChoice("I need an ale for Tyren.", 216);
      script.addChoice("Just passing by, thanks.", 0);
    }
    break;
  case 52: //tyren
    if(curMap == 0){ //aaryak drinker 2
      script.addText("Oh. It's you.");
    } else {
      if(eAle == 2){
        script.addText("Got my ale?");
        script.addChoice("Yep. Here you go.", 214);
        script.addChoice("Bye.", 0);
      } else if(eAle == 1){
        script.addText("Where's my ale? You don't get the key until I get my ale!");
      } else {
        script.addText("Go away, I'm trying to drink here!");
        if(eEmma == 2){
          script.addChoice("But do you have a certain key?", 213);
          script.addChoice("Bye.", 0);
        }
      }
    }
    break;
  case 53: //Meadow Inn
    if(curMap == 0 || curMap == 19){//aaryak inn, garrison inn
      script.addText("Welcome to the inn. A room is 100 gold pieces per night.");
      script.addChoice("No", 0);
      script.addChoice("Yes", 575);
    } else if(curMap == 17 || curMap == 18) {//fcity inn
      script.addText("Welcome to the inn. A room is 90 gold pieces per night.");
      script.addChoice("No", 0);
      script.addChoice("Yes", 577);
    } else if(curMap == 29) {//npost inn
      script.addText("Welcome to the inn. A room is 30 gold pieces per night.");
      script.addChoice("No", 0);
      script.addChoice("Yes", 576);
    } else if(curMap == 34){//Rhoeyce inn
      script.addText("Welcome to the inn. A room is 50 gold pieces per night.");
      script.addChoice("No", 0);
      script.addChoice("Yes", 573);
    } else if(curMap == 42){//wisp inn
      script.addText("Welcome to the inn. A room is 40 gold pieces per night.");
      script.addChoice("No", 0);
      script.addChoice("Yes", 572);
    } else {
      script.addText("Welcome to the inn. A room is 10 gold pieces per night.");
      script.addChoice("No", 0);
      script.addChoice("Yes", 570);
    }
    break;
  case 54: //Meadow weapon shop
    if(curMap == 1) { //amber store
      script.addText("What will it be traveler?");
      script.addChoice("Nothing", 0);
      script.addChoice("Axe (300 gp)", 504);
      script.addChoice("Chain Mail (2200 gp)", 525);
    } else if(curMap == 41){//trok weapon
      script.addText("What will it be traveler?");
      script.addChoice("Nothing", 0);
      script.addChoice("Shortsword (800 gp)", 505);
      script.addChoice("Buckler (15 gp)", 540);
      script.addChoice("Small Shield (100 gp)", 541);
    } else if(curMap == 42){//wisp weapon
      script.addText("What will it be traveler?");
      script.addChoice("Nothing", 0);
      script.addChoice("Axe (300 gp)", 504);
      script.addChoice("Chain Coif (75 gp)", 531);
      script.addChoice("Ring Mail (450 gp)", 523);
      script.addChoice("Scale Mail (1000 gp)", 524);
      script.addChoice("Large Shield (500 gp)", 542);
    } else {
      script.addText("What will it be traveler?");
      script.addChoice("Nothing", 0);
      script.addChoice("Dagger (10 gp)", 500);
      script.addChoice("Club (25 gp)", 501);
      script.addChoice("Tunic (25 gp)", 520);
      script.addChoice("Leather Vest (100 gp)", 521);
      script.addChoice("Buckler (15 gp)", 540);
    }
    break;
  case 55: //Meadow magic shop
    if(curMap == 17 || curMap == 18){
      script.addText("I can teach two magic levels if you are strong enough.");
      script.addChoice("Nothing", 0);
      script.addChoice("Level 10: Death (3500 gp)", 559);
      script.addChoice("Level 11: Travel (4000 gp)", 560);
    } else if(curMap == 21){ //laendlich magic shop
      script.addText("I can teach two magic levels if you are strong enough.");
      script.addChoice("Nothing", 0);
      script.addChoice("Level 4: Blindness (500 gp)", 553);
      script.addChoice("Level 8: Melt Flesh (2000 gp)", 557);
    } else if(curMap == 39 || curMap == 40){ //tristen magic shop
      script.addText("I can teach two magic levels if you are strong enough.");
      script.addChoice("Nothing", 0);
      script.addChoice("Level 6: Massive Heal (1000 gp)", 555);
      script.addChoice("Level 9: Full Heal (3000 gp)", 558);
    } else if(curMap == 42){ //wisp artifact
      script.addText("The card reads, 'Magical devices of Ithis, Lord of the Northern Reaches.'");
    } else {
      script.addText("I can teach two magic levels if you are strong enough.");
      script.addChoice("Nothing", 0);
      script.addChoice("Level 1: Heal (50 gp)", 550);
      script.addChoice("Level 2: Burn (150 gp)", 551);
    }
    break;
  case 60:
    if(curMap == 4) {//amber hermit
      if(eHelpDwarf == 5){
        script.addText("(You hurriedly ask where to find the Diamant.)                              Hehehe. Need it, do you? Ahh, but I'm not so senile in all these years. You, like all the other adventurers, must play the game.");
        script.addText("It's quite simple, just a riddle. Tell me my name, and I will tell you where to find the Diamant.");
        userText.clear();
        SDL_StartTextInput();
        showTextInput=true;
        //add name part here
      } else if(eHelpDwarf == 6){
        script.addText("Good luck on your quest!");
      } else {
        script.addText("Well, what have we here? It's been 2 years since the last visitor! Surprised to find someone living up here? Hehehe. I don't think I could leave if I wanted to.");
        script.addText("At least the monsters don't come here because of the sunlight and I can grow all the food I need over there. Why, this is probably the perfect existence.");
        script.addText("(You finally manage to quell his incessant ramblings long enough to ask about the Diamant.)                    Diamant? Hehehe. 50 years ago, I came here to escape the world.");
        script.addText("Little did I know that the world would seek me out. Year after year, brave young adventurers would venture into these caves searching for the prized Diamant. None of them ever found it.");
        script.addText("It is the stuff of legends! Ever wonder why no one has seen it in centuries? Because it doesn't exist, right? Ahh, but it does! I've combed these caves all these years.");
        script.addText("I know them backward and forward. And I also know how to get the Diamant!");
        eHelpDwarf=5;
      }
    } else if(curMap == 17 || curMap == 18){ //white wizard
      script.addText("Have you visited the Great Library? Thousands of volumes are contained in its lower vaults. You can peruse some of the volumes on display.");
    } else if(curMap == 38){ //temple inn
      script.addText("Welcome! For a small donation - 50 gold pieces - I can heal you.");
      script.addChoice("No", 0);
      script.addChoice("Yes", 573);
    } else if(curMap == 41){ //trok scientist
      if(eHelpDwarf == 3 || eHelpDwarf == 4 || eHelpDwarf == 5){
        script.addText("Welcome to my Observatory. Thanks to a grant from King Xandar, I have money to make lenses for my telescope so I can observe the stars. I also make smaller lenses for eyeglasses.");
        script.addText("I am also working on lenses that can concentrate solar energy.");
        script.addText("(You tell the scientist of the magic crystal encasing the medallion piece)  Hmm... If I had a material strong     enough to capture a lot of solar energy, perhaps we could direct the resulting beam through the crystal.");
        script.addText("It might just be strong enough to break the magical barrier. The only substance strong enough is Diamant. However, no one has seen it in centuries. It is rumored to be hidden in the Crystal Mountains. Go there.");
        if(eHelpDwarf == 3) eHelpDwarf = 4;
      } else if(eHelpDwarf == 6){
        script.addText("Ahh! You've found the Diamant! Give me a moment to work with it.");
        script.addText("(The scientist spends several hours shaping the Diamant crystal. Then he mounts it on some odd apparatus. He takes it outside, focuses sunlight through it, and blows a gigantic hole in the side of his lab.)");
        eHelpDwarf=7;
      } else if(eHelpDwarf == 7){
        script.addText("Ooops! It's even stronger than I thought! Better watch where you point the lens. Shine a beam of energy through the magic crystal encasing the medallion to free it. Good luck!");
      } else {
        script.addText("Welcome to my Observatory. Thanks to a grant from King Xandar, I have money to make lenses for my telescope so I can observe the stars. I also make smaller lenses for eyeglasses.");
        script.addText("I am also working on lenses that can concentrate solar energy.");
      }
    }
    break;
  case 62:
    if(eCave == 0){
      eCave=1;
      script.addText("(Lord Ithis to his Advisor)           ...but without the tunnel, we'll have to continue risking lives and money by trading through the passes.");
      script.addText("(Advisor to Lord Ithis)               I know, your majesty, but we've lost several lives already and our soldiers will never be able to defeat such a beast.");
      script.addText("(Lord Ithis to his Advisor)           We must do something!");
      script.addText("(Advisor to Lord Ithis)               The only way we can complete the tunnel is to kill that monster. But our troops are hurt and weary. We've nothing left to do.");
      script.addText("(Lord Ithis turns tiredly to you)     Welcome visitor. Pardon my lack of cordiality, but something terrible occupies my mind at the moment.");
      script.addChoice("Perhaps I can help you.", 62);
      script.addChoice("I'll leave you in peace.", 0);
    } else if(eCave == 1){
      script.addText("Some time ago, I commissioned a tunnel through the mountains to ease trade. The passes are so dangerous, many lives are lost each year trying to cross them.");
      script.addText("During the work, a miner chanced across an odd looking box somehow buried in the middle of the mountain. Curious as to what it was, he opened it.");
      script.addText("Inside was one of the missing pieces of Greyor's medallion! But also inside, there was a trap. When opened, a spell was released and a demon spider was summoned.");
      script.addText("The spider immediately killed the miner and started spawning smaller spiders to kill the other miners and guard the medallion.");
      script.addText("A few souls got out to spread the news. I instantly ordered troops to exterminate the creatures, but they were outnumbered and slaughtered.");
      script.addText("The only way to end this disaster is to kill the demon spider.");
      script.addChoice("I will help you.", 217);
      script.addChoice("Good luck with that. Bye!", 0);
    } else if(eCave == 2){
      script.addText("You will help? 'Tis suicide for sure, but I will not turn down volunteers. You may join General Ryldar's squad that is being deployed at this very moment. Meet him in the tunnel.");
      script.addText("If you succeed, you may keep the medallion piece in return for your services.");
    } else {
      eCave=4;
      script.addText("Thank you for destroying that foul creature. The tunnel is now complete. Keep the medallion, you earned it.");
    }
    break;
  case 63: //QTower advisor
    if(curMap == 1){ //amber wiseman
      script.addText("I know a way to find the underground city of dwarves. The entrance to their caves is magically hidden. You can only enter by blowing the magic horn. A dwarf in Laendlich can help you.");
      if(eHorn == 0) eHorn=1;
    } else if(curMap == 18) { //floating city, mizrak the wizard
      script.addText("Wonderful! You're here!");
      if(eFirewand == 4 || eFirewand == 5) {
        script.addText("It seems they cleaned out my lab after I was banished, so you'll need to pick up some supplies for me. The first is Ironwood. It's the only wood strong enough to handle the stress of magic. Find it and bring it back here.");
        eFirewand = 5;
      } else if(eFirewand > 5 && eFirewand < 16) {
        script.addText("Great! You found the Ironwood. Now I'll need some sulfur. It is mined from the hills near Aaryak. I suggest you visit the town and speak with the Miners' Guild.");
        if(eFirewand == 6) eFirewand = 7;
      } else if(eFirewand == 16){
        script.addText("(You give the sulfur to Mizrak. You must wait several hours before the wand is made. When it is finished, he hands it to you and says...)          To activate the wand, you will need a priest's blessing. Good luck.");
        eFirewand = 17;
      } else if(eFirewand == 17){
        script.addText("To activate the wand, you will need a priest's blessing. Good luck.");
      }
    } else if(curMap == 19) { //garrison advisor
      script.addText("There are many skilled people in the land who can help you on your quest. Be sure to visit every town and talk to everyone.");
    } else if(curMap == 39 || curMap == 40) { //tristen, mizrak the wizard
      script.addText("(Mizrak the Wizard)                   I didn't always live here. Years ago, I was the greatest magical engineer in The Floating City of Mages. I could build any magical device.");
      if(eFirewand == 1 || eFirewand == 2){
        script.addText("(You tell him of the magic ice blocking the passage in the dungeon.)  Aha! I know just the thing! A firewand could temporarily break that spell. But I would need to get at my lab.");
        script.addText("It's in the Floating City. Unless you can get me back into the city, I'm afraid I can't help you.");
        eFirewand=2;
      } else if(eFirewand > 2){
        script.addText("Thank you! I'll leave today. Meet me in my lab in the Floating City. You can't miss it. It's the building in the northwest corner of the city.");
        eFirewand=4;
      }
    } else if(curMap == 41) { //trok dude2
      script.addText("Welcome to my humble abode. Now get the HELL out!");
    } else {
      script.addText("I'm Lord Ithis's advisor.");
      if(eCave > 2) script.addText("Thank you for ending the slaughter.");
      else script.addText("So many men have lost their lives. But the lord is determined to complete the tunnel. Please help end this catastrophy.");
    }
    break;
  case 64: //QTower knight
    if(curMap == 0) { //aaryak banter
      setRandomBanter();
    } else if(curMap == 1){ //amber knight
      script.addText("I'm heading towards the Crystal Mountains to the west. They say that many rare gems are within the caves that are there.");
    } else if(curMap == 19) { //garrison treasure guard
      script.addText("No one is allowed in the treasure room.");
    } else if(curMap == 29){ //npost knight
      script.addText("This fort protects our lands from the goblin invasions from those hills to the north.");
    } else if(curMap == 39 || curMap == 40){ //tristen knight
      script.addText("A man by the name of Gawain knows where to find the Magic Sword.");
    } else if(curMap == 41){ //trok knight
      script.addText("Dwarves are said to be the best strategists. I hope to train under one someday.");
    } else if(curMap == 42){ //suit of armor in wisp
      script.addText("A small card reads, 'Suit of Armor, Enlightened Period.'");
    } else {
      script.addText("Welcome to Quinine Tower. I guard the throne room for Lord Ithis.");
      if(eCave > 2) script.addText("You truly are a great warrior! Good luck in your quest for Greyor.");
      else script.addText("The situation is grim. So many soldiers are wounded and morale is low. We need a miracle.");
    }
    break;
  case 66: //QTower weapon
    if(curMap == 0){ //aaryak weapon
      script.addText("What will it be traveler?");
      script.addChoice("Nothing", 0);
      script.addChoice("Great Axe (1500 gp)", 506);
      script.addChoice("Halberd (3500 gp)", 508);
    } else if(curMap == 19) { //garrison weapon
      script.addText("What will it be traveler?");
      script.addChoice("Nothing", 0);
      script.addChoice("Longsword (2500 gp)", 507);
      script.addChoice("Magic Axe (5000 gp)", 509);
      script.addChoice("Partial Plate Mail (4000 gp)", 526);
      script.addChoice("Full Plate Mail (6000 gp)", 527);
      script.addChoice("Plate Helm (250 gp)", 532);
      script.addChoice("Magic Helm (500 gp)", 533);
    } else if(curMap == 29){ //npost weapon
      script.addText("What will it be traveler?");
      script.addChoice("Nothing", 0);
      script.addChoice("Mace (40 gp)", 502);
      script.addChoice("Axe (300 gp)", 504);
      script.addChoice("Great Axe (1500 gp)", 506);
      script.addChoice("Scale Mail (1000 gp)", 524);
      script.addChoice("Chain Coif (75 gp)", 531);
    } else if(curMap == 34) { //rhoeyce weapon
      script.addText("Welcome to Wilhelm's Sword & Shields! What will it be traveler?");
      script.addChoice("Nothing", 0);
      script.addChoice("Shortsword (800 gp)", 505);
      script.addChoice("Buckler (15 gp)", 540);
      script.addChoice("Small Shield (100 gp)", 541);
      script.addChoice("Large Shield (500 gp)", 542);
      script.addChoice("Magic Shield (2000 gp)", 543);
    } else if(curMap == 39 || curMap == 40){ //tristen weapon
      script.addText("What will it be traveler?");
      script.addChoice("Nothing", 0);
      script.addChoice("Dagger (10 gp)", 500);
      script.addChoice("Spear (60 gp)", 503);
      script.addChoice("Longsword (2500 gp)", 507);
      script.addChoice("Tunic (25 gp)", 520);
      script.addChoice("Ring Mail (450 gp)", 523);
      script.addChoice("Chain Mail (2200 gp)", 525);
    } else if(curMap == 42){ //wisp artifact 2
      script.addText("The card reads, 'Sword and Shield of Feigling the Quick to Run Away.'");
    } else {
      script.addText("What will it be traveler?");
      script.addChoice("Nothing", 0);
      script.addChoice("Club (25 gp)", 501);
      script.addChoice("Mace (40 gp)", 502);
      script.addChoice("Spear (60 gp)", 503);
    }
    break;
  case 67:
    if(curMap == 0) { //aaryak soldier
      script.addText("I'm a soldier in Gawain's regiment. I'm on leave right now.");
    } else if(curMap == 19) { //garrison guard
      script.addText("As a guard of the castle, I must always be alert.");
    } else if(curMap == 29) {//npost banter
      setRandomBanter();
    } else {
      script.addText("These weapons are good, but not as good as a magic sword. I heard there is a magic sword hidden somewhere on this continent.");
    }
    break;
  case 68: //QTower armor
    if(curMap == 0) { //aaryak armor
      script.addText("What will it be traveler?");
      script.addChoice("Nothing", 0);
      script.addChoice("Full Plate Mail (6000 gp)", 527);
      script.addChoice("Magic Armor (9000 gp)", 528);
      script.addChoice("Plate Helm (250 gp)", 532);
    } else {
      script.addText("What will it be traveler?");
      script.addChoice("Nothing", 0);
      script.addChoice("Leather Vest (100 gp)", 521);
      script.addChoice("Studded Leather (250 gp)", 522);
      script.addChoice("Small Shield (100 gp)", 541);
      script.addChoice("Leather Cap (10 gp)", 530);
    }
    break;
  case 69:
    if(curMap == 21) {//laendlich inn
      script.addText("Welcome to the inn. A room is 40 gold pieces per night.");
      script.addChoice("No", 0);
      script.addChoice("Yes", 572);
    } else if(curMap == 1 || curMap == 39 || curMap == 40){ //tristen, amber inn
      script.addText("Welcome to the inn. A room is 50 gold pieces per night.");
      script.addChoice("No", 0);
      script.addChoice("Yes", 573);
    } else if(curMap == 29) {//npost inn
      script.addText("Welcome to the inn. A room is 30 gold pieces per night.");
      script.addChoice("No", 0);
      script.addChoice("Yes", 576);
    } else if(curMap == 41) { //trok inn
      script.addText("Welcome to the inn. A room is 60 gold pieces per night.");
      script.addChoice("No", 0);
      script.addChoice("Yes", 574);
    } else {
      script.addText("Welcome to the inn. A room is 20 gold pieces per night.");
      script.addChoice("No", 0);
      script.addChoice("Yes", 571);
    }
    break;
  case 70: //QTower magic
    if(curMap == 0){ //aaryak magic shop
      script.addText("I can teach two magic levels if you are strong enough.");
      script.addChoice("Nothing", 0);
      script.addChoice("Level 4: Blindness (500 gp)", 553);
      script.addChoice("Level 9: Full Heal (3000 gp)", 558);
    } else if(curMap == 19) { //garrison magic
      script.addText("I can teach two magic levels if you are strong enough.");
      script.addChoice("Nothing", 0);
      script.addChoice("Level 5: Firebomb (750 gp)", 554);
      script.addChoice("Level 6: Massive Heal (1000 gp)", 555);
    } else if(curMap == 34) { //rhoeyce magic shop
      script.addText("I can teach two magic levels if you are strong enough.");
      script.addChoice("Nothing", 0);
      script.addChoice("Level 5: Firebomb (750 gp)", 554);
      script.addChoice("Level 7: Acid Rain (1500 gp)", 556);
    } else if(curMap == 42){ //wisp artifact 3
      script.addText("The card reads, 'Do not touch! Highly explosive. Thought to cause chronic impotence, too.'");
    } else {
      script.addText("I can teach two magic levels if you are strong enough.");
      script.addChoice("Nothing", 0);
      script.addChoice("Level 2: Burn (150 gp)", 551);
      script.addChoice("Level 3: Great Heal (350 gp)", 552);
    }
    break;
  case 71: //QTower merchant
    if(curMap == 0) {//aaryak greeter
      script.addText("Welcome to Aaryak!");
    } else if(curMap == 42) { //wisp welcome guy
      script.addText("Welcome to Wisp! Be sure to visit our museum located in the center of the city.");
    } else {
      if(eCave > 2) {
        script.addText("Now I can trade without fear of losing my life in the mountains!");
      } else {
        script.addText("I am a merchant and my route requires I cross the mountains. But the mountain passes are very dangerous.");
        script.addText("That is why I support Lord Ithis's tunnel project. I may be ruined if I fail to make it through the mountain passes this season.");
      }
    }
    break;
  case 72:
    if(curMap == 21) { //laendlich soldier
      script.addText("I retired from the service years ago. Farming is the only way I can get by these days.");
    } else if(curMap == 29){ //npost banter
      setRandomBanter();
    } else if(curMap == 39 || curMap == 40){ //tristen soldier
      script.addText("Watch your hit points in battle and cast healing spells if needed.");
    } else {
      if(eCave > 2) script.addText("The spiders are gone! And best of all, I get two weeks vacation!");
      else script.addText("My squad will be leaving for the cave soon. I might not be here the next time you visit.");
    }
    break;
  case 73: //QTower common talk
    if(curMap == 29){ //npost soldier
      script.addText("I go on leave soon. Maybe I'll visit Castle Garrison.");
    } else {
      r=rand() % 4;
      if(r == 0) script.addText("Many years ago, after the breaking of the continents, the dwarves moved underground. Nobody has seen them since then.");
      else if(r == 1) script.addText("Spiders are tough creatures. You might need many levels before you can defeat them.");
      else if(r == 2) script.addText("250 years ago, a piece of Greyor's medallion was found encased in a magic crystal. It was on display in Wisp's museum for 50 years before it mysteriously disappeared.");
      else script.addText("General Ryldar is Quinine Tower's best fighter.");
    }
    break;
  case 74: //QTower common talk
    if(curMap == 0){ //aaryak guild guy
      if(eFirewand > 6 && eFirewand < 15){
        script.addText("You've come looking for sulfur. I'd be happy to sell some to you, but I haven't got any right now. Sulfur is mined in the hills to the east. But dark goblins live there. They don't like us miners.");
        script.addText("We mine anyway and a small regiment of guards protects us. Unfortunately, the guards were caught by surprise in a raid on our last trip. The captain, Gawain, was captured.");
        script.addText("Without him to lead the guards we cannot mine in the hills. If you could rescue him, we could continue mining. You should be able to sneak into the goblin stronghold at night.");
        if(eFirewand == 7) eFirewand=8;
      } else if(eFirewand == 15){
        script.addText("Thank you for saving Gawain. He's already back on patrol. And here is some sulfur from the latest shipment. You can have it for free.");
        script.addText("He also left this note:               `I know that to beat Greyor takes a lot of courage. But it also requires a good weapon. The Magic Sword is just that weapon. It was owned by one of my ancestors, a great warrior.");
        script.addText("During a great battle he was slain and the magic sword was buried with his body. The grave can be found at the base of a mountain peak surrounded by a small forest. Search among those forest tress to find the sword.'");
        eFirewand=16;
      } else {
        script.addText("Welcome to the Miner's Guild. Not much is happening around town right now. Feel free to explore and check out the various shops in town.");
      }
    } else if(curMap == 19){ //garrison cook
      script.addText("Welcome to the dining hall. I can cook anything just the way you like it. Stop by whenever you feel hungry.");
    } else if(curMap == 38){ //temple dude
      script.addText("This temple is a resting place for soldiers traveling to and from the Northern Post.");
    } else if(curMap == 39 || curMap == 40) {//tristen random banter
      setRandomBanter();
    } else if(curMap == 42){ //wisp museum dude
      script.addText("Welcome to the museum. We have many rare historical artifacts. One is missing, however. We used to have an actual piece of Greyor's medallion trapped in a slab of magic crystal.");
      script.addText("But it was stolen from us by a group of dwarves. They brought it to their hidden city. I'm afraid we'll never see it again.");
    } else {
      r=rand() % 4;
      if(r == 0) script.addText("Many years ago, after the breaking of the continents, the dwarves moved underground. Nobody has seen them since then.");
      else if(r == 1) script.addText("Spiders are tough creatures. You might need many levels before you can defeat them.");
      else if(r == 2) script.addText("250 years ago, a piece of Greyor's medallion was found encased in a magic crystal. It was on display in Wisp's museum for 50 years before it mysteriously disappeared.");
      else script.addText("General Ryldar is Quinine Tower's best fighter.");
    }
    break;
  case 75: //Dead soldier
    script.addText("...");
    break;
  case 76:
    script.addText("There are too many of them!");
    break;
  case 77:
    script.addText("Find General Ryldar. He went to kill the demon spider on his own, but he'll need your help!");
    break;
  case 78: //Ryldar
    if(curMap == 29){ //npost knight 2
      script.addText("Rumor has it that a piece of Greyor's medallion is trapped behind a wall of ice in the dungeon near Laendlich.");
    } else {
      script.addText("Greetings brave warrior! I am General Ryldar, and I need your help. I cannot defeat the demon spider. You must try in my place. Here, drink this magic potion. It will restore your health and magic.");
      hero.hp=hero.maxHP;
      hero.mp=hero.maxMP;
    }
    break;
  case 79: //Demon spider boss
    r=battle.fight(18);
    if(r == 2){
      curMap=6;
      eBattleNum=0;
      eCave=3;
      hero.medallion++;
      script.addText("You find a piece of Greyor's medallion!");
    } else if(r == 3) {
      death();
    }
    break;
  case 98:
    if(curMap == 0){ //aaryak ferryman
      script.addText("Going to Rhoeyce? The ferry is free.");
      script.addChoice("No", 0);
      script.addChoice("Yes", 218);
    } else if(curMap == 21){
      script.addText("Need to cross the channel? I'm headed that way, so I won't charge you.");
      script.addChoice("No", 0);
      script.addChoice("Yes", 220);
    } else if(curMap == 34){
      script.addText("Going to Aaryak? The ferry is free.");
      script.addChoice("No", 0);
      script.addChoice("Yes", 219);
    } else if(curMap == 39 || curMap == 40 || curMap == 42) { //random banter
      setRandomBanter();
    }
    break;
  case 101:
    if(curMap == 0) { //aaryak farmer
      script.addText("It's going to be a great harvest this year.");
    } else if(curMap == 19 || curMap == 41){ //garrison,trok random banter
      setRandomBanter();
    }
    break;
  case 102:
    if(curMap == 19) { //garrison welcome
      script.addText("Welcome to Castle Garrison! This is the capital of the lower region.");
    }
    break;
  case 103:
    if(curMap == 16) { //dwarf city dude
      script.addText("I don't know why any dwarves would want to live above ground, but some of them do.");
    } else if(curMap == 21) { //laendlich dwarf
      if(eHorn < 1 || eHorn>2){
        script.addText("Not all dwarves live underground. In fact, generations ago dwarves lived above ground with humans. Today our peoples do not get along well, but my love for agriculture keeps me above ground.");
      } else {
        script.addText("Yes, I can help you get to the City of Dwarves. It's magically hidden to keep humans out, but the urgency of your mission allows me to help you. Seek Bonebreaker in one of the northern towns to get the magical horn.");
        eHorn=2;
      }
    }
    break;
  case 104:
    if(curMap == 16) { //dwarf city guard
      script.addText("Stay out of trouble, human!");
    } else if(curMap == 29) {//bonebreaker
      if(eHorn < 2){
        script.addText("(Bonebreaker)                         Surprised to see a dwarf leading the forces here? Who did you expect? Only a dwarf would have the strategic capabilites to run a fort like this.");
      } else if(eHorn == 2){
        script.addText("(Bonebreaker)                         So you need to get into the dwarves' city...");
        script.addText("(Bonebreaker)                         First, you have to prove yourself. There's a goblin raiding party coming to attack. You must help us defend this fort. Go to the inn and rest. Come back when you're ready to fight.");
        eHorn=3;
      } else if(eHorn == 3){
        script.addText("(Bonebreaker)                         Get ready, here come the goblins!");
        script.addChoice("Gwaaarrrgghhlllgh!", 222);
      } else if(eHorn > 3){
        script.addText("(Bonebreaker)                         Thank you for your help. The Dwarven Tunnels are in the southern part of the continent. Just blow the horn to enter.");
        eHorn=5;
      }
    }
    break;
  case 105: //dwarf healer
    script.addText("I can bandage your wounds for 50 gold.");
    script.addChoice("No", 0);
    script.addChoice("Yes", 573);
    break;
  case 106: //dwarf leader
    if(eHelpDwarf == 1){
      script.addText("End the perils that exist within our caves, and the medallion is yours should you be able to free it from the crystal.");
    } else if(eHelpDwarf == 2){
      script.addText("Thank you for saving my people. Monsters still inhabit the upper caves. They must be the result of some other disturbance. We can handle them ourselves, but you should be wary of them when traveling out of the caves.");
      script.addText("Our bargain holds and you may have the medallion piece. Since it is encased in a magical crystal, you cannot possibly carry it.");
      script.addText("Return when you have discovered a way to break the crystal to claim your reward.");
      eHelpDwarf=3;
    } else if(eHelpDwarf > 2 && eHelpDwarf < 7){
      script.addText("Return when you have discovered a way to break the crystal to claim your reward.");
    } else if(eHelpDwarf == 7){
      script.addText("(You show your device to the Dwarven King. The King has the crystal dragged to a great hall where a skylight lets in a beam of natural light from the surface.)");
      script.addText("(You position the apparatus, aim the beam, and shatter the magical crystal with an ear-splitting eruption. When the smoke clears, the King retrieves the medallion piece from the shards.)");
      script.addText("This medallion piece is yours to keep, as was the bargain. You may keep the magic horn given to you and you are welcome in our caves whenever you desire to return.");
      hero.medallion++;
      eHelpDwarf=8;
    } else if(eHelpDwarf == 8){
      script.addText("Good journey brave warrior!");
    } else {
      script.addText("So Bonebreaker allowed you to enter. As much as I trust his judgement, I don't have time for your problems. We have problems of our own down here.");
      script.addText("Our caves have always been infested with Greyor's hideous creatures. We didn't care. It was still better than living above ground with you humans and your greed, plagues, wars, and violent natures.");
      script.addText("As a result, our people became the greatest fighters on this planet. But even that isn't enough. Each year the creatures get stronger and stronger, and each year our population suffers.");
      script.addText("We don't know the source of these creatures, but they are stronger in the depths below us. None of the fighters we send down ever return. That is why we stole the piece of medallion.");
      script.addText("We need the piece to stop the beasts. I know the medallion may not help at all, but we must try something! I've said enough. You are dismissed and may return to the surface.");
      script.addChoice("Wait! I will stop the creatures.", 223);
      script.addChoice("I'm out of here!", 0);
    }
    break;
  case 107: //dwarf quest
    if(eHelpDwarf == 1){
      script.addText("Before you is a portal to a gruesome world. Spewing from it are the fiends that plague these caves. You hack your way through the creatures and smash the gemstones from which the portal arises.");
      eHelpDwarf = 2;
    }
    script.addText("The portal closes to a solid blue door. Yellow rays of magical residue remain on the outside, but never again will the portal be opened.");
    break;
  case 108: //floating city banter
    if(curMap == 17 || curMap == 18) {
      setRandomBanter();
    }
    break;
  case 109: //black wizard
    if(curMap == 17 || curMap == 18) {
      script.addText("(Head of Council)                     Sorry, but I'm really busy at this time.  Come back later.");
      if(eFirewand == 2){
        script.addText("What! You've come to ask for Mizrak's return? Are you mad or just stupid? His experiments with fire spells nearly blew the city out of the Astral Plane!");
        script.addText("(After much begging...)               Very well. He may return to his lab as long as his fire experiments are restricted to the wand he is making for you.");
        eFirewand=3;
      }
    }
    break;
  case 110: //blue wizard
    if(curMap == 17 || curMap == 18) {
      script.addText("The patches of grass by the entrance are the physical link between the earth and the Floating City. Walk on them to return to your world.");
    }
    break;
  case 111: //temple priest
    if(curMap == 17 || curMap == 18) { //yellow wizard
      script.addText("The name Floating City of Mages is rather misleading. The city does not actually float above the world. Instead it drifts along the astral plane where we can practice our magic with the least astral friction.");
    } else {
      script.addText("As a priest, my duties are very important. I know many skills other than healing.");
      if(eFirewand == 17){
        script.addText("(You show him the firewand.)          Yes, I can activate that for you. Here you go. It should work now.");
        eFirewand = 18;
      }
    }
    break;
  case 113: //greenbook
    script.addText("The Art of Magic");
    script.addText("An informative, yet boring book on how to focus sharply the intricacies of spellcasting.");
    break;
  case 112: //redbooks
    if(cam.getTileX() == 21){
      script.addText("BOOK OF ARMS                            - a listing of weapon strengths");
      script.addText("Dagger = 1                             Club = 2                             Mace = 3                            Spear = 5                            Axe = 6");
      script.addText("Shortsword = 8                         Great Axe = 10                       Longsword = 12                      Halberd = 15");
      script.addText("Magic Axe = 17                         Magic Sword = 20");
    } else if(cam.getTileX() == 22){
      script.addText("A Tale of Magic                       - Glancing through this novel proves it to be a worthless piece of crap.");
    } else if(cam.getTileX() == 23){
      script.addText("Theory of The Breaking                  - by Gelehrt");
      script.addText("...believed that Greyor split the world by creating a medallion that mimicked the natural astral powers inside the planet. By focusing astral power from the strongest source, The Rift, and breaking the medallion,");
      script.addText("Greyor thus broke the world. The Rift was the necessary location, as it is where the flow of astral powers inside the earth are the strongest... By rejoining the medallion pieces, one could reunite the continents.");
      script.addText("However, the fixed piece of jewelry must be returned to The Rift, so the natural healing powers of the planet can undo the evil spell... Unfortunately, the tremors of The Breaking caused");
      script.addText("great mountains above The Rift. Only by journeying through the caves to the pits below, can one reach The Rift...");
    } else {
      script.addText("BOOK OF ARMOR                           - a listing of armor strengths");
      script.addText("SUITS OF ARMOR                        Tunic = 1          Leather Vest = 2   Studded Vest = 3   Ring Mail = 4      Scale Mail = 5     Chain Mail = 6     Partial Plate = 7  Full Plate = 8     Magic Armor = 10");
      script.addText("SHIELDS                               Buckler = 1   Small = 2   Large = 3   Magic = 5");
      script.addText("HELMS                                 Leather Cap = 1   Chain Coif = 2      Plate = 3   Magic = 5");
    }
    break;
  case 114: //blackbook
    if(cam.getTileY() == 25) script.addText("Garnod's Boof of Poetry");
    else script.addText("Raising the Dead for Dummies");
    break;
  case 115: //purplebook
    script.addText("Proof of the Existence of Diamant       - by Einsiedler");
    script.addText("Although no proof has ever been given as to the actual existence of Diamant, my research leads me to believe it exists inside the Crystal Mountains. I may be chasing myths, but I feel this");
    script.addText("mystery must be solved. I am preparing an expeditionary force to  explore the caves. Included in my group are great scientists such as...  - The book tapers off into the details of the expedition.");
    break;
  case 116: //laendlich dungeon ice
    if(eFirewand < 18) {
      script.addText("You encounter a magical block of ice. No matter how much you chip at it with your weapon, it still reforms. You need to find something that will remove the barrier.");
      if(eFirewand == 0) eFirewand=1;
    } else {
      eFirewand=19;
      script.addText("To point the firewand at the magical ice and a stream of flame engulfs it. After on a few seconds, the ice completely melts leaving only a puddle.");
      curMap=43; cam.setPos(23, 22); playerDir = 2; eBattleNum=5;
    }
    break;
  case 117: //gawain in dgfort
    if(cam.getTileY()!=15){
      showText=false;
      return;
    }
    script.addText("Get me out of here! They're going to eat me soon. And it stinks of piss in here. I can hardly breath. Get the key from the guard around the corner.");
    if(eFirewand == 14){
      script.addText("You've got the key! Great! Get me out of here!");
      eFirewand = 15;
      script.addText("(You escape with Gawain and hurry back to Aaryak in the dark. The Guild Master was waiting to meet you at the gate.");
      script.addText("(The Guild Master speaks to you.)     Thank you! Come talk to me in the  morning. But for now, get some rest.");
      curMap=0; cam.setPos(47, 19); playerDir=1; eBattleNum=0;
    }
    break;
  case 118: //dark goblin
    script.addText("Gwaarrrghh! A human! Guards! Guards!");
    break;
  case 119: //dark goblin guard
    if(eFirewand < 13){
      script.addText("Human out of cell? No matter. Who care? Not me. Me hungry. Can't get no food, though. Cook no make food.");
      script.addText("(You manage to strike a deal...)      Sure! You can have key if you get me dinner! Food more important than shiny piece of metal.");
      if(eFirewand == 8) eFirewand = 9;
    } else if(eFirewand == 13){
      script.addText("(You give the guard his meal.)        Food! Deal is deal. Here da key.");
      eFirewand = 14;
    } else {
      script.addText("Nom. Nom.");
    }
    break;
  case 120:  //dark goblin cook
    if(eFirewand < 9 || eFirewand > 12) {
      script.addText("I'm not cooking you today, human. Get back in your cell.");
    } else if(eFirewand == 9 || eFirewand == 10 || eFirewand == 11) {
      script.addText("(You ask for food for the guard)      No! I will not cook for him. He is in Glarg's patrol. Glarg got mad and killed my servants! I no cook for his patrol unless he give his necklace.");
      eFirewand = 10;
    } else if(eFirewand == 12){
      script.addText("Oooo! The necklace. Ok. I cook now. Here is food for guard.");
      eFirewand = 13;
    }
    break;
  case 121: //garrison king
    script.addText("Don't bother me, I'm busy! Can't you see I'm trying to run a nation?");
    break;
  case 122: //glarg
    if(eFirewand < 10 || eFirewand > 12) {
      script.addText("Go back to cell, human. I no feel like killing you now.");
    } else if(eFirewand == 10 || eFirewand == 11 || eFirewand == 12) {
      script.addText("Cook want necklace? Then give food? *GROAN* I lost necklace in gambling with Thrash. Get necklace and give to Cook. I hungry.");
      if(eFirewand == 10) eFirewand = 11;
    }
    break;
  case 123: //thrash
    if(eFirewand != 11) {
      script.addText("Stupid human. Go back to cell.");
    } else {
      script.addText("You want necklace? 100 gold. Ok?");
      if(hero.gold < 100){
        script.addText("You no have 'nuf gold. You no get necklace. Now go.");
      } else {
        script.addText("(You have no choice but to give Thrash the gold).                            Hah! I rich! Here, take stupid necklace.");
        hero.gold-=100;
        eFirewand = 12;
      }
    }
    break;
  case 124: //dungeon medallion piece
    if(hero.medallion==3 || (hero.medallion==2 && eHelpDwarf<8)){
      script.addText("You already have the medallion piece. All that remains on the podium is some glowing astral residue left behind by the piece.");
    } else {
      script.addText("You behold on the alter the third peice of Greyor's evil medallion. Gently, you remove it from its resting place. After a moment's reverence, you decide it is time to hurry out of this hellish pit.");
      hero.medallion++;
    }
    break;
  case 125: //dungeon medallion piece
    eGreyor=2;
    curMap=28; cam.setPos(11, 8); playerDir=0; eBattleNum=0;
    hero.hp=hero.maxHP;
    hero.mp=hero.maxMP;
    script.addText("There is a brilliant flash and you pass out...");
    script.addText("You awaken to a great quaking inside the cave. The walls start to collapse. You quickly shake off your stupor and rush out of the cave.");
    script.addText("The cave collapses behind you just as you escape. You look around. The world is suffering the ravages of the earthquake. You rush to the shore and look in the distance.");
    script.addText("It is there and it is moving swiftly: Greyor's continent. The one that he created years ago.  In a few hours it is done. You've rejoined the continents.");
    script.addText("But there is one more thing to be done. You must stop Greyor. You return to the Northern Post to rest and gather supplies.");
    break;

  case 130: //already have magic sword
    script.addText("There is no reason to remain in the clearing.");
    break;
  case 131: //magic sword
    script.addText("You search the trees at the base of the mountain as Gawain had instructed. It takes a few hours, but you come to a clearing. There before you stands a massive sword, point first in the dirt of a grave.");
    script.addText("It seems amazing that it shimmers despite what has to have been centuries exposed to the elements. Placing your hand upon the hilt sends a warm shock through your arm.");
    script.addText("The blade glows an eerie blue and your head feels weird, almost as if something told you you have permission to take the sword.");
    strcpy(hero.weapon, weapons[10].name);
    hero.wStr=weapons[10].value;
    break;

  case 150:
    script.addText(".");
    script.addText("You cross over to Greyor's side. His castle towers high in the distance. Hideous screams erupt from within.");
    script.addText("A chill travels down your spine as you approach. All you can think of is your own gruesome death.");
    eGreyor=4;
    curMap=20; cam.setPos(12, 8); playerDir=0; eBattleNum=0;
    break;
  case 151:
    script.addText("(You ascend the keep's steps to find Greyor in his chamber at the top.)");
    script.addText("So you're the one who has ruined my plans. It seems I must kill you now.");
    script.addText("With that, he summons three black dragons!");
    script.addChoice("Attack!", 152);
    break;
  case 152:
    multiFight=0;
    while(multiFight < 3){
      battle.noRun=true;
      r=battle.fight(17);
      battle.noRun=false;
      if(r == 3) {
        death();
        break;
      } else{
        multiFight++;
      }
    }
    if(multiFight<3) break;
    script.addText("The last dragon falls before you. Greyor turns to you and snarls.");
    script.addText("'No! You couldn't have! I'll finish you off myself!'                      With that he lifts his arm to send a bolt of energy through your heart.");
    script.addText("There's no way you can get to him in time. Desperately you turn and smash the crystal ball, hoping for a distraction.");
    script.addText("`Noooooooo! You've killed us both!'   The air in the room starts to thicken and flicker with sparks. You dive down the stairs just as the top of the keep explodes into a ball of flame.");
    eGreyor=5;
    break;
  case 153:
    script.addText(".");
    script.addText("You rush out of the keep as it crumbles behind you. As you run, debris rains down from above. Battered and bleeding, you stagger out of the dark castle. With a heavy sigh, you black out...");
    script.addText("You wake up some time later. Your head hurts and you notice it is bandaged. You're moving. You open your eyes and see General Ryldar walking beside the horse over which your body is slung.");
    script.addText("`We mustered up a force the moment you joined the continents, but it looks like you beat us to Greyor. It's a good thing we found you. You might have bled to death. Get some rest, it's a long journey.'");
    eGreyor=7;
    break;
  case 154:
    script.addText("A week later, you kneel before the throne of the King.");
    curMap=19; cam.setPos(27, 11); playerDir=0; eBattleNum=0;
    eGreyor=9;
    break;
  case 155:
    script.addText(".");
    s="Thank you for reuniting the continents and ending the evil brought forth by Greyor. As a gift to you, I pronounce you Sir ";
    s+=hero.name;
    s+="!";
    script.addText(s);
    script.addText("Your name shall be remembered until the end of time.");
    script.addText("You return to your quaint farm outside of Meadow.");
    script.addText("You have done it, the world is safe. It seems your adventures have ended.");
    script.addText("Only time will tell...");
    eGreyor=11;
    break;

  case 201:
    script.addText("Legend says that the evil Warlord Greyor created the medallion by magical means and used it to split the world into two continents - ours");
    script.addText("and his, which he is using to build an army of evil creatures that will conquer the world so he can proclaim himself ruler. Of course, this legend");
    script.addText("is over a thousand years old. No one could live that long. But, then again, monsters are roaming the land in greater numbers lately.");
    script.addChoice("Tell me more.",203);
    script.addChoice("Nice story, but I must be going.",0);
    break;
  case 202:
    script.addText("Sorry, you're underage. Come back in a few years.");
    script.addChoice("Okay. How about something else?",46);
    script.addChoice("Bye.",0);
    break;
  case 203:
    script.addText("If I remember right, it is said that after the land was split, the medallion broke into three pieces and were scattered throughout our continent.");
    script.addText("The only way to stop Greyor would be to find the three pieces and rejoin the continents.");
    script.addChoice("Can't a boat cross the ocean?",204);
    script.addChoice("Where can I find the pieces?",205);
    script.addChoice("Whatever you say. Later.",0);
    break;
  case 204:
    script.addText("Many people have tried to cross the ocean, none have ever returned. Only crazy people try to travel that way.");
    script.addChoice("Tell me more of the medallion.",205);
    script.addChoice("Bye.",0);
    break;
  case 205:
    script.addText("I don't know where the pieces are, but maybe someone in Quinine Tower can help you. It is to the west across the river, but the bridge was washed out");
    script.addText("during the spring flood. You'll have to speak to Reyd the carpenter to have a new one built.");
    script.addChoice("Thanks. See you later.",0);
    if(eBridge == 0) eBridge=1;
    break;
  case 206:
    script.addText("I'd love to do it for you, but my assistant, Mert, lost the key to my storage shed. I can't get my tools.");
    script.addText("If you can find that key, I'll build the bridge for you.");
    script.addChoice("How do I find the key?",207);
    script.addChoice("Nevermind.",0);
    break;
  case 207:
    script.addText("Ask around town. I suggest starting with Mert.");
    break;
  case 208:
    script.addText("My key! Thanks, I'll get started on the bridge right away.");
    eBridge=3;
    break;
  case 209: 
    if(eEmma < 1) eEmma=1;
    script.addText("I don't know. I put it in my pocket right before I took my little cousin, Emma, outside to play. The next time I looked in my pocket it was gone.");
    script.addChoice("Where's Emma?",210);
    script.addChoice("Bye.",0);
    break;
  case 210:
    script.addText("She's probably still in the park behind the tavern.");
    break;
  case 211:
    script.addText("Then I guess it's okay to talk to you. What do you need?");
    script.addChoice("I need a key your cousin lost.",212);
    script.addChoice("Nothing. Bye.", 0);
    break;
  case 212:
    if(eEmma == 1) eEmma=2;
    script.addText("I found it on the ground and I gave it to Tyren as he was heading to the tavern.");
    break;
  case 213:
    eAle=1;
    script.addText("Yes, I have the key. But seeing that you need it, it must be worth something. Since I'm out of money, why don't you just buy me another ale and I'll give you the key. Sound fair?");
    script.addChoice("Okay, I'll be right back.",0);
    break;
  case 214:
    eBridge=2;
    eAle=3;
    eEmma=3;
    script.addText("Here's your key.  Now let me drink in peace.");
    break;
  case 215:
    script.addText("Me un gut tip is to gain lefels. Lefels make de difference in big battles. If ye can't vin in a battle, gain lefels und try again.");
    break;
  case 216:
    if(hero.level<3){
      script.addText("Vell der, jung un, ye look scruny. Fight und gain lefels. Denn come back.");
    } else {
      script.addText("Ye vill be gut varrior, ya? I buy ye ale now.");
      eAle=2;
    }
    break;
  case 217:
    eCave=2;
    script.addText("You will help? 'Tis suicide for sure, but I will not turn down volunteers. You may join General Ryldar's squad that is being deployed at this very moment. Meet him in the tunnel.");
    script.addText("If you succeed, you may keep the medallion piece in return for your services.");
    break;
  case 218:
    changeMap(34);
    showText=false;
    return;
  case 219:
    changeMap(0);
    showText=false;
    return;
  case 220:
    changeMap(50);
    showText=false;
    return;
  case 221:
    if(hero.gold<50){
      script.addText("Come back when you have the money!");
    } else {
      hero.gold-=50;
      changeMap(51);
      showText=false;
      return;
    }
    break;
  case 222: //northern post goblin battle
    multiFight=0;
    while(multiFight < 5){
      r=battle.fight(6);
      if(r == 1){
        script.addText("(Bonebreaker)                         Whoa, that was rough. You had to fall back, eh? Well, we still held them off, but too many got away. You will have to help us when they return.");
        break;
      } else if(r == 3) {
        death();
        break;
      } else{
        multiFight++;
      }
    }
    if(multiFight<5) break;
    script.addText("(Bonebreaker)                         You held them off! Great job! Here's the horn, as I promised. Rest up then speak with me to find out how to use it.");
    eHorn=4;
    break;
  case 223: //dwarf leader quest
    script.addText("Interesting. The medallion would be worthless to us if you could end our troubles, and I would gladly give it to you.");
    script.addText("But it does not matter. The medallion is encased in a crystal even our strongest warriors cannot break.");
    script.addText("End the perils that exist within our caves, and the medallion is yours should you be able to free it from the crystal.");
    eHelpDwarf=1;
    break;
  case 224: //einsiedler answer
    for(r=0; r < (int)userText.size();r++){
      userText[r]=toupper(userText[r]);
    }
    if(userText.compare("EINSIEDLER")==0){
      script.addText("Hehehe! Correct! In 50 years, not one soul guessed it! I'll tell you where the Diamant is.");
      script.addText("(He reaches into his robe and pulls out a sparkling jewel.) Here it is. Diamant is the rarest substance in the world. It takes millennia to form just one small crystal.");
      script.addText("This is the only Diamant in these caverns. It took me half my life to find it. It's yours now. Use it wisely. Farewell!");
      eHelpDwarf=6;
    } else {
      script.addText("Incorrect. The Diamant remains mine!");
    }
    break;

  case 500: buyWeapon(0); return;
  case 501: buyWeapon(1); return;
  case 502: buyWeapon(2); return;
  case 503: buyWeapon(3); return;
  case 504: buyWeapon(4); return;
  case 505: buyWeapon(5); return;
  case 506: buyWeapon(6); return;
  case 507: buyWeapon(7); return;
  case 508: buyWeapon(8); return;
  case 509: buyWeapon(9); return;
  case 510: buyWeapon(10); return;
  case 520: buyArmor(0); return;
  case 521: buyArmor(1); return;
  case 522: buyArmor(2); return;
  case 523: buyArmor(3); return;
  case 524: buyArmor(4); return;
  case 525: buyArmor(5); return;
  case 526: buyArmor(6); return;
  case 527: buyArmor(7); return;
  case 528: buyArmor(8); return;
  case 530: buyHelm(0); return;
  case 531: buyHelm(1); return;
  case 532: buyHelm(2); return;
  case 533: buyHelm(3); return;
  case 540: buyShield(0); return;
  case 541: buyShield(1); return;
  case 542: buyShield(2); return;
  case 543: buyShield(3); return;
  case 550: buySpell(0); return;
  case 551: buySpell(1); return;
  case 552: buySpell(2); return;
  case 553: buySpell(3); return;
  case 554: buySpell(4); return;
  case 555: buySpell(5); return;
  case 556: buySpell(6); return;
  case 557: buySpell(7); return;
  case 558: buySpell(8); return;
  case 559: buySpell(9); return;
  case 560: buySpell(10); return;
  case 570: buyRoom(10); return;
  case 571: buyRoom(20); return;
  case 572: buyRoom(40); return;
  case 573: buyRoom(50); return;
  case 574: buyRoom(60); return;
  case 575: buyRoom(100); return;
  case 576: buyRoom(30); return;
  case 577: buyRoom(90); return;

  case 600: buyWeaponB(0); return;
  case 601: buyWeaponB(1); return;
  case 602: buyWeaponB(2); return;
  case 603: buyWeaponB(3); return;
  case 604: buyWeaponB(4); return;
  case 605: buyWeaponB(5); return;
  case 606: buyWeaponB(6); return;
  case 607: buyWeaponB(7); return;
  case 608: buyWeaponB(8); return;
  case 609: buyWeaponB(9); return;
  case 610: buyWeaponB(10); return;
  case 620: buyArmorB(0); return;
  case 621: buyArmorB(1); return;
  case 622: buyArmorB(2); return;
  case 623: buyArmorB(3); return;
  case 624: buyArmorB(4); return;
  case 625: buyArmorB(5); return;
  case 626: buyArmorB(6); return;
  case 627: buyArmorB(7); return;
  case 628: buyArmorB(8); return;
  case 630: buyHelmB(0); return;
  case 631: buyHelmB(1); return;
  case 632: buyHelmB(2); return;
  case 633: buyHelmB(3); return;
  case 640: buyShieldB(0); return;
  case 641: buyShieldB(1); return;
  case 642: buyShieldB(2); return;
  case 643: buyShieldB(3); return;
  case 650: buySpellB(0); return;
  case 651: buySpellB(1); return;
  case 652: buySpellB(2); return;
  case 653: buySpellB(3); return;
  case 654: buySpellB(4); return;
  case 655: buySpellB(5); return;
  case 656: buySpellB(6); return;
  case 657: buySpellB(7); return;
  case 658: buySpellB(8); return;
  case 659: buySpellB(9); return;
  case 660: buySpellB(10); return;

  case 700:
    showLoad=true;
    showText=false;
    selection=0;
    return;
  case 701:
    stop=true;
    showText=false;
    return;

  default:
    script.addText("Error with text.");
    break;
  }
  
  showText = true;

}

void CDarkages::title(){
  SDL_Event e;
  int chMap = -1;
  bool anim = false;
  bool moving = false;
  stop = false;

  //music.playSong(TitleSong);

  while(true){

    while(SDL_PollEvent(&e) != 0) {
      if(e.type == SDL_TEXTINPUT){
        if(showTextInput && userText.size()<16 && e.text.text[0] != ' ') userText+=e.text.text;
      } else if(e.type == SDL_KEYDOWN)  {
        //Select surfaces based on key press
        switch(e.key.keysym.sym)  {
        case SDLK_UP:
          if(showLoad){
            if(selection == 0) selection=6;
            else selection--;
            break;
          }
          if(selection == 0) selection=3;
          else selection--;
          break;
        case SDLK_DOWN:
          if(showLoad){
            if(selection == 6) selection=0;
            else selection++;
            break;
          }
          if(selection == 3) selection=0;
          else selection++;
          break;
        case SDLK_ESCAPE:
          break;
        case SDLK_q: stop = true; break;
        case SDLK_BACKSPACE:
          if(showTextInput && userText.size()>0) userText.pop_back();
          break;
        case SDLK_RETURN:
        case SDLK_SPACE:
          if(showLoad){
            showLoad=false;
            if(selection > 0) {
              loadGame(selection - 1);
              run();
            } else {
              selection=1;
            }
            break;
          }
          if(selection == 0) {
            if(newGame()) {
              fadeIn=255;
              script.clear();
              script.addText("Your adventure begins!");
              showText=true;
              run();
            }
          } else if(selection == 1) {
            showLoad=true;
          } else if(selection ==2){
            COptions opt(display, &music, &font, &gfx, conf);
            opt.run();
          } else if(selection == 3) {
            stop=true;
          }
          break;
        default: break;
        }
      }
    }

    if(stop) break;
    renderTitle();

  }

}

void CDarkages::travel(int index){
  if(index > 0) hero.mp-=15;
  switch(index){
  case 1:
    curMap = 27; cam.setPos(24, 24); playerDir = 3; eBattleNum=0;
    break;
  case 2:
    curMap = 32; cam.setPos(16, 23); playerDir = 3; eBattleNum=0;
    break;
  case 3:
    curMap=42; cam.setPos(29, 33); playerDir=3; eBattleNum=0; //wisp
    break;
  case 4:
    curMap=41; cam.setPos(21, 39); playerDir = 3; eBattleNum=0;
    break;
  case 5:
    cam.setPos(23, 5); playerDir=0; eBattleNum=0;
    if(eFirewand < 4) curMap=39;
    else curMap=40;
    break;
  case 6:
    curMap=29; cam.setPos(8, 16); playerDir = 2; eBattleNum=0;
    break;
  case 7:
    curMap=19; cam.setPos(27, 45); playerDir=3; eBattleNum=0;
    break;
  case 8:
    curMap=1; cam.setPos(17, 29); playerDir=3; eBattleNum=0; //amber
    break;
  case 9:
    curMap = 21; cam.setPos(8, 22); playerDir = 2; eBattleNum=0; //laendlich
    break;
  case 10:
    curMap=34; cam.setPos(21, 28); playerDir = 2; eBattleNum=0; //rhoeyce
    break;
  case 11:
    curMap=0; cam.setPos(47, 19); playerDir=1; eBattleNum=0;
    break;
  case 12:
    if(eFirewand<4) {
      curMap = 17; cam.setPos(24, 35); playerDir = 0; eBattleNum=0;
    } else {
      curMap = 18; cam.setPos(24, 35); playerDir = 0; eBattleNum=0;
    }
    break;
  default:
    break;
  }
}
