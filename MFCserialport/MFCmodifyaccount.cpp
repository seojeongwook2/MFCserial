// MFCmodifyaccount.cpp: 구현 파일
//

#include "pch.h"
#include "MFCserialport.h"
#include "MFCmodifyaccount.h"
#include "afxdialogex.h"


// MFCmodifyaccount 대화 상자

IMPLEMENT_DYNAMIC(MFCmodifyaccount, CDialogEx)

MFCmodifyaccount::MFCmodifyaccount(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

BOOL MFCmodifyaccount::OnInitDialog() {
	CDialog::OnInitDialog();
	SetWindowText("목록 수정하기");
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
