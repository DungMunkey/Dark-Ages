#include "CGfxCollection.h"

CGfxCollection::CGfxCollection(){
  tiles = NULL;
  player = NULL;
  extra=NULL;
  monster=NULL;
  death=NULL;
  endgame1=NULL;
  endgame2=NULL;
  endgame3=NULL;
  title=NULL;
}

CGfxCollection::~CGfxCollection(){
  if(tiles!=NULL) delete tiles;
  if (player != NULL) delete player;
  if(extra != NULL) delete extra;
  if(monster != NULL) delete monster;
  if(death != NULL) delete death;
  if(endgame1 != NULL) delete endgame1;
  if(endgame2 != NULL) delete endgame2;
  if(endgame3 != NULL) delete endgame3;
  if(title != NULL) delete title;
}

bool CGfxCollection::loadGfx(SDL_Renderer *rend){
  /*
  if(tiles!=NULL) delete tiles;
  tiles = new CGraphic("Gfx/DA1Tiles.bmp",rend);
  tiles->createTiles(20,20,320,200);

  if (player != NULL) delete player;
  player = new CGraphic("Gfx/DA1Hero.bmp", rend,true,0,0,0);
  player->createTiles(20, 20, 160, 20);

  if(extra != NULL) delete extra;
  extra = new CGraphic("Gfx/DA1Extra.bmp", rend, true, 0, 0, 0);
  extra->createTiles(8, 8, 16, 8);

  if(monster != NULL) delete monster;
  monster = new CGraphic("Gfx/DA1Monst.bmp", rend, true, 0, 0, 0);
  monster->createTiles(50, 50, 600, 200);
  */

  if(tiles != NULL) delete tiles;
  tiles = new CGraphic("Gfx/DA1TilesL.bmp", rend);
  tiles->createTiles(40, 40, 640, 400);

  if(player != NULL) delete player;
  player = new CGraphic("Gfx/DA1HeroL.bmp", rend, true, 0, 0, 0);
  player->createTiles(40, 40, 320, 40);

  if(extra != NULL) delete extra;
  extra = new CGraphic("Gfx/DA1ExtraL.bmp", rend, true, 0, 0, 0);
  extra->createTiles(16, 16, 64, 16);

  if(monster != NULL) delete monster;
  monster = new CGraphic("Gfx/DA1MonstL.bmp", rend, true, 0, 0, 0);
  monster->createTiles(100, 100, 1200, 400);

  if(death != NULL) delete death;
  death = new CGraphic("Gfx/death.bmp", rend);
  death->createTiles(320, 200, 320, 200);

  if(endgame1 != NULL) delete endgame1;
  endgame1 = new CGraphic("Gfx/daend1.bmp", rend);
  endgame1->createTiles(320, 200, 320, 200);

  if(endgame2 != NULL) delete endgame2;
  endgame2 = new CGraphic("Gfx/daend2.bmp", rend);
  endgame2->createTiles(320, 200, 320, 200);

  if(endgame3 != NULL) delete endgame3;
  endgame3 = new CGraphic("Gfx/explode.bmp", rend);
  endgame3->createTiles(320, 200, 320, 200);

  if(title != NULL) delete title;
  title = new CGraphic("Gfx/datitle.bmp", rend);
  title->createTiles(640, 400, 640, 400);

  return true;
}