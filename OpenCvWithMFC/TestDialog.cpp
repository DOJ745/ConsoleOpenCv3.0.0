// TestDialog.cpp : implementation file
//

#include "stdafx.h"
#include "OpenCvWithMFC.h"
#include "TestDialog.h"
#include "afxdialogex.h"
#include "OrbMatcher.h"
#include "AkazeMatcher.h"

// Space in name impacts the window style
const std::string OPEN_CV_WINDOW_NAME = "OpenCVWindow";

const char* IMG_ROI_NAME = "SelectedROI.jpg";
const char* IMG_COMPARE_FRAME_NAME = "CompareFrame.jpg";

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

		// Устанавливаем положение окна OpenCV относительно контейнера MFC
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
	, m_cannyThreshold1(50)
	, m_cannyThreshold2(90)
	, m_kernelSize1(7)
	, m_kernelSize2(7)
	, m_resizeFrameWidth(1280)
	, m_resizeFrameHeight(860)
	, m_claheClipLimit(2)
	, m_claheWidth(8)
	, m_claheHeight(8)
	, m_applyCanny(FALSE)
	, m_applyGaussian(FALSE)
	, m_applyClahe(FALSE)
	, m_makeGray(FALSE)
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

	//DeleteCriticalSection(&m_criticalSection);

	if (m_videoCapture.isOpened())
	{
		m_videoCapture.release();
	}

	m_rectFrame.release();
	m_currentFrame.release();

	cv::destroyAllWindows();

	DeleteCriticalSection(&m_criticalSection);
}

void TestDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_MOUSE_COORDS, m_cursorChildCoord);

	DDX_Text(pDX, IDC_EDIT_TEST_CANNY_THRESHOLD_1, m_cannyThreshold1);
	DDV_MinMaxInt(pDX, m_cannyThreshold1, 0, 200);
	DDX_Text(pDX, IDC_EDIT_TEST_CANNY_THRESHOLD_2, m_cannyThreshold2);
	DDV_MinMaxInt(pDX, m_cannyThreshold2, 0, 200);
	DDX_Text(pDX, IDC_EDIT_TEST_GAUSSIAN_KERNEL_SIZE_1, m_kernelSize1);
	DDV_MinMaxInt(pDX, m_kernelSize1, 1, 101);
	DDX_Text(pDX, IDC_EDIT_TEST_GAUSSIAN_KERNEL_SIZE_2, m_kernelSize2);
	DDV_MinMaxInt(pDX, m_kernelSize2, 1, 101);
	DDX_Text(pDX, IDC_EDIT_TEST_RESIZE_FRAME_WIDTH, m_resizeFrameWidth);
	DDV_MinMaxInt(pDX, m_resizeFrameWidth, 640, 1920);
	DDX_Text(pDX, IDC_EDIT_TEST_RESIZE_FRAME_HEIGHT, m_resizeFrameHeight);
	DDV_MinMaxInt(pDX, m_resizeFrameHeight, 480, 1080);
	DDX_Text(pDX, IDC_EDIT_TEST_CLAHE_CLIP_LIMIT, m_claheClipLimit);
	DDV_MinMaxDouble(pDX, m_claheClipLimit, 1, 40);
	DDX_Text(pDX, IDC_EDIT_TEST_CLAHE_WIDTH, m_claheWidth);
	DDV_MinMaxInt(pDX, m_claheWidth, 1, 100);
	DDX_Text(pDX, IDC_EDIT_TEST_CLAHE_HEIGHT, m_claheHeight);
	DDV_MinMaxInt(pDX, m_claheHeight, 1, 100);

	DDX_Check(pDX, IDC_CHECK_TEST_APPLY_CANNY, m_applyCanny);
	DDX_Check(pDX, IDC_CHECK_TEST_APPLY_GAUSSIAN, m_applyGaussian);
	DDX_Check(pDX, IDC_CHECK_TEST_APPLY_CLAHE, m_applyClahe);
	DDX_Check(pDX, IDC_CHECK_MAKE_GRAY, m_makeGray);
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
	ON_BN_CLICKED(IDC_BUTTON_STOP_VIDEO, &TestDialog::OnBnClickedButtonStopVideo)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, &TestDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &TestDialog::OnBnClickedCancel)

	ON_EN_UPDATE(IDC_EDIT_TEST_CANNY_THRESHOLD_1, &TestDialog::OnEnUpdateEditCannyThreshold1)
	ON_EN_UPDATE(IDC_EDIT_TEST_CANNY_THRESHOLD_2, &TestDialog::OnEnUpdateEditCannyThreshold2)
	ON_EN_UPDATE(IDC_EDIT_TEST_GAUSSIAN_KERNEL_SIZE_1, &TestDialog::OnEnUpdateEditGaussianKernelSize1)
	ON_EN_UPDATE(IDC_EDIT_TEST_GAUSSIAN_KERNEL_SIZE_2, &TestDialog::OnEnUpdateEditGaussianKernelSize2)
	ON_EN_UPDATE(IDC_EDIT_TEST_RESIZE_FRAME_WIDTH, &TestDialog::OnEnUpdateEditResizeFrameWidth)
	ON_EN_UPDATE(IDC_EDIT_TEST_RESIZE_FRAME_HEIGHT, &TestDialog::OnEnUpdateEditResizeFrameHeight)
	ON_EN_UPDATE(IDC_EDIT_TEST_CLAHE_CLIP_LIMIT, &TestDialog::OnEnUpdateEditClaheClipLimit)
	ON_EN_UPDATE(IDC_EDIT_TEST_CLAHE_WIDTH, &TestDialog::OnEnUpdateEditClaheWidth)
	ON_EN_UPDATE(IDC_EDIT_TEST_CLAHE_HEIGHT, &TestDialog::OnEnUpdateEditClaheHeight)
	ON_BN_CLICKED(IDC_CHECK_TEST_APPLY_CANNY, &TestDialog::OnBnClickedCheckApplyCanny)
	ON_BN_CLICKED(IDC_CHECK_TEST_APPLY_GAUSSIAN, &TestDialog::OnBnClickedCheckApplyGaussian)
	ON_BN_CLICKED(IDC_CHECK_TEST_APPLY_CLAHE, &TestDialog::OnBnClickedCheckApplyClahe)
	ON_BN_CLICKED(IDC_CHECK_MAKE_GRAY, &TestDialog::OnBnClickedCheckMakeGray)
	ON_BN_CLICKED(IDC_BUTTON_TEST_COMPARE_FRAMES, &TestDialog::OnBnClickedButtonTestCompareFrames)
