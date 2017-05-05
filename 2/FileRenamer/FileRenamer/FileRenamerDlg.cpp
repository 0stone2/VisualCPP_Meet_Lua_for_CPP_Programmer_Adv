
// FileRenamerDlg.cpp : 구현 파일
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

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CFileRenamerDlg 대화 상자



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


// CFileRenamerDlg 메시지 처리기

BOOL CFileRenamerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	::DragAcceptFiles(m_hWnd, true);


	// ChakraCore.DLL을 지연 로드 DLL로 등록하고, 여기서 DLL 검색 경로를 추가한다. 
	SetDllDirectory("C:/Script/Lua\5.3.2");


	((CListCtrl *)GetDlgItem(IDC_FROM_LIST)) -> InsertColumn(0, "번호", LVCFMT_LEFT, 40, -1);
	((CListCtrl *)GetDlgItem(IDC_FROM_LIST)) -> InsertColumn(1, "파일 이름", LVCFMT_LEFT, 200, -1);

	((CListCtrl *)GetDlgItem(IDC_TO_LIST))->InsertColumn(0, "번호", LVCFMT_LEFT, 40, -1);
	((CListCtrl *)GetDlgItem(IDC_TO_LIST))->InsertColumn(1, "파일 이름", LVCFMT_LEFT, 200, -1);

	LoadPlugInList();

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CFileRenamerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CFileRenamerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void DumpTable(lua_State *pLuaState)
{
	char szStack[MAX_PATH]{ 0, };

	lua_pushnil(pLuaState); /* first key */
	while (lua_next(pLuaState, -2) != 0)
	{
		if (lua_isstring(pLuaState, -1)) {
			sprintf(szStack, "%s = %s\n", lua_tostring(pLuaState, -2), lua_tostring(pLuaState, -1));
			OutputDebugString(szStack);
		}
		else if (lua_isnumber(pLuaState, -1)) {
			sprintf(szStack, "%s = %d\n", lua_tostring(pLuaState, -2), lua_tonumber(pLuaState, -1));
		}
		else if (lua_istable(pLuaState, -1)) {
			DumpTable(pLuaState);
		}

		lua_pop(pLuaState, 1);
	}
}

void StackDump(lua_State *pLuaState) {
	char szStack[MAX_PATH]{ 0, };
	int nStack{ 0 };

	OutputDebugString(" ----------------  Stack Dump ----------------\n");

	nStack = lua_gettop(pLuaState);
	while (nStack) {
		int nType = lua_type(pLuaState, nStack);

		switch (nType) {
			case LUA_TSTRING:
				sprintf(szStack, "%d:`%s'\n", nStack, lua_tostring(pLuaState, nStack));
				break;
			case LUA_TBOOLEAN:
				sprintf(szStack, "%d: %s\n", nStack, lua_toboolean(pLuaState, nStack) ? "true" : "false");
				break;
			case LUA_TNUMBER:
				sprintf(szStack, "%d: %g\n", nStack, lua_tonumber(pLuaState, nStack));
				break;
			case LUA_TTABLE:
				sprintf(szStack, "%d: LUA_TTABLE\n", nStack);
				//nType = lua_getfield(pLuaState, nStack, "files");
				//nStack = lua_gettop(pLuaState);
				break;
			default: 
				sprintf(szStack, "%d: %s\n", nStack, lua_typename(pLuaState, nType));
				break;
		}
		OutputDebugString(szStack);
		nStack--;
	}
	OutputDebugString("--------------- Stack Dump Finished ---------------\n");
}

void CFileRenamerDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CDialogEx::OnDropFiles(hDropInfo);

	int nItem{ 0 };
	int nIndex{ 0 };
	int nFiles{ 0 };
	char	szSrcFile[MAX_PATH]{ 0, };
	char	szFname[MAX_PATH]{ 0, };
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


