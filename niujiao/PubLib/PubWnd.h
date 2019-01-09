#pragma once
#include "stdafx.h"
#include "UIlib.h"
#include "DbgEngine/ImageInfo.h"
#include "StrTrie.h"

using namespace DuiLib;

class CPubWnd : public WindowImplBase
{
protected:
	LPCTSTR GetWindowClassName() const override { return m_className; };
	CDuiString GetSkinFile() override { return m_strXMLPath; };
private:
	CDuiString m_strXMLPath;
	TCHAR* m_className;
	CStrTrie* m_strTrie;

public:
	explicit CPubWnd(LPCTSTR pszXMLPath);
	~CPubWnd();
	void Notify(TNotifyUI& msg) override;
	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled) override;
	LRESULT OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled) override;

	void ReadPeAttribute(LPCTSTR FileName) const;
	void FillGeneral(CImageInfo* tmp) const;
	void FillDataDir(CImageInfo* tmp) const;
	void FillSeg(CImageInfo* tmp) const;
	void FillImport(CImageInfo* tmp) const;
	void FillExport(CImageInfo* tmp) const;

	HWND hWnd;


	static void HandleCommandEvt(WindowImplBase * pBase,TNotifyUI& msg, int Value);
	static void HandleNewWndEvt(WindowImplBase * pBase, TNotifyUI& msg, int Value);
	static void HandleFunction(WindowImplBase * pBase, TNotifyUI& msg, int Value);
	static void HandleCaptionMsg(WindowImplBase * pBase, TNotifyUI& msg, int Value);
	static void HandleSubWndClose(WindowImplBase * pBase, TNotifyUI& msg, int Value);
	static void HandleBrowserFile(WindowImplBase * pBase, TNotifyUI& msg, int Value);
	static void HandlePeBrowser(WindowImplBase * pBase, TNotifyUI& msg, int Value);
	static void HandlePeSwitchImportDll(WindowImplBase * pBase, TNotifyUI& msg, int Value);
	
};

typedef struct s_wnd_list
{
	CPubWnd* WndClass;
	struct s_wnd_list* Next;
}SWndList;
void HandleNotify(WindowImplBase* pBase, TNotifyUI& msg);
TCHAR* GenerateClassName();
void UpdateWindowBackground();

typedef int(*SEventHandle)(WindowImplBase * pBase, TNotifyUI& msg,int Value);
/**
 * \brief �¼��ַ���ǰ׺������
 */
#define MAX_STRING_LENGTH 16
typedef struct s_eventString
{
	char m_Str[MAX_STRING_LENGTH];
	int Value;
	void (*EventHandle)(WindowImplBase * pBase, TNotifyUI& msg, int Value); //�¼�������
}SEventString;
static SEventString gEventString[] =
{
	{"command"     ,0          ,CPubWnd::HandleCommandEvt},  //ִ���ⲿ����
	{"sub_wnd"     ,0          ,CPubWnd::HandleNewWndEvt},  //���Ӵ���
	{"function"    ,0          ,CPubWnd::HandleFunction}, //ִ���ڽ�ָ��
	{"main_close"  ,SC_CLOSE   ,CPubWnd::HandleCaptionMsg}, //�����˳�
	{"main_max"    ,SC_MAXIMIZE,CPubWnd::HandleCaptionMsg}, //���������
	{"main_restore",SC_RESTORE ,CPubWnd::HandleCaptionMsg}, //��ԭ�����ڴ�С
	{"main_min"    ,SC_MINIMIZE,CPubWnd::HandleCaptionMsg}, //��С��������
	{"close_btn"    ,0,CPubWnd::HandleSubWndClose}, //�Ӵ�������
	{"browser_btn"    ,0,CPubWnd::HandleBrowserFile}, //����ǰ���ļ� �����д��browser_edit �ؼ���
	{"browser_pe"    ,0,CPubWnd::HandlePeBrowser}, //PE���� ���ļ���ť
	{"pe_impdll"    ,0,CPubWnd::HandlePeSwitchImportDll}, //PE���� �л���ʾ dll���뺯��
};
