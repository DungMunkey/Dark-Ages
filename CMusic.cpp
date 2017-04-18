#include "CMusic.h"

CMusic::CMusic(){

  int i;
  for(i=0; i < 5;i++){
    music[i].setLoop(true);
  }

  //music[BattleSong].openFromFile("test4.ogg");
  //music[TitleSong].openFromFile("test4.ogg");
  currentSong=TitleSong;
}

CMusic::~CMusic(){
  //music[TitleSong].
}

void CMusic::playSong(eMusic m){
  music[currentSong].stop();
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