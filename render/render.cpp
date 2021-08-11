#include "render.hpp"


void render::Line(overlay* obj, int posx1, int posy1, int posx2, int posy2, Color color)
{
    D3DCOLOR dwColor = COL2DWORD(color);
    vertex vert[2] =
    {
        { float(posx1), float(posy1), 0.0f, 1.0f, dwColor },
        { float(posx2), float(posy2), 0.0f, 1.0f, dwColor }
    };

    obj->d3d_device->SetTexture(0, nullptr);
    obj->d3d_device->DrawPrimitiveUP(D3DPT_LINELIST, 1, &vert, sizeof(vertex));
}

void render::Rect(overlay* obj, int posx1, int posy1, int posx2, int posy2, Color color) {
    D3DCOLOR dwColor = COL2DWORD(color);

    posx2 -= 1; posy2 -= 1;

    vertex vert[5] =
    {   
        { float(posx1), float(posy1), 1.0f, 1.0f, dwColor }, 
        { float(posx2), float(posy1), 1.0f, 1.0f, dwColor }, 
        { float(posx2), float(posy2), 1.0f, 1.0f, dwColor }, 
        { float(posx1), float(posy2), 1.0f, 1.0f, dwColor }, 
        { float(posx1), float(posy1), 1.0f, 1.0f, dwColor }  
    };

    obj->d3d_device->SetTexture(0, nullptr);
    obj->d3d_device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);  
    obj->d3d_device->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, &vert, sizeof(vertex));
    obj->d3d_device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);  
}

void render::RectBordered(overlay* obj, int posx1, int posy1, int posx2, int posy2, Color color)
{
    auto col = Color::Black();
    col.alpha = color.alpha;
    render::Rect(obj, posx1, posy1, posx2, posy2, color);
    render::Rect(obj, posx1 - 1, posy1 - 1, posx2 + 1, posy2 + 1, col);
    render::Rect(obj, posx1 + 1, posy1 + 1, posx2 - 1, posy2 - 1, col);
}

void render::RectFilled(overlay* obj, int posx1, int posy1, int posx2, int posy2, Color color) 
{
    D3DCOLOR dwColor = COL2DWORD(color);

    vertex vert[4] =
    {
        { float(posx1), float(posy1), 0.0f, 1.0f, dwColor },
        { float(posx2), float(posy1), 0.0f, 1.0f, dwColor },
        { float(posx1), float(posy2), 0.0f, 1.0f, dwColor },
        { float(posx2), float(posy2), 0.0f, 1.0f, dwColor }
    };

    obj->d3d_device->SetTexture(0, nullptr);
    obj->d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &vert, sizeof(vertex));

}