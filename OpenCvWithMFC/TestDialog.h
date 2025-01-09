#pragma once


// TestDialog dialog

class TestDialog : public CDialogEx
{
	DECLARE_DYNAMIC(TestDialog)

public:
	TestDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~TestDialog();

// Dialog Data
	enum { IDD = IDD_TEST_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnDestroy();

	DECLARE_MESSAGE_MAP()
};
