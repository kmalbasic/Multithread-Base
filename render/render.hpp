#pragma once
#include "color.hpp"
#include "../overlay/overlay.hpp"
#define COL2DWORD(x) (D3DCOLOR_ARGB(x.alpha, x.red, x.green, x.blue))

struct vertex
{
	float x, y, z, rhw;
	unsigned long long color;
};

namespace render {

	void Line(overlay* obj, int pos_x1, int pos_y1, int pos_x2, int pos_y2, Color color);
	void Rect(overlay* obj, int posx1, int posy1, int posx2, int posy2, Color color);
	void RectBordered(overlay* obj, int posx1, int posy1, int posx2, int posy2, Color color);
	void RectFilled(overlay* obj, int posx1, int posy1, int posx2, int posy2, Color color);

}