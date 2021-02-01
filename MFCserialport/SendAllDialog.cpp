// SendAllDialog.cpp: 구현 파일
//

#include "pch.h"
#include "MFCserialport.h"
#include "SendAllDialog.h"
#include "afxdialogex.h"
#include<map>


#include "sqlite3.h"
#pragma comment(lib, "sqlite3.lib")

// SendAllDialog 대화 상자

CFont static_font2;
IMPLEMENT_DYNAMIC(SendAllDialog, CDialogEx)

SendAllDialog::SendAllDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG2, pParent)
{

}

SendAllDialog::~SendAllDialog()
{
}

void SendAllDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(SendAllDialog, CDialogEx)
	ON_BN_CLICKED(IDOK, &SendAllDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &SendAllDialog::OnBnClickedButton1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1002, &SendAllDialog::OnDblclkList1002)
	ON_BN_CLICKED(IDC_BUTTON300, &SendAllDialog::OnBnClickedButton300)
	ON_BN_CLICKED(IDC_BUTTON301, &SendAllDialog::OnBnClickedButton301)
	ON_BN_CLICKED(IDC_BUTTON302, &SendAllDialog::OnBnClickedButton302)
	ON_BN_CLICKED(IDC_BUTTON303, &SendAllDialog::OnBnClickedButton303)
	ON_BN_CLICKED(IDC_BUTTON304, &SendAllDialog::OnBnClickedButton304)
	ON_BN_CLICKED(IDC_BUTTON200, &SendAllDialog::OnBnClickedButton200)
	ON_BN_CLICKED(IDC_BUTTON305, &SendAllDialog::OnBnClickedButton305)
END_MESSAGE_MAP()


// SendAllDialog 메시지 처리기

int AnsiToUTF888(char* szSrc, char* strDest, int destSize)
{
	WCHAR 	szUnicode[255];
	char 	szUTF8code[255];

	int nUnicodeSize = MultiByteToWideChar(CP_ACP, 0, szSrc, (int)strlen(szSrc), szUnicode, sizeof(szUnicode));
	int nUTF8codeSize = WideCharToMultiByte(CP_UTF8, 0, szUnicode, nUnicodeSize, szUTF8code, sizeof(szUTF8code), NULL, NULL);
	assert(destSize > nUTF8codeSize);
	memcpy(strDest, szUTF8code, nUTF8codeSize);
	strDest[nUTF8codeSize] = 0;
	return nUTF8codeSize;
}

int UTF8ToAnsi88(char* szSrc, char* strDest, int destSize)
{
	WCHAR 	szUnicode[255];
	char 	szAnsi[255];

	int nSize = MultiByteToWideChar(CP_UTF8, 0, szSrc, -1, 0, 0);
	int nUnicodeSize = MultiByteToWideChar(CP_UTF8, 0, szSrc, -1, szUnicode, nSize);
	int nAnsiSize = WideCharToMultiByte(CP_ACP, 0, szUnicode, nUnicodeSize, szAnsi, sizeof(szAnsi), NULL, NULL);
	assert(destSize > nAnsiSize);
	memcpy(strDest, szAnsi, nAnsiSize);
	strDest[nAnsiSize] = 0;
	return nAnsiSize;
}


