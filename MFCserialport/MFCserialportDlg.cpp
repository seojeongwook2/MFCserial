﻿
// MFCserialportDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "MFCserialport.h"
#include "MFCserialportDlg.h"
#include "afxdialogex.h"
#include<map>
#include<string>
#include<algorithm>
#include "sqlite3.h"
#include <assert.h>

#pragma comment(lib, "sqlite3.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "MFCmodifyaccount.h"


// CMFCserialportDlg 대화 상자

std::map<char, CString> mapping;
CFont big_font;
static HANDLE wait_handle;

// SQLite는 UTF8을 사용하기 때문에 코드 변환이 필요합니다 by jang
// 출처 - http://dolphin.ivyro.net/file/algorithm/SQLite/tutoria03.html
int AnsiToUTF8(char* szSrc, char* strDest, int destSize)
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

int UTF8ToAnsi(char* szSrc, char* strDest, int destSize)
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

//여까지


class CUSTOM_MESSAGE {
private:
	int index;
	CString number;
	CString time;
	CString content;

public:
	CUSTOM_MESSAGE() : index(0), number(_T("")), time(_T("")), content(_T("")) { }
	CUSTOM_MESSAGE(int in_index, CString in_number, CString in_time, CString in_content) :
		index(in_index), number(in_number), time(in_time), content(in_content) { }

	int getIndex() { return index; }
	CString getNumber() { return number; }
	CString getTime() { return time; }
	CString getCotent() { return content; }
	void setIndex(int in_index) { index = in_index; }
	void setNumber(CString in_number) { number = in_number; }
	void setTime(CString in_time) { time = in_time; }
	void setContent(CString in_content) { content = in_content; }

	bool operator<(CUSTOM_MESSAGE &target) {
		return this->index < target.index;
	}
};

CUSTOM_MESSAGE total_message_in_modem[277];

CMFCserialportDlg::CMFCserialportDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCSERIALPORT_DIALOG, pParent)
	, m_str_comport(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCserialportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_COMPORT, m_combo_comport_list);
	DDX_CBString(pDX, IDC_COMBO_COMPORT, m_str_comport);
	DDX_Control(pDX, IDC_EDIT_REVMSG, m_edit_revmsg);
}

BEGIN_MESSAGE_MAP(CMFCserialportDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST1, OnCustomdrawList)
	ON_MESSAGE(WM_MYCLOSE, &CMFCserialportDlg::OnThreadClosed)
	ON_MESSAGE(WM_MYRECEIVE, &CMFCserialportDlg::OnReceive)
	ON_BN_CLICKED(IDC_BT_CONNECT, &CMFCserialportDlg::OnBnClickedBtConnect)
	ON_CBN_SELCHANGE(IDC_COMBO_COMPORT, &CMFCserialportDlg::OnCbnSelchangeComboComport)
	ON_EN_CHANGE(IDC_EDIT_REVMSG, &CMFCserialportDlg::OnEnChangeEditRevmsg)
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCserialportDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CMFCserialportDlg 메시지 처리기