int l_FileList_create(lua_State * pLuaState)
{
	OutputDebugString("Enter l_FileList_create\n");

	lua_newtable(pLuaState);
	lua_pushstring(pLuaState, "files");

	lua_newtable(pLuaState);
	lua_settable(pLuaState, -3);

	luaL_getmetatable(pLuaState, "FileList");

	lua_setmetatable(pLuaState, -2);
	
	return 1;
}

int l_FileList_addFile(lua_State * pLuaState)
{
	int		nStack = 0;
	char	szDbgString[MAX_PATH]{ 0, };
	int     nType;
	const char *	pType = NULL;
	lua_Number number;
	bool	UIUpdate = false;
	char 	pPath[MAX_PATH]{ 0, };

	OutputDebugString("Enter l_FileList_addFile\n");
	//StackDump(pLuaState);

	__try {
		nStack = lua_gettop(pLuaState);
		UIUpdate = lua_toboolean(pLuaState, nStack);
		lua_pop(pLuaState, 1);

		// Path 처리 
		nStack = lua_gettop(pLuaState);
		strcpy(pPath, lua_tostring(pLuaState, nStack));
		lua_pop(pLuaState, 1);

		// files 가져오기
		nStack = lua_gettop(pLuaState);
		lua_getfield(pLuaState, nStack, "files");

		nStack = lua_gettop(pLuaState);
		int nSize = luaL_len(pLuaState, nStack);  // get size of table

		nStack = lua_gettop(pLuaState);

		// files에 Path 추가
		lua_pushstring(pLuaState, pPath);
		lua_seti(pLuaState, nStack, nSize + 1);
		lua_settable(pLuaState, -2);

		lua_pop(pLuaState, 1);

		nStack = lua_gettop(pLuaState);
		lua_pop(pLuaState, 1);

		if (true == UIUpdate) {
			CFileRenamerDlg *dlg = (CFileRenamerDlg *)AfxGetMainWnd();
			dlg->addFileToRightPane(pPath);
		}
	}
	__finally {

	}

	return 0;
}

void RegisterFileList(lua_State * pLuaState)
{
	luaL_Reg APIs[] =
	{
		{ "create",		l_FileList_create },
		{ "addFile",	l_FileList_addFile },
		{ NULL, NULL }
	};

	OutputDebugString("Enter RegisterFileList\n");

	luaL_newmetatable(pLuaState, "FileList");
	luaL_setfuncs(pLuaState, APIs, 0);

	lua_pushvalue(pLuaState, -1);

	lua_setfield(pLuaState, -1, "__index");

	lua_setglobal(pLuaState, "FileList");
}


void createFileList(lua_State *pLuaState, char *szObj)
{
	int		nStack = 0;
	char	szDbgString[MAX_PATH]{ 0, };

	OutputDebugString("Enter createFileList\n");

	__try {
		lua_getglobal(pLuaState, "FileList");
		nStack = lua_gettop(pLuaState);
		lua_getfield(pLuaState, -1, "create");

		if (lua_pcall(pLuaState, 0, 1, 0) != 0) {
			sprintf_s(szDbgString, "error running function `f': %s\n", lua_tostring(pLuaState, -1));
			OutputDebugStringA(szDbgString);
			__leave;
		}
		nStack = lua_gettop(pLuaState);
		lua_setglobal(pLuaState, szObj);
		nStack = lua_gettop(pLuaState);
		if (1 == nStack) lua_pop(pLuaState, 1);
		nStack = lua_gettop(pLuaState);
	}
	__finally {

	}
}

