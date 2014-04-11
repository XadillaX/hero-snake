#include <hge.h>
#include <hgegui.h>
#include <hgesprite.h>
#include <cstdio>
#include <io.h>
#include <cstdlib>
#include <fstream>
#include "./FontCN/GfxFont.h"
#include "./FontCN/GfxEdit.h"
#include "StageGUI.h"
using namespace std;

#define RES_FILTER "HeroSnake地图(*.txt)\0*.txt\0\0"

#define SAFEDEL(p) { if(p) { delete p; p = NULL; } }

HGE *hge = NULL;
GfxEdit *score_editor = NULL, *ball_editor = NULL;
GfxFont *fnt;
char filename[1000];
hgeGUI *Stage = NULL;
OPENFILENAME ofn;

void Init()
{
    filename[0] = '\0';

    score_editor = new GfxEdit(100, 0xffffffff, "宋体", 12, false, false, false);
    score_editor->SetNumOnly(true);
    score_editor->InsertCookie("2000");

    ball_editor = new GfxEdit(100, 0xffffffff, "宋体", 12, false, false, false);
    ball_editor->SetNumOnly(true);
    ball_editor->InsertCookie("15");

    fnt = new GfxFont("宋体", 12, true, false, false);

    /** 初始化Stage */
    Stage = new hgeGUI();
    for(int i = 0; i < 15; i++)
    {
        for(int j = 0; j < 20; j++)
        {
            Stage->AddCtrl(new StageGUI(i * 20 + j, j, i, Stage));
        }
    }
    ((StageGUI *)(Stage->GetCtrl(0)))->SetCurrent();

    /** 文件打开框 */
    memset(&ofn, 0, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hge->System_GetState(HGE_HWND);
    ofn.lpstrFilter = RES_FILTER;
    ofn.lpstrFile = filename;
    ofn.nMaxFile = 1000;
    ofn.Flags = OFN_OVERWRITEPROMPT;
    ofn.lpstrDefExt = "txt";
}

void SetTitle()
{
    char tmp[255];
    sprintf(tmp, "Hero Snake - MapEditor:%s", (strlen(filename)) ? filename : "No Name");
    if(!saved) strcat(tmp, " *");

    hge->System_SetState(HGE_TITLE, tmp);
}

void Open_Func()
{
    /** 若未保存 */
    if(!saved)
        if(MessageBox(hge->System_GetState(HGE_HWND), "文件未保存，确定打开？", "请确认", MB_OKCANCEL) == IDCANCEL) return;

    ofn.lpstrTitle = "打开地图";
    if(!GetOpenFileName(&ofn)) return;
    FILE *fp = fopen(ofn.lpstrFile, "r");
    if(!fp)
    {
        MessageBox(hge->System_GetState(HGE_HWND), "文件读取失败！", "失败", MB_ICONERROR);
        return;
    }

    char ch;
    for(int i = 0; i < 15; i++)
    {
        int j = 0;
        while(ch = fgetc(fp))
        {
            if(ch == '\n') break;
            mat[i][j++] = ch - '0';
            ((StageGUI *)Stage->GetCtrl(i * 20 + j - 1))->SetWall((ch == '0') ? false : true);
        }
    }

    char score_need[20], ball_need[20];
    fscanf(fp, "%s%s", ball_need, score_need);

    ball_editor->ClearCookie();
    ball_editor->InsertCookie(ball_need);
    score_editor->ClearCookie();
    score_editor->InsertCookie(score_need);

    fclose(fp);

    memcpy(filename, ofn.lpstrFile, sizeof(filename));
    ((StageGUI *)(Stage->GetCtrl(0)))->_SetCurrent();

    saved = true;
}

void New_Func()
{
    /** 若未保存 */
    if(!saved)
        if(MessageBox(hge->System_GetState(HGE_HWND), "文件未保存，确定新建？", "请确认", MB_OKCANCEL) == IDCANCEL) return;

    memset(mat, 0, sizeof(mat));
    for(int i = 0; i < 15; i++)
    {
        for(int j = 0; j < 20; j++)
        {
            ((StageGUI *)(Stage->GetCtrl(i * 20 + j)))->SetWall(false);
        }
    }

    filename[0] = '\0';
    ((StageGUI *)(Stage->GetCtrl(0)))->_SetCurrent();
    saved = true;
}

void Save_Func()
{
    ofn.lpstrTitle = "保存地图";
    
    if(strlen(filename) == 0)
    {
        if(!GetSaveFileName(&ofn)) return;
        /*if(0 == access(ofn.lpstrFile, 0))
        {
            if(MessageBox(hge->System_GetState(HGE_HWND), "文件已存在，确定覆盖？", "请确认", MB_OKCANCEL) == IDCANCEL) return;
        }*/
        memcpy(filename, ofn.lpstrFile, sizeof(filename));
    }

    FILE *fp = fopen(filename, "w+");
    if(!fp)
    {
        MessageBox(NULL, "保存地图出错！", "错误", MB_ICONERROR);
        return;
    }

    for(int i = 0; i < 15; i++)
    {
        for(int j = 0; j < 20; j++) fprintf(fp, "%d", mat[i][j]);
        fprintf(fp, "\n");
    }

    char ball[20], score[20];
    strcpy(ball, ball_editor->GetCookie());
    strcpy(score, score_editor->GetCookie());
    if(!strlen(ball))
    {
        strcpy(ball, "0");
        ball_editor->InsertCookie("0");
    }
    if(!strlen(score))
    {
        strcpy(score, "0");
        score_editor->InsertCookie("0");
    }
    fprintf(fp, "%s %s\n", ball, score);
    fclose(fp);

    saved = true;
}

bool FrameFunc()
{
    float dt = hge->Timer_GetDelta();

    if(hge->Input_GetKeyState(HGEK_CTRL))
    {
        if(hge->Input_GetKeyState(HGEK_O))
        {
            Open_Func();
            return false;
        }
        else
        if(hge->Input_GetKeyState(HGEK_N))
        {
            New_Func();
            return false;
        }
        else
        if(hge->Input_GetKeyState(HGEK_S))
        {
            Save_Func();
            return false;
        }
    }

    Stage->Update(dt);

    score_editor->Updata(dt);
    ball_editor->Updata(dt);

    /** 设置窗口标题 */
    SetTitle();

    return false;
}

void Input_Render()
{
    fnt->SetColor(0xffffffff);

    fnt->Print(80, 35, "球数需求：");
    ball_editor->Render(85 + 12 * 5, 35);

    fnt->Print(85 + 12 * 5 + 150, 35, "分数需求：");
    score_editor->Render(85 + 12 * 10 + 150, 35);

    fnt->SetColor(0xffff0000);
    fnt->Print(80, 60 + 480 + 15, "Space: 加墙    Ctrl + Space: 消墙\nEnter: 填充墙  Ctrl + Enter: 填充空\nCtrl + O: 打开  Ctrl + N: 新建   Ctrl + S: 保存");
}

void Render_Matrix()
{
    char tmp[1000] = "";
    for(int i = 0; i < 15; i++)
    {
        for(int j = 0; j < 20; j++) strcat(tmp, (mat[i][j] == 0) ? "0 " : "4 ");
        strcat(tmp, "\n");
    }

    fnt->Print(150, 5, tmp);
}

bool RenderFunc()
{
    hge->Gfx_BeginScene();
    hge->Gfx_Clear(0xff000000);

    Input_Render();
    Stage->Render();

    hge->Gfx_EndScene();

    return false;
}

void Release()
{
    SAFEDEL(score_editor);
    SAFEDEL(ball_editor);
    SAFEDEL(Stage);
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    hge = hgeCreate(HGE_VERSION);

    hge->System_SetState(HGE_TITLE, "Hero Snake - MapEditor");
    hge->System_SetState(HGE_WINDOWED, true);
    hge->System_SetState(HGE_SHOWSPLASH, false);
    hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
    hge->System_SetState(HGE_FPS, 100);
    hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
    hge->System_SetState(HGE_HIDEMOUSE, false);

    if(hge->System_Initiate())
    {
        Init();
        hge->System_Start();
    }

    Release();

    hge->System_Shutdown();
    hge->Release();
    
    return 0;
}
