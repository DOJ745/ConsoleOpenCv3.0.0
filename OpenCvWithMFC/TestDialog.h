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
	cv::VideoCapture m_Capture;			// Захват видео
	cv::Mat m_CurrentFrame;				// Текущий кадр
	cv::Mat m_DrawFrame;				// Кадр с прямоугольником
	CRITICAL_SECTION m_CriticalSection; // Критическая секция для синхронизации
	volatile bool m_StopThread;         // Флаг завершения потока
	volatile bool m_IsDrawing;          // Флаг рисования прямоугольника
	cv::Point m_StartPoint;             // Начало прямоугольника
	cv::Point m_EndPoint;               // Конец прямоугольника
	CWinThread* m_VideoThread;          // Поток для обработки захвата видео

	static UINT VideoThread(LPVOID pParam);
	static void MouseCallback(int event, int x, int y, int flags, void* userdata);

	void CloneFrame(const cv::Mat& source, cv::Mat& destination);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonShowMessage();

	void SetCursorCoords(int x, int y);
private:
	// Координаты указателя мыши в дочернем окне
	CString m_CursorChildCoord;
public:
	afx_msg void OnBnClickedButtonStopVideo();
};
