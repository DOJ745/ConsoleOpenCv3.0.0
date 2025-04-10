// TestDialog.cpp : implementation file
//

#include "stdafx.h"
#include "OpenCvWithMFC.h"
#include "TestDialog.h"
#include "afxdialogex.h"

// Space in name impacts the window style
const std::string OPEN_CV_WINDOW_NAME = "OpenCVWindow";

void setPointInImage(const int width, const int height, int& x, int& y)
{
	if (x > width)
	{
		x = width;
	}

	if (x < 0)
	{
		x = 0;
	}

	if (y > height)
	{
		y = height;
	}

	if (y < 0)
	{
		y = 0;
	}
}

// Функция для привязки окна OpenCV к области MFC
void AttachOpenCVWindowToMFC(HWND hwndDialog)
{	
	cv::namedWindow(OPEN_CV_WINDOW_NAME, cv::WINDOW_NORMAL);

	CRect windowContainer;
	CRect clientParentRect;      // Размеры клиентской области окна MFC

	// Получаем размеры клиентской области окна MFC
	::GetClientRect(hwndDialog, &clientParentRect);

	// Найти окно OpenCV (контейнер вместе с элементом, где отображаются кадры)
	HWND hwndOpenCV = static_cast<HWND>(cvGetWindowHandle(OPEN_CV_WINDOW_NAME.c_str()));
	HWND parentHwndOpenCV = ::GetParent(hwndOpenCV);

	CWnd* ptrWindowContainer = CWnd::FromHandle(hwndDialog)->GetDlgItem(IDC_OPENCV_CONTAINER);
	HWND windowContainerHwnd; 

	if (!ptrWindowContainer) 
	{
		return;
	}
	else
	{
		windowContainerHwnd = ptrWindowContainer->GetSafeHwnd();

		::GetClientRect(windowContainerHwnd, &windowContainer);

		::SetWindowPos(windowContainerHwnd
			, hwndDialog
			, 0
			, 0
			, windowContainer.Width()
			, windowContainer.Height()
			, SWP_NOZORDER
			);
	}

	if (parentHwndOpenCV != NULL) 
	{
		// Ограничиваем размеры окна OpenCV в пределах окна клиентской области MFC
		int maxWidth = clientParentRect.Width();
		int maxHeight = clientParentRect.Height();

		int minWidth = 1280;
		int minHeight = 1024;

		int width = std::min(minWidth, maxWidth);
		int height = std::min(minHeight, maxHeight);;

		// Устанавливаем окно OpenCV как дочернее для контейнера внутри окна MFC
		HWND prevParent = ::SetParent(parentHwndOpenCV, windowContainerHwnd);

		if (prevParent == NULL)
		{
			MessageBox(NULL
				, _TEXT("Не удалось задать родительское окно!")
				, _TEXT("Ошибка")
				, MB_OKCANCEL | MB_ICONERROR);
			return;
		}

		LONG style = GetWindowLong(parentHwndOpenCV, GWL_STYLE);

		// Убираем заголовок и предотвращаем перемещение
		style &= ~(WS_POPUP | WS_CAPTION | WS_THICKFRAME);
		style |= WS_CHILD | DS_MODALFRAME;

		::SetWindowLongPtr(parentHwndOpenCV, GWL_STYLE, style);

		// Устанавливаем положение окна OpenCV относительно окна MFC
		::SetWindowPos(parentHwndOpenCV
			, NULL
			, 0
			, 0
			, 800
			, 600
			, SWP_NOZORDER);
	}
	else
	{
		return;
	}
}

// TestDialog dialog

IMPLEMENT_DYNAMIC(TestDialog, CDialogEx)

TestDialog::TestDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(TestDialog::IDD, pParent)
	, m_videoCapture(NULL)
	, m_currentFrame()
	, m_rectFrame()
	, m_stopThread(false)
	, m_isDrawing(false)
	, m_videoThread(NULL)
	, m_cursorChildCoord(_T("MOUSE COORDS"))
{
	  InitializeCriticalSection(&m_criticalSection);
}

