#include "Snake.h"

Snake::Snake(HGE* hge, Stage* stg, bool DR):hge(hge),stg(stg),DR(DR) {
    tex = hge->Texture_Load("images\\particles.png");
    coin = hge->Effect_Load("sound\\coin.mp3");
    dokan = hge->Effect_Load("sound\\dokan.mp3");
    boomsnd = hge->Effect_Load("sound\\tekifire.mp3");
    tresnd = hge->Effect_Load("sound\\powerup.mp3");
    parspt = new hgeSprite(tex, 64, 64, 32, 32);
    par = new hgeParticleSystem("images\\tail.psi",parspt);
    if(!DR) stg->tre->BindSnake(this, 0);
    else stg->tre->BindSnake(this, 1);
    ToLevel(0);
    BodyColor = 0xFFFF7800;
}

void Snake::Render() {
    for (list<SnakeNode*>::iterator it = l.begin(); it != l.end(); it++) {
        (*it)->Render(BodyColor, it == l.begin(), DR);
    }
    par->Render();
}

void Snake::Update(float dt) {
    par->Update(dt);
}

void Snake::SetDir(int _dir) {
    if (abs(_dir - last) != 2)
        dir = _dir;
}

void Snake::SetColor(DWORD c) {
    BodyColor = c;
}

//返回0则死亡 1表示正常移动 2表示吃到球 3表示win
int Snake::Move(float dt) {
    int id = (!DR) ? 0 : 1;
    SnakeNode *head = l.front();
    int newx = head->x + dirx[dir];
    int newy = head->y + diry[dir];
    bool flag = false;
    if (newx < 0 || newx >= 20 || newy < 0 || newy >= 15) {
        newx = (newx + 20) % 20;
        newy = (newy + 15) % 15;
    }
    
    if (stg->Mat[newy][newx] == SNAKE && !DR)       ///< 1P撞自己
        return 0;
    else if(stg->Mat[newy][newx] == SNAKE2 && DR)   ///< 2P撞自己
        return 0;
    else if (stg->Mat[newy][newx] == WALL) {
        if (stg->tre->WallLeft[id] == 0)
            return 0;
        else {
            stg->tre->WallLeft[id]--;
            stg->AddScore(-100, id);
        }
    }
    else if (stg->Mat[newy][newx] == BALL) {
        stg->Mat[newy][newx] = EMPTY;
        stg->AddScore(100, id);
        stg->AddBall(1, id);
        flag = true;
        hge->Effect_PlayEx(coin, EffectVol, 0, 1.0f, false);
    }
    else if (stg->Mat[newy][newx] == TREASURE) {
        stg->Mat[newy][newx] = EMPTY;
        stg->AddScore(stg->tre->DoEffect(newx, newy, id), id);
        if (stg->GetScore(id) < -500000000) {
            stg->AddScore(1000000000, id);
            hge->Effect_PlayEx(boomsnd, EffectVol, 0, 1.0f, false);
            return 0;
        }
        hge->Effect_PlayEx(tresnd, EffectVol, 0, 1.0f, false);
    }
    else if (stg->Mat[newy][newx] == DOOR && !DR/** 2P就不进门 */) {
        stg->id++;
        if (stg->id == LevelNum) {
            return 3;
        } else {
            ToLevel(stg->id);
        }
        return 1;
    }
    l.push_front(new SnakeNode("images\\particles.png", 96, 64, 32, 32, newx, newy, hge));
    if(!DR) stg->Mat[newy][newx] = SNAKE;           ///< 1P
    else if(DR) stg->Mat[newy][newx] = SNAKE2;      ///< 2P
    last = dir;
    if (!flag) {
        SnakeNode *tail = l.back();
        par->info.colColorStart = 0xFFFF0000;
        par->info.colColorEnd = 0xFFFFFF00;
        par->info.nEmission = 500;//(int)(90 * dt * 90 * dt + 90 * dt + 90 * dt) * 20;
        par->MoveTo(32 * tail->x + 80, 32 * tail->y + 60);
        l.pop_back();
        stg->Mat[tail->y][tail->x] = EMPTY;
        delete tail;
    }
    else {
        int bx = hge->Random_Int(0,19);
        int by = hge->Random_Int(0,14);
        while (!stg->SetBall(bx, by)) {
            bx = hge->Random_Int(0,19);
            by = hge->Random_Int(0,14);
        }
        return 2;
    }
    if (((!stg->Doored && !DR) && stg->BallNeed <= stg->NowBall[0] && stg->ScoreNeed <= stg->NowScore[0]) || (DR && stg->Doored)) { 
        if(!DR)      ///< 1P或者单人模式建门
        {
            int bx = hge->Random_Int(0,19);
            int by = hge->Random_Int(0,14);
            while (!stg->SetDoor(bx, by)) {
                bx = hge->Random_Int(0,19);
                by = hge->Random_Int(0,14);
            }
            stg->Doored = true;
        }
        else        ///< 2P毁门
        if(DR)
        {
            for(int i = 0; i < 15; i++)
                for(int j = 0; j < 20; j++)
                    if(stg->Mat[i][j] == DOOR) stg->Mat[i][j] = EMPTY;

            stg->Doored = false;
            stg->DoorX = stg->DoorY = -1;
        }

    }
    //if (newx == 19 && newy == 14)
    //    ToLevel(1);
    return 1;
}