BOOL CMFCserialportDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	// ShowWindow(SW_SHOWMAXIMIZED);
	buffer_complete = TRUE;
	save_buffer = "";

	for (int i = 0; i < 277; i++) {
		total_message_in_modem[i].setContent(_T(""));
		total_message_in_modem[i].setTime(_T(""));
		total_message_in_modem[i].setNumber(_T(""));
		total_message_in_modem[i].setIndex(0);
	}

	m_combo_comport_list.AddString(_T("COM1"));
	m_combo_comport_list.AddString(_T("COM2"));
	m_combo_comport_list.AddString(_T("COM3"));
	m_combo_comport_list.AddString(_T("COM4"));
	m_combo_comport_list.AddString(_T("COM5"));
	m_combo_comport_list.AddString(_T("COM6"));

	mapping.insert(std::pair<char, CString>('@', "00"));
	mapping.insert(std::pair<char, CString>('!', "21"));
	mapping.insert(std::pair<char, CString>('\"', "22"));
	mapping.insert(std::pair<char, CString>('#', "23"));

	mapping.insert(std::pair<char, CString>('%', "25"));
	mapping.insert(std::pair<char, CString>('&', "26"));
	mapping.insert(std::pair<char, CString>('\'', "27"));
	mapping.insert(std::pair<char, CString>('(', "28"));
	mapping.insert(std::pair<char, CString>(')', "29"));
	mapping.insert(std::pair<char, CString>('*', "2A"));
	mapping.insert(std::pair<char, CString>('+', "2B"));
	mapping.insert(std::pair<char, CString>(',', "2C"));
	mapping.insert(std::pair<char, CString>('-', "2D"));
	mapping.insert(std::pair<char, CString>('.', "2E"));
	mapping.insert(std::pair<char, CString>('/', "2F"));

	mapping.insert(std::pair<char, CString>('0', "30"));
	mapping.insert(std::pair<char, CString>('1', "31"));
	mapping.insert(std::pair<char, CString>('2', "32"));
	mapping.insert(std::pair<char, CString>('3', "33"));
	mapping.insert(std::pair<char, CString>('4', "34"));
	mapping.insert(std::pair<char, CString>('5', "35"));
	mapping.insert(std::pair<char, CString>('6', "36"));
	mapping.insert(std::pair<char, CString>('7', "37"));
	mapping.insert(std::pair<char, CString>('8', "38"));
	mapping.insert(std::pair<char, CString>('9', "39"));
	mapping.insert(std::pair<char, CString>(':', "3A"));
	mapping.insert(std::pair<char, CString>(';', "3B"));
	mapping.insert(std::pair<char, CString>('<', "3C"));
	mapping.insert(std::pair<char, CString>('=', "3D"));
	mapping.insert(std::pair<char, CString>('>', "3E"));
	mapping.insert(std::pair<char, CString>('?', "3F"));

	mapping.insert(std::pair<char, CString>('A', "41"));
	mapping.insert(std::pair<char, CString>('B', "42"));
	mapping.insert(std::pair<char, CString>('C', "43"));
	mapping.insert(std::pair<char, CString>('D', "44"));
	mapping.insert(std::pair<char, CString>('E', "45"));
	mapping.insert(std::pair<char, CString>('F', "46"));
	mapping.insert(std::pair<char, CString>('G', "47"));
	mapping.insert(std::pair<char, CString>('H', "48"));
	mapping.insert(std::pair<char, CString>('I', "49"));
	mapping.insert(std::pair<char, CString>('J', "4A"));
	mapping.insert(std::pair<char, CString>('K', "4B"));
	mapping.insert(std::pair<char, CString>('L', "4C"));
	mapping.insert(std::pair<char, CString>('M', "4D"));
	mapping.insert(std::pair<char, CString>('N', "4E"));
	mapping.insert(std::pair<char, CString>('O', "4F"));
	mapping.insert(std::pair<char, CString>('P', "50"));
	mapping.insert(std::pair<char, CString>('Q', "51"));
	mapping.insert(std::pair<char, CString>('R', "52"));
	mapping.insert(std::pair<char, CString>('S', "53"));
	mapping.insert(std::pair<char, CString>('T', "54"));
	mapping.insert(std::pair<char, CString>('U', "55"));
	mapping.insert(std::pair<char, CString>('V', "56"));
	mapping.insert(std::pair<char, CString>('W', "57"));
	mapping.insert(std::pair<char, CString>('X', "58"));
	mapping.insert(std::pair<char, CString>('Y', "59"));
	mapping.insert(std::pair<char, CString>('Z', "5A"));

	mapping.insert(std::pair<char, CString>('a', "61"));
	mapping.insert(std::pair<char, CString>('b', "62"));
	mapping.insert(std::pair<char, CString>('c', "63"));
	mapping.insert(std::pair<char, CString>('d', "64"));
	mapping.insert(std::pair<char, CString>('e', "65"));
	mapping.insert(std::pair<char, CString>('f', "66"));
	mapping.insert(std::pair<char, CString>('g', "67"));
	mapping.insert(std::pair<char, CString>('h', "68"));
	mapping.insert(std::pair<char, CString>('i', "69"));
	mapping.insert(std::pair<char, CString>('j', "6A"));
	mapping.insert(std::pair<char, CString>('k', "6B"));
	mapping.insert(std::pair<char, CString>('l', "6C"));
	mapping.insert(std::pair<char, CString>('m', "6D"));
	mapping.insert(std::pair<char, CString>('n', "6E"));
	mapping.insert(std::pair<char, CString>('o', "6F"));
	mapping.insert(std::pair<char, CString>('p', "70"));
	mapping.insert(std::pair<char, CString>('q', "71"));
	mapping.insert(std::pair<char, CString>('r', "72"));
	mapping.insert(std::pair<char, CString>('s', "73"));
	mapping.insert(std::pair<char, CString>('t', "74"));
	mapping.insert(std::pair<char, CString>('u', "75"));
	mapping.insert(std::pair<char, CString>('v', "76"));
	mapping.insert(std::pair<char, CString>('w', "77"));
	mapping.insert(std::pair<char, CString>('x', "78"));
	mapping.insert(std::pair<char, CString>('y', "79"));
	mapping.insert(std::pair<char, CString>('z', "7A"));

	comport_state = false;
	GetDlgItem(IDC_BT_CONNECT)->SetWindowText(_T("OPEN"));
	m_str_comport = _T("COM2");
	UpdateData(FALSE);

	big_font.CreatePointFont(200, TEXT("굴림"));
	GetDlgItem(IDC_EDIT_REVMSG)->SetFont(&big_font);

	SetWindowText("문자 송/수신 프로그램");

	//윈도우 크기 설정 by jang
	SIZE s;
	ZeroMemory(&s, sizeof(SIZE));
	s.cx = (LONG) ::GetSystemMetrics(SM_CXFULLSCREEN);
	s.cy = (LONG) ::GetSystemMetrics(SM_CYFULLSCREEN);
	SetWindowPos(&wndTop, 0, 0, s.cx, s.cy, SWP_SHOWWINDOW);

	//init by jang
	
	mList.Attach(GetDlgItem(IDC_LIST1)->m_hWnd);
	add_Button.Attach(GetDlgItem(IDC_BUTTON1)->m_hWnd);
	name_EditCtrl.Attach(GetDlgItem(IDC_EDIT2)->m_hWnd);
	phone_EditCtrl.Attach(GetDlgItem(IDC_EDIT3)->m_hWnd);
	password_EditCtrl.Attach(GetDlgItem(IDC_EDIT4)->m_hWnd);

	//init list by jang

	mList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	CRect rect;
	//list 크기 얻어오기 by jang
	mList.GetClientRect(&rect);


	// 리스트 컨트롤에 컬럼 이름 입력 by jang
	CString tmp;
	tmp = "이름";
	int width = rect.Width() / 4;
	mList.InsertColumn(0, tmp, LVCFMT_LEFT , width);
	tmp = "전화번호";
	mList.InsertColumn(1, tmp, LVCFMT_LEFT, width);

	tmp = "가동여부";
	mList.InsertColumn(2, tmp, LVCFMT_LEFT, width);

	tmp = "수위위험";
	mList.InsertColumn(3, tmp, LVCFMT_LEFT,rect.Width() - 3 * width);


	//db create by jang

	// 데이터베이스 파일 생성 및 열기
	sqlite3* db;
	sqlite3_stmt* stmt;
	char* errmsg = NULL;

	int rc = sqlite3_open("test.db", &db);

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
		"TEL           TEXT     NOT NULL);";

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
		UTF8ToAnsi((char*)sqlite3_column_text(stmt, 1), szAnsi, 300);
		CString name(szAnsi);

		UTF8ToAnsi((char*)sqlite3_column_text(stmt, 2), szAnsi, 300);
		CString tel(szAnsi);

		/*
		UTF8ToAnsi((char*)sqlite3_column_text(stmt, 3), szAnsi, 300);
		CString ison(szAnsi);

		UTF8ToAnsi((char*)sqlite3_column_text(stmt, 4), szAnsi, 300);
		CString danger(szAnsi);
		*/


		int nItem = mList.InsertItem(0, name);
		mList.SetItemText(nItem, 1, tel);
		mList.SetItemText(nItem, 2, "정상");
		mList.SetItemText(nItem, 3, "정상");
	}

	sqlite3_finalize(stmt);

	sqlite3_close(db);



	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}


// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMFCserialportDlg::OnPaint()
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
HCURSOR CMFCserialportDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CMFCserialportDlg::OnThreadClosed(WPARAM length, LPARAM lpara) {
	((CMycomm*)lpara)->HandleClose();
	delete ((CMycomm*)lpara);
	return 0;
}

LRESULT CMFCserialportDlg::OnReceive(WPARAM length, LPARAM lpara) {
	CString str;
	char data[20000];
	if (m_comm) {
		m_comm->Receive(data, length);
		data[length] = _T('\0');
		str += _T("\r\n");

		for (int i = 0; i < length; i++) {
			str += data[i];
		}

		if (buffer_complete == FALSE) {
			str.Remove('\r');
			str.Remove('\n');
			save_buffer += str;
			save_buffer.Remove('\r');
			save_buffer.Remove('\n');

			if (save_buffer.Find("OK") != -1) {

				for (int i = 0; i < 255; i++) {
					CString temp;
					AfxExtractSubString(temp, save_buffer, i + 1, ':');

					temp = temp.Right(temp.GetLength() - 1);
					if (temp.Find("*SKT*READTI") != -1) {
						temp = temp.Left(temp.GetLength() - 11);
					}
					else {
						temp = temp.Left(temp.GetLength() - 2);
					}

					CString temp_index, temp_time, temp_number, temp_content;
					AfxExtractSubString(temp_index, temp, 0, ',');
					AfxExtractSubString(temp_time, temp, 1, ',');
					AfxExtractSubString(temp_number, temp, 2, ',');
					AfxExtractSubString(temp_content, temp, 4, ',');

					total_message_in_modem[i].setIndex(_ttoi(temp_index));
					total_message_in_modem[i].setTime(temp_time);
					total_message_in_modem[i].setContent(temp_content);
					total_message_in_modem[i].setNumber(temp_number);


					temp += "\r\n";
					m_edit_revmsg.ReplaceSel(temp);
					m_edit_revmsg.LineScroll(m_edit_revmsg.GetLineCount());
				}

				std::sort(total_message_in_modem, total_message_in_modem + 255);
				save_buffer = "";
				buffer_complete = TRUE;
			}

			return 0;
		}

		if (str.Find("*SMSAL") != -1) {
			/*
			** *SMSALERT가 들어옴. ERT가 잘려도 문제 없게 작성
			** OK가 들어올 때까지 buffer를 열어두자
			*/

			buffer_complete = FALSE; 
			save_buffer = "";
			CString command_send_string = "AT*SKT*READTI=4098\r\n";
			m_comm->Send(command_send_string, command_send_string.GetLength());
		}
		
		str = "";
	}

	return 0;
}

