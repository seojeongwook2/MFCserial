
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

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "MFCmodifyaccount.h"


// CMFCserialportDlg 대화 상자

std::map<char, CString> mapping;
CFont big_font;
static HANDLE wait_handle;

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
	DDX_Control(pDX, IDC_EDIT_RCV_VIEW, m_edit_rcv_view);
	DDX_Control(pDX, IDC_EDIT_SEND_DATA, m_edit_send_data);
	DDX_Control(pDX, IDC_EDIT_PHONE_NUM, m_edit_phone_num);
	DDX_Control(pDX, IDC_EDIT_BODY, m_edit_body);
	DDX_Control(pDX, IDC_EDIT_REVMSG, m_edit_revmsg);
}

BEGIN_MESSAGE_MAP(CMFCserialportDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_MESSAGE(WM_MYCLOSE, &CMFCserialportDlg::OnThreadClosed)
	ON_MESSAGE(WM_MYRECEIVE, &CMFCserialportDlg::OnReceive)
	ON_BN_CLICKED(IDC_BT_CONNECT, &CMFCserialportDlg::OnBnClickedBtConnect)
	ON_CBN_SELCHANGE(IDC_COMBO_COMPORT, &CMFCserialportDlg::OnCbnSelchangeComboComport)
	ON_BN_CLICKED(IDC_BT_CLEAR, &CMFCserialportDlg::OnBnClickedBtClear)
	ON_BN_CLICKED(IDC_BT_SEND, &CMFCserialportDlg::OnBnClickedBtSend)
	ON_BN_CLICKED(IDC_BT_MESSAGE_SEND, &CMFCserialportDlg::OnBnClickedBtMessageSend)
	ON_EN_CHANGE(IDC_EDIT_REVMSG, &CMFCserialportDlg::OnEnChangeEditRevmsg)
	ON_BN_CLICKED(IDC_BT_MODIFY_ACCOUNT, &CMFCserialportDlg::OnBnClickedBtModifyAccount)
	ON_EN_CHANGE(IDC_EDIT_SEND_DATA, &CMFCserialportDlg::OnEnChangeEditSendData)
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

		m_edit_rcv_view.ReplaceSel(str);
		m_edit_rcv_view.LineScroll(m_edit_rcv_view.GetLineCount());

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


					/*
					temp += "\r\n";
					m_edit_revmsg.ReplaceSel(temp);
					m_edit_revmsg.LineScroll(m_edit_revmsg.GetLineCount());
					*/
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
			GetDlgItem(IDC_EDIT_RCV_VIEW)->SetWindowTextA(_T(" "));
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
			GetDlgItem(IDC_BT_SEND)->EnableWindow(false);
		}
	}
	else {
		m_comm = new CMycomm(_T("\\\\.\\") + m_str_comport, _T("115200"), _T("None"), _T("8 Bit"), _T("1 Bit"));
		if (m_comm->Create(GetSafeHwnd()) != 0) {
			AfxMessageBox(_T("COM 포트열림"));
			comport_state = true;
			GetDlgItem(IDC_BT_CONNECT)->SetWindowTextA(_T("CLOSE"));
			GetDlgItem(IDC_BT_SEND)->EnableWindow(true);
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

void CMFCserialportDlg::OnBnClickedBtClear()
{
	GetDlgItem(IDC_EDIT_RCV_VIEW)->SetWindowTextA(_T(" "));

	/*
	for (int i = 0; i < 10; i++) {
		CString final_send_string = "AT*SMSMO=";
		final_send_string += "01062817950";
		final_send_string += ",01224606372,";
		final_send_string += "504D";
		final_send_string += "\r\n";
		m_comm->Send(final_send_string, final_send_string.GetLength()); 
		Wait(2000);
		// 일괄전송용.
	}
	*/
}


void CMFCserialportDlg::OnBnClickedBtSend()
{
	CString str;
	GetDlgItem(IDC_EDIT_SEND_DATA)->GetWindowTextA(str);
	str += "\r\n";
	m_comm->Send(str, str.GetLength());
}


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

void CMFCserialportDlg::OnBnClickedBtModifyAccount()
{
	MFCmodifyaccount dlg;
	dlg.DoModal();
}

void CMFCserialportDlg::OnEnChangeEditRevmsg()
{

}

void CMFCserialportDlg::OnEnChangeEditSendData()
{

}