void Snake::CutTail() {
    stg->Mat[l.back()->y][l.back()->x] = EMPTY;
    l.pop_back();
}

void Snake::ToLevel(int id) {
    if (id) hge->Effect_PlayEx(dokan, EffectVol, 0, 1.0f, false);
    if(!DR) stg->ToLevel(id);
    l.clear();
    int tx, ty;

    if(!DR)
    {
        /** 1P */
        for (int i = 0;i < MAXN;i++) {
            bool flag;
            for (int j = 0;j < MAXM - 5;j++) {
                flag = 1;

                for (int k = 0;k < 5 && flag;k++) {
                    flag &= stg->Mat[i][j + k] == EMPTY;
                }
                if (flag) {
                    tx = j;
                    ty = i;
                    break;
                }
            }
            if (flag) break;
        }

        last = dir = RIGHT;
    }
    else
    if(DR)
    {
        /** 2P */
        for (int i = MAXN - 1;i >= 0;i--) {
            bool flag;
            for (int j = MAXM - 6; j >= 0; j--) {
                flag = 1;
                for (int k = 0;k < 5 && flag;k++) {
                    flag &= stg->Mat[i][j + k] == EMPTY;
                }
                if (flag) {
                    tx = j;
                    ty = i;
                    break;
                }




            }
            if (flag) break;
        }

        last = dir = LEFT;

    }

    /** 推入蛇身 */
    for (int i = 3; i >= 0; i--) {
        if(!DR)
        {
            l.push_back(new SnakeNode("images\\particles.png", 96, 64, 32, 32, tx + i, ty, hge));
            stg->Mat[ty][tx + i] = SNAKE;
        }
        else
        if(DR)
        {
            l.push_front(new SnakeNode("images\\particles.png", 96, 64, 32, 32, tx + i + 1, ty, hge));
            stg->Mat[ty][tx + i + 1] = SNAKE2;
        }
        
    }
    if(!DR) par->FireAt(80 + (tx - 1) * 32, 60 + ty * 32);
    else par->FireAt(80 + (tx + 3) * 32, 60 + ty * 32);
    //last = dir = RIGHT;
    if (!DR) {
        int bx = hge->Random_Int(0,19);
        int by = hge->Random_Int(0,14);
        while (!stg->SetBall(bx, by)) {
            bx = hge->Random_Int(0,19);
            by = hge->Random_Int(0,14);
        }
    }
}

Snake::~Snake() {
    delete parspt;
    delete par;
    hge->Texture_Free(tex);
    while (!l.empty()) {
        SnakeNode *tmp = l.front();
        l.pop_front();
        delete tmp;
    }
}
