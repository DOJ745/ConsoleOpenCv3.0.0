// TestDialog.cpp : implementation file
//

#include "stdafx.h"
#include "OpenCvWithMFC.h"
#include "TestDialog.h"
#include "afxdialogex.h"

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <afxwin.h>
#include <windows.h>

// ���������� ���������� ��� ������������� � ������
cv::VideoCapture TEST_CAP;				// ��� ������� ������
cv::Mat TEST_FRAME;						// ��� �������� �����������
volatile bool stopTestThread = false;	// ��� ���������� �������

// ��������� ������ ����
void MouseCallback(int event, int x, int y, int flags, void* userdata)
{
	if (event == cv::EVENT_LBUTTONDOWN) 
	{
		cv::Mat* img = reinterpret_cast<cv::Mat*>(userdata);
		circle(*img, cv::Point(x, y), 10, cv::Scalar(0, 0, 255), -1);
		imshow("OpenCV Window", *img);
	}
}

// ������� ������ ��� ����������� ������
UINT VideoThread(LPVOID pParam)
{
	while (!stopTestThread && TEST_CAP.isOpened()) 
	{
		TEST_CAP >> TEST_FRAME;

		if (TEST_FRAME.empty())
		{
			break;
		}	

		imshow("OpenCV Window", TEST_FRAME);

		// ��������� �� ������� ESC
		if (cv::waitKey(1) == 27) 
		{
			stopTestThread = true;
			break;
		}
	}

	TEST_CAP.release();
	cv::destroyWindow("OpenCV Window");

	return 0;
}

// ������� ��� �������� ���� OpenCV � ������� MFC
void AttachOpenCVWindowToMFC(HWND hwndParent)
{
	//cv::namedWindow("OpenCV Window", cv::WINDOW_NORMAL);

	RECT parentRect;               // ���������� ���������� ����
	RECT clientRect;               // ������� ���������� �������
	POINT clientOffset = {0, 0};   // �������� ���������� ������� ������������ ����

	// �������� ���������� ���������� ���� MFC
	GetWindowRect(hwndParent, &parentRect);

	// �������� ������� ���������� ������� ���� MFC
	GetClientRect(hwndParent, &clientRect);

	// ����������� ������� ����� ���� ���������� ������� � ���������� ����������
	ClientToScreen(hwndParent, &clientOffset);

	// ������������ �������� ��-�� ������ � ���������
	int borderOffsetX = clientOffset.x - parentRect.left;
	int borderOffsetY = clientOffset.y - parentRect.top;

	// ����� ���� OpenCV
	HWND hwndOpenCV = FindWindow(NULL, L"OpenCV Window");

	if (hwndOpenCV) 
	{
		// ������������ ������� ���� OpenCV � �������� ���������� ������� MFC
		int maxWidth = clientRect.right - clientRect.left;
		int maxHeight = clientRect.bottom - clientRect.top;

		int width = std::min(640, maxWidth);  // ������ ���� OpenCV �� ������ ���������� ������������
		int height = std::min(480, maxHeight); // ������ ���� OpenCV �� ������ ���������� ������������

		int offsetX = 10; // ����� �� ������ ���� ���������� �������
		int offsetY = 10; // ����� �� �������� ���� ���������� �������

		// ��������� ������� OpenCV ���� ������������ ���������� �������
		int posX = clientOffset.x + offsetX;
		int posY = clientOffset.y + offsetY;

		// ������������� ��������� ���� OpenCV ������������ MFC
		SetWindowPos(hwndOpenCV,
			HWND_TOP,
			posX,
			posY,
			width,
			height,
			SWP_NOZORDER | SWP_SHOWWINDOW);

		// ������� ��������� � ������������� �����������
		LONG style = GetWindowLong(hwndOpenCV, GWL_STYLE);
		style &= ~WS_CAPTION;
		SetWindowLong(hwndOpenCV, GWL_STYLE, style);

		// ������������� ���� OpenCV ��� �������� ��� ���� MFC
		SetParent(hwndOpenCV, hwndParent);
	}
}

// TestDialog dialog

IMPLEMENT_DYNAMIC(TestDialog, CDialogEx)

TestDialog::TestDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(TestDialog::IDD, pParent)
{

}

TestDialog::~TestDialog()
{
}

void TestDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL TestDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ������������� ������
	if (!TEST_CAP.open(0)) 
	{
		AfxMessageBox(L"Failed to open camera!");
		return FALSE;
	}

	const int defaultFrameWidth = 1280;
	const int defaultFrameHeight = 1024;

	if (!TEST_CAP.set(cv::CAP_PROP_FRAME_WIDTH, defaultFrameWidth))
	{
		MessageBox(_T("�� ������� ������ ������ ����� ������!"));
		return FALSE;
	}

	if (!TEST_CAP.set(cv::CAP_PROP_FRAME_HEIGHT, defaultFrameHeight))
	{
		MessageBox(_T("�� ������� ������ ������ ����� ������!"));
		return FALSE;
	}

	// ������� ���� OpenCV
	cv::namedWindow("OpenCV Window", cv::WINDOW_NORMAL);

	// ������������� MouseCallback
	setMouseCallback("OpenCV Window", MouseCallback, &TEST_FRAME);

	// ���������� ���� OpenCV
	AttachOpenCVWindowToMFC(GetSafeHwnd());

	// ������ ������ �����������
	AfxBeginThread(VideoThread, NULL);

	return TRUE;
}

void TestDialog::OnDestroy()
{
	stopTestThread = true; // ������������� ����� ��� ���������� �������
	CDialogEx::OnDestroy();
}

BEGIN_MESSAGE_MAP(TestDialog, CDialogEx)
END_MESSAGE_MAP()


// TestDialog message handlers
