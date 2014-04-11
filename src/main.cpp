#include <hge.h>
#include <hgesprite.h>
#include <hgegui.h>
#include <hgefont.h>
#include <hgeparticle.h>
#include "guiappscrollbar.h" 
#include "Stage.h"
#include "Snake.h"
#include "hMenu.h"
#include "Input.h"
#include "Rank.h"
#include "MapPreview.h"
#include "Firework.h"
#include "FireworkDisplay.h"
#include "FireworkManager.h"
#include "global.h"
#include <sstream>
#include <fstream>
#include <string>
using namespace std;

HGE *hge;

hgeSprite *bg;
hgeSprite *win;
hgeFont *fnt;

HTEXTURE bgtex;
HTEXTURE wintex;
HCHANNEL deathchn = 0;
HEFFECT death;
HEFFECT deathbgm;
HCHANNEL bgmchn;
HEFFECT bgm;
HEFFECT startbgm;
HEFFECT winbgm;
HCHANNEL startbgmchn;

HEFFECT snd;
HTEXTURE cursor;
hgeQuad quad;

hgeGUI *gui;
hgeGUI *losegui;
hgeGUI *wingui;
hgeGUI *rankgui;
hgeGUI *optgui;
hgeGUI *insgui;
hgeGUI *pausegui;
hgeGUI *Dpausegui;
hgeSprite *guispr;

/* 烟花 */
bool        quitgame;
HWND        g_hWnd;
FireworkManager* fireworks;
FireworkDisplay* firework_display;

tagPOINT mouse_pos;

HTEXTURE particles_texture;
hgeSprite* rocket_sprite;
hgeSprite* explosion_sprite;

/* New Var */
HTEXTURE logotex;
hgeSprite *logo;
Map* X_MAP = NULL;
MapPreview* pPreview = NULL;
HTEXTURE hPreview = 0;
hgeSprite* pPrevSpr = NULL;
int NowChsID;
const float fPrebs = 0.5f;
const float PauseTimer = 0.2f;
float TPauseTimer = 0.0f;
hgeFont* XFont;
hgeGUI* DChsGUI;
Snake *Dsnk[2];
HTEXTURE tretex[5];
hgeSprite *trespr[5];
int wholose;            ///< 0: 1P, 1: 2P, 3: Both
//Consts
float MoveTimer[2];
float NowTimer[2];
char Name[255];
bool NewRecord = 0;

//准备 运行 胜利 失败 排行榜 设置 说明 双人选择 双人运行 双人失败 暂停 双人暂停
enum GameState{READY, RUNNING, WIN, LOSE, RANK, OPTION, INSTRUCTION, DRUNNING_CHS, DRUNNING, DRUNNINGLOSE, PAUSE, DPAUSE};
GameState State = READY;
Snake *snk;
Stage *stg;
Rank *rnk;

//初始化关卡
void InitStage() {
    stg = new Stage(hge, &MoveTimer[0]);
    snk = new Snake(hge, stg);
    MoveTimer[0] = OrigTimer;
    NowTimer[0] = 0.0f;
}

//准备状态函数
void ReadyFunc() {
    InitStage();
    State = RUNNING;
    bgmchn = hge->Effect_PlayEx(bgm, MusicVol, 0, 1.0F, true);
}

//相当于烟花析构
static void App_DeInit() {
}

//烟花初始化
static BOOL InitApp() {
    wingui = new hgeGUI();
    wingui->SetCursor(new hgeSprite(hge->Texture_Load("images\\cursor.png"),0,0,32,32));
    particles_texture = hge->Texture_Load("images\\particles.png");
    rocket_sprite = new hgeSprite(particles_texture, 96, 32, 32, 32);
    explosion_sprite = new hgeSprite(particles_texture, 32, 32, 32, 32);
    rocket_sprite->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHAADD | BLEND_NOZWRITE);
    rocket_sprite->SetHotSpot(16,16);
    explosion_sprite->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHAADD | BLEND_NOZWRITE);
    explosion_sprite->SetHotSpot(16,16);

    fireworks = new FireworkManager(hge, "images\\rocket.psi","images\\explosion.psi",rocket_sprite,explosion_sprite);
    firework_display = new FireworkDisplay(hge, "images\\rocket.psi","images\\explosion.psi",rocket_sprite,explosion_sprite);

    firework_display->AddFirework(400,600,100,100,0xFFFF0000,0.000);
    firework_display->AddFirework(400,600,200,100,0xFF00FF00,0.200);
    firework_display->AddFirework(400,600,300,100,0xFF0000FF,0.400);
    firework_display->AddFirework(400,600,400,100,0xFFFF00FF,0.600);
    firework_display->AddFirework(400,600,500,100,0xFF00FFFF,0.800);
    firework_display->AddFirework(400,600,600,100,0xFFFFFF00,1.000);
    firework_display->AddFirework(400,600,700,100,0xFFFFFFFF,1.200);

    firework_display->Play();

    return TRUE;
}

//获得随机烟花颜色
DWORD GetColor() {
    DWORD color = 0xFF000000;
    if (hge->Random_Int(0,100) > 50 ) color += 0x00FF0000;
    if (hge->Random_Int(0,100) > 50 ) color += 0x0000FF00;
    if (hge->Random_Int(0,100) > 50 ) color += 0x000000FF;
    if (color == 0xFF000000) color = 0xFFFFFFFF;
    return color;
}

