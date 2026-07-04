#include "CMods.h"
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

string CMods::resolve(const string& modName, const string& defaultPath){
  if(modName == "None" || modName.empty()) return defaultPath;

  string modPath = "Mods/" + modName + "/" + defaultPath;
  ifstream f(modPath.c_str());
  if(f.good()) return modPath;

  return defaultPath;
}
