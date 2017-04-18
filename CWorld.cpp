#include "CWorld.h"

CMap& CWorld::operator[](const int& index){
  return maps[index];
}

/*
int CWorld::checkTile(int map, int x, int y){
  switch (maps[map].getTile(x, y)){
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
  case 29: return -1;
  case 30: return 27;
  case 87: return 87;
  default: return 0;
  }
  return -1;
}
*/
void CWorld::loadMaps(){
  CMap m;
  m.loadMap("Maps/Aaryak.map"); //0
  maps.push_back(m);
  m.loadMap("Maps/Amber.map");
  maps.push_back(m);
  m.loadMap("Maps/Crystal1.map");
  maps.push_back(m);
  m.loadMap("Maps/crystal2.map");
  maps.push_back(m);
  m.loadMap("Maps/crystal3.map");
  maps.push_back(m);
  m.loadMap("Maps/dgfort.map"); //5
  maps.push_back(m);
  m.loadMap("Maps/Dungeon1.map");
  maps.push_back(m);
  m.loadMap("Maps/Dungeon2.map");
  maps.push_back(m);
  m.loadMap("Maps/Dungeon3.map");
  maps.push_back(m);
  m.loadMap("Maps/Dwarf1.map");
  maps.push_back(m);
  m.loadMap("Maps/Dwarf2.map"); //10
  maps.push_back(m);
  m.loadMap("Maps/DwarfA.map");
  maps.push_back(m);
  m.loadMap("Maps/DwarfB.map");
  maps.push_back(m);
  m.loadMap("Maps/DwarfC.map");
  maps.push_back(m);
  m.loadMap("Maps/DwarfD.map");
  maps.push_back(m);
  m.loadMap("Maps/DwarfE.map"); //15
  maps.push_back(m);
  m.loadMap("Maps/Dwarftwn.map");
  maps.push_back(m);
  m.loadMap("Maps/Fcity.map");
  maps.push_back(m);
  m.loadMap("Maps/Fcity2.map");
  maps.push_back(m);
  m.loadMap("Maps/Garrison.map");
  maps.push_back(m);
  m.loadMap("Maps/Greyor.map"); //20
  maps.push_back(m);
  m.loadMap("Maps/Landlich.map");
  maps.push_back(m);
  m.loadMap("Maps/ld1.map");
  maps.push_back(m);
  m.loadMap("Maps/ld2.map");
  maps.push_back(m);
  m.loadMap("Maps/ld3.map");
  maps.push_back(m);
  m.loadMap("Maps/ld4.map"); //25
  maps.push_back(m);
  m.loadMap("Maps/ld5.map");
  maps.push_back(m);
  m.loadMap("Maps/Meadow.map");
  maps.push_back(m);
  m.loadMap("Maps/newcont1.map");
  maps.push_back(m);
  m.loadMap("Maps/Npost.map");
  maps.push_back(m);
  m.loadMap("Maps/outside1.map"); //30
  maps.push_back(m);
  m.loadMap("Maps/outside2.map");
  maps.push_back(m);
  m.loadMap("Maps/Qtower1.map");
  maps.push_back(m);
  m.loadMap("Maps/Qtower2.map");
  maps.push_back(m);
  m.loadMap("Maps/Rhoeyce.map");
  maps.push_back(m);
  m.loadMap("Maps/Rift1.map"); //35
  maps.push_back(m);
  m.loadMap("Maps/Rift2.map");
  maps.push_back(m);
  m.loadMap("Maps/Rift3.map");
  maps.push_back(m);
  m.loadMap("Maps/Temple.map");
  maps.push_back(m);
  m.loadMap("Maps/Tristen.map");
  maps.push_back(m);
  m.loadMap("Maps/Tristen2.map"); //40
  maps.push_back(m);
  m.loadMap("Maps/Trok.map");
  maps.push_back(m);
  m.loadMap("Maps/Wisp.map");
  maps.push_back(m);
  m.loadMap("Maps/ld1b.map");
  maps.push_back(m);
}
