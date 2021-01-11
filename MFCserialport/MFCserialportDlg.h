﻿
// MFCserialportDlg.h: 헤더 파일
//

#pragma once
#include "afxwin.h"
#include "Mycomm.h"


// CMFCserialportDlg 대화 상자
class CMFCserialportDlg : public CDialogEx
{
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
	CComboBox m_combo_baudrate_list;
	CString m_str_baudrate;
	CEdit m_edit_rcv_view;
	CEdit m_edit_send_data;
	afx_msg void OnBnClickedBtConnect();
	afx_msg void OnCbnSelchangeComboComport();
	afx_msg void OnCbnSelchangeComboBaudrate();
	afx_msg void OnBnClickedBtClear();
	afx_msg void OnBnClickedBtSend();
	CEdit m_edit_phone_num;
	CEdit m_edit_body;
	afx_msg void OnBnClickedBtMessageSend();
	CEdit m_edit_revmsg;
	afx_msg void OnEnChangeEditRevmsg();
	afx_msg void OnBnClickedBtModifyAccount();
};
