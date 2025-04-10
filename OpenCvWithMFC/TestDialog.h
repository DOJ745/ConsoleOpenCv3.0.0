#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>
#include <afxwin.h>
#include <windows.h>
#include <memory>

// TestDialog dialog

class TestDialog : public CDialogEx
{
	DECLARE_DYNAMIC(TestDialog)
public:
	// Canny filter parameters
	int m_cannyThreshold1;
	int m_cannyThreshold2;

	// Kernel size parameters
	int m_kernelSize1;
	int m_kernelSize2;

	// CLAHE filter parameters
	double m_claheClipLimit;
	int m_claheWidth;
	int m_claheHeight;

	// ������ ����� ��� ���������������
	int m_resizeFrameWidth;

	// ������ ����� ��� ���������������
	int m_resizeFrameHeight;

	BOOL m_applyCanny;
	BOOL m_applyGaussian;
	BOOL m_applyClahe;

public:
	TestDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~TestDialog();

// Dialog Data
	enum { IDD = IDD_TEST_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

private:
	cv::VideoCapture m_videoCapture;	// ������ �����
	cv::Mat m_currentFrame;				// ������� ����
	cv::Mat m_rectFrame;				// ���� � ���������������
	CRITICAL_SECTION m_criticalSection; // ����������� ������ ��� �������������
	volatile bool m_stopThread;         // ���� ���������� ������
	volatile bool m_isDrawing;          // ���� ��������� ��������������
	cv::Point m_startPoint;             // ������ ��������������
	cv::Point m_endPoint;               // ����� ��������������
	CWinThread* m_videoThread;          // ����� ��� ��������� ������� �����

	static UINT VideoThread(LPVOID pParam);
	static void MouseCallback(int event, int x, int y, int flags, void* userdata);

	void CloneFrame(const cv::Mat& source, cv::Mat& destination);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonShowMessage();

	void SetCursorCoords(int x, int y);
private:
	// ���������� ��������� ���� � �������� ����
	CString m_cursorChildCoord;
public:
	afx_msg void OnBnClickedButtonStopVideo();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
