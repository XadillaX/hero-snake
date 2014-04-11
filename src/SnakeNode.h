#pragma once

#ifndef SNAKENODE_H
#define SNAKENODE_h

#include <hge.h>
#include <hgesprite.h>
#include <hgecolor.h>
const float BodyWidth = 32.0f;
const float BodyHeight = 32.0f;

class SnakeNode { 
public:
    SnakeNode(char*, float, float, float, float, int, int, HGE*);
    ~SnakeNode(void);
    void Render(DWORD, bool ishead = false, bool DR = false);
    int x, y; //舞台上格子坐标
private:
    hgeSprite *body;
    HTEXTURE tex;
    HGE* hge;
};

#endif