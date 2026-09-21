#include "CMusic.h"
#include "CMods.h"

using namespace std;

CMusic::CMusic(){

  int i;
  for(i=0; i < 5;i++){
    music[i]=NULL;
    loaded[i]=false;
  }

  mixer=NULL;
  track=NULL;
  currentSong=TitleSong;
  currentIsOneShot=false;
  gain=0.5f;
}

CMusic::~CMusic(){
  if(track != NULL) MIX_DestroyTrack(track);
  int i;
  for(i=0; i < 5;i++){
    if(music[i]!=NULL) MIX_DestroyAudio(music[i]);
  }
  if(mixer != NULL) MIX_DestroyMixer(mixer);
  MIX_Quit();
}

void CMusic::loadMusic(const string& modName){
  MIX_Init();
  mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
  if(mixer == NULL){
    SDL_Log("No audio output: %s", SDL_GetError());
    return; //the game runs silently
  }
  track = MIX_CreateTrack(mixer);
  MIX_SetMixerGain(mixer, gain);

  //false = stream the file from disk while it plays, instead of decoding it all up front
  music[BattleSong]  = MIX_LoadAudio(mixer, CMods::resolve(modName, "Music/battle.ogg").c_str(), false);
  music[DungeonSong] = MIX_LoadAudio(mixer, CMods::resolve(modName, "Music/dungeon.ogg").c_str(), false);
  music[TitleSong]   = MIX_LoadAudio(mixer, CMods::resolve(modName, "Music/title.ogg").c_str(), false);
  music[TownSong]    = MIX_LoadAudio(mixer, CMods::resolve(modName, "Music/town.ogg").c_str(), false);
  music[WorldSong]   = MIX_LoadAudio(mixer, CMods::resolve(modName, "Music/world.ogg").c_str(), false);
  if(music[WorldSong] == NULL) music[WorldSong] = MIX_LoadAudio(mixer, CMods::resolve(modName, "Music/title.ogg").c_str(), false);

  loaded[BattleSong]  = music[BattleSong]  != NULL;
  loaded[DungeonSong] = music[DungeonSong] != NULL;
  loaded[TitleSong]   = music[TitleSong]   != NULL;
  loaded[TownSong]    = music[TownSong]    != NULL;
  loaded[WorldSong]   = music[WorldSong]   != NULL;
}

bool CMusic::trackPlaying() const {
  return track != NULL && MIX_TrackPlaying(track);
}

void CMusic::startTrack(eMusic m, int loops){
  if(track == NULL) return;

  MIX_StopTrack(track, 0);                 //cut whatever was playing
  MIX_SetTrackAudio(track, music[m]);      //always (re)starts from the beginning
  SDL_PropertiesID options = SDL_CreateProperties();
  SDL_SetNumberProperty(options, MIX_PROP_PLAY_LOOPS_NUMBER, loops);
  MIX_PlayTrack(track, options);
  SDL_DestroyProperties(options);

  currentSong=m;
  currentIsOneShot=(loops == 0);
}

void CMusic::playSong(eMusic m, bool restart){
  if(!loaded[m]) return; //asset not available; leave whatever is currently playing alone

  //already playing this song, nothing to do - unless it was started by playSongOnce(): that playback won't loop,
  //so a normal request for the same song (e.g. the title screen right after the credits, which play the title
  //song once) must restart it looping rather than let it run out into silence
  if(m == currentSong && !restart && !currentIsOneShot && trackPlaying()) return;

  startTrack(m, -1); //loops indefinitely; always (re)starts from the beginning
}

//Unlike playSong(), this never skips a song that is already playing (it always restarts it) and doesn't loop:
//the music stops by itself when the track ends. Used for the end credits, whose scroll is timed to the track.
void CMusic::playSongOnce(eMusic m){
  if(!loaded[m]) return; //asset not available; leave whatever is currently playing alone

  startTrack(m, 0); //play through exactly once, from the beginning
}

double CMusic::getSongDuration(eMusic m) const {
  if(!loaded[m]) return -1.0;
  Sint64 frames = MIX_GetAudioDuration(music[m]); //in sample frames; negative if unknown or endless
  if(frames < 0) return -1.0;
  return MIX_AudioFramesToMS(music[m], frames) / 1000.0; //seconds
}

void CMusic::setVolume(int vol){
  gain = vol / 10.0f; //the game's 0-10 setting; 10 is full volume
  if(mixer != NULL) MIX_SetMixerGain(mixer, gain);
}
