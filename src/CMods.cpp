#include "CMods.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <filesystem>
#include <fstream>

//case-insensitive string compare, spelled differently on Windows and elsewhere
#ifdef _WIN32
#define da_stricmp _stricmp
#else
#include <strings.h>
#define da_stricmp strcasecmp
#endif

using namespace std;

vector<string> CMods::listMods(){
  vector<string> mods;
  mods.push_back("None");

  //every folder inside Mods/ is a mod
  vector<string> found;
  error_code ec; //Mods/ may not exist (or be unreadable): that just means there are no mods
  for(filesystem::directory_iterator it("Mods", ec), end; !ec && it != end; it.increment(ec)){
    error_code entryEc; //separate, so one unreadable entry doesn't end the listing
    if(it->is_directory(entryEc)) found.push_back(it->path().filename().string());
  }

  //directory order is up to the file system (Windows happened to give name order); sort so the Options menu is the same everywhere
  sort(found.begin(), found.end(), [](const string& a, const string& b){ return da_stricmp(a.c_str(), b.c_str()) < 0; });
  mods.insert(mods.end(), found.begin(), found.end());

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

    if(da_stricmp(key, "TileSize") == 0){
      int sz = atoi(val);
      if(sz > 0) s.tileSize = sz;
    } else if(da_stricmp(key, "MonsterSize") == 0){
      int sz = atoi(val);
      if(sz > 0) s.monsterSize = sz;
    } else if(da_stricmp(key, "HighResBorders") == 0){
      s.highResBorders = (atoi(val) != 0 || da_stricmp(val, "true") == 0 || da_stricmp(val, "yes") == 0);
    } else if(da_stricmp(key, "BevelDetail") == 0){
      int d = atoi(val);
      if(d > 0) s.bevelDetail = d;
    } else if(da_stricmp(key, "HeroWalkFrames") == 0){
      int f = atoi(val);
      if(f > 0) s.heroWalkFrames = (f > 32) ? 32 : f; //DA1HeroL.bmp is 16 columns wide, 4 per direction block - 32 is as many 8-row blocks as that supports
    } else if(da_stricmp(key, "SolidTiles") == 0){
      //comma-separated list of map-tile values to additionally treat as impassable - see CDarkages::checkTile()
      char* tok = strtok(val, ",");
      while(tok != NULL){
        s.solidTiles.push_back(atoi(tok));
        tok = strtok(NULL, ",");
      }
    } else if(da_stricmp(key, "HeroIdleAnimations") == 0){
      //comma-separated frame count per idle animation, e.g. "12,16,8" = 3 animations - see Gfx/DA1HeroIdle.bmp
      char* tok = strtok(val, ",");
      while(tok != NULL){
        int f = atoi(tok);
        if(f > 0) s.heroIdleAnimations.push_back(f);
        tok = strtok(NULL, ",");
      }
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
