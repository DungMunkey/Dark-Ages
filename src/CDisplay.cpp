#include "CDisplay.h"
#include "CFont.h"
#include "CMods.h"
#include <cmath>

using namespace std;

CDisplay::CDisplay(){

  screenHeight = 800; // 1050;
  screenWidth =  1280; // 1680;
  minScale=1;
  maxScale=1;
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

    //World canvas size (display-scaling-plan.md section 4.1): 16 x TileSize by 10 x TileSize. Stored
    //directly on `this` here because both the window sizing below and setCanvasSize() (called once the
    //real window size is known, further down) need it; CDarkages' constructor, which used to compute
    //this itself, now just reads it back off canvasW/H when it creates the canvas render-target texture
    //at the same size.
    canvasW = 16 * modSettings.tileSize;
    canvasH = 10 * modSettings.tileSize;

    SDL_DisplayID displayId = SDL_GetPrimaryDisplay();

    //The range Options offers for the scale setting (sConf::scaleN): the smallest whole number that
    //makes the window at least 640x400 (so the UI layer always has room), and the largest that still
    //fits the display's usable area (its bounds minus the taskbar and the like). Computed once here,
    //since the canvas size can't change without a restart (the mod was just loaded above, once) and the
    //game doesn't react to the display changing size while it runs.
    double minScaleD = 640.0 / canvasW;
    double minScaleD2 = 400.0 / canvasH;
    if(minScaleD2 > minScaleD) minScaleD = minScaleD2;
    minScale = (int)ceil(minScaleD);
    if(minScale < 1) minScale = 1;

    SDL_Rect usable = {0, 0, 0, 0};
    SDL_GetDisplayUsableBounds(displayId, &usable);
    if(usable.w <= 0 || usable.h <= 0){ usable.w = 1280; usable.h = 1024; } //nothing usable reported; fall back rather than a zero/negative maxScale
    maxScale = usable.w / canvasW;
    int maxScaleH = usable.h / canvasH;
    if(maxScaleH < maxScale) maxScale = maxScaleH;
    if(maxScale < minScale) maxScale = minScale; //a display too small to truly fit even the minimum: the window will exceed the usable area rather than the game refusing to run

    if(conf.scaleN < minScale) conf.scaleN = minScale;
    else if(conf.scaleN > maxScale) conf.scaleN = maxScale;

    screenWidth = canvasW * conf.scaleN;
    screenHeight = canvasH * conf.scaleN;

    //In SDL3 a fullscreen window with no fullscreen mode set is borderless "desktop" fullscreen - the same thing the
    //game has always used (it never changes the display's resolution). It ignores the size passed to
    //SDL_CreateWindow below; screenWidth/screenHeight are re-queried from the real window after creating it.
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
			//be re-queried from the real, resulting window size rather than trusted from what was requested above.
			SDL_GetWindowSize(window, &screenWidth, &screenHeight);
      setCanvasSize(canvasW, canvasH); //now that screenWidth/screenHeight are the real, final values

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