void CMFCserialportDlg::Wait(DWORD dwMillisecond)
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

/*
문자 수신시 "\r\n\r\n*SMSALERT\r\n" str에 저장되면서 Recieve.
명령을 보낼 때 마지막에 무조건 "\r\n" 보낼것
20210110 Git Test
*/

/*
버튼 : ID / caption 변경

Edit Control : ID설정
변수 추가 m_edit_send_data -> Control --> Control ID 와 설정해준 ID가 일치해야함.
*/


// comport button 
void CMFCserialportDlg::OnBnClickedBtConnect()
{
	if (comport_state) {
		if (m_comm) {
			m_comm->Close();
			m_comm = NULL;
			AfxMessageBox(_T("COM 포트닫힘"));
			comport_state = false;
			GetDlgItem(IDC_BT_CONNECT)->SetWindowText(_T("OPEN"));
		}
	}
	else {
		m_comm = new CMycomm(_T("\\\\.\\") + m_str_comport, _T("115200"), _T("None"), _T("8 Bit"), _T("1 Bit"));
		if (m_comm->Create(GetSafeHwnd()) != 0) {
			AfxMessageBox(_T("COM 포트열림"));
			comport_state = true;
			GetDlgItem(IDC_BT_CONNECT)->SetWindowTextA(_T("CLOSE"));
		}
		else {
			AfxMessageBox(_T("ERROR!"));
		}
	}
}


void CMFCserialportDlg::OnCbnSelchangeComboComport()
{
	UpdateData();
}

