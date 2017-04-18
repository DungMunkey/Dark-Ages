#include "CBattle.h"

CBattle::CBattle(){
  display=NULL;
  font=NULL;
  gfx=NULL;
  hero=NULL;
  noRun=false;
}

CBattle::~CBattle(){
  display=NULL;
  font=NULL;
  gfx=NULL;
  hero=NULL;
}

void CBattle::castSpell(int index){
  if(index == -1) return;

  char str[128];
  string s;
  int i;
  bool lowMP=false;
  text.clear();

  switch(index){
  case 0:
    if(hero->mp < 2) {
      lowMP=true;
      break;
    }
    hero->hp+=5;
    hero->mp-=2;
    if(hero->hp > hero->maxHP) hero->hp=hero->maxHP;
    text.push_back("You gain 5 HP.");
    break;
  case 1:
    if(hero->mp < 5) {
      lowMP=true;
      break;
    }
    curMon.hp-=10;
    hero->mp-=5;
    sprintf(str, "The %s is burned for 10 damage.", curMon.name);
    s=str;
    text.push_back(s);
    break;
  case 2:
    if(hero->mp < 7) {
      lowMP=true;
      break;
    }
    hero->hp+=30;
    hero->mp-=7;
    if(hero->hp > hero->maxHP) hero->hp=hero->maxHP;
    text.push_back("You gain 30 HP.");
    break;
  case 3:
    if(hero->mp < 5) {
      lowMP=true;
      break;
    }
    hero->mp-=5;
    i=rand() % 100;
    if(i<35) {
      text.push_back("The spell failed!");
      break;
    }
    curMon.hitp/=3;
    sprintf(str, "The %s loses sight of you.", curMon.name);
    s=str;
    text.push_back(s);
    break;
  case 4:
    if(hero->mp < 10) {
      lowMP=true;
      break;
    }
    curMon.hp-=25;
    hero->mp-=10;
    sprintf(str, "The %s is burned for 25 damage.", curMon.name);
    s=str;
    text.push_back(s);
    break;
  case 5:
    if(hero->mp < 12) {
      lowMP=true;
      break;
    }
    hero->hp+=60;
    hero->mp-=12;
    if(hero->hp > hero->maxHP) hero->hp=hero->maxHP;
    text.push_back("You gain 60 HP.");
    break;
  case 6:
    if(hero->mp < 18) {
      lowMP=true;
      break;
    }
    curMon.hp-=50;
    hero->mp-=18;
    sprintf(str, "The %s is corroded for 50 damage.", curMon.name);
    s=str;
    text.push_back(s);
    break;
  case 7:
    if(hero->mp < 25) {
      lowMP=true;
      break;
    }
    curMon.hp-=80;
    hero->mp-=25;
    sprintf(str, "The %s is burned for 80 damage. Strangely, it smells like bacon.", curMon.name);
    s=str;
    text.push_back(s);
    break;
  case 8:
    if(hero->mp < 20) {
      lowMP=true;
      break;
    }
    hero->hp=hero->maxHP;
    hero->mp-=20;
    text.push_back("You gain full HP.");
    break;
  case 9:
    if(hero->mp < 30) {
      lowMP=true;
      break;
    }
    hero->mp-=30;
    i=rand() % 100;
    if(i<50) {
      sprintf(str, "The %s slows, staggers, then ... recovers!", curMon.name);
      s=str;
      text.push_back(s);
      break;
    }
    curMon.hp=0;
    sprintf(str, "The %s slows, staggers, then ... crumbles to dust!", curMon.name);
    s=str;
    text.push_back(s);
    break;
  default:
    break;
  }

  if(lowMP){
    text.push_back("You don't have enought MP!");
  } else {
    text.push_back(" ");
    if(curMon.hp>0) monsterAttack();
  }

  showText=true;
}

