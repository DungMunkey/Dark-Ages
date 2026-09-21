#include "CWorld.h"
#include "CMods.h"

using namespace std;

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
void CWorld::loadMaps(const string& modName){
  CMap m;
  m.loadMap(CMods::resolve(modName, "Maps/Aaryak.map").c_str()); //0
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/Amber.map").c_str());
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/Crystal1.map").c_str());
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/crystal2.map").c_str());
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/crystal3.map").c_str());
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/dgfort.map").c_str()); //5
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/Dungeon1.map").c_str());
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/Dungeon2.map").c_str());
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/Dungeon3.map").c_str());
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/Dwarf1.map").c_str());
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/Dwarf2.map").c_str()); //10
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/DwarfA.map").c_str());
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/DwarfB.map").c_str());
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/DwarfC.map").c_str());
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/DwarfD.map").c_str());
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/DwarfE.map").c_str()); //15
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/Dwarftwn.map").c_str());
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/Fcity.map").c_str());
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/Fcity2.map").c_str());
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/Garrison.map").c_str());
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/Greyor.map").c_str()); //20
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/Landlich.map").c_str());
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/ld1.map").c_str());
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/ld2.map").c_str());
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/ld3.map").c_str());
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/ld4.map").c_str()); //25
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/ld5.map").c_str());
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/Meadow.map").c_str());
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/newcont1.map").c_str());
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/Npost.map").c_str());
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/outside1.map").c_str()); //30
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/outside2.map").c_str());
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/Qtower1.map").c_str());
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/Qtower2.map").c_str());
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/Rhoeyce.map").c_str());
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/Rift1.map").c_str()); //35
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/Rift2.map").c_str());
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/Rift3.map").c_str());
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/Temple.map").c_str());
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/Tristen.map").c_str());
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/Tristen2.map").c_str()); //40
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/Trok.map").c_str());
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/Wisp.map").c_str());
  maps.push_back(m);
  m.loadMap(CMods::resolve(modName, "Maps/ld1b.map").c_str());
  maps.push_back(m);
}