//双人运行状态
void DRunning(float dt) {
    TPauseTimer += dt;
    if (TPauseTimer >= PauseTimer) {
        if (hge->Input_GetKeyState(HGEK_SPACE) || hge->Input_GetKeyState(HGEK_ENTER)) {
            State = DPAUSE;
            hge->Channel_PauseAll();
            Dpausegui->SetNavMode(HGEGUI_UPDOWN | HGEGUI_CYCLED);
            Dpausegui->SetCursor(guispr);
            Dpausegui->SetFocus(1);
            Dpausegui->Enter();
        }
    }
    for (int i = 0;i < 4;i++) {
        if (hge->Input_GetKeyState(HGEK_LEFT + i)) {
            Dsnk[0]->SetDir(i);
        }
    }

    /** 2P Dir */
    if(hge->Input_GetKeyState(HGEK_A)) Dsnk[1]->SetDir(LEFT);
    if(hge->Input_GetKeyState(HGEK_W)) Dsnk[1]->SetDir(UP);
    if(hge->Input_GetKeyState(HGEK_D)) Dsnk[1]->SetDir(RIGHT);
    if(hge->Input_GetKeyState(HGEK_S)) Dsnk[1]->SetDir(DOWN);

    NowTimer[0] += dt;
    NowTimer[1] += dt;
    stg->Update(dt);
    Dsnk[0]->Update(dt);
    Dsnk[1]->Update(dt);
    bool lose[2] = { false, false };
    for(int i = 0; i < 2; i++) {
        if(NowTimer[i] >= MoveTimer[i]) {
            NowTimer[i] = 0.0f;
            int MoveState= Dsnk[i]->Move(dt);
            /** 消门 */
            stg->Doored = false;
            for(int i = 0; i < 15; i++)
                for(int j = 0; j < 20; j++)
                    if(DOOR == stg->Mat[i][j]) stg->Mat[i][j] = EMPTY;
            /** 消门 */
            if(!MoveState) {
                lose[i] = true;
            } else if (MoveState == 2) {
                MoveTimer[i] *= 0.98f;
            }
        }
    }

    /** 查两蛇
                    /^\/^\
                  _|__|  O|
         \/     /~     \_/ \
          \____|__________/  \
                 \_______      \
                         `\     \                 \
                           |     |                  \
                          /      /                    \
                         /     /                       \ \
                       /      /                         \ \
                      /     /                            \  \
                    /     /             _----_            \   \
                   /     /           _-~      ~-_         |   |
                  (      (        _-~    _--_    ~-_     _/   |
                   \      ~-____-~    _-~    ~-_    ~-_-~    /
                     ~-_           _-~          ~-_       _-~   
                        ~--______-~                ~-___-~相撞 */
    for(int i = 0; i < 2; i++) {
        int o = (i == 0) ? 1 : 0;
        int x = Dsnk[i]->l.front()->x;
        int y = Dsnk[i]->l.front()->y;
        for(list<SnakeNode*>::iterator it = Dsnk[o]->l.begin(); it != Dsnk[o]->l.end(); it++) {
            if(x == (*it)->x && y == (*it)->y) {
                lose[i] = true;
                break;
            }
        }
    }

    /** 有蛇死了 */
    if(lose[0] || lose[1]) {
        if(lose[0] && lose[1]) wholose = 3;
        else
        if(lose[0]) wholose = 0;
        else wholose = 1;

        State = DRUNNINGLOSE;
        hge->Channel_StopAll();
        hge->Effect_PlayEx(death, EffectVol, 0, 1.0f, false);
        deathchn = hge->Effect_PlayEx(deathbgm, MusicVol, 0, 1.0f, false);

        losegui->SetNavMode(HGEGUI_UPDOWN | HGEGUI_CYCLED);
        losegui->SetCursor(guispr);
        losegui->SetFocus(1);
        losegui->Enter();
    }
}

//运行状态功能函数
void RunningFunc(float dt) {
    if (hge->Input_GetKeyState(HGEK_SPACE) || hge->Input_GetKeyState(HGEK_ENTER)) {
        State = PAUSE;
        hge->Channel_PauseAll();
        pausegui->SetNavMode(HGEGUI_UPDOWN | HGEGUI_CYCLED);
        pausegui->SetCursor(guispr);
        pausegui->SetFocus(1);
        pausegui->Enter();
    }
    for (int i = 0;i < 4;i++) {
        if (hge->Input_GetKeyState(HGEK_LEFT + i)) {
            snk->SetDir(i);
        }
    }
    NowTimer[0] += dt;
    stg->Update(dt);
    snk->Update(dt);
    if (NowTimer[0] >= MoveTimer[0]) {
        NowTimer[0] = 0.0f;
        int MoveState = snk->Move(dt);
        if (!MoveState) {
            State = LOSE;
            hge->Channel_Stop(bgmchn);
            hge->Effect_PlayEx(death, EffectVol, 0, 1.0f, false);
            deathchn = hge->Effect_PlayEx(deathbgm, MusicVol, 0, 1.0f, false);
            RankData tmp;
            sprintf(tmp.Score, "%d", stg->GetScore());
            sprintf(tmp.Level, "%d", stg->id + 1);
            if (tmp > rnk->R[9]) {
                for (int i = 0; i < 10; i++) {
                    if (tmp > rnk->R[i]) {
                        for (int j = 9; j > i; j--) {
                            rnk->R[j] = rnk->R[j - 1];
                            sprintf(rnk->R[j].Id, "%d", j + 1);
                        }
                        sprintf(tmp.Id, "%d", i + 1);
                        strcpy(tmp.Name, Name);
                        rnk->R[i] = tmp;
                        break;
                    }
                }
                NewRecord = true;
                rnk->SaveRank();
            }
            losegui->SetNavMode(HGEGUI_UPDOWN | HGEGUI_CYCLED);
            losegui->SetCursor(guispr);
            losegui->SetFocus(1);
            losegui->Enter();
        }
        else if (MoveState == 2) {
            MoveTimer[0] *= 0.98f;
        }
        else if (MoveState == 3) {
            State = WIN;
            hge->Channel_Stop(bgmchn);
            bgmchn = hge->Effect_PlayEx(winbgm, MusicVol, 0, 1.0f, false);
            wingui->SetNavMode(HGEGUI_UPDOWN | HGEGUI_CYCLED);
            wingui->SetCursor(guispr);
            wingui->SetFocus(1);
            wingui->Enter();
            InitApp();
        }
    }
}

