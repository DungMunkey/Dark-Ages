#include "CMusic.h"

CMusic::CMusic(){

  int i;
  for(i=0; i < 5;i++){
    music[i]=NULL;
    loaded[i]=false;
  }

  currentSong=TitleSong;
}

CMusic::~CMusic(){
  int i;
  for(i=0; i < 5;i++){
    if(music[i]!=NULL) Mix_FreeMusic(music[i]);
  }
  Mix_CloseAudio();
}

void CMusic::loadMusic(){
  Mix_Init(MIX_INIT_OGG);
  Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

  music[BattleSong]  = Mix_LoadMUS("Music/battle.ogg");
  music[DungeonSong] = Mix_LoadMUS("Music/dungeon.ogg");
  music[TitleSong]   = Mix_LoadMUS("Music/title.ogg");
  music[TownSong]    = Mix_LoadMUS("Music/town.ogg");
  music[WorldSong]   = Mix_LoadMUS("Music/title.ogg");

  loaded[BattleSong]  = music[BattleSong]  != NULL;
  loaded[DungeonSong] = music[DungeonSong] != NULL;
  loaded[TitleSong]   = music[TitleSong]   != NULL;
  loaded[TownSong]    = music[TownSong]    != NULL;
  loaded[WorldSong]   = music[WorldSong]   != NULL;
}

void CMusic::playSong(eMusic m, bool restart){
  if(!loaded[m]) return; //asset not available; leave whatever is currently playing alone

  if(m == currentSong && !restart && Mix_PlayingMusic()) return; //already playing this song, nothing to do

  Mix_PlayMusic(music[m], -1); //loops indefinitely; always (re)starts from the beginning
  currentSong=m;
}

void CMusic::setVolume(int vol){
  Mix_VolumeMusic(vol * MIX_MAX_VOLUME / 10);
}
