// MFCguide.cpp: 구현 파일
//

#include "pch.h"
#include "MFCserialport.h"
#include "MFCguide.h"
#include "afxdialogex.h"


// MFCguide 대화 상자
CFont static_font5;

IMPLEMENT_DYNAMIC(MFCguide, CDialogEx)

MFCguide::MFCguide(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG4, pParent)
{

}

BOOL MFCguide::OnInitDialog() {
	CDialog::OnInitDialog();
	SetWindowText("도움말");
	static_font5.CreatePointFont(150, "Arial");

	help_text.Attach(GetDlgItem(IDC_STATIC_HELP)->m_hWnd);
	help_text.SetFont(&static_font5);

	// \n : 엔터키
	// \" 
	// 빌드 -> 솔루션 다시 빌드
	help_text.SetWindowTextA(" 여기에 도움말을 입력하세요ㄹ \"ㅇㅁㄴㄹ\"");

	return TRUE;
}

MFCguide::~MFCguide()
{
}

void MFCguide::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(MFCguide, CDialogEx)
END_MESSAGE_MAP()


// MFCguide 메시지 처리기