//排行榜状态功能函数
void RankFunc() {
    State = RANK;
    rankgui->SetNavMode(HGEGUI_UPDOWN | HGEGUI_CYCLED);
    rankgui->SetCursor(guispr);
    rankgui->SetFocus(1);
    rankgui->Enter();
}

//设置状态功能函数
void OptionFunc() {
    State = OPTION;
    optgui->SetNavMode(HGEGUI_UPDOWN | HGEGUI_CYCLED);
    optgui->SetCursor(guispr);
    optgui->SetFocus(6);
    optgui->Enter();
}

//说明状态功能函数
void InstructionFunc() {
    State = INSTRUCTION;
    insgui->SetNavMode(HGEGUI_UPDOWN | HGEGUI_CYCLED);
    insgui->SetCursor(guispr);
    insgui->SetFocus(1);
    insgui->Enter();
}

//双人选择状态功能函数
void DRunningChsFunc() {
    State = DRUNNING_CHS;
    //NowChsID = 0;
    hPreview = pPreview->GetTexture();
    pPrevSpr = new hgeSprite(hPreview, 0, 0, 640, 480);

    DChsGUI->SetFocus(3);
    DChsGUI->Enter();
}

//主GUI每帧函数
bool GUIFrameFunc(float dt) {
    static int lastid = 0;
    int id;
    if (hge->Input_GetKeyState(HGEK_ESCAPE)) {
        lastid = 5;
        gui->Leave();
    }
    id = gui->Update(dt);
    if (id == -1) {
        switch (lastid) {
            case 1:
                hge->Channel_StopAll();
                ReadyFunc();
                break;
            case 2:
                DRunningChsFunc();
                break;
            case 3:
                RankFunc();
                break;
            case 4:
                OptionFunc();
                break;
            case 5:
                InstructionFunc();
                break;
            case 6:
                return true;
            case 7: break;
        }
    }
    else if (id) {
        lastid = id;
        gui->Leave();
    }
    return false;
}

//双人失败GUI每帧函数
void DRunningLoseFrameFunc(float dt) {
    static int lastid = 0;
    int id;
    id = losegui->Update(dt);
    if (id == -1) {
        switch (lastid) {
            case 1:
                delete Dsnk[0];
                delete Dsnk[1];
                delete stg;
                DRunningChsFunc();
                hge->Channel_StopAll();
                bgmchn = hge->Effect_PlayEx(startbgm, MusicVol, 0, 1.0F, true);
                break;
            case 2:
                delete Dsnk[0];
                delete Dsnk[1];
                delete stg;
                hge->Channel_StopAll();
                startbgmchn = hge->Effect_PlayEx(startbgm, MusicVol, 0, 1.0F, true);
                State = READY;
                gui->SetNavMode(HGEGUI_UPDOWN | HGEGUI_CYCLED);
                gui->SetCursor(guispr);
                gui->SetFocus(1);
                gui->Enter();
                break;
            default: break;
        }
    }
    else if (id) {
        lastid = id;
        losegui->Leave();
    }
}

//失败GUI每帧函数
void LoseGUIFrameFunc(float dt) {
    static int lastid = 0;
    int id;
    id = losegui->Update(dt);
    if (id == -1) {
        switch (lastid) {
            case 1:
                delete snk;
                delete stg;
                InitStage();
                hge->Channel_StopAll();
                bgmchn = hge->Effect_PlayEx(bgm, MusicVol, 0, 1.0F, true);
                State = RUNNING;
                NewRecord = false;
                break;
            case 2:
                delete snk;
                delete stg;
                hge->Channel_StopAll();
                startbgmchn = hge->Effect_PlayEx(startbgm, MusicVol, 0, 1.0F, true);
                State = READY;
                NewRecord = false;
                gui->SetNavMode(HGEGUI_UPDOWN | HGEGUI_CYCLED);
                gui->SetCursor(guispr);
                gui->SetFocus(1);
                gui->Enter();
                break;
            default: break;
        }
    }
    else if (id) {
        lastid = id;
        losegui->Leave();
    }
}

//胜利GUI每帧函数
void WinGUIFrameFunc(float dt) {
    static int lastid = 0;
    int id;
    id = wingui->Update(dt);
    if (id == -1) {
        switch (lastid) {
            case 1:
                delete snk;
                delete stg;
                InitStage();
                hge->Channel_StopAll();
                bgmchn = hge->Effect_PlayEx(bgm, MusicVol, 0, 1.0F, true);
                State = RUNNING;
                NewRecord = false;
                break;
            case 2:
                delete snk;
                delete stg;
                hge->Channel_StopAll();
                startbgmchn = hge->Effect_PlayEx(startbgm, MusicVol, 0, 1.0F, true);
                State = READY;
                NewRecord = false;
                gui->SetNavMode(HGEGUI_UPDOWN | HGEGUI_CYCLED);
                gui->SetCursor(guispr);
                gui->SetFocus(1);
                gui->Enter();
                break;
            default: break;
        }
    }
    else if (id) {
        lastid = id;
        wingui->Leave();
    }
}