int CBattle::fight(int index){

  if(index < 0) return 0; //don't accept invalid indexes

  SDL_Event e;
  selection=0;
  spellSelection=0;
  showSpell=false;
  showText=false;
  curMon=monsters[index];
  string s;

  int run=0;
  int win=0;

  setHeroSpells();

  while(true){

    while(SDL_PollEvent(&e) != 0) {
      if(e.type == SDL_KEYDOWN)  {
        //Select surfaces based on key press
        switch(e.key.keysym.sym)  {
        case SDLK_UP:
          if(showText) break;
          if(showSpell) {
            if(spellSelection > 0) spellSelection--;
            break;
          }
          if(selection > 0) selection--;
          break;
        case SDLK_DOWN:
          if(showText) break;
          if(showSpell) {
            if(spellSelection < (int)(spells.size()-1)) spellSelection++;
            break;
          }
          if(selection < 2) selection++;
          break;
        case SDLK_b: return 0;
        case SDLK_RETURN:
        case SDLK_SPACE:
          if(showText) {
            text.clear();
            showText = false;
            if(run == 1)run++;
          } else if(showSpell){
            showSpell=false;
            castSpell(spells[spellSelection]);
          } else {
            switch(selection){
            case 0: //attack
              playerAttack();
              if(curMon.hp > 0) monsterAttack();
              showText=true;
              break;
            case 1: //spell
              spellSelection=0;
              showSpell=true;
              //if(curMon.hp > 0) monsterAttack();
              //showText=true;
              break;
            case 2: //run
              if(noRun){
                s="There is no running from this battle!";
                text.push_back(s);
                s=" ";
                text.push_back(s);
                monsterAttack();
              } else {
                if(rand() % 100 < 50){ //escape
                  s="You escape!";
                  text.push_back(s);
                  run=1;
                } else { //fail
                  s="You can't get away!";
                  text.push_back(s);
                  s=" ";
                  text.push_back(s);
                  monsterAttack();
                }
              }
              showText=true;
              break;
            default:
              break;
            }
          }
          break;
        default: break;
        }
      }

    }

    if(!showText && curMon.hp<1 && win==0){
      getReward();
      win=1; //end battle with victory code
    }

    if(run == 2) return 1; //end battle with run code
    if(!showText && win == 1) return 2;
    if(!showText && hero->hp < 1) return 3;
    render();

  }
  return 0;  //shouldn't ever happen.
}

void CBattle::getReward(){
  showText=true;
  string s;
  char str[64];
  int st, dex, wis;

  sprintf(str, "The %s is dead.", curMon.name);
  s=str;
  text.push_back(s);

  sprintf(str, "You gain %d experience points.", curMon.expr);
  s=str;
  text.push_back(s);

  sprintf(str, "The find %d gold pieces.", curMon.gold);
  s=str;
  text.push_back(s);

  hero->expr+=curMon.expr;
  hero->gold+=curMon.gold;

  if(hero->level >= 100) return;

  if(hero->expr>=level[hero->level]){
  
    sprintf(str, "You gained an experience level!");
    s=str;
    text.push_back(s);
    hero->level++;
    st=rand() % 4 + 1;
    dex=rand() % 4 + 1;
    wis=rand() % 4 + 1;
    sprintf(str, "  Strength up +%d", st);
    s=str;
    text.push_back(s);
    sprintf(str, "  Dexterity up +%d", dex);
    s=str;
    text.push_back(s);
    sprintf(str, "  Wisdom up +%d", wis);
    s=str;
    text.push_back(s);
    sprintf(str, "  Hit Points up +%d", st*2);
    s=str;
    text.push_back(s);
    sprintf(str, "  Magic Points up +%d", wis);
    s=str;
    text.push_back(s);

    hero->maxHP+=2 * st;
    hero->maxMP+=wis;
    hero->str+=st;
    hero->dex+=dex;
    hero->wis+=wis;
  }
}

