#pragma once

#ifndef STAGE_H
#define STAGE_H

#include <hge.h>
#include <hgesprite.h>
#include "CMap.h"
#include "Treasure.h"
#include "global.h"

const float STAGEWIDTH = 640.0f, STAGEHEIGHT = 480.0f;
enum Item{EMPTY, SNAKE, BALL, TREASURE, WALL, DOOR, SNAKE2};
class Treasure;
class Map;

class Stage {
    friend class Snake;
    friend bool RenderFunc();
    friend void RunningFunc(float);
    friend void DRunning(float);
    friend void DRunningRender();
public:
    Stage(HGE*, float*, float* MoveTimer2 = NULL);
    ~Stage();
    void Update(float);
    void Render();
    bool SetBall(int, int);
    void AddBall(int value, int id = 0) {
        NowBall[id] += value;
    }
    int GetBall(int id = 0) {
        return NowBall[id];
    }
    bool SetTreasure(int, int);
    bool SetDoor(int, int);
    void ToLevel(int);
    void AddScore(int value, int id = 0) {
        Score[id] += value;
        NowScore[id] += value;
    }
    int GetScore(int id = 0) {
        return Score[id];
    }
    int Mat[MAXN][MAXM];
    float *MoveTimer[2];
    int* operator [](int i) { return Mat[i]; }
private:
    bool Doored;
    int BallX, BallY;
    int DoorX, DoorY;
    int Score[2];
    int NowScore[2];
    int NowBall[2];
    int BallNeed;
    int ScoreNeed;
    int id;
    HGE *hge;
    HTEXTURE tex;
    HTEXTURE ball;
    HTEXTURE wall;
    HTEXTURE door;
    hgeSprite *bg;
    hgeSprite *ba;
    hgeSprite *wa;
    hgeSprite *doo;
    Map *level;
    Treasure *tre;
};

#endif