//排行榜GUI每帧函数
void RankGUIFrameFunc(float dt) {
    static int lastid = 0;
    int id;
    id = rankgui->Update(dt);
    if (id == -1) {
        switch (lastid) {
            case 1:
                State = READY;
                hge->Channel_StopAll();
                bgmchn = hge->Effect_PlayEx(startbgm, MusicVol, 0, 1.0f, false);
                gui->SetNavMode(HGEGUI_UPDOWN | HGEGUI_CYCLED);
                gui->SetCursor(guispr);
                gui->SetFocus(1);
                gui->Enter();
                break;
            default: break;
        }
    }
    else if (id) {
        lastid = id;
        rankgui->Leave();
    }
}

//设置GUI每帧函数
void OptionGUIFrameFunc(float dt) {
    static int lastid = 0;
    int id;
    id = optgui->Update(dt);
    if (id == -1) {
        switch (lastid) {
            case 6:
                State = READY;
                gui->SetNavMode(HGEGUI_UPDOWN | HGEGUI_CYCLED);
                gui->SetCursor(guispr);
                gui->SetFocus(1);
                gui->Enter();
                break;
            default: 
                optgui->SetFocus(6);
                break;
        }
    }
    else if (id) {
        lastid = id;
        optgui->Leave();
    }
}

//说明GUI每帧函数
void InstructionGUIFrameFunc(float dt) {
    static int lastid = 0;
    int id;
    id = insgui->Update(dt);
    if (id == -1) {
        switch (lastid) {
            case 1:
                State = READY;
                gui->SetNavMode(HGEGUI_UPDOWN | HGEGUI_CYCLED);
                gui->SetCursor(guispr);
                gui->SetFocus(1);
                gui->Enter();
                break;
            default: 
                //optgui->SetFocus(6);
                break;
        }
    }
    else if (id) {
        lastid = id;
        insgui->Leave();
    }
}

//暂停GUI每帧函数
void PauseGUIFrameFunc(float dt) {
    static int lastid = 0;
    int id;
    id = pausegui->Update(dt);
    if (id == -1) {
        switch (lastid) {
            case 1:
                hge->Channel_ResumeAll();
                State = RUNNING;
                break;
            case 2:
                delete snk;
                delete stg;
                hge->Channel_StopAll();
                startbgmchn = hge->Effect_PlayEx(startbgm, MusicVol, 0, 1.0F, true);
                State = READY;
                NewRecord = false;
                gui->SetNavMode(HGEGUI_UPDOWN | HGEGUI_CYCLED);
                gui->SetCursor(guispr);
                gui->SetFocus(1);
                gui->Enter();
                break;
            default: break;
        }
    }
    else if (id) {
        lastid = id;
        pausegui->Leave();
    }
}

//双人暂停GUI每帧函数
void DPauseGUIFrameFunc(float dt) {
    static int lastid = 0;
    int id;
    id = Dpausegui->Update(dt);
    if (id == -1) {
        switch (lastid) {
            case 1:
                hge->Channel_ResumeAll();
                State = DRUNNING;
                break;
            case 2:
                delete snk;
                delete stg;
                hge->Channel_StopAll();
                startbgmchn = hge->Effect_PlayEx(startbgm, MusicVol, 0, 1.0F, true);
                State = READY;
                NewRecord = false;
                gui->SetNavMode(HGEGUI_UPDOWN | HGEGUI_CYCLED);
                gui->SetCursor(guispr);
                gui->SetFocus(1);
                gui->Enter();
                break;
            default: break;
        }
    }
    else if (id) {
        lastid = id;
        Dpausegui->Leave();
    }
}

//双人初始化游戏
void InitDStage() {
    stg = new Stage(hge, MoveTimer, MoveTimer + 1);
    Dsnk[0] = new Snake(hge, stg, false);
    Dsnk[1] = new Snake(hge, stg, true);
    Dsnk[0]->ToLevel(NowChsID);
    Dsnk[1]->ToLevel(NowChsID);
    MoveTimer[0] = MoveTimer[1] = OrigTimer;
    NowTimer[0] = NowTimer[1] = 0.0f;
}

//双人准备
void GetReady4DRunning() {
    InitDStage();
    State = DRUNNING;
    hge->Channel_StopAll();
    bgmchn = hge->Effect_PlayEx(bgm, MusicVol, 0, 1.0F, true);
    TPauseTimer = 0.0f;
}

//双人选择GUI每帧函数
void DRunning_ChsFrameFunc(float dt) {
    /** 选择GUI */
    static int lastchsid = 0;
    int chsid;
    chsid = DChsGUI->Update(dt);
    if(chsid != -1) {
        SAFEDEL(pPreview);                          ///< 删除PRE指针
        SAFEDEL(pPrevSpr);                          ///< 删除精灵指针
        switch (chsid) {
            case 1:
                NowChsID--;
                if(NowChsID < 0) NowChsID = LevelNum - 1;
                break;
            case 2:
                NowChsID++;
                NowChsID %= LevelNum;
                break;
            case 3:
                DChsGUI->Leave();
                GetReady4DRunning();
                break;
            case 4:
                DChsGUI->Leave();
                State = READY;
                gui->SetNavMode(HGEGUI_UPDOWN | HGEGUI_CYCLED);
                gui->SetCursor(guispr);
                gui->SetFocus(1);
                gui->Enter();
                break;
            default: break;
        }

        pPreview = new MapPreview(hge, X_MAP, NowChsID);
        hPreview = pPreview->GetTexture();
        pPrevSpr = new hgeSprite(hPreview, 0, 0, 640, 480);
    }
    /** Ending... */
}

