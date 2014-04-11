#include "StageGUI.h"
#include "string.h"

bool saved = true;                              ///< ��ǰ��ͼ�Ƿ񱣴�
HTEXTURE hStageBG = 0;                          ///< ��̨����������
HTEXTURE hWall = 0;                             ///< ǽ������
hgeSprite *StageBG = NULL;                      ///< ��̨��������
hgeSprite *Wall = NULL;                         ///< ǽ����
HEFFECT hEff = 0;                               ///< GUI��Ч
const float MAXTOYCUR_TIMER = 0.1f;             ///< ���̿�����ʱʱ��

int mat[15][20];                                ///< ��ͼ����
bool vis[15][20];                               ///< ����_floodfill����

/**
 * @brief ��亯��
 * ��ĳ����������ָ���ı��
 *
 * @param x ��ǰ��ĺ�����
 * @param y ��ǰ���������
 * @param flag ��ҪѰ�ҵı��
 * @param gui ��Ӧ��GUIϵͳ
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
 * @brief ���캯��
 * 
 * @param _id GUIԪ�صı��
 * @param x GUI�ĺ�����
 * @param y GUI��������
 * @param par �����ڵ�GUIϵͳ
 * @param walled �Ƿ���Ͱ���
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
 * @brief ��Ⱦ����
 * ��Ⱦ��GUIԪ��
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
 * @brief ���º���
 * ���´�GUIԪ��
 *
 * @param dt ����һ֡��ʱ��������λ����
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
 * @brief �����������
 * ���õ�ǰGUIԪ��Ϊ�˶���
 *
 * @param bOver �Ƿ�����
 */
void StageGUI::MouseOver(bool bOver)
{
    if(bOver)
    {
        _SetCurrent();
    }
}

/**
 * @brief (*)���õ�ǰGUI����
 * ��������GUIϵͳ�ĵ�ǰGUI
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
 * @brief ����һ�����
 * �����ǰGUI��ǽ
 *
 * @param bDown �Ƿ���
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
 * @brief ����������
 * ���ӵ�ǰGUI��ǽ
 *
 * @param bDown �Ƿ���
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
 * @brief ��������
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
