//////////////////////////////////////////////////////////////////////////
//
// �༭�������
//
// ���ߣ�΢���ƽ��(BOGY)
//
// �汾��For HGE v1.7
//
//////////////////////////////////////////////////////////////////////////
#ifndef GDIEDIT_H
#define GDIEDIT_H

#include <hgerect.h>
#include	"GfxFont.h"
#pragma   warning(disable:4312)
#pragma   warning(disable:4311)

class GfxEdit
{
public:

	///
	/// nEditWidth		�༭����
	///	nFontColor		������ɫ
	/// lpFontName		��������
	///	nFontSize		�����С
	/// bBold			�Ƿ����
	/// bItalic			�Ƿ�б��
	/// bAntialias		�Ƿ�ƽ��
	///
	GfxEdit(UINT nEditWidth, DWORD nFontColor, CHAR* lpFontName = "����", UINT nFontSize = 12, BOOL bBold = FALSE, BOOL bItalic = FALSE, BOOL bAntialias = TRUE);
	~GfxEdit(void);

	typedef enum tagDeletePos
	{
		DP_LEFT = 0,
		DP_RIGHT
	}TDP;

public:
	void			SetFocus(); // ���ñ༭�򽹵㣬ֻ��ӵ�б༭�򽹵������Ӧ����
	void			KillFocus(); // ����༭�򽹵㣬ʧȥ�༭�򽹵�󽫲�����Ӧ����

	void			ClearCookie(); // ����༭������

	void			InsertCookie(const char* lpCookie); // �����ַ������༭��ǰ��괦
	const char*		GetCookie(); // ��ȡ�༭��ǰ����

	void			InsertCookieW(const wchar_t* lpCookie); // �����ַ������༭��ǰ��괦(���ַ�ģʽ)
	const wchar_t*	GetCookieW(); // ��ȡ�༭��ǰ����(���ַ�ģʽ)

	void			Updata(float dt);
	void			Render(float x, float y); //
	void			ShowOuterFrame(bool	show)		{IsShowOuterFrame=show;}
	bool			GetOuterFrame()	const			{return IsShowOuterFrame;}
	void			SetNumOnly(bool	NumOnly)		{IsNumOnly=NumOnly;}
	bool			DoKeyDown();
	BOOL			CopyStringToClipboard(wchar_t* WText);
	BOOL			SetStrToClipboard();
	BOOL			GetStrFromClipboard();
	BOOL			SetCuttingClip();

private:
	void			OnChar( UINT nChar, UINT nRepCnt, UINT nFlags );
	void			OnCharHZ( UINT nChar, UINT nRepCnt, UINT nFlags );
	void			InsertChar(wchar_t aChar);
	void			DeleteChar(TDP aPos);
	void			DeleteMoreChar();
	void			MouseLButton();
	void			MouseLMove();
	void			SelectAllChar();

private:
	static const int COOKIE_SIZE = 1024;
	wchar_t			m_sCookie[COOKIE_SIZE];
	char			m_sBuffer[COOKIE_SIZE];

	FLOAT			m_nEditW;
	FLOAT			m_nEditH;

	UINT			m_nCharPos;		// �ַ�λ��

	hgeRect			m_rcWorld;
    
	HGE*			m_pHGE;			// HGEָ��
	hgeSprite*		m_pSprite;		// ��꾫��
	hgeSprite*		bgSprite;		// ���ѡ�־���
	GfxFont*		m_pFont;		// ���徫��

	bool			IsShowOuterFrame;	//��߿�
	bool			IsShowCursor;		//���
	bool			IsNumOnly;			//������
	bool			IsCuttingClipOn;	//���а�

	bool			MouseLDown;
	UINT			MouseL1CharPos;
	float			MouseL1x,MouseL1y;
	UINT			MouseL2CharPos;
	float			MouseL2x,MouseL2y;
	float			LastDtTime;

	float			box_x;
	float			box_y;
	float			box_w;
	float			box_h;

	static LRESULT CALLBACK GfxEditWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
};
#endif//GDIEDIT_H