END_MESSAGE_MAP()


// TestDialog message handlers


UINT TestDialog::VideoThread(LPVOID pParam) 
{
	TestDialog* dlg = static_cast<TestDialog*>(pParam);

	while (!dlg->m_stopThread && dlg->m_videoCapture.isOpened()) 
	{
		cv::Mat tempFrame;

		EnterCriticalSection(&dlg->m_criticalSection);
		dlg->m_videoCapture >> tempFrame;
		LeaveCriticalSection(&dlg->m_criticalSection);

		cv::Size targetSize(dlg->m_resizeFrameWidth, dlg->m_resizeFrameHeight);
		cv::Mat resizedFrame;

		if (tempFrame.empty())
		{
			continue;
		}	

		int frameWidth = tempFrame.cols;
		int frameHeight = tempFrame.rows; 

		double scaleWidth = static_cast<double>(targetSize.width) / frameWidth;
		double scaleHeight = static_cast<double>(targetSize.height) / frameHeight;
		double scale = std::min(scaleWidth, scaleHeight);

		// Новый размер изображения с сохранением пропорций
		int newWidth = static_cast<int>(frameWidth * scale);
		int newHeight = static_cast<int>(frameHeight * scale);

		cv::resize(tempFrame, resizedFrame, cv::Size(newWidth, newHeight), 0, 0, cv::INTER_LINEAR);

		// Создаем рамку (letterbox)
		cv::Mat letterboxFrame(targetSize, tempFrame.type(), cv::Scalar(255, 255, 0));

		int xOffset = (targetSize.width - newWidth) / 2;
		int yOffset = (targetSize.height - newHeight) / 2;

		// Вставляем масштабированное изображение в центр области
		resizedFrame.copyTo(letterboxFrame(cv::Rect(xOffset, yOffset, newWidth, newHeight)));

		// Обработка изображения
		// 1. Удаление шумов
		// 2. Выделение границ
		// 3. Коррекция яркости и контраста
		// 4. Нормализация (градации серого)

		cv::Mat gray; 
		cv::Mat finalFiltered;

		if (dlg->m_makeGray)
		{
			cv::cvtColor(letterboxFrame, finalFiltered, cv::COLOR_BGR2GRAY);
		}
		else
		{
		   cv::cvtColor(letterboxFrame, finalFiltered, cv::COLOR_BGR2RGB);
		}
		

		if (dlg->m_applyClahe && dlg->m_makeGray)
		{
			// CLAHE до размытия
			int tempLimit = dlg->m_claheClipLimit;
			cv::Size tempSize(dlg->m_claheWidth, dlg->m_claheHeight);
			cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(tempLimit, tempSize);
			clahe->apply(finalFiltered, finalFiltered);

			clahe->clear();
		}

		if (dlg->m_applyGaussian && dlg->m_makeGray)
		{
			cv::Size kernelSize(dlg->m_kernelSize1 | 1, dlg->m_kernelSize2 | 1);
			cv::GaussianBlur(finalFiltered, finalFiltered, kernelSize, 1.5, 1.5);
		}

		if (dlg->m_applyCanny && dlg->m_makeGray)
		{
			cv::Canny(finalFiltered, finalFiltered, dlg->m_cannyThreshold1, dlg->m_cannyThreshold2);
		}

		EnterCriticalSection(&dlg->m_criticalSection);

		dlg->m_currentFrame = finalFiltered.clone();

		if (!dlg->m_isDrawing) 
		{
			//dlg->m_rectFrame = finalFiltered.clone();
			dlg->m_rectFrame = dlg->m_currentFrame;
		}

		if (!dlg->m_rectFrame.empty()) 
		{
			cv::imshow(OPEN_CV_WINDOW_NAME, dlg->m_rectFrame);
		}

		LeaveCriticalSection(&dlg->m_criticalSection);

		CRect* rect = new CRect();
		CWnd* pWnd = dlg->GetDlgItem(IDC_OPENCV_CONTAINER);

		if (pWnd)
		{
			pWnd->GetWindowRect(rect);
			::ScreenToClient(pWnd->GetSafeHwnd(), (LPPOINT)rect);

			// Установка новых размеров
			rect->right = rect->left + letterboxFrame.cols;
			rect->bottom = rect->top + letterboxFrame.rows;
			pWnd->MoveWindow(rect);
		}

		// Найти окно OpenCV (контейнер вместе с элементом, где отображаются кадры)
		HWND hwndOpenCV = static_cast<HWND>(cvGetWindowHandle(OPEN_CV_WINDOW_NAME.c_str()));
		HWND parentHwndOpenCV = ::GetParent(hwndOpenCV);

		::SetWindowPos(parentHwndOpenCV
			, NULL
			, 0
			, 0
			, rect->Width()
			, rect->Height()
			, SWP_NOZORDER);

		delete rect;
		rect = nullptr;
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

			if (cv::imwrite(IMG_ROI_NAME, imageRoi))
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

void TestDialog::OnBnClickedButtonStopVideo()
{
	m_stopThread = true;
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

void TestDialog::OnEnUpdateEditCannyThreshold1()
{
	m_cannyThreshold1 = GetDlgItemInt(IDC_EDIT_TEST_CANNY_THRESHOLD_1);
}


void TestDialog::OnEnUpdateEditCannyThreshold2()
{
	m_cannyThreshold2 = GetDlgItemInt(IDC_EDIT_TEST_CANNY_THRESHOLD_2);
}


void TestDialog::OnEnUpdateEditGaussianKernelSize1()
{
	m_kernelSize1 = (GetDlgItemInt(IDC_EDIT_TEST_GAUSSIAN_KERNEL_SIZE_1) % 2 != 0) ? GetDlgItemInt(IDC_EDIT_TEST_GAUSSIAN_KERNEL_SIZE_1) : 1;
}


void TestDialog::OnEnUpdateEditGaussianKernelSize2()
{
	m_kernelSize2 = (GetDlgItemInt(IDC_EDIT_TEST_GAUSSIAN_KERNEL_SIZE_2) % 2 != 0) ? GetDlgItemInt(IDC_EDIT_TEST_GAUSSIAN_KERNEL_SIZE_2) : 1;
}

void TestDialog::OnEnUpdateEditResizeFrameWidth()
{
	m_resizeFrameWidth = GetDlgItemInt(IDC_EDIT_TEST_RESIZE_FRAME_WIDTH) < 640 ? 640 : GetDlgItemInt(IDC_EDIT_TEST_RESIZE_FRAME_WIDTH);
}


void TestDialog::OnEnUpdateEditResizeFrameHeight()
{
	m_resizeFrameHeight = GetDlgItemInt(IDC_EDIT_TEST_RESIZE_FRAME_HEIGHT) < 480 ? 480 : GetDlgItemInt(IDC_EDIT_TEST_RESIZE_FRAME_HEIGHT);
}

void TestDialog::OnEnUpdateEditClaheClipLimit()
{
	m_claheClipLimit = GetDlgItemInt(IDC_EDIT_TEST_CLAHE_CLIP_LIMIT);
}


void TestDialog::OnEnUpdateEditClaheWidth()
{
	m_claheWidth = GetDlgItemInt(IDC_EDIT_TEST_CLAHE_WIDTH);
}


void TestDialog::OnEnUpdateEditClaheHeight()
{
	m_claheHeight = GetDlgItemInt(IDC_EDIT_TEST_CLAHE_HEIGHT);
}

void TestDialog::OnBnClickedCheckApplyCanny()
{
	UpdateData(TRUE);
}


void TestDialog::OnBnClickedCheckApplyGaussian()
{
	UpdateData(TRUE);
}


void TestDialog::OnBnClickedCheckApplyClahe()
{
	UpdateData(TRUE);
}

void TestDialog::OnBnClickedCheckMakeGray()
{
	UpdateData(TRUE);
}


void TestDialog::OnBnClickedButtonTestCompareFrames()
{
	const double maxDistance = 50.0;
	std::string templatePath = IMG_ROI_NAME;
	std::string currentPath = IMG_COMPARE_FRAME_NAME;

	// Сохраняем текущий кадр для сравнения
	if (!cv::imwrite(IMG_COMPARE_FRAME_NAME, m_currentFrame))
	{
		return;
	}

	OrbMatcher orbMatcher(templatePath, currentPath, maxDistance);
	AkazeMatcher akazeMatcher(templatePath, currentPath, maxDistance);

	orbMatcher.performAll();
	akazeMatcher.performAll();

	for (;;)
	{
		int key = cv::waitKey(0);

		// Окно закрыто по любой клавише
		if (key >= 0) 
		{
			orbMatcher.destroyWindow();
			akazeMatcher.destroyWindow();
		}
	}
}
