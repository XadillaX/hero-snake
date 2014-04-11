#pragma once
#ifndef RANK_H
#define RANK_H

#include <fstream>
#include <algorithm>

class RankData {
    friend class Rank;
    friend void RunningFunc(float);
private:
    char Id[10], Score[255], Level[255], Name[255];
    bool operator >(const RankData& r) const {
        return atoi(Score) == atoi(r.Score) ? atoi(Level) > atoi(Score) : atoi(Score) > atoi(r.Score);
    }
};

class Rank {
    friend void RunningFunc(float);
public:
    Rank();
    ~Rank();
    void SaveRank();
    char* GetRankScore(int id) {
        return R[id].Score;
    }
    char* GetRankLevel(int id) {
        return R[id].Level;
    }
    char* GetRankId(int id) {
        return R[id].Id;
    }
    char* GetRankName(int id) {
        return R[id].Name;
    }
private:
    RankData R[10];
};

#endif