//总每帧函数
bool FrameFunc() {
    float dt = hge->Timer_GetDelta();

    //背景浮动
    float dx = BGSpeed * dt, dy = -BGSpeed * dt;
    BGX += dx, BGY += dy;
    if (BGX >= BGOrigX + BGWidth)
        BGX = BGOrigX;
    if (BGY <= BGOrigY - BGHeight)
        BGY = BGOrigY;
    
    // 按ESC退出
    /*if (hge->Input_GetKeyState(HGEK_ESCAPE)) {
        return true;
    }*/

    if (State == READY) {
        if (startbgmchn == 0) {
            startbgmchn = hge->Effect_PlayEx(startbgm, MusicVol, 0, 1.0F, true);
        }
        if (GUIFrameFunc(dt))
            return true;
    }
    else if (State == RUNNING) {
        RunningFunc(dt);
    }
    else if (State == LOSE) {
        LoseGUIFrameFunc(dt);
    }
    else if (State == RANK) {
        RankGUIFrameFunc(dt);
    }
    else if (State == OPTION) {
        OptionGUIFrameFunc(dt);
        //更改名字
        Forminput* tForm = (Forminput*)optgui->GetCtrl(5);
        strcpy(Name ,tForm->getText());
        if (Name != hge->Ini_GetString("game", "name", "Snake")) {
            hge->Ini_SetString("game", "name", Name);
        }
        char tmp[255];
        //更改音乐音量
        GUIAppScrollBar* scroll = (GUIAppScrollBar*)optgui->GetCtrl(1);
        sprintf(tmp, "%.0f", scroll->GetValue());
        if (strcmp(tmp, hge->Ini_GetString("game", "musicvol", "100")) != 0) {
            hge->Ini_SetString("game", "musicvol", tmp);
            MusicVol = 1.0f * atoi(tmp);
            //hge->System_SetState(HGE_MUSVOLUME, atoi(tmp));
        }
        //更改音效音量
        scroll = (GUIAppScrollBar*)optgui->GetCtrl(2);
        sprintf(tmp, "%.0f", scroll->GetValue());
        if (strcmp(tmp, hge->Ini_GetString("game", "effectvol", "100")) != 0) {
            hge->Ini_SetString("game", "effectvol", tmp);
            EffectVol = 1.0f * atoi(tmp);
            //hge->System_SetState(HGE_FXVOLUME, atoi(tmp));
        }
        scroll = (GUIAppScrollBar*)optgui->GetCtrl(3);
        OrigTimer = (101.0f - scroll->GetValue()) / 200.0f;
        sprintf(tmp, "%.1f", 1.0f / OrigTimer);
        if (strcmp(tmp, hge->Ini_GetString("game", "speed", "4.0")) != 0) {
            hge->Ini_SetString("game", "speed", tmp);
        }
        scroll = (GUIAppScrollBar*)optgui->GetCtrl(4);
        int flag = scroll->GetValue() >= 50.0f;
        scroll->SetValue(100.0f * flag);
        if (flag != hge->Ini_GetInt("game", "fullscreen", 0)) {
            hge->Ini_SetInt("game", "fullscreen", flag);
            hge->System_SetState(HGE_WINDOWED, !flag);
        }
    } else if (State == INSTRUCTION) {
        InstructionGUIFrameFunc(dt);
    } else if(State == DRUNNING_CHS) {
        /** Todo: */
        DRunning_ChsFrameFunc(dt);
    } else if (State == DRUNNING) {
        DRunning(dt);
    } else if (State == DRUNNINGLOSE) {
        DRunningLoseFrameFunc(dt);
    } else if (State == PAUSE) {
        PauseGUIFrameFunc(dt);
    } else if (State == DPAUSE) {
        DPauseGUIFrameFunc(dt);
    } else if (State == WIN) {
        WinGUIFrameFunc(dt);
        if (hge->Input_GetKeyState(HGEK_SPACE) || hge->Input_GetKeyState(HGEK_ENTER)) {
            RankFunc();
        }
    }
    return false;
}

//双人选择渲染函数
void DR_CHS_Render() {
    static float x = (800 - (640 * fPrebs)) / 2;
    static float y = (600 - (480 * fPrebs)) / 2;

    XFont->SetColor(0xffff0099);
    XFont->printf(400, 100, HGETEXT_CENTER, "Please Choose A Map");
    XFont->printf(400, 500, HGETEXT_CENTER, "Level %d", NowChsID + 1);
    pPrevSpr->RenderEx(x, y, 0, fPrebs);                ///< 渲染地图预览

    DChsGUI->Render();
}

//双人运行渲染函数
void DRunningRender() {
    stg->Render();
    Dsnk[0]->Render();
    Dsnk[1]->Render();
    fnt->SetColor(0xFFFFFF00);
    fnt->printf(20, 15, HGETEXT_LEFT, "1P: ");
    fnt->printf(80, 15, HGETEXT_LEFT, "Score: %d", stg->GetScore(0));
    fnt->printf(250, 15, HGETEXT_LEFT, "Length: %d", Dsnk[0]->Count());
    fnt->printf(470, 15, HGETEXT_CENTER, "Speed: %.2lf", BaseTimer / MoveTimer[0]);
    fnt->printf(800 - 80, 15, HGETEXT_RIGHT, "Axe(s): %d", stg->tre->WallLeft[0]);
    fnt->printf(20, 555, HGETEXT_LEFT, "2P: ");
    fnt->printf(80, 555, HGETEXT_LEFT, "Score: %d", stg->GetScore(1));
    fnt->printf(250, 555, HGETEXT_LEFT, "Length: %d", Dsnk[1]->Count());
    fnt->printf(470, 555, HGETEXT_CENTER, "Speed: %.2lf", BaseTimer / MoveTimer[1]);
    fnt->printf(800 - 80, 555, HGETEXT_RIGHT, "Axe(s): %d", stg->tre->WallLeft[1]);
}

