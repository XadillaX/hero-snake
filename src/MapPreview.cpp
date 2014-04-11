#include "MapPreview.h"
#include <cstdio>

const float PreWidth = 640.0f;
const float PreHeight = 480.0f;
HTEXTURE hPreBG = 0;
HTEXTURE hPreWall = 0;
hgeSprite* PreBG = NULL;
hgeSprite* PreWall = NULL;
Map* PreMap;

MapPreview::MapPreview(HGE *hge, Map* m, int id) : hge(hge), id(id)
{
    if(hPreBG == 0)
    {
        PreMap = m;
        hPreBG = hge->Texture_Load("images\\stagebg.png");
        hPreWall = hge->Texture_Load("images\\wall.png");

        PreBG = new hgeSprite(hPreBG, 0, 0, 640, 480);
        PreWall = new hgeSprite(hPreWall, 0, 0, 32, 32);
    }

    //panel = hge->Target_Create(PreWidth, PreHeight, false);
}

HTEXTURE MapPreview::GetTexture()
{
    panel = hge->Target_Create(PreWidth, PreHeight, false);
    hge->Gfx_BeginScene(panel);
    hge->Gfx_Clear(0);

    PreBG->Render(0, 0);
    for(int i = 0; i < 15; i++)
        for(int j = 0; j < 20; j++)
        {
            if(PreMap->Mat[id][i][j] == 4)
                PreWall->Render(j * 32, i * 32);
        }

    //hge->Gfx_RenderLine(0, 0, 0, 480);
    //hge->Gfx_RenderLine(0, 480, 640, 480);
    //hge->Gfx_RenderLine(640, 480, 640, 0);
    //hge->Gfx_RenderLine(640, 0, 0, 0);
    hge->Gfx_EndScene();

    return hge->Target_GetTexture(panel);
}

MapPreview::~MapPreview(void)
{
    if(hPreBG != 0)
    {
        hge->Texture_Free(hPreBG);
        hge->Texture_Free(hPreWall);
        hPreBG = hPreWall = 0;

        SAFEDEL(PreBG);
        SAFEDEL(PreWall);
    }

    hge->Target_Free(panel);
}
