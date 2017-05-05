
// FileRenamerDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "FileRenamer.h"
#include "FileRenamerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


static int MyDbgString(lua_State *pLuaState)
{
	/* get number of arguments */
	int nTop{ 0 };
	const char *szDbgString{ NULL };
	wchar_t		wszDbgString[MAX_PATH]{ 0, };

	__try {
		nTop = lua_gettop(pLuaState);
		if (1 != nTop) __leave;
		if (!lua_isstring(pLuaState, nTop)) __leave;

		szDbgString = lua_tostring(pLuaState, nTop);

		int nLen = MultiByteToWideChar(CP_UTF8, 0, szDbgString, strlen(szDbgString), NULL, NULL);
		MultiByteToWideChar(CP_UTF8, 0, szDbgString, strlen(szDbgString), wszDbgString, nLen);

		OutputDebugStringW(wszDbgString);
	}
	__finally {

	}

	return 0;
}

BOOL UTF8toANSI(const char *szUTF8, char *szANSI)
{
	BOOL	bSuccess{ FALSE };
	wchar_t	wszString[MAX_PATH]{ 0, };
	int		nLen{ 0 };

	__try {
		nLen = MultiByteToWideChar(CP_UTF8, 0, szUTF8, strlen(szUTF8), NULL, NULL);
		MultiByteToWideChar(CP_UTF8, 0, szUTF8, strlen(szUTF8), wszString, nLen);
		nLen = WideCharToMultiByte(CP_ACP, 0, wszString, -1, NULL, 0, NULL, NULL);
		WideCharToMultiByte(CP_ACP, 0, wszString, -1, szANSI, nLen, NULL, NULL);

		bSuccess = TRUE;
	}
	__finally {
		int nErr = GetLastError();
	}

	return bSuccess;
}

// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFileRenamerDlg ��ȭ ����



CFileRenamerDlg::CFileRenamerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_FILERENAMER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFileRenamerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFileRenamerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_RENAME_BUTTON, &CFileRenamerDlg::OnBnClickedRenameButton)
	ON_BN_CLICKED(IDOK, &CFileRenamerDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CFileRenamerDlg::OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_PLUGIN_COMBO, &CFileRenamerDlg::OnCbnSelchangePluginCombo)
END_MESSAGE_MAP()


// CFileRenamerDlg �޽��� ó����

BOOL CFileRenamerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	::DragAcceptFiles(m_hWnd, true);


	// ChakraCore.DLL�� ���� �ε� DLL�� ����ϰ�, ���⼭ DLL �˻� ��θ� �߰��Ѵ�. 
//	SetDllDirectory("C:/Script/Lua");


	((CListCtrl *)GetDlgItem(IDC_FROM_LIST)) -> InsertColumn(0, "��ȣ", LVCFMT_LEFT, 40, -1);
	((CListCtrl *)GetDlgItem(IDC_FROM_LIST)) -> InsertColumn(1, "���� �̸�", LVCFMT_LEFT, 200, -1);

	((CListCtrl *)GetDlgItem(IDC_TO_LIST))->InsertColumn(0, "��ȣ", LVCFMT_LEFT, 40, -1);
	((CListCtrl *)GetDlgItem(IDC_TO_LIST))->InsertColumn(1, "���� �̸�", LVCFMT_LEFT, 200, -1);

	LoadPlugInList();

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CFileRenamerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CFileRenamerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CFileRenamerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CFileRenamerDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	CDialogEx::OnDropFiles(hDropInfo);

	int nItem{ 0 };
	int nIndex{ 0 };
	int nFiles{ 0 };
	char  szSrcFile[MAX_PATH]{ 0, };
	char szFname[MAX_PATH]{ 0, };
	char	szExt[MAX_PATH]{ 0, };

	char szIndex[MAX_PATH]{ 0 };


	nItem = ((CListCtrl *)GetDlgItem(IDC_FROM_LIST))->GetItemCount();
	nFiles = DragQueryFile(hDropInfo, -1, 0, 0);

	for (nIndex = 0; nIndex < nFiles; nIndex++)
	{
		::DragQueryFile(hDropInfo, nIndex, szSrcFile, sizeof(szSrcFile));

		strcpy_s(m_FileList[nItem].szSrcFile, MAX_PATH, szSrcFile);

		_splitpath(szSrcFile, NULL, NULL, szFname, szExt);
		sprintf_s(szSrcFile, MAX_PATH, "%s%s", szFname, szExt);

		sprintf_s(szIndex, MAX_PATH, "%d", nItem + 1);
		((CListCtrl *)GetDlgItem(IDC_FROM_LIST))->InsertItem(nItem, szIndex, 0);
		((CListCtrl *)GetDlgItem(IDC_FROM_LIST))->SetItem(nItem, 1, LVIF_TEXT, szSrcFile, 0, 0, 0, 0);

		nItem++;
	}

	m_nNumberOfFiles = nItem;
}