//总渲染函数
bool RenderFunc() {
    hge->Gfx_BeginScene();
    hge->Gfx_Clear(0xFF000000);
    if (State == READY || State == RANK || State == OPTION || State == INSTRUCTION || State == DRUNNING_CHS) {
        bg->SetColor(0xFFFFFFFF);
    } else {
        bg->SetColor(0xFFFF0000);
    }
    bg->Render(BGX, BGY);
    if (State == READY) {
        logo->Render(100, 50);

        fnt->SetColor(0xFFFF0080);
        fnt->printf(785, 525, HGETEXT_RIGHT, "2011.02 Version 1.0\nMade by XadillaX & Matrush @ X2Studio");
        //fnt->printf(785, 525, HGETEXT_RIGHT, "Version 1.0 Beta\nPowered by X2Studio.Net");
        gui->Render();
    } else if (State == RUNNING) {
        stg->Render();
        snk->Render();
        fnt->SetColor(0xFFFFFF00);
        fnt->printf(80, 15, HGETEXT_LEFT, "Level: %d", stg->id + 1);
        fnt->printf(250, 15, HGETEXT_LEFT, "Score: %d", stg->GetScore(0));
        fnt->printf(470, 15, HGETEXT_CENTER, "Length: %d", snk->Count());
        fnt->printf(800 - 80, 15, HGETEXT_RIGHT, "Speed: %.2lf", BaseTimer / MoveTimer[0]);
        fnt->printf(80, 555, HGETEXT_LEFT, "Axe(s): %d", stg->tre->WallLeft[0]);
        fnt->printf(400, 555, HGETEXT_CENTER, "ScoreLeft: %d", max(0, stg->ScoreNeed - stg->NowScore[0]));
        fnt->printf(800 - 80, 555, HGETEXT_RIGHT, "BallLeft: %d", max(0, stg->BallNeed - stg->NowBall[0]));
    } else if (State == LOSE) {
        stg->Render();
        snk->Render();

        if (NewRecord) {
            fnt->SetColor(0xFFFF0000);
            fnt->printf(400, 210, HGETEXT_CENTER, "New Record!");
        }
        fnt->SetColor(0xFFFFFFFF);
        fnt->printf(400, 250, HGETEXT_CENTER, "Game Over!\n Your Score is %d.", stg->GetScore(0));
        losegui->Render();
    } else if (State == RANK) {

        fnt->SetColor(0xFF7A1494);
        fnt->printf(400, 20, HGETEXT_CENTER, "RankList");
        fnt->SetColor(0xFFFF0000);
        fnt->printf(180-50, 60, HGETEXT_LEFT, "Rank");
        fnt->printf(250, 60, HGETEXT_LEFT, "Player");
        fnt->printf(400, 60, HGETEXT_LEFT, "Score");
        fnt->printf(600-37, 60, HGETEXT_LEFT, "MaxLevel");
        fnt->SetColor(0xFF7A1494);
        for (int i = 0; i < 10; i++) {
            fnt->printf(150, 105 + i * 45, HGETEXT_LEFT, rnk->GetRankId(i));
            fnt->printf(250, 105 + i * 45, HGETEXT_LEFT, rnk->GetRankName(i));
            fnt->printf(400, 105 + i * 45, HGETEXT_LEFT, rnk->GetRankScore(i));
            fnt->printf(600, 105 + i * 45, HGETEXT_LEFT, rnk->GetRankLevel(i));
        }

        rankgui->Render();
    } else if (State == OPTION) {

        fnt->SetColor(0xFF7A1494);
        fnt->printf(400, 60, HGETEXT_CENTER, "Option");
        fnt->printf(150, 120, HGETEXT_LEFT, "Music Volume:");
        fnt->printf(150, 190, HGETEXT_LEFT, "Effect Volume:");
        fnt->printf(150, 260, HGETEXT_LEFT, "Speed:");
        fnt->printf(150, 330, HGETEXT_LEFT, "FullScreen:");
        fnt->printf(150, 400, HGETEXT_LEFT, "Player Name:");

        optgui->Render();
    } else if (State == INSTRUCTION) {

        fnt->SetColor(0xFF7A1494);
        string s;
        s += "2088, the Earth are facing a unprecedented famine. Men,\n";
        s += "women, children and the old are all in hunger. At that\n";
        s += "time, the savior, Greedy Snake of 1976 appeared. He found\n";
        s += "that a distant planet, Tisiphone, has a great amount of\n";
        s += "food. But it's dangerous. To save human beings and all living\n";
        s += "beings on Earth, he decided to collect food from Tisiphone.\n";
        s += "There are several dimensions on Tisiphone. And there are tools\n";
        s += "for him to make it easy:\n\n\n\n\n";
        s += "He must be qualified enough to the next dimension. The more\n";
        s += "food he eat, the more lives can be saved. Yeah, you are him.";

        /**
        s += "This game is based on the traditional \"Greedy Snake\" game. \n";
        s += "In this game the player act as a \"Hero Snake\" to adventure. \n";
        s += "There are two modes in this game: 1 Player and 2 Players. \n\n";
        s += "In \"1 Player\" mode, player will face different maps and some\n";
        s += "random treasures in each level. In each level, the player must\n";
        s += "eat enough ball and get enough score to advance. Treasures\n";// next level. ";
        s += "and their effect can be seen below: \n\n\n\n\n";
        s += "In \"2 Players\" mode, both player will control a snake and try\n";
        s += "to get higher score, who have high scores will win the match.";
        */
        fnt->printf(65, 60, HGETEXT_LEFT, s.c_str());

        fnt->SetColor(0xffff0000);
        trespr[0]->Render(65, 280);
        fnt->printf(100, 283, HGETEXT_LEFT, "[Axe] Break the wall.");
        trespr[1]->Render(400, 280);
        fnt->printf(435, 283, HGETEXT_LEFT, "[Scissors] Cut one tail.");
        trespr[2]->Render(65, 315);
        fnt->printf(100, 318, HGETEXT_LEFT, "[Speeder] Slower speed.");
        trespr[3]->Render(400, 315);
        fnt->printf(435, 318, HGETEXT_LEFT, "[Skull] Make you die.");
        trespr[4]->Render(65, 350);
        fnt->printf(100, 353, HGETEXT_LEFT, "[Bottle] Change color.");

        insgui->Render();
    } else if (State == DRUNNING_CHS) {
        DR_CHS_Render();
    } else if (State == DRUNNING) {
        DRunningRender();
    } else if (State == DRUNNINGLOSE) {
        DRunningRender();
        losegui->Render();
        char *tmp = (wholose == 0) ? "1P " : ((wholose == 1) ? "2P " : "Both ");
        fnt->SetColor(0xFFFFFFFF);
        fnt->printf(400, 250, HGETEXT_CENTER, "Game Over!\n%s lose!", tmp);
    } else if (State == PAUSE) {
        stg->Render();
        snk->Render();

        fnt->SetColor(0xFFFFFFFF);
        fnt->printf(400, 270, HGETEXT_CENTER, "Pause");
        pausegui->Render();
    }
    else if (State == DPAUSE) {
        stg->Render();
        Dsnk[0]->Render();
        Dsnk[1]->Render();
        fnt->SetColor(0xFFFFFFFF);
        fnt->printf(400, 270, HGETEXT_CENTER, "Pause");
        Dpausegui->Render();
    } else if (State == WIN) {
        win->Render(0, 0);
        fnt->SetColor(0xFFFFFFFF);
        fnt->printf(600, 500, HGETEXT_CENTER, "Press Space To Continue!");
        
        wingui->Render();
        float xxx, yyy;
        hge->Input_GetMousePos(&xxx, &yyy);
        //if left mouse button clicked launch firework
        if (hge->Input_KeyDown(HGEK_LBUTTON)) {
            fireworks->AddFirework(400,600,xxx,yyy,GetColor());
        }
        fireworks->Tick();
        firework_display->Tick();
    }
    hge->Gfx_EndScene();
    return false;
}