void CFileRenamerDlg::OnBnClickedRenameButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

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

		lua_register(pLuaState, "DbgString", MyDbgString);

		////////////////////////////////////
		int nStack = lua_gettop(pLuaState);
		RegisterFileList(pLuaState);
		nStack = lua_gettop(pLuaState);

		createFileList(pLuaState, "leftPane");
		createFileList(pLuaState, "rightPane");

		////////////////////////////////////

		if (luaL_dofile(pLuaState, m_PlugIns[nPlugIn].szPath)) {
			const char *pError = lua_tostring(pLuaState, -1);
			sprintf_s(szDbgString, "error running function `f': %s\n", pError);
			OutputDebugStringA(szDbgString);
			__leave;
		}

		for (nIndex = 0; nIndex < m_nNumberOfFiles; nIndex++)
		{
			nStack = lua_gettop(pLuaState);
			lua_getglobal(pLuaState, "leftPane");

			nStack = lua_gettop(pLuaState);
			lua_getfield(pLuaState, -1, "addFile");

			nStack = lua_gettop(pLuaState);

			lua_pushvalue(pLuaState, 1); //for self
			lua_pushstring(pLuaState, m_FileList[nIndex].szSrcFile);
			lua_pushboolean(pLuaState, false);
			nStack = lua_gettop(pLuaState);

			if (lua_pcall(pLuaState, 3, 0, 0) != 0) {
				sprintf_s(szDbgString, "error running function `f': %s\n", lua_tostring(pLuaState, -1));
				OutputDebugStringA(szDbgString);
				__leave;
			}
		}

		lua_getglobal(pLuaState, "RenameAll");

		if (lua_pcall(pLuaState, 0, 0, 0) != 0) {
			sprintf_s(szDbgString, "error running function `f': %s\n", lua_tostring(pLuaState, -1));
			OutputDebugStringA(szDbgString);
			__leave;
		}

		bSuccess = TRUE;
	}
	__finally {
		if (NULL != pLuaState) lua_close(pLuaState);
	}
}


void CFileRenamerDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}


void CFileRenamerDlg::OnCbnSelchangePluginCombo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

}


// PlugIn 목록을 얻어 온다...
void CFileRenamerDlg::LoadPlugInList()
{
	// 1. PlugIn 폴더에 있는 특정 확장자(js)를 가진 파일들을 목록을 구한다.
	CFileFind	PlugInFinder;
	BOOL		bFound{ FALSE };
	BOOL		bSuccess{ FALSE };
	PLUGIN		PlugIn{ 0, };
	UINT		nIndex{ 0 };

	bFound = PlugInFinder.FindFile("./PlugIn/*.lua");

	while (bFound)
	{
		bFound = PlugInFinder.FindNextFile();

		// 2. 얻어온 파일들에서 특정 함수 (GetInfo)를 호출한다. 
		// 이름, 설명, 버전, 날짜, 만든이
		ZeroMemory(&PlugIn, sizeof(PlugIn));
		bSuccess = GetPlugInInfo((LPCTSTR)PlugInFinder.GetFilePath(), &PlugIn);

		
		if (TRUE == bSuccess)
		{
			// 이 함수가 리턴한 문자열을 콤보 박스에 추가한다.....
			((CComboBox *)GetDlgItem(IDC_PLUGIN_COMBO))->AddString(PlugIn.szName);


			// 3. PlugIn 리스트에 보관한다.
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

		luaL_openlibs(pLuaState);

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

void CFileRenamerDlg::addFileToRightPane(LPCSTR pszFile)
{
	int nItem = 0;
	char szIndex[MAX_PATH];

	char szFname[MAX_PATH]{ 0, };
	char szExt[MAX_PATH]{ 0, };
	char szDstFile[MAX_PATH]{ 0, };

	nItem = ((CListCtrl *)GetDlgItem(IDC_TO_LIST))->GetItemCount();
	strcpy_s(m_FileList[nItem].szDstFile, MAX_PATH, pszFile);
	/////////////////////////////////////////////////////////////////////////////

	sprintf_s(szIndex, MAX_PATH, "%d", nItem + 1);
	((CListCtrl *)GetDlgItem(IDC_TO_LIST))->InsertItem(nItem, szIndex, 0);

	_splitpath(pszFile, NULL, NULL, szFname, szExt);
	sprintf_s(szDstFile, MAX_PATH, "%s%s", szFname, szExt);

	((CListCtrl *)GetDlgItem(IDC_TO_LIST))->SetItem(nItem, 1, LVIF_TEXT, szDstFile, 0, 0, 0, 0);
}
