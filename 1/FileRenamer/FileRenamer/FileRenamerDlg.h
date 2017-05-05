
// FileRenamerDlg.h : 헤더 파일
//

#pragma once


#if defined _M_AMD64
#pragma comment(lib,"C:/Script/Lua/5.3.2/Library/x64/lua53.lib")
#elif defined _M_IX86
#pragma comment(lib,"C:/Script/Lua/5.3.2/Library/x86/lua53.lib")
#endif

#include "C:/Script/Lua/5.3.2/Library/include/lua.hpp"
#include "C:/Script/Lua/5.3.2/Library/include/luaconf.h"



#define MAX_PLUGINS		10
typedef struct {
	char szPath[MAX_PATH];
	char szName[MAX_PATH];
	char szDesc[MAX_PATH];
	char szVersion[MAX_PATH];
	char szDate[MAX_PATH];
	char szAuthor[MAX_PATH];
	char szEmail[MAX_PATH];
} PLUGIN, *PPLUGIN;


#define MAX_FILES		256
typedef struct {
	char szSrcFile[MAX_PATH];
	char szDstFile[MAX_PATH];
} FILE_RENAMER, * PFILE_RENAMER;




// CFileRenamerDlg 대화 상자
class CFileRenamerDlg : public CDialogEx
{
// 생성입니다.
public:
	CFileRenamerDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILERENAMER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedRenameButton();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

public:
	afx_msg void OnCbnSelchangePluginCombo();
	
private:
	UINT			m_nNumberOfPlugIns;
	PLUGIN			m_PlugIns[MAX_PLUGINS];

	UINT			m_nNumberOfFiles;
	FILE_RENAMER	m_FileList[MAX_FILES];

	void LoadPlugInList();
	BOOL GetPlugInInfo(LPCSTR szPlugIn, PPLUGIN pPlugIn);
};