//初始化双人选择GUI
void InitializeGUI() {
    guispr = new hgeSprite(cursor, 0, 0, 32, 32);
    gui = new hgeGUI();

    gui->AddCtrl(new hgeGUIMenuItem(1, fnt, snd, 400, 200 + 30, 0.0f, "[1 Player]"));
    gui->AddCtrl(new hgeGUIMenuItem(2, fnt, snd, 400, 240 + 30, 0.1f, "[2 Players]"));
    gui->AddCtrl(new hgeGUIMenuItem(3, fnt, snd, 400, 280 + 30, 0.2f, "[Rank]"));
    gui->AddCtrl(new hgeGUIMenuItem(4, fnt, snd, 400, 320 + 30, 0.3f, "[Option]"));
    gui->AddCtrl(new hgeGUIMenuItem(5, fnt, snd, 400, 360 + 30, 0.4f, "[Instruction]"));
    gui->AddCtrl(new hgeGUIMenuItem(6, fnt, snd, 400, 400 + 30, 0.5f, "[Exit]"));

    gui->SetNavMode(HGEGUI_UPDOWN | HGEGUI_CYCLED);
    gui->SetCursor(guispr);
    gui->SetFocus(1);
    gui->Enter();

    losegui = new hgeGUI();
    losegui->AddCtrl(new hgeGUIMenuItem(1, fnt, snd, 400, 320, 0.0f, "Restart"));
    losegui->AddCtrl(new hgeGUIMenuItem(2, fnt, snd, 400, 360, 0.1f, "Main Menu"));
        
    wingui = new hgeGUI();
    wingui->AddCtrl(new hgeGUIMenuItem(1, fnt, snd, 400, 320, 0.0f, "Restart"));
    wingui->AddCtrl(new hgeGUIMenuItem(2, fnt, snd, 400, 360, 0.1f, "Main Menu"));
        
    rankgui = new hgeGUI();
    rnk = new Rank();
    rankgui->AddCtrl(new hgeGUIMenuItem(1, fnt, snd, 400, 60 + 11 * 45, 0.0f, "Main Menu"));

    optgui = new hgeGUI();
    GUIAppScrollBar *scroll = new GUIAppScrollBar(1, GUIAppScrollBar::H_SCROLL, 350, 128, 270, "images\\btn-left-arrow.png", "images\\btn-right-arrow.png", "images\\btn-clear-arrow.png");
    scroll->SetLimits(0,100,10);
    scroll->SetValue(MusicVol);
    optgui->AddCtrl(scroll);
    scroll = new GUIAppScrollBar(2, GUIAppScrollBar::H_SCROLL, 350, 198, 270, "images\\btn-left-arrow.png", "images\\btn-right-arrow.png", "images\\btn-clear-arrow.png");
    scroll->SetLimits(0,100,10);
    scroll->SetValue(EffectVol);
    optgui->AddCtrl(scroll);
    scroll = new GUIAppScrollBar(3, GUIAppScrollBar::H_SCROLL, 350, 268, 270, "images\\btn-left-arrow.png", "images\\btn-right-arrow.png", "images\\btn-clear-arrow.png");
    scroll->SetLimits(0,100,10);
    scroll->SetValue(101.f - 200.0f * OrigTimer);
    optgui->AddCtrl(scroll);
    scroll = new GUIAppScrollBar(4, GUIAppScrollBar::H_SCROLL, 350, 338, 270, "images\\btn-left-arrow.png", "images\\btn-right-arrow.png", "images\\btn-clear-arrow.png");
    scroll->SetLimits(0,100,100);
    scroll->SetValue(hge->Ini_GetInt("game", "fullscreen", 0) * 100.0f);
    optgui->AddCtrl(scroll);
    optgui->AddCtrl(new Forminput(5, fnt, snd, 490, 400, 0.1f, Name));
    optgui->AddCtrl(new hgeGUIMenuItem(6, fnt, snd, 400, 470, 0.0f, "Main Menu"));
    optgui->SetFocus(6);
    //scroll->SetValue(scroll->GetMaxValue());
    for(int i = 0; i < 5; i++) {
        string a = "images\\treasure";
        a += (char)(i + '0');
        a += ".png";

        tretex[i] = hge->Texture_Load(a.c_str());
        trespr[i] = new hgeSprite(tretex[i], 0, 0, 32, 32);
        if(i == 4) {
            delete trespr[i];
            trespr[i] = new hgeSprite(tretex[i], 0, 0, 27, 27);
        }
    }

    insgui = new hgeGUI();
    insgui->AddCtrl(new hgeGUIMenuItem(1, fnt, snd, 400, 470, 0.0f, "Main Menu"));

    XFont = new hgeFont("images\\font.fnt");
    X_MAP = new Map();
    pPreview = new MapPreview(hge, X_MAP, 0);
    DChsGUI = new hgeGUI();
    DChsGUI->AddCtrl(new hgeGUIMenuItem(1, XFont, snd, 200, 300, 0, "<<<<"));
    DChsGUI->AddCtrl(new hgeGUIMenuItem(2, XFont, snd, 600, 300, 0, ">>>>"));
    DChsGUI->AddCtrl(new hgeGUIMenuItem(3, XFont, snd, 200, 500, 0, "Fight!"));
    DChsGUI->AddCtrl(new hgeGUIMenuItem(4, XFont, snd, 600, 500, 0, "Main Menu"));
    DChsGUI->SetCursor(guispr);
    DChsGUI->SetNavMode(HGEGUI_LEFTRIGHT | HGEGUI_CYCLED);

    pausegui = new hgeGUI();
    pausegui->AddCtrl(new hgeGUIMenuItem(1, fnt, snd, 400, 320, 0.0f, "Continue"));
    pausegui->AddCtrl(new hgeGUIMenuItem(2, fnt, snd, 400, 360, 0.1f, "Main Menu"));
    Dpausegui = new hgeGUI();
    Dpausegui->AddCtrl(new hgeGUIMenuItem(1, fnt, snd, 400, 320, 0.0f, "Continue"));
    Dpausegui->AddCtrl(new hgeGUIMenuItem(2, fnt, snd, 400, 360, 0.1f, "Main Menu"));
        
}