TestDialog::~TestDialog()
{
	m_stopThread = true;
	
	if (m_videoThread && m_videoThread->m_hThread)
	{
		WaitForSingleObject(m_videoThread->m_hThread, INFINITE);
	}

	DeleteCriticalSection(&m_criticalSection);

	if (m_videoCapture.isOpened())
	{
		m_videoCapture.release();
	}

	m_rectFrame.release();
	m_currentFrame.release();

	cv::destroyAllWindows();
}

void TestDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_MOUSE_COORDS, m_cursorChildCoord);
}

BOOL TestDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	int newX = 0; 
	int newY = 0;

	RECT tempRect;
	GetWindowRect(&tempRect);

	SetWindowPos(
		this
		, newX
		, newY
		, tempRect.right - tempRect.left
		, tempRect.top - tempRect.bottom
		, SWP_NOSIZE | SWP_NOZORDER);

	if (m_videoCapture.isOpened())
	{
		m_videoCapture.release();
	}

	m_videoCapture = cv::VideoCapture(0);

	if (!m_videoCapture.isOpened()) 
	{
		AfxMessageBox(L"Не удалось открыть камеру!");

		m_videoCapture.release();

		return FALSE;
	}

	const unsigned int defaultFrameWidth = 1280;
	const unsigned int defaultFrameHeight = 1024;

	if (!m_videoCapture.set(cv::CAP_PROP_FRAME_WIDTH, defaultFrameWidth))
	{
		MessageBox(_T("Не удалось задать ширину кадра камеры!"));
		return FALSE;
	}

	if (!m_videoCapture.set(cv::CAP_PROP_FRAME_HEIGHT, defaultFrameHeight))
	{
		MessageBox(_T("Не удалось задать высоту кадра камеры!"));
		return FALSE;
	}

	AttachOpenCVWindowToMFC(GetSafeHwnd());

	cv::setMouseCallback(OPEN_CV_WINDOW_NAME, MouseCallback, this);

	m_videoThread = AfxBeginThread(VideoThread, this);
	
	if (m_videoThread == NULL)
	{
		MessageBox(_T("Не удалось создать поток!"));
		return FALSE;
	}

	return TRUE;
}

BEGIN_MESSAGE_MAP(TestDialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_SHOW_MESSAGE, &TestDialog::OnBnClickedButtonShowMessage)
	ON_BN_CLICKED(IDC_BUTTON_STOP_VIDEO, &TestDialog::OnBnClickedButtonStopVideo)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, &TestDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &TestDialog::OnBnClickedCancel)
END_MESSAGE_MAP()


// TestDialog message handlers


UINT TestDialog::VideoThread(LPVOID pParam) 
{
	TestDialog* dlg = static_cast<TestDialog*>(pParam);

	while (!dlg->m_stopThread && dlg->m_videoCapture.isOpened()) 
	{
		cv::Mat frame;

		dlg->m_videoCapture >> frame;

		if (frame.empty())
		{
			continue;
		}	

		EnterCriticalSection(&dlg->m_criticalSection);

		dlg->m_currentFrame = frame.clone();

		if (!dlg->m_isDrawing) 
		{
			dlg->m_rectFrame = frame.clone();
		}

		if (!dlg->m_rectFrame.empty()) 
		{
			cv::imshow(OPEN_CV_WINDOW_NAME, dlg->m_rectFrame);
		}

		LeaveCriticalSection(&dlg->m_criticalSection);
	}

	return 0;
}