void CFileRenamerDlg::OnBnClickedRenameButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	BOOL bSuccess{ FALSE };


	char			szDbgString[MAX_PATH]{ 0, };
	lua_State *		pLuaState{ NULL };
	const char *	szString{ NULL };

	int				nIndex{ 0 };
	int				nItem{ 0 };
	char			szIndex[MAX_PATH]{ 0, };
	char			szFname[MAX_PATH]{ 0, };
	char			szExt[MAX_PATH]{ 0, };
	char			szDstFile[MAX_PATH]{ 0, };

	__try {
		int nPlugIn = ((CComboBox *)GetDlgItem(IDC_PLUGIN_COMBO))->GetCurSel();
		((CListCtrl *)GetDlgItem(IDC_TO_LIST))->DeleteAllItems();

		if (-1 == nPlugIn) __leave;


		pLuaState = luaL_newstate();
		if (NULL == pLuaState) __leave;

		luaL_openlibs(pLuaState);

		/* register our function */
		lua_register(pLuaState, "DbgString", MyDbgString);

		if (luaL_dofile(pLuaState, m_PlugIns[nPlugIn].szPath)) {
			const char *pError = lua_tostring(pLuaState, -1);
			sprintf_s(szDbgString, "error running function `f': %s\n", pError);
			OutputDebugStringA(szDbgString);
			__leave;
		}

		for (nIndex = 0; nIndex < m_nNumberOfFiles; nIndex++)
		{
			lua_getglobal(pLuaState, "Rename");
			lua_pushstring(pLuaState, m_FileList[nIndex].szSrcFile);

			if (lua_pcall(pLuaState, 1, 1, 0) != 0) {
				sprintf_s(szDbgString, "error running function `f': %s\n", lua_tostring(pLuaState, -1));
				OutputDebugStringA(szDbgString);
				__leave;
			}

			szString = lua_tostring(pLuaState, 1);
			UTF8toANSI(szString, m_FileList[nIndex].szDstFile);

			sprintf_s(szIndex, MAX_PATH, "%d", nItem + 1);
			((CListCtrl *)GetDlgItem(IDC_TO_LIST))->InsertItem(nItem, szIndex, 0);

			_splitpath(m_FileList[nIndex].szDstFile, NULL, NULL, szFname, szExt);
			sprintf_s(szDstFile, MAX_PATH, "%s%s", szFname, szExt);

			((CListCtrl *)GetDlgItem(IDC_TO_LIST))->SetItem(nItem, 1, LVIF_TEXT, szDstFile, 0, 0, 0, 0);

			nItem++;

			int stackSize = lua_gettop(pLuaState);
			lua_pop(pLuaState, stackSize);
		}
		bSuccess = TRUE;
	}
	__finally {
		if (NULL != pLuaState) lua_close(pLuaState);
	}


}