void CBattle::init(CDisplay* d, CFont* f, CGfxCollection* g, CPlayer* p){

  display=d;
  font=f;
  gfx=g;
  hero=p;

  //levels
  level[0]=0; level[1]=20; level[2]=40; level[3]=100; level[4]=220; level[5]=350; level[6]=600; level[7]=900;
  level[8]=1500; level[9]=2500; level[10]=4000; level[11]=5700; level[12]=7600; level[13]=9600; level[14]=11800;
  level[15]=14200; level[16]=16800; level[17]=20000; level[18]=23200; level[19]=26400; level[20]=29599;
  level[21]=33000; level[22]=36400; level[23]=40000; level[24]=43600; level[25]=47400; level[26]=51200;
  for(int i=27; i < 100; i++) level[i]=51200 + (i-26) * 4500;

  //spell names
  spellNames[0]="Heal (2 MP)";
  spellNames[1]="Burn (5 MP)";
  spellNames[2]="Great Heal (7 MP)";
  spellNames[3]="Blindness (5 MP)";
  spellNames[4]="Firebomb (10 MP)";
  spellNames[5]="Massive Heal (12 MP)";
  spellNames[6]="Acid Rain (18 MP)";
  spellNames[7]="Melt Flesh (25 MP)";
  spellNames[8]="Full Heal (20 MP)";
  spellNames[9]="Death (30 MP)";
  spellNames[10]="Travel";

  //monster stats
  monsters[0].hp=10; monsters[0].maxHP=10; monsters[0].ap=2; monsters[0].hitp=30; monsters[0].expr=10;
  monsters[0].gold=10; monsters[0].gfx=0; strcpy(monsters[0].name,"Orc");
  monsters[1].hp=25; monsters[1].maxHP=25; monsters[1].ap=5; monsters[1].hitp=40; monsters[1].expr=25;
  monsters[1].gold=20; monsters[1].gfx=2; strcpy(monsters[1].name, "Ferocious Armless");
  monsters[2].hp=40; monsters[2].maxHP=40; monsters[2].ap=8; monsters[2].hitp=50; monsters[2].expr=35;
  monsters[2].gold=25; monsters[2].gfx=4; strcpy(monsters[2].name, "Highway Bandit");
  monsters[3].hp=35; monsters[3].maxHP=35; monsters[3].ap=14; monsters[3].hitp=60; monsters[3].expr=40;
  monsters[3].gold=20; monsters[3].gfx=6; strcpy(monsters[3].name, "Acid Slime");
  monsters[4].hp=45; monsters[4].maxHP=45; monsters[4].ap=10; monsters[4].hitp=50; monsters[4].expr=50;
  monsters[4].gold=40; monsters[4].gfx=8; strcpy(monsters[4].name, "Giant Spider");
  monsters[5].hp=40; monsters[5].maxHP=40; monsters[5].ap=16; monsters[5].hitp=65; monsters[5].expr=100;
  monsters[5].gold=60; monsters[5].gfx=10; strcpy(monsters[5].name, "Wizard");
  monsters[6].hp=65; monsters[6].maxHP=65; monsters[6].ap=18; monsters[6].hitp=70; monsters[6].expr=150;
  monsters[6].gold=100; monsters[6].gfx=12; strcpy(monsters[6].name, "Goblin");
  monsters[7].hp=90; monsters[7].maxHP=90; monsters[7].ap=20; monsters[7].hitp=70; monsters[7].expr=200;
  monsters[7].gold=150; monsters[7].gfx=14; strcpy(monsters[7].name, "Skeleton");
  monsters[8].hp=130; monsters[8].maxHP=130; monsters[8].ap=25; monsters[8].hitp=75; monsters[8].expr=225;
  monsters[8].gold=200; monsters[8].gfx=16; strcpy(monsters[8].name, "Troll");
  monsters[9].hp=150; monsters[9].maxHP=150; monsters[9].ap=28; monsters[9].hitp=80; monsters[9].expr=300;
  monsters[9].gold=280; monsters[9].gfx=18; strcpy(monsters[9].name, "Death Knight");
  monsters[10].hp=200; monsters[10].maxHP=200; monsters[10].ap=33; monsters[10].hitp=80; monsters[10].expr=320;
  monsters[10].gold=350; monsters[10].gfx=20; strcpy(monsters[10].name, "Bronze Golem");
  monsters[11].hp=200; monsters[11].maxHP=200; monsters[11].ap=37; monsters[11].hitp=85; monsters[11].expr=340;
  monsters[11].gold=450; monsters[11].gfx=22; strcpy(monsters[11].name, "Dark Goblin");
  monsters[12].hp=250; monsters[12].maxHP=250; monsters[12].ap=40; monsters[12].hitp=85; monsters[12].expr=365;
  monsters[12].gold=550; monsters[12].gfx=24; strcpy(monsters[12].name, "Iron Golem");
  monsters[13].hp=275; monsters[13].maxHP=275; monsters[13].ap=44; monsters[13].hitp=85; monsters[13].expr=390;
  monsters[13].gold=600; monsters[13].gfx=26; strcpy(monsters[13].name, "Gold Dragon");
  monsters[14].hp=300; monsters[14].maxHP=300; monsters[14].ap=47; monsters[14].hitp=90; monsters[14].expr=425;
  monsters[14].gold=700; monsters[14].gfx=28; strcpy(monsters[14].name, "Lost Soul");
  monsters[15].hp=325; monsters[15].maxHP=325; monsters[15].ap=50; monsters[15].hitp=90; monsters[15].expr=450;
  monsters[15].gold=750; monsters[15].gfx=30; strcpy(monsters[15].name, "Ice Dragon");
  monsters[16].hp=350; monsters[16].maxHP=350; monsters[16].ap=55; monsters[16].hitp=95; monsters[16].expr=500;
  monsters[16].gold=850; monsters[16].gfx=32; strcpy(monsters[16].name, "Platinum Golem");
  monsters[17].hp=400; monsters[17].maxHP=400; monsters[17].ap=60; monsters[17].hitp=95; monsters[17].expr=550;
  monsters[17].gold=1000; monsters[17].gfx=34; strcpy(monsters[17].name, "Black Dragon");
  monsters[18].hp=130; monsters[18].maxHP=130; monsters[18].ap=18; monsters[18].hitp=65; monsters[18].expr=300;
  monsters[18].gold=300; monsters[18].gfx=36; strcpy(monsters[18].name, "Demon Spider");
}