BOOL SendAllDialog::OnInitDialog()
{

	CDialog::OnInitDialog();
	SetWindowText("명령 일괄 전송");

	static_font2.CreatePointFont(150, "Arial");
	mList.Attach(GetDlgItem(IDC_LIST1002)->m_hWnd);
	mList.SetFocus();
	mList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	text106.Attach(GetDlgItem(IDC_STATIC106)->m_hWnd);
	button200.Attach(GetDlgItem(IDC_BUTTON200)->m_hWnd);
	edit200.Attach(GetDlgItem(IDC_EDIT200)->m_hWnd);

	button300.Attach(GetDlgItem(IDC_BUTTON300)->m_hWnd);
	button301.Attach(GetDlgItem(IDC_BUTTON301)->m_hWnd);
	button302.Attach(GetDlgItem(IDC_BUTTON302)->m_hWnd);
	button303.Attach(GetDlgItem(IDC_BUTTON303)->m_hWnd);
	button304.Attach(GetDlgItem(IDC_BUTTON304)->m_hWnd);
	button305.Attach(GetDlgItem(IDC_BUTTON305)->m_hWnd);

	button_ok.Attach(GetDlgItem(IDOK)->m_hWnd);
	button_cancel.Attach(GetDlgItem(IDCANCEL)->m_hWnd);

	text106.SetFont(&static_font2);
	mList.SetFont(&static_font2);
	edit200.SetFont(&static_font2);
	button200.SetFont(&static_font2);
	button300.SetFont(&static_font2);
	button301.SetFont(&static_font2);
	button302.SetFont(&static_font2);
	button303.SetFont(&static_font2);
	button304.SetFont(&static_font2);
	button305.SetFont(&static_font2);

	button_ok.SetFont(&static_font2);
	button_cancel.SetFont(&static_font2);



	CRect rect;
	//list 크기 얻어오기 by jang
	mList.GetClientRect(&rect);
	// 리스트 컨트롤에 컬럼 이름 입력 by jang
	CString tmp;
	tmp = "설비이름(위치)";
	int width = rect.Width() / 4;
	mList.InsertColumn(1, tmp, LVCFMT_CENTER, width);
	tmp = "전 화 번 호";
	mList.InsertColumn(2, tmp, LVCFMT_CENTER, width);
	tmp = "명   령   어";
	mList.InsertColumn(3, tmp, LVCFMT_CENTER, rect.Width() - 2 * width);

	for (int i = 0; i < recv.size(); i++) {

		CString name = recv[i].first;
		CString phone = recv[i].second;

		int nItem = mList.InsertItem(0, name);
		mList.SetItemText(nItem, 1, phone);
		mList.SetItemText(nItem, 2, "");

	}

	return TRUE;

}


void SendAllDialog::SendMessageFunction(CString target_number, CString body) {
	CString encode_msg = "";
		
	for (int i = 0; i < body.GetLength(); i++) {
		encode_msg += n_mapping[body.GetAt(i)];
	}

	CString final_send_string = "AT*SMSMO=";
	final_send_string += target_number;
	final_send_string += ",01228388505,";
	final_send_string += encode_msg;
	final_send_string += "\r\n";

	mn_comm->Send(final_send_string, final_send_string.GetLength());



	sqlite3* db;
	sqlite3_stmt* stmt;
	char* errmsg = NULL;


	int rc = sqlite3_open("history.db", &db);

	if (rc != SQLITE_OK)
	{
		printf("Failed to open DB\n");
		sqlite3_close(db);
		exit(1);
	}


	char* sql;
	sql = "CREATE TABLE IF NOT EXISTS DB("
		"ID INTEGER PRIMARY        KEY     AUTOINCREMENT,"
		"NUMBER          TEXT     NOT NULL,"
		"TIME           TEXT     NOT NULL,"
		"CONTENT		TEXT	NOT NULL);";

	rc = sqlite3_exec(db, sql, NULL, NULL, &errmsg);

	if (rc != SQLITE_OK)
	{
		printf("create table");
		sqlite3_free(errmsg);
		sqlite3_close(db);
		exit(1);
	}


	char* s_number;

	CStringW strw(target_number);
	LPCWSTR ptr = strw;

	int sLen = WideCharToMultiByte(CP_ACP, 0, ptr, -1, NULL, 0, NULL, NULL);
	s_number = new char[sLen + 1];
	WideCharToMultiByte(CP_ACP, 0, ptr, -1, s_number, sLen, NULL, NULL);

	char szNumber[100];
	AnsiToUTF888(s_number, szNumber, 100);

	delete[]s_number;








	char* s_time;
	CTime t = CTime::GetCurrentTime();
	CString s = t.Format("%Y년 %m월 %d일 %H:%M:%S");


	CStringW strw1(s);
	ptr = strw1;

	sLen = WideCharToMultiByte(CP_ACP, 0, ptr, -1, NULL, 0, NULL, NULL);
	s_time = new char[sLen + 1];
	WideCharToMultiByte(CP_ACP, 0, ptr, -1, s_time, sLen, NULL, NULL);

	char szTime[100];
	AnsiToUTF888(s_time, szTime, 100);

	delete[]s_time;





	char* s_content;
	CStringW strw2(body);
	ptr = strw2;
	sLen = WideCharToMultiByte(CP_ACP, 0, ptr, -1, NULL, 0, NULL, NULL);
	s_content = new char[sLen + 1];
	WideCharToMultiByte(CP_ACP, 0, ptr, -1, s_content, sLen, NULL, NULL);

	char szContent[100];
	AnsiToUTF888(s_content, szContent, 100);

	delete[]s_content;




	char sqll[255] = { 0 };
	sprintf(sqll, "insert into db(NUMBER, TIME,CONTENT) values('%s','%s','%s');", szNumber, szTime, szContent);

	if (SQLITE_OK != sqlite3_exec(db, sqll, NULL, NULL, &errmsg))
	{
		printf("insert");
	}

	sqlite3_close(db);


}

