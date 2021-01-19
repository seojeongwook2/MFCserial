
// MFCserialportDlg.h: 헤더 파일
//

#pragma once
#include "afxwin.h"
#include "Mycomm.h"


// CMFCserialportDlg 대화 상자
class CMFCserialportDlg : public CDialogEx
{

	CString manage_password = "1234";
	CListCtrl mList;
	CButton add_Button;
	CEdit phone_EditCtrl, name_EditCtrl, password_EditCtrl;
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
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()


public:
	BOOL comport_state;
	CComboBox m_combo_comport_list;
	CString m_str_comport;
	afx_msg void OnBnClickedBtConnect();
	afx_msg void OnCbnSelchangeComboComport();
	CEdit m_edit_revmsg;
	afx_msg void OnEnChangeEditRevmsg();
	//afx_msg void OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnCustomdrawList(NMHDR* pNMHDR, LRESULT* pResult); //색상떄매 추가해봄

};
