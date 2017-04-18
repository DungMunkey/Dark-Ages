#include "CMap.h"

CMap::CMap(){
  map = NULL;
  szX = 0;
  szY = 0;
}

CMap::CMap(const CMap& m){
  szX = m.szX;
  szY = m.szY;
  map = new int*[szX];
  for (int i = 0; i < szX; i++) {
    map[i] = new int[szY];
    for (int j = 0; j < szY; j++) map[i][j] = m.map[i][j];
  }
}

CMap::~CMap(){
  deleteMap();
}

CMap& CMap::operator=(const CMap& m){
  if (this != &m){
    szX = m.szX;
    szY = m.szY;
    deleteMap();
    map = new int*[szX];
    for (int i = 0; i < szX; i++) {
      map[i] = new int[szY];
      for (int j = 0; j < szY; j++) map[i][j] = m.map[i][j];
    }
  }
}

void CMap::deleteMap(){
  if (map != NULL){
    for (int i = 0; i < szX; i++) delete[] map[i];
    delete[] map;
  }
}

int CMap::getTile(int x, int y){
  if (x >= szX || x < 0 || y >= szY || y < 0) return 0;
  return map[x][y];
}

void CMap::loadMap(char* fn){
  char str[16000];
  char* tok;
  int i,j;

  //clear memory
  deleteMap();

  //open map and get the dimensions
  FILE* f = fopen(fn, "rt");
  fgets(str, 16000, f);
  tok = strtok(str, ",\n\r");
  szX = atoi(tok);
  tok = strtok(NULL, ",\n\r");
  szY = atoi(tok);

  //redimension map array
  map = new int*[szX];
  for (i = 0; i < szX; i++) map[i] = new int[szY];
  
  //load map tiles
  for (j = 0; j < szY; j++){
    i = 0;
    fgets(str, 16000, f);
    tok = strtok(str, ",\n\r");
    while (tok != NULL){
      map[i++][j] = atoi(tok);
      tok = strtok(NULL, ",\n\r");
    }
  }

  fclose(f);

}