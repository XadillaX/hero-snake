#pragma once

#ifndef SNAKE_H
#define SNAKE_H

#include "SnakeNode.h"
#include "Stage.h"
#include "global.h"
#include <list>
#include <hge.h>
#include <hgeparticle.h>

using namespace std;
enum Dir{LEFT, UP, RIGHT, DOWN};
const int dirx[] = {-1,0,1,0};
const int diry[] = {0,-1,0,1};

class Stage;

class Snake {
public:
    Snake(HGE*, Stage*, bool DR = false);
    ~Snake();
    void Render();
    void Update(float);
    void SetDir(int);
    int Move(float);
    void CutTail();
    void ToLevel(int);
    void SetColor(DWORD);
    int Count() {
        return l.size();
    }
    list<SnakeNode*> l;
private:
    int dir, last;
    HGE* hge;
    HTEXTURE tex;
    HEFFECT coin;
    HEFFECT dokan;
    HEFFECT boomsnd;
    HEFFECT tresnd;
    DWORD BodyColor;
    Stage* stg;
    hgeSprite *parspt;
    hgeParticleSystem* par;
    bool DR;                    ///< 1P或者单人模式为false，2P为true
};

#endif