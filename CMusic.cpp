#include "CMusic.h"

CMusic::CMusic(){

  int i;
  for(i=0; i < 5;i++){
    music[i].setLoop(true);
    loaded[i]=false;
  }

  currentSong=TitleSong;
}

CMusic::~CMusic(){
}

void CMusic::loadMusic(){
  loaded[BattleSong]  = music[BattleSong].openFromFile("Music/battle.ogg");
  loaded[DungeonSong] = music[DungeonSong].openFromFile("Music/dungeon.ogg");
  loaded[TitleSong]   = music[TitleSong].openFromFile("Music/title.ogg");
  loaded[TownSong]    = music[TownSong].openFromFile("Music/town.ogg");
  loaded[WorldSong]   = music[WorldSong].openFromFile("Music/title.ogg");
}

void CMusic::playSong(eMusic m, bool restart){
  if(!loaded[m]) return; //asset not available; leave whatever is currently playing alone

  if(m != currentSong){
    music[currentSong].stop();
  } else if(restart){
    music[m].stop();
  } else if(music[m].getStatus() == sf::Music::Playing){
    return; //already playing this song, nothing to do
  }

  music[m].play();
  currentSong=m;
}

void CMusic::setVolume(int vol){
  int i;
  int v=10 * vol;
  float f=(float)v;
  for(i=0; i < 5;i++){
    music[i].setVolume(f);
  }
}