void CBattle::monsterAttack(){
  int dam;
  int x;
  char str[128];
  string s;
  
  x = rand() % 100;
  if(x<curMon.hitp){
    
    dam = 0;
    while(dam < curMon.ap / 2){
      dam = rand() % curMon.ap + 1;
    }
    dam -= (hero->hStr + hero->aStr + hero->sStr);
    if(dam < 1) dam=1;

    if(dam<=(hero->maxHP/20)){
      sprintf(str, "The %s barely grazes you for %d damage.", curMon.name, dam);  
    } else if(dam<(hero->maxHP/10)){
      sprintf(str, "The %s smacks you a bit for %d damage.", curMon.name, dam);
    } else if(dam<(hero->maxHP/5)){
      sprintf(str, "The %s pounds you for %d damage.", curMon.name, dam);
    } else if(dam<(hero->maxHP/3)){
      sprintf(str, "The %s ferociously slams you for %d damage.", curMon.name, dam);
    } else {
      sprintf(str, "The %s utterly stomps you for %d damage.", curMon.name, dam);
    }
    s=str;
    text.push_back(s);

    hero->hp-=dam;
    if(hero->hp<1){
      text.push_back(" ");
      text.push_back("Alas, you are slain!");
    }

  } else {
    sprintf(str, "The %s whiffs.", curMon.name);
    s=str;
    text.push_back(s);
  }
}

void CBattle::playerAttack(){
  int dam;
  int x;
  int power=(hero->str / 2 + hero->wStr);
  char str[128];
  bool bWeap=true;
  string s;
  
  if(strcmp(hero->weapon, "None") == 0) bWeap=false;

  x = rand() % 100;
  if(x<=(60 + hero->dex / 2)){

    dam = 0;
    while(dam < power / 2){
      dam = rand() % power + 1;
    }

    if(dam<=curMon.maxHP/6){
      if(!bWeap) sprintf(str, "The gently slap the %s with your bare hands for %d damage.", curMon.name, dam);
      else sprintf(str, "Your %s glances the %s for %d damage.", hero->weapon,curMon.name, dam);
    } else if(dam<=(curMon.maxHP/3)){
      if(!bWeap) sprintf(str, "You slam your knuckles into the %s for %d damage.", curMon.name, dam);
      else sprintf(str, "You hit the %s with your %s for %d damage.", curMon.name, hero->weapon, dam);
    } else if(dam<=(curMon.maxHP / 2 + curMon.maxHP / 6)){
      if(!bWeap) sprintf(str, "You land a fistful of pain on the %s for %d damage.", curMon.name, dam);
      else sprintf(str, "You decimate the %s with your %s for %d damage.", curMon.name, hero->weapon, dam);
    } else {
      if(!bWeap) sprintf(str, "You unleash five fingers of death on %s for %d damage.", curMon.name, dam);
      else sprintf(str, "You get medieval on the %s with your %s for %d damage.", curMon.name, hero->weapon, dam);
    }
    s=str;
    text.push_back(s);

    curMon.hp-=dam;

  } else {
    sprintf(str, "You miss by a long shot");
    s=str;
    text.push_back(s);
  }

  s=" ";
  text.push_back(s);
}

