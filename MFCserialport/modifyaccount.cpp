// modifyaccount.cpp: 구현 파일
//

#include "pch.h"
#include "MFCserialport.h"
#include "modifyaccount.h"
#include "afxdialogex.h"


// modifyaccount 대화 상자

IMPLEMENT_DYNAMIC(modifyaccount, CDialogEx)

modifyaccount::modifyaccount(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

modifyaccount::~modifyaccount()
{
}

void modifyaccount::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(modifyaccount, CDialogEx)
END_MESSAGE_MAP()


// modifyaccount 메시지 처리기
