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
	TestDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~TestDialog();

// Dialog Data
	enum { IDD = IDD_TEST_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnDestroy();

private:
	cv::VideoCapture m_Capture;			// ������ �����
	cv::Mat m_CurrentFrame;				// ������� ����
	cv::Mat m_DrawFrame;				// ���� � ���������������
	CRITICAL_SECTION m_CriticalSection; // ����������� ������ ��� �������������
	volatile bool m_StopThread;         // ���� ���������� ������
	volatile bool m_IsDrawing;          // ���� ��������� ��������������
	cv::Point m_StartPoint;             // ������ ��������������
	cv::Point m_EndPoint;               // ����� ��������������
	CWinThread* m_VideoThread;          // ����� ��� ��������� ������� �����

	static UINT VideoThread(LPVOID pParam);
	static void MouseCallback(int event, int x, int y, int flags, void* userdata);

	void CloneFrame(const cv::Mat& source, cv::Mat& destination);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonShowMessage();

	void SetCursorCoords(int x, int y);
private:
	// ���������� ��������� ���� � �������� ����
	CString m_CursorChildCoord;
public:
	afx_msg void OnBnClickedButtonStopVideo();
};