void CBattle::render(){
  SDL_Rect r;
  char str[64];

  SDL_RenderClear(display->renderer);
  
  //Draw Title
  renderBox(100, 6, 440, 34);
  sprintf(str, "%s Attack!", curMon.name);
  font->render((640 - font->getStringWidth(str)) / 2, 10, str);

  //Display Player stats and options
  renderBox(210, 52, 348, 168);
  font->render(240, 66, hero->name);
  sprintf(str, "HP = %d", hero->hp);
  font->render(240, 82, str);
  sprintf(str, "MP = %d", hero->mp);
  font->render(240, 98, str);
  r.w=336; r.h=16; r.x=216; r.y=152+16*selection;
  SDL_SetRenderDrawColor(display->renderer, 0,0,128, 255);
  SDL_RenderFillRect(display->renderer, &r);
  SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);
  font->render(240, 146, "Attack");
  font->render(240, 162, "Spell");
  font->render(240, 178, "Run");

  //Draw Monster
  renderBox(10, 52, 120, 120);
  r.w=100;  r.h=100;  r.x = 20;  r.y = 62;
  if(curMon.hp <= curMon.maxHP / 2) SDL_RenderCopy(display->renderer, gfx->monster->texture, gfx->monster->getTile(curMon.gfx+1), &r);
  else SDL_RenderCopy(display->renderer, gfx->monster->texture, gfx->monster->getTile(curMon.gfx), &r);

  if(showText) renderText();
  if(showSpell) renderSpell();

  SDL_RenderPresent(display->renderer);
}

void CBattle::renderBox(int x, int y, int w, int h){
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

void CBattle::renderSpell(){
  SDL_Rect r;
  int lineCount=0;
  size_t i;

  renderBox(150, 80, 342, 242);

  r.w=230; r.h=16; r.x=156; r.y=88 + 16 * spellSelection;
  SDL_SetRenderDrawColor(display->renderer, 0, 0, 128, 255);
  SDL_RenderFillRect(display->renderer, &r);
  SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);

  for(i=0; i < spells.size();i++){
    if(spells[i]==-1) font->render(166, 82, "Cancel");
    else font->render(166, 98 + 16 * lineCount++, spellNames[spells[i]]);
  }

}

void CBattle::renderText(){
  size_t i,j;
  string word;
  string line;
  string curText;
  int sz;
  int wsz;
  int lineNum;
  bool bDownArrow=false;

  renderBox(0, 180, 640, 170);

  //Render word by word, applying line breaks as needed.
  lineNum = 0;
  for(j=0; j < text.size(); j++){
    sz = 0;
    word.clear();
    line.clear();
    curText=text[j];
    for(i = 0; i < curText.size(); i++){
      if(curText[i] == ' '){
        wsz=font->getStringWidth(word);
        if(sz + wsz>608){
          //print current line
          font->render(16, 188 + 16 * lineNum++, line);
          line = word;
          line += " ";
          sz = wsz + font->getStringWidth(' ');
        } else {
          line += word;
          line += " ";
          sz += wsz + font->getStringWidth(' ');
        }
        word.clear();
        wsz = 0;
        continue;
      }
      word += curText[i];
      wsz += font->getStringWidth(curText[i]);
    }

    if(sz + wsz > 608){
      font->render(16, 188 + 16 * lineNum++, line);
      line = word;
    } else {
      line += word;
    }
    font->render(16, 188 + 16 * lineNum++, line);
  }

}

void CBattle::setHeroSpells(){
  int i;

  spells.clear();
  spells.push_back(-1);

  for(i=0; i < 10; i++){
    if(hero->spells[i])spells.push_back(i);
  }

}
