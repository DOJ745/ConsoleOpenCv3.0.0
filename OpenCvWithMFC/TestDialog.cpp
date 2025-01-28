// TestDialog.cpp : implementation file
//

#include "stdafx.h"
#include "OpenCvWithMFC.h"
#include "TestDialog.h"
#include "afxdialogex.h"

const std::string OPEN_CV_WINDOW_NAME = "OpenCV Window";

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

// ‘ункци€ дл€ прив€зки окна OpenCV к области MFC
void AttachOpenCVWindowToMFC(HWND hwndParent)
{	
	//cv::namedWindow(OPEN_CV_WINDOW_NAME, cv::WINDOW_AUTOSIZE);
	cv::namedWindow(OPEN_CV_WINDOW_NAME, cv::WINDOW_NORMAL);

	RECT clientParentRect;      // –азмеры клиентской области окна MFC
	POINT clientOffsetTopLeft;	// —мещение клиентской области относительно окна

	// ѕолучаем размеры клиентской области окна MFC
	::GetClientRect(hwndParent, &clientParentRect);

	// ѕреобразуем верхний левый угол клиентской области в глобальные координаты
	::ClientToScreen(hwndParent, &clientOffsetTopLeft);

	clientOffsetTopLeft.x = 0;
	clientOffsetTopLeft.y = 0;

	// Ќайти окно OpenCV
	HWND hwndOpenCV = static_cast<HWND>(cvGetWindowHandle(OPEN_CV_WINDOW_NAME.c_str()));
	HWND parentHwndOpenCV = ::GetParent(hwndOpenCV);

	if (parentHwndOpenCV != NULL)
	{
		::ShowWindow(parentHwndOpenCV, SW_HIDE);
	}

	if (hwndOpenCV) 
	{
		// ќграничиваем размеры окна OpenCV в пределах клиентской области MFC
		int maxWidth = clientParentRect.right - clientParentRect.left;
		int maxHeight = clientParentRect.bottom - clientParentRect.top;

		int minWidth = 800;
		int minHeight = 640;

		int width = std::min(minWidth, maxWidth);
		int height = std::min(minHeight, maxHeight);

		int offsetX = 0;
		int offsetY = 0;

		// ¬ычисл€ем позицию OpenCV окна относительно клиентской области
		int posX = clientOffsetTopLeft.x + offsetX;
		int posY = clientOffsetTopLeft.y + offsetY;

		// ”станавливаем окно OpenCV как дочернее дл€ окна MFC
		HWND prevParent = ::SetParent(hwndOpenCV, hwndParent);

		if (prevParent == NULL)
		{
			MessageBox(NULL
				, _TEXT("Ќе удалось задать родительское окно!")
				, _TEXT("ќшибка")
				, MB_OKCANCEL | MB_ICONERROR);
			return;
		}

		// ”бираем заголовок и предотвращаем перемещение
		LONG style = GetWindowLong(hwndOpenCV, GWL_STYLE);
		style &= ~(WS_POPUP | WS_BORDER);
		::SetWindowLongPtr(hwndOpenCV, GWL_STYLE, style);

		// ”станавливаем положение окна OpenCV относительно окна MFC
		::SetWindowPos(hwndOpenCV
			, NULL
			, posX
			, posY
			, width
			, height
			, SWP_NOZORDER | SWP_FRAMECHANGED);
	}
}

// TestDialog dialog

IMPLEMENT_DYNAMIC(TestDialog, CDialogEx)

TestDialog::TestDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(TestDialog::IDD, pParent)
	, m_Capture(NULL)
	, m_CurrentFrame(NULL)
	, m_DrawFrame(NULL)
	, m_StopThread(false)
	, m_IsDrawing(false)
	, m_MousePointerCoord(_T("MOUSE COORDS"))
{
	  InitializeCriticalSection(&m_CriticalSection);
}

TestDialog::~TestDialog()
{
	m_StopThread = true;

	WaitForSingleObject(m_VideoThread->m_hThread, INFINITE);

	//cv::destroyWindow(OPEN_CV_WINDOW_NAME);
	cv::destroyAllWindows();

	if (m_Capture) 
	{
		m_Capture->release();
	}

	if (m_CurrentFrame) 
	{
		delete m_CurrentFrame;
		m_CurrentFrame = nullptr;
	}

	if (m_DrawFrame) 
	{
		delete m_DrawFrame;
		m_DrawFrame = nullptr;
	}

	DeleteCriticalSection(&m_CriticalSection);
}

void TestDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_MOUSE_COORDS, m_MousePointerCoord);
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

	m_Capture = new cv::VideoCapture(0);

	if (!m_Capture->isOpened()) 
	{
		AfxMessageBox(L"Failed to open camera!");
		return FALSE;
	}

	const unsigned int defaultFrameWidth = 1280;
	const unsigned int defaultFrameHeight = 1024;

	if (!m_Capture->set(cv::CAP_PROP_FRAME_WIDTH, defaultFrameWidth))
	{
		MessageBox(_T("Ќе удалось задать ширину кадра камеры!"));
		return FALSE;
	}

	if (!m_Capture->set(cv::CAP_PROP_FRAME_HEIGHT, defaultFrameHeight))
	{
		MessageBox(_T("Ќе удалось задать высоту кадра камеры!"));
		return FALSE;
	}

	m_CurrentFrame = new cv::Mat();
	m_DrawFrame = new cv::Mat();

	AttachOpenCVWindowToMFC(GetSafeHwnd());

	cv::setMouseCallback(OPEN_CV_WINDOW_NAME, MouseCallback, this);

	m_VideoThread = AfxBeginThread(VideoThread, this);

	return TRUE;
}

void TestDialog::OnDestroy()
{
	CDialogEx::OnDestroy();
}

BEGIN_MESSAGE_MAP(TestDialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_SHOW_MESSAGE, &TestDialog::OnBnClickedButtonShowMessage)
END_MESSAGE_MAP()


// TestDialog message handlers


UINT TestDialog::VideoThread(LPVOID pParam) 
{
	TestDialog* dlg = static_cast<TestDialog*>(pParam);

	while (!dlg->m_StopThread && dlg->m_Capture->isOpened()) 
	{
		cv::Mat frame;

		*(dlg->m_Capture) >> frame;

		if (frame.empty())
		{
			continue;
		}	

		EnterCriticalSection(&dlg->m_CriticalSection);
		dlg->safeCloneFrame(frame, dlg->m_CurrentFrame);

		if (!dlg->m_IsDrawing) 
		{
			dlg->safeCloneFrame(frame, dlg->m_DrawFrame);
		}

		if (!dlg->m_DrawFrame->empty()) 
		{
			cv::imshow(OPEN_CV_WINDOW_NAME, *(dlg->m_DrawFrame));
		}

		LeaveCriticalSection(&dlg->m_CriticalSection);

		if (cv::waitKey(1) == 27) 
		{
			dlg->m_StopThread = true;
		}
	}

	return 0;
}

void TestDialog::MouseCallback(int event, int x, int y, int flags, void* userdata) 
{
	TestDialog* dlg = static_cast<TestDialog*>(userdata);

	switch (event) 
	{
	case cv::EVENT_LBUTTONDOWN:
		dlg->m_IsDrawing = true;
		dlg->m_StartPoint = cv::Point(x, y);
		dlg->SetMouseCoords(x, y);
		break;

	case cv::EVENT_MOUSEMOVE:
		if (dlg->m_IsDrawing) 
		{
			EnterCriticalSection(&dlg->m_CriticalSection);

			if (!dlg->m_CurrentFrame->empty()) 
			{
				dlg->safeCloneFrame(*(dlg->m_CurrentFrame), dlg->m_DrawFrame);
				setPointInImage(dlg->m_DrawFrame->cols, dlg->m_DrawFrame->rows, x, y);
				dlg->m_EndPoint = cv::Point(x, y);
				cv::rectangle(*(dlg->m_DrawFrame), dlg->m_StartPoint, dlg->m_EndPoint, cv::Scalar(0, 255, 0), 2);

				dlg->SetMouseCoords(x, y);
			}

			LeaveCriticalSection(&dlg->m_CriticalSection);
		}
		break;

	case cv::EVENT_LBUTTONUP:
		dlg->m_IsDrawing = false;

		EnterCriticalSection(&dlg->m_CriticalSection);

		if (!dlg->m_CurrentFrame->empty()) 
		{
			dlg->safeCloneFrame(*(dlg->m_CurrentFrame), dlg->m_DrawFrame);
			setPointInImage(dlg->m_DrawFrame->cols, dlg->m_DrawFrame->rows, x, y);
			dlg->m_EndPoint = cv::Point(x, y);
			cv::rectangle(*(dlg->m_DrawFrame), dlg->m_StartPoint, dlg->m_EndPoint, cv::Scalar(0, 255, 0), 2);

			dlg->SetMouseCoords(x, y);
		}

		LeaveCriticalSection(&dlg->m_CriticalSection);
		break;
	}
}

void TestDialog::safeCloneFrame(cv::Mat& source, cv::Mat*& destination) 
{
	if (destination) 
	{
		delete destination;
	}

	destination = new cv::Mat(source.clone());
}

void TestDialog::SetMouseCoords(int x, int y)
{
	m_MousePointerCoord.Format(_TEXT("Mouse coord x: %d, y: %d"), x, y);
	SetDlgItemText(IDC_STATIC_MOUSE_COORDS, m_MousePointerCoord);
}

void TestDialog::OnBnClickedButtonShowMessage()
{
	MessageBox(_T("“естовое сообщение"));
}