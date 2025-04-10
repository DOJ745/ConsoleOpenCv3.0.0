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
	RECT clientParentRect;      // Размеры клиентской области окна MFC
	POINT clientOffsetTopLeft;	// Смещение клиентской области относительно окна

	// Получаем размеры клиентской области окна MFC
	::GetClientRect(hwndDialog, &clientParentRect);

	// Преобразуем верхний левый угол клиентской области в глобальные координаты
	::ClientToScreen(hwndDialog, &clientOffsetTopLeft);

	clientOffsetTopLeft.x = 0;
	clientOffsetTopLeft.y = 0;

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
		// Ограничиваем размеры окна OpenCV в пределах клиентской области MFC
		int maxWidth = clientParentRect.right - clientParentRect.left;
		int maxHeight = clientParentRect.bottom - clientParentRect.top;

		int minWidth = 1280;
		int minHeight = 1024;

		int width = std::min(minWidth, maxWidth);
		int height = std::min(minHeight, maxHeight);

		int offsetX = 0;
		int offsetY = 0;

		// Вычисляем позицию OpenCV окна относительно клиентской области
		int posX = clientOffsetTopLeft.x + offsetX;
		int posY = clientOffsetTopLeft.y + offsetY;

		// Устанавливаем окно OpenCV как дочернее для окна MFC
		//HWND prevParent = ::SetParent(parentHwndOpenCV, hwndDialog);
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
			, hwndDialog
			//, NULL
			, posX
			, posY
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
	, m_Capture(NULL)
	, m_CurrentFrame()
	, m_DrawFrame()
	, m_StopThread(false)
	, m_IsDrawing(false)
	, m_VideoThread(NULL)
	, m_CursorChildCoord(_T("MOUSE COORDS"))
{
	  InitializeCriticalSection(&m_CriticalSection);
}

TestDialog::~TestDialog()
{
	m_StopThread = true;
	
	if (m_VideoThread)
	{
		WaitForSingleObject(m_VideoThread->m_hThread, INFINITE);
	}

	DeleteCriticalSection(&m_CriticalSection);

	if (m_Capture.isOpened())
	{
		m_Capture.release();
	}

	cv::destroyAllWindows();
}

void TestDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_MOUSE_COORDS, m_CursorChildCoord);
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

	m_Capture = cv::VideoCapture(0);

	if (!m_Capture.isOpened()) 
	{
		AfxMessageBox(L"Не удалось открыть камеру!");

		m_Capture.release();

		return FALSE;
	}

	const unsigned int defaultFrameWidth = 1280;
	const unsigned int defaultFrameHeight = 1024;

	if (!m_Capture.set(cv::CAP_PROP_FRAME_WIDTH, defaultFrameWidth))
	{
		MessageBox(_T("Не удалось задать ширину кадра камеры!"));
		return FALSE;
	}

	if (!m_Capture.set(cv::CAP_PROP_FRAME_HEIGHT, defaultFrameHeight))
	{
		MessageBox(_T("Не удалось задать высоту кадра камеры!"));
		return FALSE;
	}

	AttachOpenCVWindowToMFC(GetSafeHwnd());

	cv::setMouseCallback(OPEN_CV_WINDOW_NAME, MouseCallback, this);

	m_VideoThread = AfxBeginThread(VideoThread, this);
	
	if (m_VideoThread == NULL)
	{
		MessageBox(_T("Не удалось создать поток!"));
		return FALSE;
	}

	return TRUE;
}

void TestDialog::OnDestroy()
{
	if (m_Capture.isOpened())
	{
		m_Capture.release();
	}
	CDialogEx::OnDestroy();
}

BEGIN_MESSAGE_MAP(TestDialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_SHOW_MESSAGE, &TestDialog::OnBnClickedButtonShowMessage)
	ON_BN_CLICKED(IDC_BUTTON_STOP_VIDEO, &TestDialog::OnBnClickedButtonStopVideo)
END_MESSAGE_MAP()


// TestDialog message handlers


UINT TestDialog::VideoThread(LPVOID pParam) 
{
	TestDialog* dlg = static_cast<TestDialog*>(pParam);

	while (!dlg->m_StopThread && dlg->m_Capture.isOpened()) 
	{
		cv::Mat frame;

		dlg->m_Capture >> frame;

		if (frame.empty())
		{
			continue;
		}	

		EnterCriticalSection(&dlg->m_CriticalSection);

		dlg->CloneFrame(frame, dlg->m_CurrentFrame);

		if (!dlg->m_IsDrawing) 
		{
			dlg->CloneFrame(frame, dlg->m_DrawFrame);
		}

		/*if (!dlg->m_DrawFrame.empty()) 
		{
			cv::imshow(OPEN_CV_WINDOW_NAME, dlg->m_DrawFrame);
		} */

		LeaveCriticalSection(&dlg->m_CriticalSection);

		if (!dlg->m_DrawFrame.empty()) 
		{
			cv::imshow(OPEN_CV_WINDOW_NAME, dlg->m_DrawFrame);
		}

		//if (cv::waitKey(1) == 27) 
		//{
		//	dlg->m_StopThread = true;
		//}
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
		dlg->SetCursorCoords(x, y);
		break;

	case cv::EVENT_MOUSEMOVE:
		if (dlg->m_IsDrawing) 
		{
			EnterCriticalSection(&dlg->m_CriticalSection);

			if (!dlg->m_CurrentFrame.empty())
			{
				dlg->CloneFrame(dlg->m_CurrentFrame, dlg->m_DrawFrame);
				setPointInImage(dlg->m_DrawFrame.cols, dlg->m_DrawFrame.rows, x, y);
				dlg->m_EndPoint = cv::Point(x, y);
				cv::rectangle(dlg->m_DrawFrame, dlg->m_StartPoint, dlg->m_EndPoint, cv::Scalar(0, 255, 0), 2);

				dlg->SetCursorCoords(x, y);
			}

			LeaveCriticalSection(&dlg->m_CriticalSection);
		}
		break;

	case cv::EVENT_LBUTTONUP:
		dlg->m_IsDrawing = false;

		EnterCriticalSection(&dlg->m_CriticalSection);

		if (!dlg->m_CurrentFrame.empty())
		{
			dlg->CloneFrame(dlg->m_CurrentFrame, dlg->m_DrawFrame);

			setPointInImage(dlg->m_DrawFrame.cols, dlg->m_DrawFrame.rows, x, y);
			dlg->m_EndPoint = cv::Point(x, y);
			cv::rectangle(dlg->m_DrawFrame, dlg->m_StartPoint, dlg->m_EndPoint, cv::Scalar(0, 255, 0), 2);

			dlg->SetCursorCoords(x, y);
		}

		if (cv::imwrite("CelectedArea.jpg", dlg->m_DrawFrame))
		{
			AfxMessageBox(L"Image has been saved!", MB_OK | MB_ICONINFORMATION, NULL);
		}

		LeaveCriticalSection(&dlg->m_CriticalSection);

		break;
	}
}

void TestDialog::CloneFrame(const cv::Mat& source, cv::Mat& destination)
{
	destination = source.clone();
}

void TestDialog::SetCursorCoords(int x, int y)
{
	m_CursorChildCoord.Format(_TEXT("Cursor coords: x: %d, y: %d"), x, y);
	SetDlgItemText(IDC_STATIC_MOUSE_COORDS, m_CursorChildCoord);
}

void TestDialog::OnBnClickedButtonShowMessage()
{
	MessageBox(_T("Тестовое сообщение"));
}

void TestDialog::OnBnClickedButtonStopVideo()
{
	m_Capture.release();
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