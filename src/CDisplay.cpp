#include "CDisplay.h"
#include "CFont.h"
#include "CMods.h"
#include <cmath>

using namespace std;

CDisplay::CDisplay(){

  screenHeight = 800; // 1050;
  screenWidth =  1280; // 1680;
  currentScreenMode=0;
  renderer=NULL;
  //screenSurface = NULL;
  window = NULL;
  scale=1.0;
  font=NULL;

  txtColors[0].r=255;
  txtColors[0].g=255;
  txtColors[0].b=255;
  txtColors[0].a=255;
}

CDisplay::~CDisplay(){
  if(renderer!=NULL) SDL_DestroyRenderer(renderer);
  renderer=NULL;
  //if(screenSurface != NULL) SDL_DestroySurface(screenSurface);
	//screenSurface = NULL;
  if(window != NULL) SDL_DestroyWindow(window);
  window = NULL;
  SDL_Quit();
}

bool CDisplay::init(sConf& conf) {
	//Initialization flag
	bool success = true;

	//Initialize SDL (SDL3 functions return true on success)
  if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_AUDIO)){
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}	else	{

    modSettings = CMods::loadModSettings(conf.modName);
    scale = modSettings.tileSize / 40.0;

    //The list of window sizes offered in Options: every mode of the primary display that has the desktop's current pixel
    //format and refresh rate, smallest first.
    SDL_DisplayID displayId = SDL_GetPrimaryDisplay();
    const SDL_DisplayMode* desktop = SDL_GetDesktopDisplayMode(displayId);
    sDAVidMode vm;
    char str[32];
    bool exactMode=false; //did the display offer exactly the size saved in conf?

    int modeCount = 0;
    SDL_DisplayMode** modes = SDL_GetFullscreenDisplayModes(displayId, &modeCount);
    if(modes != NULL && desktop != NULL){
      for(int i=modeCount-1; i >=0; i--){ //SDL lists the largest mode first; the game's list runs smallest to largest
        const SDL_DisplayMode* mode = modes[i];
        if(mode->format != desktop->format || mode->refresh_rate != desktop->refresh_rate) continue;
        vm.h=mode->h;
        vm.w=mode->w;
        sprintf(str, "%dx%d", mode->w, mode->h);
        vm.name=str;
        if(conf.w==vm.w && conf.h==vm.h){
          currentScreenMode=screenModes.size();
          exactMode=true;
        }
        screenModes.push_back(vm);
      }
    }
    SDL_free(modes);
    if(screenModes.empty()){ //no usable mode was reported: offer the desktop's own size (or a safe default) so the list is never empty
      vm.w = (desktop != NULL) ? desktop->w : 1280;
      vm.h = (desktop != NULL) ? desktop->h : 1024;
      sprintf(str, "%dx%d", vm.w, vm.h);
      vm.name=str;
      screenModes.push_back(vm);
      currentScreenMode=0;
      exactMode=true;
    }
    if(!exactMode){
      //The saved size isn't one this display offers - e.g. the default 1280x1024 on a small or high-refresh display, or a
      //monitor that changed since the size was saved. Use the largest mode that fits inside it, or the smallest if none does.
      int best=-1;
      for(size_t i=0; i < screenModes.size(); i++){
        if(screenModes[i].w > conf.w || screenModes[i].h > conf.h) continue;
        if(best < 0 || screenModes[i].w*screenModes[i].h > screenModes[best].w*screenModes[best].h) best=(int)i;
      }
      currentScreenMode=(best >= 0) ? best : 0;
    }
    conf.w=screenModes[currentScreenMode].w;
    conf.h=screenModes[currentScreenMode].h;

    //for(int i=0; i < screenModes.size();i++){
    //  printf("%s\n", &screenModes[i].name[0]);
    //}
    screenWidth=screenModes[currentScreenMode].w;
    screenHeight=screenModes[currentScreenMode].h;
    //In SDL3 a fullscreen window with no fullscreen mode set is borderless "desktop" fullscreen - the same thing the
    //game has always used (it never changes the display's resolution).
    SDL_WindowFlags wf = 0;
    if(conf.fullScreen) wf |= SDL_WINDOW_FULLSCREEN;

		//Create window
		window = SDL_CreateWindow( "Dark Ages: The Continents", screenWidth, screenHeight, wf );
		if( window == NULL )	{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}	else	{
			//Desktop fullscreen ignores the w/h passed to SDL_CreateWindow and instead takes over the desktop at its own
			//current resolution, so screenWidth/screenHeight (used everywhere below to compute the world/UI layout) must
			//be re-queried from the real, resulting window size rather than trusted from the display-mode list picked above.
			SDL_GetWindowSize(window, &screenWidth, &screenHeight);

      renderer = SDL_CreateRenderer(window, NULL);
			if( renderer == NULL ) {
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			} else {
        SDL_SetRenderVSync(renderer, conf.vSync ? 1 : 0);

        //Nearest-neighbor filtering for every texture created from now on (SDL3's default is linear, which would blur
        //the pixel art). Pixels rule!!!
        SDL_SetDefaultTextureScaleMode(renderer, SDL_SCALEMODE_NEAREST);

				//Initialize renderer color
				SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
			}

		}
	}

	return success;
}

