#pragma once


// MFCguide 대화 상자

class MFCguide : public CDialogEx
{
	DECLARE_DYNAMIC(MFCguide)

public:
	MFCguide(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~MFCguide();

	CStatic help_text;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG4 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
};
