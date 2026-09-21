#include "CWindow.h"
#include <cmath>
#include <vector>

using namespace std;

CWindow::CWindow(){
  display=NULL;
  font=NULL;
  gfx=NULL;
}

CWindow::CWindow(CDisplay* d, CFont* f, CGfxCollection* g){
  display=d;
  font=f;
  gfx=g;
}

CWindow::~CWindow(){
  display=NULL;
  font=NULL;
  gfx=NULL;
}

void CWindow::render(){
}

int CWindow::renderBox(int x, int y, int w, int h){
  return renderBox(display, x, y, w, h);
}

//builds a symmetric brightness ramp: 1.0 at the center band, falling off to minBrightness
//at the outer edge over `depth` steps. Total bands = depth*2+1.
static vector<float> buildBevelRamp(int depth, float minBrightness){
  vector<float> ramp;
  int totalBands = depth*2 + 1;
  for(int i = 0; i < totalBands; i++){
    int distFromCenter = abs(i - depth);
    float t = (depth == 0) ? 0.0f : (float)distFromCenter / (float)depth;
    ramp.push_back(1.0f - t * (1.0f - minBrightness));
  }
  return ramp;
}

int CWindow::renderBox(CDisplay* display, int x, int y, int w, int h, eBevelStyle style, SDL_Color baseColor, bool drawBackground){
  int baseDepth = (style == BevelRich) ? 2 : 1;         //today's Rich = 5 bands (depth 2), Simple = 3 bands (depth 1)
  float minBrightness = (style == BevelRich) ? 0.5f : 0.75f;

  int detail = display->modSettings.bevelDetail;
  if(detail < 1) detail = 1;
  int depth = baseDepth * detail;

  int pixelsPerBand = display->modSettings.highResBorders ? 1 : 2;

  vector<float> ramp = buildBevelRamp(depth, minBrightness);
  int totalStrokes = (int)ramp.size() * pixelsPerBand;

  SDL_FRect r;
  r.x = (float)(x); r.y = (float)(y); r.w = (float)(w); r.h = (float)(h);
  if(drawBackground){
    SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 200);
    SDL_RenderFillRect(display->renderer, &r);
  }

  int strokeIndex = 0;
  for(size_t i = 0; i < ramp.size(); i++){
    Uint8 cr = (Uint8)(baseColor.r * ramp[i]);
    Uint8 cg = (Uint8)(baseColor.g * ramp[i]);
    Uint8 cb = (Uint8)(baseColor.b * ramp[i]);
    SDL_SetRenderDrawColor(display->renderer, cr, cg, cb, 255);
    for(int p = 0; p < pixelsPerBand; p++){
      SDL_RenderRect(display->renderer, &r);
      strokeIndex++;
      if(strokeIndex < totalStrokes){ r.w -= (float)(2); r.h -= (float)(2); r.x++; r.y++; } //no shrink after the innermost stroke
    }
  }

  SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);

  return totalStrokes - 1; //pixels from the outer edge to the innermost stroke
}