int CDisplay::S(int refValue){
  return (int)round(refValue * scale);
}

void CDisplay::setCanvasSize(int w, int h){
  canvasW = w;
  canvasH = h;
  computeLayout();
}

void CDisplay::computeLayout(){
  //world canvas: its own best-fit integer multiple of the mod's native tile-art resolution
  worldScale = screenWidth / canvasW;
  int worldScaleH = screenHeight / canvasH;
  if(worldScaleH < worldScale) worldScale = worldScaleH;
  if(worldScale < 1) worldScale = 1;
  worldRect.w = canvasW * worldScale;
  worldRect.h = canvasH * worldScale;
  worldRect.x = (screenWidth - worldRect.w) / 2;
  worldRect.y = (screenHeight - worldRect.h) / 2;

  //UI layer: its own best-fit integer multiple of the fixed 640x400 reference space, independent of
  //the mod's tile size, so text/borders are always sized the same regardless of which mod is loaded
  uiScale = screenWidth / 640;
  int uiScaleH = screenHeight / 400;
  if(uiScaleH < uiScale) uiScale = uiScaleH;
  if(uiScale < 1) uiScale = 1;
  uiRect.w = 640 * uiScale;
  uiRect.h = 400 * uiScale;
  uiRect.x = (screenWidth - uiRect.w) / 2;
  uiRect.y = (screenHeight - uiRect.h) / 2;

  //Canonical UI-pass font size, built purely from uiScale since UI-pass text is never touched by the
  //mod's tile size at all.
  uiFontPx = (int)(32.0 * uiScale + 0.5);
  if(uiFontPx < 32) uiFontPx = 32;
}

void CDisplay::setFont(CFont* f){
  font = f;
}

//Reset to the full-target viewport and fill it, rather than SDL_RenderClear() - a filled rect goes through the
//normal draw path, which applies the current (full) viewport to the device first, whereas the Direct3D
//renderer's clear used whatever viewport the previous draw had left on the device. Verified with a standalone
//SDL 2.0.12 test: after drawing inside a small viewport and resetting it to NULL, SDL_RenderClear left pixels
//outside that region untouched, while SDL_RenderFillRect(NULL) cleared everything.
void CDisplay::clearScreen(){
  SDL_SetRenderViewport(renderer, NULL);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderFillRect(renderer, NULL);
}

void CDisplay::beginUIPass(){
  savedScale = scale;
  scale = uiScale;
  SDL_SetRenderScale(renderer, 1.0f, 1.0f); //S() does the scaling in software here, so SDL's own scale must stay neutral
  SDL_SetRenderViewport(renderer, &uiRect);
  if(font != NULL) font->setFontSize(uiFontPx);
}

void CDisplay::endUIPass(){
  scale = savedScale;
  SDL_SetRenderViewport(renderer, NULL);
}

