
// MFCserialportDlg.h: 헤더 파일
//

#pragma once
#include "afxwin.h"
#include "Mycomm.h"
#include <vector>
#include<map>

// CMFCserialportDlg 대화 상자
class CMFCserialportDlg : public CDialogEx
{

	CString manage_password = "eosint390*";
	CListCtrl mList, msgList;
	CButton add_Button, PUMP1, PUMP2, PUMP3, STOP, RESET, delete_button, button08, button09, button_connect, PUMP4, button_history;
	CEdit phone_EditCtrl, name_EditCtrl, password_EditCtrl, sendName_EditCtrl, sendPhone_EditCtrl, sendMessage_EditCtrl;
	CStatic text101, text102, text103, text104, text100, text600, text601, text602;
	
	std::vector<std::pair<CString, CString>> vt;
	std::map<char, CString> mapping;

	int idx = -1;
	BOOL color_flag;

// 생성입니다.
public:
	CMFCserialportDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	CMycomm* m_comm;
	LRESULT OnThreadClosed(WPARAM length, LPARAM lpara);
	LRESULT OnReceive(WPARAM length, LPARAM lpara);


// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCSERIALPORT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

private:
	BOOL buffer_complete;
	CString save_buffer;
	void CMFCserialportDlg::Wait(DWORD dwMillisecond);
	void CMFCserialportDlg::SendMessageFunction(CString target_number, CString body);

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()


public:
	BOOL comport_state;
	CComboBox m_combo_comport_list;
	CString m_str_comport;
	afx_msg void OnBnClickedBtConnect();
	afx_msg void OnCbnSelchangeComboComport();
	afx_msg void OnEnChangeEditRevmsg();
	//afx_msg void OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnCustomdrawList(NMHDR* pNMHDR, LRESULT* pResult); //색상떄매 추가해봄

	afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton8();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnBnClickedButton9();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedButtonHistory();
	afx_msg void OnBnClickedButton11();
};