void CFileRenamerDlg::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	OutputDebugString("\n===========================================\n");
	for (int nIndex = 0; nIndex < m_nNumberOfFiles; nIndex++)
	{
		OutputDebugString(m_FileList[nIndex].szSrcFile);
		OutputDebugString("\n");
		OutputDebugString(m_FileList[nIndex].szDstFile);
		OutputDebugString("\n===========================================\n");
		rename(m_FileList[nIndex].szSrcFile, m_FileList[nIndex].szDstFile);
	}

	//CDialogEx::OnOK();
}


void CFileRenamerDlg::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CDialogEx::OnCancel();
}


void CFileRenamerDlg::OnCbnSelchangePluginCombo()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

}


// PlugIn ����� ��� �´�...
void CFileRenamerDlg::LoadPlugInList()
{
	// 1. PlugIn ������ �ִ� Ư�� Ȯ����(js)�� ���� ���ϵ��� ����� ���Ѵ�.
	CFileFind	PlugInFinder;
	BOOL		bFound{ FALSE };
	BOOL		bSuccess{ FALSE };
	PLUGIN		PlugIn{ 0, };
	UINT		nIndex{ 0 };

	bFound = PlugInFinder.FindFile("./PlugIn/*.lua");

	while (bFound)
	{
		bFound = PlugInFinder.FindNextFile();

		// 2. ���� ���ϵ鿡�� Ư�� �Լ� (GetInfo)�� ȣ���Ѵ�. 
		// �̸�, ����, ����, ��¥, ������
		ZeroMemory(&PlugIn, sizeof(PlugIn));
		bSuccess = GetPlugInInfo((LPCTSTR)PlugInFinder.GetFilePath(), &PlugIn);

		
		if (TRUE == bSuccess)
		{
			// �� �Լ��� ������ ���ڿ��� �޺� �ڽ��� �߰��Ѵ�.....
			((CComboBox *)GetDlgItem(IDC_PLUGIN_COMBO))->AddString(PlugIn.szName);


			// 3. PlugIn ����Ʈ�� �����Ѵ�.
			memcpy(&(m_PlugIns[nIndex]), &PlugIn, sizeof(PLUGIN));


			nIndex++;
		}
		

		if (nIndex >= MAX_PLUGINS) break;
	}



}



BOOL CFileRenamerDlg::GetPlugInInfo(LPCSTR szPlugIn, PPLUGIN pPlugIn)
{
	BOOL bSuccess{ FALSE };

	char			szDbgString[MAX_PATH]{ 0, };
	lua_State *		pLuaState{ NULL };
	const char *	szString{ NULL };

	__try {
		pLuaState = luaL_newstate();
		if (NULL == pLuaState) __leave;

		if (luaL_dofile(pLuaState, szPlugIn)) {
			const char *pError = lua_tostring(pLuaState, -1);
			sprintf_s(szDbgString, "error running function `f': %s\n", pError);
			OutputDebugStringA(szDbgString);
			__leave;
		}

		lua_getglobal(pLuaState, "GetInfo");

		if (lua_pcall(pLuaState, 0, 6, 0) != 0) {
			sprintf_s(szDbgString, "error running function `f': %s\n", lua_tostring(pLuaState, -1));
			OutputDebugStringA(szDbgString);
			__leave;
		}

		szString = lua_tostring(pLuaState, 1);
		UTF8toANSI(szString, pPlugIn->szName);


		szString = lua_tostring(pLuaState, 2);
		UTF8toANSI(szString, pPlugIn->szDesc);

		szString = lua_tostring(pLuaState, 3);
		UTF8toANSI(szString, pPlugIn->szVersion);


		szString = lua_tostring(pLuaState, 4);
		UTF8toANSI(szString, pPlugIn->szDate);


		szString = lua_tostring(pLuaState, 5);
		UTF8toANSI(szString, pPlugIn->szAuthor);


		szString = lua_tostring(pLuaState, 6);
		UTF8toANSI(szString, pPlugIn->szEmail);


		strcpy_s(pPlugIn->szPath, MAX_PATH, szPlugIn);

		bSuccess = TRUE;
	}
	__finally {
		if (NULL != pLuaState) lua_close(pLuaState);
	}

	return bSuccess;
}