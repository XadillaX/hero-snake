#ifndef MAPPREVIEW_H
#define MAPPREVIEW_H
#pragma once
#include <hge.h>
#include <hgesprite.h>
#include "CMap.h"

#define SAFEDEL(p) { if(p != NULL) { delete p; p = NULL; } }

class MapPreview
{
public:
    MapPreview(HGE *hge, Map *m, int id);
    ~MapPreview(void);
    HTEXTURE GetTexture();

private:
    HGE *hge;
    HTARGET panel;
    int id;
};

#endif