// SendAllDialog.cpp: 구현 파일
//

#include "pch.h"
#include "MFCserialport.h"
#include "SendAllDialog.h"
#include "afxdialogex.h"
#include<map>

// SendAllDialog 대화 상자

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
END_MESSAGE_MAP()


// SendAllDialog 메시지 처리기


BOOL SendAllDialog::OnInitDialog()
{

	CDialog::OnInitDialog();
	SetWindowText("명령 일괄 전송");

	mList.Attach(GetDlgItem(IDC_LIST1002)->m_hWnd);
	mList.SetFocus();
	mList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	text106.Attach(GetDlgItem(IDC_STATIC106)->m_hWnd);
	button200.Attach(GetDlgItem(IDC_BUTTON200)->m_hWnd);
	edit200.Attach(GetDlgItem(IDC_EDIT200)->m_hWnd);

	CRect rect;
	//list 크기 얻어오기 by jang
	mList.GetClientRect(&rect);
	// 리스트 컨트롤에 컬럼 이름 입력 by jang
	CString tmp;
	tmp = "이름";
	int width = rect.Width() / 4;
	mList.InsertColumn(0, tmp, LVCFMT_LEFT, width);
	tmp = "전화번호";
	mList.InsertColumn(1, tmp, LVCFMT_LEFT, width);
	tmp = "메시지 내용";
	mList.InsertColumn(2, tmp, LVCFMT_LEFT, rect.Width() - 2 * width);

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
	final_send_string += ",01224606372,";
	final_send_string += encode_msg;
	final_send_string += "\r\n";

	mn_comm->Send(final_send_string, final_send_string.GetLength());
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
