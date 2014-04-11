#pragma once

#ifndef CMAP_H
#define CMAP_H

#include <hge.h>
#include <string>

const int MAXN = 15, MAXM = 20;
const int MAXLEVEL = 99;
extern int LevelNum;

class Map {
public:
    Map();
    ~Map();
    bool GetMap(int, int[MAXN][MAXM]);
    int Mat[MAXLEVEL][MAXN][MAXM];
    int BallNeed[MAXLEVEL];
    int ScoreNeed[MAXLEVEL];
};

#endif