//보내기
void SendAllDialog::OnBnClickedOk()
{
	int Answer = AfxMessageBox("명령을 문자로 보내시겠습니까?", MB_OKCANCEL);
	if (Answer == IDOK) {
		for (int i = 0; i < mList.GetItemCount(); i++) {
			CString number = mList.GetItemText(i, 1);
			CString body = mList.GetItemText(i, 2);
			if (body == "") { continue; }
			SendMessageFunction(number, body);
			Wait(500);
		}
	}
	else {

	}
}

void SendAllDialog::Wait(DWORD dwMillisecond)
{
	MSG msg;
	DWORD dwStart;
	dwStart = GetTickCount();

	while (GetTickCount() - dwStart < dwMillisecond)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}


void SendAllDialog::OnBnClickedButton1()
{

}


void SendAllDialog::OnDblclkList1002(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (pNMItemActivate->iItem != -1) {
		idx = pNMItemActivate->iItem;
		CString strSelectdName = mList.GetItemText(idx, 0);
		CString strSelectedPhone = mList.GetItemText(idx, 1);
		strSelectdName += "에 전송할 명령을 입력하세요.";

		text106.SetWindowTextA(strSelectdName);
/*
		sendName_EditCtrl.SetWindowTextA(strSelectdName);

		m_edit_revmsg.SetWindowTextA("");

		for (int i = 254; i >= 0; i--) {
			if (total_message_in_modem[i].getNumber().Compare(strSelectedPhone) == 0) {
				CString display_string = "";
				display_string += total_message_in_modem[i].getCotent();
				display_string += "            ";
				display_string += total_message_in_modem[i].getTime();
				display_string += "\r\n";

				m_edit_revmsg.ReplaceSel(display_string);
				m_edit_revmsg.LineScroll(m_edit_revmsg.GetLineCount());
			}
		}
		*/
	}


	*pResult = 0;
}


void SendAllDialog::OnBnClickedButton300()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	edit200.SetWindowTextA("PUMP1");
}


void SendAllDialog::OnBnClickedButton301()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	edit200.SetWindowTextA("PUMP2");
}


void SendAllDialog::OnBnClickedButton302()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	edit200.SetWindowTextA("PUMP3");

}


void SendAllDialog::OnBnClickedButton303()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	edit200.SetWindowTextA("0");

}


void SendAllDialog::OnBnClickedButton304()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	edit200.SetWindowTextA("7");

}


void SendAllDialog::OnBnClickedButton200()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (idx == -1)return;
	CString str;
	edit200.GetWindowTextA(str);
	mList.SetItemText(idx, 2, str);
	idx = -1;
}


void SendAllDialog::OnBnClickedButton305()
{
	edit200.SetWindowTextA("PUMP4");
}
