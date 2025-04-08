
// OpenCvWithMFCDlg.h : header file
//

#pragma once
#include "CameraPictureControl.h"

// COpenCvWithMFCDlg dialog
class COpenCvWithMFCDlg : public CDialogEx
{
// Construction
public:
	COpenCvWithMFCDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_OPENCVWITHMFC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnUpdatePicture(WPARAM wParam, LPARAM lParam);

	// Canny filter parameters
	int m_CannyThreshold1;
	int m_CannyThreshold2;

	// Kernel size parameters
	int m_KernelSize1;
	int m_KernelSize2;

	// CLAHE filter parameters
	double m_ClaheClipLimit;
	int m_ClaheWidth;
	int m_ClaheHeight;

	afx_msg void OnEnUpdateEditCannyThreshold1();
	afx_msg void OnEnUpdateEditCannyThreshold2();
	afx_msg void OnEnUpdateEditGaussianKernelSize1();
	afx_msg void OnEnUpdateEditGaussianKernelSize2();
	afx_msg void OnBnClickedButtonSavePicture();
	afx_msg void OnBnClickedButtonCompareFrame();
	// Ширина кадра для масштабирования
	int m_ResizeFrameWidth;
	// Высота кадра для масштабирования
	int m_ResizeFrameHeight;
	afx_msg void OnEnUpdateEditResizeFrameWidth();
	afx_msg void OnEnUpdateEditResizeFrameHeight();
	afx_msg void OnBnClickedButtonOpenDialog();
	afx_msg void OnEnUpdateEditClaheClipLimit();
	afx_msg void OnEnUpdateEditClaheWidth();
	afx_msg void OnEnUpdateEditClaheHeight();
	int m_testDDXInt;
	afx_msg void OnEnKillfocusEdit15();
	BOOL m_applyCanny;
	BOOL m_applyGaussian;
	BOOL m_applyClahe;
	afx_msg void OnBnClickedCheckApplyCanny();
	afx_msg void OnBnClickedCheckApplyGaussian();
	afx_msg void OnBnClickedCheckApplyClahe();

private:
	// Drawing area
	CameraPictureControl m_cameraPictureControl;
public:
	afx_msg void OnBnClickedButtonClearPicture();
};