//Main
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    hge = hgeCreate(HGE_VERSION);
    hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
    hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
    hge->System_SetState(HGE_TITLE, "Hero Snake");
    hge->System_SetState(HGE_HIDEMOUSE, true);
    hge->System_SetState(HGE_DONTSUSPEND, true);
    hge->System_SetState(HGE_FPS, 100);
    hge->System_SetState(HGE_SCREENBPP, 32);
    hge->System_SetState(HGE_SHOWSPLASH, false);
    hge->System_SetState(HGE_ICON, "IDI_ICO");
    hge->System_SetState(HGE_INIFILE, "config.ini");
    hge->System_SetState(HGE_WINDOWED, !hge->Ini_GetInt("game", "fullscreen", 0));

    if (hge->System_Initiate()) {
        hge->Resource_AttachPack("Images.x2", "X2Studio");
        hge->Resource_AttachPack("Audio.x2", "X2Studio");


        bgtex = hge->Texture_Load("images\\bg.jpg");
        wintex = hge->Texture_Load("images\\win.png");
        death = hge->Effect_Load("sound\\death.mp3");
        deathbgm = hge->Effect_Load("sound\\deathbgm.mp3");
        bgm = hge->Effect_Load("sound\\bgm.mp3");
        cursor = hge->Texture_Load("images\\cursor.png");
        logotex = hge->Texture_Load("images\\logo.png");
        snd = hge->Effect_Load("sound\\menu.wav");
        startbgm = hge->Effect_Load("sound\\swd.mp3");
        winbgm = hge->Effect_Load("sound\\win.mp3");
        if (!bgtex || !death || !deathbgm || !bgm || !cursor || !snd || !startbgm) {
            MessageBox(NULL, "Can't load data files!", "Error", MB_OK | MB_ICONERROR | MB_APPLMODAL);
            goto CLOSE;
        }
        hge->Random_Seed(); //初始化随机种子
        fnt = new hgeFont("images\\font.fnt");
        bg = new hgeSprite(bgtex, 0, 0, 800 + BGWidth, 600 + BGHeight);
        logo = new hgeSprite(logotex, 0, 0, 600, 150);
        win = new hgeSprite(wintex, 0, 0, 800, 600);

        LevelNum = hge->Ini_GetInt("map", "levelnum", 0);
        OrigTimer = 1.0f / hge->Ini_GetFloat("game", "speed", 0);
        MusicVol = hge->Ini_GetFloat("game", "musicvol", 100.0f);
        EffectVol = hge->Ini_GetFloat("game", "effectvol", 100.0f);
        strcpy(Name, hge->Ini_GetString("game", "name", "Snake"));

        startbgmchn = 0;
        InitializeGUI();

        hge->System_Start();
CLOSE:
        hge->Texture_Free(bgtex);
        delete fnt;
        hge->Effect_Free(death);
        hge->Effect_Free(deathbgm);
        hge->Effect_Free(bgm);
        hge->System_Shutdown();
    }
    else {
        MessageBox(NULL, hge->System_GetErrorMessage(), "Error", MB_OK | MB_ICONERROR | MB_APPLMODAL);
    }

    hge->Release();

    return 0;
}