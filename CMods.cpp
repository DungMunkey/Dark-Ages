#include "CMods.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <windows.h>

using namespace std;

vector<string> CMods::listMods(){
  vector<string> mods;
  mods.push_back("None");

  WIN32_FIND_DATAA data;
  HANDLE h = FindFirstFileA("Mods\\*", &data);
  if(h != INVALID_HANDLE_VALUE){
    do{
      if((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
         strcmp(data.cFileName, ".") != 0 &&
         strcmp(data.cFileName, "..") != 0){
        mods.push_back(data.cFileName);
      }
    } while(FindNextFileA(h, &data));
    FindClose(h);
  }

  return mods;
}

sModSettings CMods::loadModSettings(const string& modName){
  sModSettings s;
  if(modName == "None" || modName.empty()) return s;

  string path = "Mods/" + modName + "/mod.cfg";
  FILE* f = fopen(path.c_str(), "rt");
  if(f == NULL) return s;

  //skip a UTF-8 BOM if present - some editors write one by default when saving as "UTF-8"
  unsigned char bom[3];
  if(fread(bom, 1, 3, f) != 3 || bom[0] != 0xEF || bom[1] != 0xBB || bom[2] != 0xBF){
    fseek(f, 0, SEEK_SET);
  }

  char line[256];
  while(fgets(line, 256, f) != NULL){
    char* key = line;
    while(*key == ' ' || *key == '\t') key++;
    if(*key == '#' || *key == '\0' || *key == '\r' || *key == '\n') continue;

    char* eq = strchr(key, '=');
    if(eq == NULL) continue;
    *eq = '\0';
    char* val = eq + 1;

    size_t klen = strlen(key);
    while(klen > 0 && (key[klen - 1] == ' ' || key[klen - 1] == '\t')) key[--klen] = '\0';

    size_t vlen = strlen(val);
    while(vlen > 0 && (val[vlen - 1] == '\n' || val[vlen - 1] == '\r' || val[vlen - 1] == ' ' || val[vlen - 1] == '\t')) val[--vlen] = '\0';

    if(_stricmp(key, "TileSize") == 0){
      int sz = atoi(val);
      if(sz > 0) s.tileSize = sz;
    } else if(_stricmp(key, "MonsterSize") == 0){
      int sz = atoi(val);
      if(sz > 0) s.monsterSize = sz;
    }
  }

  fclose(f);
  return s;
}

string CMods::resolve(const string& modName, const string& defaultPath){
  if(modName == "None" || modName.empty()) return defaultPath;

  string modPath = "Mods/" + modName + "/" + defaultPath;
  ifstream f(modPath.c_str());
  if(f.good()) return modPath;

  return defaultPath;
}
