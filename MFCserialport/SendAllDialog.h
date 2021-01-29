#pragma once

// SendAllDialog 대화 상자
#include <vector>
#include<map>
#include"Mycomm.h"

class SendAllDialog : public CDialogEx
{
	DECLARE_DYNAMIC(SendAllDialog)

public:
	int tmp;
	std::vector<std::pair<CString, CString>> recv;
	CListCtrl mList;
	CStatic text106;
	CButton button200, button300, button301, button302, button303, button304, button_ok, button_cancel, button305;
	CEdit edit200;
	int idx = -1;
	CMycomm* mn_comm;
	std::map<char, CString> n_mapping;

	SendAllDialog(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~SendAllDialog();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

private:
	void SendAllDialog::SendMessageFunction(CString target_number, CString body);
	void SendAllDialog::Wait(DWORD dwMillisecond);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();


	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnDblclkList1002(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButton300();
	afx_msg void OnBnClickedButton301();
	afx_msg void OnBnClickedButton302();
	afx_msg void OnBnClickedButton303();
	afx_msg void OnBnClickedButton304();
	afx_msg void OnBnClickedButton200();
	afx_msg void OnBnClickedButton305();
};
