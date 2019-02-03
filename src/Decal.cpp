#include "..\include\Decal.h"

Decal::Decal() {
   renderOrder = RENDER_IN_ORDER;
   attachedToEntity = false;
}

void Decal::SetPartialAlphaOnPoint(int x, int y, float magnitude) {
   // See this forum post: https://discourse.libsdl.org/t/is-it-possible-to-modify-a-part-of-an-sdl-textures-alpha-value/22841
}

void Decal::SetPartialAlphaOnRect(SDL_Rect* rect, float magnitude) {
   // See above
}

Decal::~Decal() {

}
