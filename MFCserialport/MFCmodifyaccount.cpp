// MFCmodifyaccount.cpp: 구현 파일
//

#include "pch.h"
#include "MFCserialport.h"
#include "MFCmodifyaccount.h"
#include "afxdialogex.h"
#include <assert.h>

#include "sqlite3.h"
#pragma comment(lib, "sqlite3.lib")

// MFCmodifyaccount 대화 상자

IMPLEMENT_DYNAMIC(MFCmodifyaccount, CDialogEx)


CFont static_font3;
// SQLite는 UTF8을 사용하기 때문에 코드 변환이 필요합니다 by jang
// 출처 - http://dolphin.ivyro.net/file/algorithm/SQLite/tutoria03.html
int AnsiToUTF88(char* szSrc, char* strDest, int destSize)
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

int UTF8ToAnsi8(char* szSrc, char* strDest, int destSize)
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



MFCmodifyaccount::MFCmodifyaccount(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}



BOOL MFCmodifyaccount::OnInitDialog() {
	CDialog::OnInitDialog();
	SetWindowText("명령 전송 내역 조회");
	static_font3.CreatePointFont(150, "Arial");


	myList.Attach(GetDlgItem(IDC_LIST_HISTORY)->m_hWnd);
	myList.SetFont(&static_font3);

	CRect rect;
	//list 크기 얻어오기 by jang
	myList.GetClientRect(&rect);


	// 리스트 컨트롤에 컬럼 이름 입력 by jang
	CString tmp;
	int width = rect.Width() / 4;
	tmp = "설비이름(위치)";
	myList.InsertColumn(1, tmp, LVCFMT_CENTER, width);
	tmp = "수신번호";
	myList.InsertColumn(2, tmp, LVCFMT_CENTER, width);
	tmp = "시간";
	myList.InsertColumn(3, tmp, LVCFMT_CENTER, width);
	tmp = "내용";
	myList.InsertColumn(4, tmp, LVCFMT_CENTER, rect.Width() - 3 * width);
	


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
		"NAME          TEXT     NOT NULL,"
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

	// 테이블을 읽어와 리스트 컨트롤에 보여주기
	sqlite3_prepare_v2(db, "select * from db", -1, &stmt, NULL);

	while (sqlite3_step(stmt) != SQLITE_DONE) {
		int i;
		int num_cols = sqlite3_column_count(stmt);

		char szAnsi[300];
		UTF8ToAnsi8((char*)sqlite3_column_text(stmt, 1), szAnsi, 300);
		CString NAME(szAnsi);

		UTF8ToAnsi8((char*)sqlite3_column_text(stmt, 2), szAnsi, 300);
		CString NUMBER(szAnsi);

		UTF8ToAnsi8((char*)sqlite3_column_text(stmt, 3), szAnsi, 300);
		CString TIME(szAnsi);
		
		UTF8ToAnsi8((char*)sqlite3_column_text(stmt, 4), szAnsi, 300);
		CString CONTENT(szAnsi);

		int nItem = myList.InsertItem(0, NAME);
		myList.SetItemText(nItem, 1, NUMBER);
		myList.SetItemText(nItem, 2, TIME);
		myList.SetItemText(nItem, 3, CONTENT);
	}

	sqlite3_finalize(stmt);
	sqlite3_close(db);






	return TRUE;
}

MFCmodifyaccount::~MFCmodifyaccount()
{

}

void MFCmodifyaccount::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(MFCmodifyaccount, CDialogEx)
END_MESSAGE_MAP()


// MFCmodifyaccount 메시지 처리기
