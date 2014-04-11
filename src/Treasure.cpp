#include "Treasure.h"

Treasure::Treasure(HGE* hge, Stage* stg):hge(hge), stg(stg) {
    wall = hge->Texture_Load("images\\treasure0.png");
    wa = new hgeSprite(wall, 0, 0, 32, 32);
    scissors = hge->Texture_Load("images\\treasure1.png");
    sc = new hgeSprite(scissors, 0, 0, 32, 32);
    clock = hge->Texture_Load("images\\treasure2.png");
    cl = new hgeSprite(clock, 0, 0, 32, 32);
    boom = hge->Texture_Load("images\\treasure3.png");
    bo = new hgeSprite(boom, 0, 0, 32, 32);

    for(int i = 0;  i < 5; i++)
    {
        char tmp[255];
        sprintf(tmp, "images\\treasure%d.png", i + 4);
        chgColor[i] = hge->Texture_Load(tmp);
        chgCo[i] = new hgeSprite(chgColor[i], 0, 0, 27, 27);
    }
    delete chgCo[4];
    chgCo[4] = new hgeSprite(chgColor[4], 0, 0, 32, 31);

    NowTimer = 0.0f;
    MaxTimer = hge->Random_Float(3.0f,5.0f);
    WallLeft[0] = WallLeft[1] = 0;

    l.clear();
}

void Treasure::AddTreasure() {
    Stuff tmp;
    tmp.Type = hge->Random_Int(0, 100);
    if (tmp.Type >= 0 && tmp.Type <= 20) tmp.Type = 0;
    else if (tmp.Type > 20 && tmp.Type <= 40) tmp.Type = 1;
    else if (tmp.Type > 40 && tmp.Type <= 60) tmp.Type = 2;
    else if (tmp.Type > 60 && tmp.Type <= 80) tmp.Type = 3;
    else if (tmp.Type > 80)
    {
        tmp.Type = hge->Random_Int(4, 8);
    }
    else return;
    tmp.NowTimer = 0.0f;
    tmp.MaxTimer = hge->Random_Float(3.0f,5.0f);
    int bx = hge->Random_Int(0,19);
    int by = hge->Random_Int(0,14);
    while (!stg->SetTreasure(bx, by)) {
        bx = hge->Random_Int(0,19);
        by = hge->Random_Int(0,14);
    }
    tmp.x = bx, tmp.y = by;
    l.push_back(tmp);
}

void Treasure::ClearTreasure() {
    for (list<Stuff>::iterator it = l.begin(); it != l.end();it++) {
        stg->Mat[it->y][it->x] = EMPTY;
    }
    l.clear();
}

void Treasure::Update(float dt) {
    NowTimer += dt;
    if (NowTimer >= MaxTimer) {
        AddTreasure();
        NowTimer = 0;
        MaxTimer = hge->Random_Float(3.0f,5.0f);
    }
    for (list<Stuff>::iterator it = l.begin(); it != l.end();) {
        it->NowTimer += dt;
        if (it->NowTimer >= it->MaxTimer) {
            stg->Mat[it->y][it->x] = EMPTY;
            l.erase(it++);
        }
        else {
            it++;
        }
    }
}

void Treasure::Render() {
    for (list<Stuff>::iterator it = l.begin(); it != l.end();it++) {
        hgeSprite* tmp;
        switch (it->Type) {
        case 0: tmp = wa; break;
        case 1: tmp = sc; break;
        case 2: tmp = cl; break;
        case 3: tmp = bo; break;
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
            tmp = chgCo[it->Type - 4]; break;
        default: return; break;
        }
        tmp->Render(80 + 32 * it->x, 60 + 32 * it->y);
    }
}

int Treasure::DoEffect(int x, int y, int id) {
    for (list<Stuff>::iterator it = l.begin(); it != l.end();it++) {
        if (it->x == x && it->y == y) {
            int t = hge->Random_Int(1, 3);
            switch (it->Type) {
            case 0: WallLeft[id] += t; t *= 100; break;
            case 1: if (snk[id]->Count() > 1) snk[id]->CutTail(); t = 100; break;
            case 2: *(stg->MoveTimer[id]) /= 0.9f; t = 100; break;
            case 3: t = -1000000000; break;
            case 4: snk[id]->SetColor(0xFFFFFFFF); t = 100; break;
            case 5: snk[id]->SetColor(0xFFFF7800); t = 100; break;
            case 6: snk[id]->SetColor(0xFFFF0000); t = 100; break;
            case 7: snk[id]->SetColor(0xFF4C52D9); t = 100; break;
            case 8: snk[id]->SetColor(0xFFB33ECC); t = 300; break;
            default: break;
            }
            l.erase(it);
            return t;
        }
    }
    return 0;
}

void Treasure::BindSnake(Snake* _snk, int id) {
    snk[id] = _snk;
}

Treasure::~Treasure() {
    delete wa;
    hge->Texture_Free(wall);
    delete sc;
    hge->Texture_Free(scissors);
    delete cl;
    hge->Texture_Free(clock);
    delete bo;
    hge->Texture_Free(boom);
    for(int i = 0; i < 5; i++) {
        delete chgCo[i];
        hge->Texture_Free(chgColor[i]);
    }
    l.clear();
}