/* 문자보내는 코드
void CMFCserialportDlg::OnBnClickedBtMessageSend()
{
	CString str_num;
	GetDlgItem(IDC_EDIT_PHONE_NUM)->GetWindowTextA(str_num);
	CString str_body;
	GetDlgItem(IDC_EDIT_BODY)->GetWindowTextA(str_body);
	SendMessageFunction(str_num, str_body);
}


void CMFCserialportDlg::SendMessageFunction(CString target_number, CString body) {
	int Answer = AfxMessageBox("명령을 문자로 보내시겠습니까?", MB_OKCANCEL);
	
	if (Answer == IDOK) {
		CString encode_msg = "";
		for (int i = 0; i < body.GetLength(); i++) {
			encode_msg += mapping[body.GetAt(i)];
		}

		CString final_send_string = "AT*SMSMO=";
		final_send_string += target_number;
		final_send_string += ",01224606372,";
		final_send_string += encode_msg;
		final_send_string += "\r\n";
		m_comm->Send(final_send_string, final_send_string.GetLength());
	}
	else {

	}
}

*/

void CMFCserialportDlg::OnEnChangeEditRevmsg()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}




//추가 버튼 클릭 리스너
void CMFCserialportDlg::OnBnClickedButton1()
{

	CString name;
	name_EditCtrl.GetWindowText(name);
	CString phone;
	phone_EditCtrl.GetWindowText(phone);
	CString password;
	password_EditCtrl.GetWindowText(password);

	if (password.Compare(manage_password) != 0) {
		CString tmp = "비밀번호가 틀립니다";
		MessageBox(tmp);
		return;
	}

	int nItem = mList.InsertItem(0, name);
	mList.SetItemText(nItem, 1, phone);
	mList.SetItemText(nItem, 2, "위험");
	mList.SetItemText(nItem, 3, "정상");
	

	// 에디터 박스에 입력된 데이터를 리스트컨트롤에 입력

	sqlite3* db;
	int rc = sqlite3_open("test.db", &db);

	if (rc != SQLITE_OK)
	{
		printf("Failed to open DB\n");
		sqlite3_close(db);
		exit(1);
	}

	char* s_name;

	CStringW strw(name);
	LPCWSTR ptr = strw;

	int sLen = WideCharToMultiByte(CP_ACP, 0, ptr, -1, NULL, 0, NULL, NULL);
	s_name = new char[sLen + 1];
	WideCharToMultiByte(CP_ACP, 0, ptr, -1, s_name, sLen, NULL, NULL);

	char szName[100];
	AnsiToUTF8(s_name, szName, 100);

	delete[]s_name;




	char* s_tel;
	CStringW strw2(phone);
	ptr = strw2;
	sLen = WideCharToMultiByte(CP_ACP, 0, ptr, -1, NULL, 0, NULL, NULL);
	s_tel = new char[sLen + 1];
	WideCharToMultiByte(CP_ACP, 0, ptr, -1, s_tel, sLen, NULL, NULL);

	char szTel[100];
	AnsiToUTF8(s_tel, szTel, 100);

	delete[]s_tel;



	char* errmsg = NULL;
	char sql[255] = { 0 };
	sprintf(sql, "insert into db(name, tel) values('%s','%s');", szName, szTel);

	if (SQLITE_OK != sqlite3_exec(db, sql, NULL, NULL, &errmsg))
	{
		printf("insert");
	}

	sqlite3_close(db);

}


void CMFCserialportDlg::OnCustomdrawList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	*pResult = 0;

	NMLVCUSTOMDRAW* pLVCD = (NMLVCUSTOMDRAW*)pNMHDR;

	switch (pLVCD->nmcd.dwDrawStage){
		case CDDS_PREPAINT:{
			*pResult = CDRF_NOTIFYITEMDRAW;
		}
		break;
		case CDDS_ITEMPREPAINT:	{
			int iRow = pLVCD->nmcd.dwItemSpec; // 행을 알수있다.
			CString isOn = mList.GetItemText(iRow, 2);
			CString isDanger = mList.GetItemText(iRow, 2);
			if (isOn.Compare("위험") == 0 || isDanger.Compare("위험") ==0) {
				pLVCD->clrText = RGB(255, 0, 0); // 텍스트 색 지정
				pLVCD->clrTextBk = RGB(255, 255, 255); // 텍스트 배경색 지정
			}
			*pResult = CDRF_DODEFAULT;
		}
		break;
		default:
		{
			*pResult = CDRF_DODEFAULT;
		}
		break;
	}

}