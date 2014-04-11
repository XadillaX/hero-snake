#include "Stage.h"
#include <cstring>

Stage::Stage(HGE* hge, float* MoveTimer1, float* MoveTimer2):hge(hge) {
    memset(Mat,0,sizeof(Mat));
    tex = hge->Texture_Load("images\\stagebg.png");
    bg = new hgeSprite(tex, 0, 0, STAGEWIDTH, STAGEHEIGHT);
    ball = hge->Texture_Load("images\\particles.png");
    ba = new hgeSprite(ball, 96, 64, 32, 32);
    wall = hge->Texture_Load("images\\wall.png");
    wa = new hgeSprite(wall, 0, 0, 32, 32);
    door = hge->Texture_Load("images\\door.png");
    doo = new hgeSprite(door, 0, 0, 32, 32);
    tre = new Treasure(hge, this);
    Score[0] = Score[1] = 0;
    NowScore[0] = NowScore[1] = 0;
    NowBall[0] = NowBall[1] = 0;

    MoveTimer[0] = MoveTimer1;
    MoveTimer[1] = MoveTimer2;

    level = new Map();
    ToLevel(0);
}

void Stage::Update(float dt) {
    tre->Update(dt);
}

void Stage::Render() {
    bg->Render((800 - STAGEWIDTH) / 2, (600 - STAGEHEIGHT) / 2);
    ba->SetColor(0xFF52B416);
    ba->Render(BallX * 32 + 80, BallY * 32 + 60);
    tre->Render();
    if (Doored) {
        doo->Render(DoorX * 32 + 80, DoorY * 32 + 60);
    }
    for (int i = 0;i < MAXN;i++) {
        for (int j = 0;j < MAXM;j++) {
            if (Mat[i][j] == WALL) {
                wa->Render(j * 32 + 80, i * 32 + 60);
            }
        }
    }
}

bool Stage::SetBall(int bx, int by) {
    if (Mat[by][bx] != EMPTY)
        return false;
    int tmp = 0;
    for (int i = 0;i < 4;i++) {
        int tx = bx + dirx[i], ty = by + diry[i];
        if (ty < 0 || ty >= 15 || tx < 0 || tx >= 20 || Mat[ty][tx] != EMPTY)
            tmp++;
    }
    if (tmp == 4)
        return false;
    BallX = bx, BallY = by;
    Mat[by][bx] = BALL;
    return true;
}

bool Stage::SetTreasure(int bx, int by) {
    if (Mat[by][bx] != EMPTY)
        return false;
    int tmp = 0;
    for (int i = 0;i < 4;i++) {
        int tx = bx + dirx[i], ty = by + diry[i];
        if (ty < 0 || ty >= 15 || tx < 0 || tx >= 20 || Mat[ty][tx] != EMPTY)
            tmp++;
    }
    if (tmp == 4)
        return false;
    Mat[by][bx] = TREASURE;
    return true;
}

bool Stage::SetDoor(int bx, int by) {
    if (Mat[by][bx] != EMPTY)
        return false;
    DoorX = bx, DoorY = by;
    Mat[by][bx] = DOOR;
    return true;
}

void Stage::ToLevel(int _id) {
    id = _id;
    NowScore[0] = NowScore[1] = 0;
    NowBall[0] = NowBall[1] = 0;
    Doored = false;
    BallNeed = level->BallNeed[id];
    ScoreNeed = level->ScoreNeed[id];
    level->GetMap(id, Mat);
    tre->ClearTreasure();
}

Stage::~Stage(void) {
    delete bg;
    hge->Texture_Free(tex);
    delete ba;
    hge->Texture_Free(ball);
    delete wa;
    hge->Texture_Free(wall);
    delete tre;
}