void TestDialog::MouseCallback(int event, int x, int y, int flags, void* userdata) 
{
	TestDialog* dlg = static_cast<TestDialog*>(userdata);

	switch (event) 
	{
	case cv::EVENT_LBUTTONDOWN:
		dlg->m_isDrawing = true;
		dlg->m_startPoint = cv::Point(x, y);
		dlg->SetCursorCoords(x, y);
		break;

	case cv::EVENT_MOUSEMOVE:
		if (dlg->m_isDrawing) 
		{
			EnterCriticalSection(&dlg->m_criticalSection);

			if (!dlg->m_currentFrame.empty())
			{
				dlg->m_rectFrame = dlg->m_currentFrame.clone();

				setPointInImage(dlg->m_rectFrame.cols, dlg->m_rectFrame.rows, x, y);
				dlg->m_endPoint = cv::Point(x, y);
				cv::rectangle(dlg->m_rectFrame, dlg->m_startPoint, dlg->m_endPoint, cv::Scalar(0, 255, 0), 2);

				dlg->SetCursorCoords(x, y);
			}

			LeaveCriticalSection(&dlg->m_criticalSection);
		}
		break;

	case cv::EVENT_LBUTTONUP:
		dlg->m_isDrawing = false;

		EnterCriticalSection(&dlg->m_criticalSection);

		if (!dlg->m_currentFrame.empty())
		{
			dlg->m_rectFrame = dlg->m_currentFrame.clone();

			setPointInImage(dlg->m_rectFrame.cols, dlg->m_rectFrame.rows, x, y);
			dlg->m_endPoint = cv::Point(x, y);
			cv::rectangle(dlg->m_rectFrame, dlg->m_startPoint, dlg->m_endPoint, cv::Scalar(0, 255, 0), 2);

			dlg->SetCursorCoords(x, y);

			// Min задаёт верхний левый угол прямоугольника
			int x1 = std::min(dlg->m_startPoint.x, dlg->m_endPoint.x);
			int y1 = std::min(dlg->m_startPoint.y, dlg->m_endPoint.y);
			int x2 = std::max(dlg->m_startPoint.x, dlg->m_endPoint.x);
			int y2 = std::max(dlg->m_startPoint.y, dlg->m_endPoint.y);

			cv::Rect roiRect(x1, y1, x2 - x1, y2 - y1);
			cv::Mat imageRoi = dlg->m_currentFrame(roiRect);

			cv::imshow("ROI image", imageRoi);

			if (cv::imwrite("SelectedROI.jpg", imageRoi))
			{
				AfxMessageBox(L"Image has been saved!", MB_OK | MB_ICONINFORMATION, NULL);
			}
		}

		LeaveCriticalSection(&dlg->m_criticalSection);

		break;
	}
}

void TestDialog::CloneFrame(const cv::Mat& source, cv::Mat& destination)
{
	destination = source.clone();
}

void TestDialog::SetCursorCoords(int x, int y)
{
	m_cursorChildCoord.Format(_TEXT("Cursor coords: x: %d, y: %d"), x, y);
	SetDlgItemText(IDC_STATIC_MOUSE_COORDS, m_cursorChildCoord);
}

void TestDialog::OnBnClickedButtonShowMessage()
{
	MessageBox(_T("Тестовое сообщение"));
}

void TestDialog::OnBnClickedButtonStopVideo()
{
	m_videoCapture.release();
}

void TestDialog::OnDestroy()
{
	CDialogEx::OnDestroy();

	if (m_videoCapture.isOpened())
	{
		m_videoCapture.release();
	}
	Sleep(2000);
}

// FILTERS TO APPLY

//cv::Mat gray; 
//cv::Mat finalFiltered;

//cv::cvtColor(dlg->m_CurrentFrame, finalFiltered, cv::COLOR_BGR2GRAY);

//// CLAHE до размытия
//int tempLimit = 2;
//int tempWidth = 8;
//int tempHeight = 8;
//cv::Size tempSize(tempWidth, tempHeight);
//cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(tempLimit, tempSize);
//clahe->apply(finalFiltered, finalFiltered);

//clahe->clear();

//int tempKernelSize1 = 11;
//int tempKernelSize2 = 11;
//cv::Size kernelSize(tempKernelSize1 | 1, tempKernelSize2 | 1);
//cv::GaussianBlur(finalFiltered, finalFiltered, kernelSize, 1.5, 1.5);


//int tempCannyThreshold1 = 50;
//int tempCannyThreshold2 = 90;
//cv::Canny(finalFiltered, finalFiltered, tempCannyThreshold1, tempCannyThreshold2);

void TestDialog::OnBnClickedOk()
{
	OnDestroy();
	CDialogEx::OnOK();
}


void TestDialog::OnBnClickedCancel()
{
	OnDestroy();
	CDialogEx::OnCancel();
}
