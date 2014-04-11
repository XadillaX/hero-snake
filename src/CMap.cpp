#include "CMap.h"
#include <fstream>
#include <sstream>

int LevelNum;

Map::Map() {
    for (int k = 0;k < LevelNum;k++) {
        std::string s = "data\\map";
        std::ostringstream out;
        out << k;
        s += out.str();
        s += ".txt";
        std::ifstream fin(s.c_str());
        if (fin.fail()) {
            MessageBox(NULL, "Can't load level files.", "Error", MB_ICONSTOP);
            exit(-1);
        }
        for (int i = 0;i < MAXN;i++) {
            getline(fin,s);
            for (int j = 0;j < MAXM;j++) {
                Mat[k][i][j] = s[j] - '0';
            }
        }
        fin >> BallNeed[k] >> ScoreNeed[k];
    }
}

bool Map::GetMap(int id, int mat[MAXN][MAXM]) {
    if (id >= LevelNum || id < 0)
        return false;
    memcpy(mat, Mat[id], sizeof(Mat[id]));
    return true;
}

Map::~Map() {
}
