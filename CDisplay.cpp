#include "CDisplay.h"
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

  txtColors[0].r=255;
  txtColors[0].g=255;
  txtColors[0].b=255;
  txtColors[0].a=255;
}

CDisplay::~CDisplay(){
  if(renderer!=NULL) SDL_DestroyRenderer(renderer);
  renderer=NULL;
  //if(screenSurface != NULL) SDL_FreeSurface(screenSurface);
	//screenSurface = NULL;
  if(window != NULL) SDL_DestroyWindow(window);
  window = NULL;
  SDL_Quit();
}

bool CDisplay::init(sConf& conf) {
	//Initialization flag
	bool success = true;

	//Initialize SDL
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO) < 0){
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}	else	{

    modSettings = CMods::loadModSettings(conf.modName);
    scale = modSettings.tileSize / 40.0;

    int display_count = 0, display_index = 0, mode_index = 0;
    SDL_DisplayMode mode ={SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0};

    SDL_GetCurrentDisplayMode(0, &mode);
    Uint32 pFormat=mode.format;
    int rRate=mode.refresh_rate;
    sDAVidMode vm;
    char str[32];

    for(int i=SDL_GetNumDisplayModes(0)-1; i >=0; i--){
      if(SDL_GetDisplayMode(0, i, &mode) != 0) {
        SDL_Log("SDL_GetDisplayMode failed: %s", SDL_GetError());
      } else {
        if(mode.format != pFormat || mode.refresh_rate != rRate) continue;
        vm.h=mode.h;
        vm.w=mode.w;
        sprintf(str, "%dx%d", mode.w, mode.h);
        vm.name=str;
        if(conf.w==vm.w && conf.h==vm.h){
          currentScreenMode=screenModes.size();
        }
        screenModes.push_back(vm);
        //printf("%d\tSDL_GetDisplayMode(0, 0, &mode):\t\t%i bpp\t%i x %i, %ihz\n", i,SDL_BITSPERPIXEL(mode.format), mode.w, mode.h,mode.refresh_rate);
      }
    }
    conf.w=screenModes[currentScreenMode].w;
    conf.h=screenModes[currentScreenMode].h;

    //for(int i=0; i < screenModes.size();i++){
    //  printf("%s\n", &screenModes[i].name[0]);
    //}
    screenWidth=screenModes[currentScreenMode].w;
    screenHeight=screenModes[currentScreenMode].h;
    int wf;
    if(conf.fullScreen) {
      wf=SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP;
    } else {
      wf=SDL_WINDOW_SHOWN;
    }

    //Set texture filtering to nearest neighbor. Pixels rule!!!
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "0" ) ) {
			printf( "Warning: nearest neighbor texture filtering not enabled!" );
		}

		//Create window
		window = SDL_CreateWindow( "Dark Ages: The Continents", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, wf );
		if( window == NULL )	{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}	else	{
			//Get window surface
			//screenSurface = SDL_GetWindowSurface(window);
      if(conf.vSync) renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
      else renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
			if( renderer == NULL ) {
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			} else {
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
}

void CDisplay::beginUIPass(){
  savedScale = scale;
  scale = uiScale;
  SDL_RenderSetScale(renderer, 1.0f, 1.0f); //S() does the scaling in software here, so SDL's own scale must stay neutral
  SDL_RenderSetViewport(renderer, &uiRect);
}

void CDisplay::endUIPass(){
  scale = savedScale;
  SDL_RenderSetViewport(renderer, NULL);
}

void CDisplay::beginCompatPass(){
  SDL_RenderSetViewport(renderer, &worldRect);
  SDL_RenderSetScale(renderer, (float)worldScale, (float)worldScale);
}

void CDisplay::endCompatPass(){
  SDL_RenderSetScale(renderer, 1.0f, 1.0f);
  SDL_RenderSetViewport(renderer, NULL);
}