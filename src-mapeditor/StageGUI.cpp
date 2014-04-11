#include "StageGUI.h"
#include "string.h"

bool saved = true;                              ///< 当前地图是否保存
HTEXTURE hStageBG = 0;                          ///< 舞台背景纹理句柄
HTEXTURE hWall = 0;                             ///< 墙纹理句柄
hgeSprite *StageBG = NULL;                      ///< 舞台背景精灵
hgeSprite *Wall = NULL;                         ///< 墙精灵
HEFFECT hEff = 0;                               ///< GUI音效
const float MAXTOYCUR_TIMER = 0.1f;             ///< 键盘控制延时时间

int mat[15][20];                                ///< 地图矩阵
bool vis[15][20];                               ///< 用于_floodfill函数

/**
 * @brief 填充函数
 * 将某块区域填充成指定的标记
 *
 * @param x 当前点的横坐标
 * @param y 当前点的纵坐标
 * @param flag 需要寻找的标记
 * @param gui 对应的GUI系统
 */
void _floodfill(int x, int y, int flag, hgeGUI* gui)
{
    if(flag != mat[y][x]) return;
    mat[y][x] = (flag == 0) ? 4 : 0;
    vis[y][x] = true;
    ((StageGUI *)gui->GetCtrl(y * 20 + x))->walled = ((flag == 0) ? true : false);
    int dx[] = { 0, -1, 0, 1 };
    int dy[] = { -1, 0, 1, 0 };

    for(int i = 0; i < 4; i++)
    {
        int newx = x + dx[i], newy = y + dy[i];
        if(newx >= 0 && newx < 20 && newy >= 0 && newy < 15)
        {
            if(!vis[newy][newx] && mat[newy][newx] == flag) _floodfill(newx, newy, flag, gui);
        }
    }
}

StageGUI::StageGUI(void)
{
}

/**
 * @brief 构造函数
 * 
 * @param _id GUI元素的编号
 * @param x GUI的横坐标
 * @param y GUI的纵坐标
 * @param par 所属于的GUI系统
 * @param walled 是否是桶标记
 */
StageGUI::StageGUI(int _id, int x, int y, hgeGUI* par, bool walled) : x(x), y(y), walled(walled), parent(par), cur(false), ycur(false),
ToYCURTimer(0.0f)
{
    if(!hStageBG)
    {
        hStageBG = hge->Texture_Load("stagebg.png");
        StageBG = new hgeSprite(hStageBG, 0, 0, 32, 32);
        hWall = hge->Texture_Load("images\\wall.png");
        Wall = new hgeSprite(hWall, 0, 0, 32, 32);
        hEff = hge->Effect_Load("sound\\menu.wav");
    }

    id = _id;
    rx = (float)(80 + x * 32), ry = (float)(60 + y * 32);
    rect.Set(rx, ry, rx + 32.0f, ry + 32.0f);

    bEnabled = true;
    bStatic = true;
    bVisible = true;

    mat[y][x] = 0;
}

/**
 * @brief 渲染函数
 * 渲染此GUI元素
 */
void StageGUI::Render()
{
    StageBG->Render(rx, ry);
    if(walled) Wall->Render(rx, ry);

    DWORD clr = (cur) ? 0xffff0000 : 0xffffffff;

    hge->Gfx_RenderLine(rx, ry, rx + 31.0f, ry, clr);
    hge->Gfx_RenderLine(rx + 31.0f, ry, rx + 31.0f, ry + 31.0f, clr);
    hge->Gfx_RenderLine(rx + 31.0f, ry + 31.0f, rx, ry + 31.0f, clr);
    hge->Gfx_RenderLine(rx, ry + 31.0f, rx, ry, clr);
}

/**
 * @brief 更新函数
 * 更新此GUI元素
 *
 * @param dt 与上一帧的时间间隔，单位毫秒
 */
void StageGUI::Update(float dt)
{
    if(cur)
    {
        if(ycur)
        {
            int dirx[] = { -1, 0, 1, 0 }, diry[] = { 0, -1, 0, 1 };
            for(int i = HGEK_LEFT; i <= HGEK_DOWN; i++)
            {
                if(hge->Input_GetKeyState(i))
                {
                    int nx = x + dirx[i - HGEK_LEFT], ny = y + diry[i - HGEK_LEFT];
                    if(nx >= 0 && nx < 20 && ny >= 0 && ny < 15)
                    {
                        cur = false;
                        ((StageGUI *)(parent->GetCtrl(ny * 20 + nx)))->SetCurrent();
                        return;
                    }
                }
            }
        }
        else
        {
            ToYCURTimer += dt;
            if(ToYCURTimer >= MAXTOYCUR_TIMER)
            {
                ToYCURTimer = 0.0f;
                ycur = true;
            }
        }

        if(hge->Input_GetKeyState(HGEK_SPACE) && hge->Input_GetKeyState(HGEK_CTRL))
        {
            if(walled) saved = false;
            walled = false;
        }
        else
        if(hge->Input_GetKeyState(HGEK_SPACE))
        {
            if(!walled) saved = false;
            walled = true;
        }
        else
        if(hge->Input_GetKeyState(HGEK_ENTER) && hge->Input_GetKeyState(HGEK_CTRL))
        {
            if(walled) saved = false;
            memset(vis, 0, sizeof(vis));
            _floodfill(x, y, 4, parent);
        }
        else
        if(hge->Input_GetKeyState(HGEK_ENTER))
        {
            if(!walled) saved = false;
            memset(vis, 0, sizeof(vis));
            _floodfill(x, y, 0, parent);
        }
    }

    mat[y][x] = (walled) ? 4 : 0;
}

/**
 * @brief 鼠标移至函数
 * 设置当前GUI元素为此对象
 *
 * @param bOver 是否移至
 */
void StageGUI::MouseOver(bool bOver)
{
    if(bOver)
    {
        _SetCurrent();
    }
}

/**
 * @brief (*)设置当前GUI函数
 * 设置整个GUI系统的当前GUI
 */
void StageGUI::_SetCurrent()
{
    for(int i = 0; i < 15; i++)
    {
        for(int j = 0; j < 20; j++)
        {
            StageGUI *tmp = (StageGUI *)(parent->GetCtrl(i * 20 + j));
            tmp->SetCurrent(false);
        }
    }

    SetCurrent();
    ycur = true;
}

/**
 * @brief 鼠标右击函数
 * 清除当前GUI的墙
 *
 * @param bDown 是否按下
 */
bool StageGUI::MouseRButton(bool bDown)
{
    if(bDown)
    {
        walled = false;
        mat[y][x] = 0;
    }

    return !bDown;
}

/**
 * @brief 鼠标左击函数
 * 增加当前GUI的墙
 *
 * @param bDown 是否按下
 */
bool StageGUI::MouseLButton(bool bDown)
{
    if(bDown)
    {
        walled = true;
        mat[y][x] = 4;
    }

    return !bDown;
}

/**
 * @brief 析构函数
 */
StageGUI::~StageGUI(void)
{
    if(hStageBG)
    {
        hge->Texture_Free(hStageBG);
        hStageBG = 0;
        SAFEDEL(StageBG);
        hge->Texture_Free(hWall);
        SAFEDEL(Wall);
        hge->Effect_Free(hEff);
    }
}
