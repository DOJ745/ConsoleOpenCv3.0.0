
// OpenCvWithMFCDlg.cpp : implementation file
//

#include "stdafx.h"
#include "OpenCvWithMFC.h"
#include "OpenCvWithMFCDlg.h"
#include "afxdialogex.h"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <windows.h>
#include "TestDialog.h"
#include <afxwin.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

cv::VideoCapture CAP; // Глобальный объект захвата камеры
cv::Mat FRAME;		  // Исходный кадр камеры
cv::Mat EDGES;		  // Обработанный кадр камеры
cv::Mat SAVED_FRAME;

const char* COMPARE_IMG = "compare.jpg";
const char* ETHALON_IMG = "ethalon.jpg";

const int DEFAULT_CAMERA_FRAME_WIDTH = 1280;
const int DEFAULT_CAMERA_FRAME_HEIGHT = 1024;

UINT_PTR TIMER_ID = 1;
UINT TIMER_INTERVAL_MS = 1;

volatile bool stopThread = false; // Флаг для остановки потока

void DrawFrameToPictureControl(CWnd* pWnd, const cv::Mat& img)
{
	if (img.empty())
	{
		return;
	}

	CWnd* pPictureCtrl = pWnd->GetDlgItem(IDC_PICTURE_CONTROL_CAMERA);

	if (!pPictureCtrl)
	{
		return;
	}

	CRect rect;
	pPictureCtrl->GetClientRect(&rect);

	// Преобразуем изображение в формат BGR24 (если требуется)
	cv::Mat bgrImage;

	if (img.channels() == 1)
	{
		cv::cvtColor(img, bgrImage, cv::COLOR_GRAY2RGB);
	}
	else
	{
		bgrImage = img;
	}

	// Получаем контекст устройства (Picture Control)
	HDC hdc = ::GetDC(pPictureCtrl->m_hWnd);

	BITMAPINFO bInfo;
	memset(&bInfo, 0, sizeof(BITMAPINFO));
	bInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bInfo.bmiHeader.biWidth = bgrImage.cols;
	bInfo.bmiHeader.biHeight = -bgrImage.rows; // Используется отрицательная высота, чтобы изображение не переворачивалось вверх ногами
	bInfo.bmiHeader.biPlanes = 1;			   // Количество цветовых плоскостей
	bInfo.bmiHeader.biBitCount = 24;           // 24 бита на пиксель (3 байта на пиксель, BGR)
	bInfo.bmiHeader.biCompression = BI_RGB;    // Без компрессии

	::SetDIBitsToDevice(
		hdc,
		0, 0,                               // Координаты верхнего левого угла
		rect.Width(), rect.Height(),        // Размеры области отрисовки
		0, 0,                               // Начальные координаты данных изображения
		0, bgrImage.rows,                   // Число строк данных изображения
		bgrImage.data,                      // Указатель на массив пикселей
		&bInfo,                             // Информация о формате изображения
		DIB_RGB_COLORS);                    // Интерпретация цветов (RGB)

	::ReleaseDC(pPictureCtrl->m_hWnd, hdc);
}

// Потоковая функция для захвата кадров
UINT CameraCaptureThread(LPVOID pParam)
{
	COpenCvWithMFCDlg* dlg = static_cast<COpenCvWithMFCDlg*>(pParam);
	CWnd* pWnd = dlg;
	cv::Mat resizedFrame;
	bool isResized = false;

	while (!stopThread)
	{
		cv::Size targetSize(dlg->m_ResizeFrameWidth, dlg->m_ResizeFrameHeight);

		CAP >> FRAME;

		if (FRAME.empty())
		{
			continue;
		}

		int frameWidth = FRAME.cols;
		int frameHeight = FRAME.rows; 

		double scaleWidth = static_cast<double>(targetSize.width) / frameWidth;
		double scaleHeight = static_cast<double>(targetSize.height) / frameHeight;
		double scale = std::min(scaleWidth, scaleHeight);

		// Новый размер изображения с сохранением пропорций
		int newWidth = static_cast<int>(frameWidth * scale);
		int newHeight = static_cast<int>(frameHeight * scale);

		cv::resize(FRAME, resizedFrame, cv::Size(newWidth, newHeight), 0, 0, cv::INTER_LINEAR);

		// Создаем рамку (letterbox)
		cv::Mat letterboxFrame(targetSize, FRAME.type(), cv::Scalar(0, 255, 255));
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

		cv::cvtColor(letterboxFrame, finalFiltered, cv::COLOR_BGR2GRAY);

		if (dlg->m_applyClahe)
		{
			// CLAHE до размытия
			int tempLimit = dlg->m_ClaheClipLimit;
			cv::Size tempSize(dlg->m_ClaheWidth, dlg->m_ClaheHeight);
			cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(tempLimit, tempSize);
			clahe->apply(finalFiltered, finalFiltered);

			clahe->clear();
		}

		if (dlg->m_applyGaussian)
		{
			cv::Size kernelSize(dlg->m_KernelSize1 | 1, dlg->m_KernelSize2 | 1);
			cv::GaussianBlur(finalFiltered, finalFiltered, kernelSize, 1.5, 1.5);
		}

		if (dlg->m_applyCanny)
		{
			cv::Canny(finalFiltered, finalFiltered, dlg->m_CannyThreshold1, dlg->m_CannyThreshold2);
		}

		//// Рисование перекрестья
		//const int lineOffset = 45;
		//int centerX = letterboxFrame.cols / 2;
		//int centerY = letterboxFrame.rows / 2;
		//cv::Scalar crossColor(0, 0, 255);		// Цвет перекрестья: красный (BGR)
		//int thickness = 2;

		//// Горизонтальная линия
		//cv::line(
		//	letterboxFrame
		//	, cv::Point(lineOffset, centerY)
		//	, cv::Point(letterboxFrame.cols - lineOffset, centerY)
		//	, crossColor
		//	, thickness);	

		//// Вертикальная линия
		//cv::line(
		//	letterboxFrame
		//	, cv::Point(centerX, lineOffset)
		//	, cv::Point(centerX, letterboxFrame.rows - lineOffset)
		//	, crossColor
		//	, thickness);	

		// Изменение размеров PictureControl под отмасштабированный кадр
		CRect* rect = new CRect();
		CWnd* pWnd = dlg->GetDlgItem(IDC_PICTURE_CONTROL_CAMERA);

		if (pWnd)
		{
			pWnd->GetWindowRect(rect);
			ScreenToClient(pWnd->GetSafeHwnd(), (LPPOINT)rect);

			// Установка новых размеров
			rect->right = rect->left + letterboxFrame.cols;
			rect->bottom = rect->top + letterboxFrame.rows;
			pWnd->MoveWindow(rect);
		}

		delete rect;
		rect = nullptr;

		SAVED_FRAME = finalFiltered;

		DrawFrameToPictureControl(dlg, finalFiltered);
	}

	return 0;
}


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// COpenCvWithMFCDlg dialog

COpenCvWithMFCDlg::COpenCvWithMFCDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(COpenCvWithMFCDlg::IDD, pParent)
	, m_CannyThreshold1(50)
	, m_CannyThreshold2(90)
	, m_KernelSize1(7)
	, m_KernelSize2(7)
	, m_ResizeFrameWidth(800)
	, m_ResizeFrameHeight(600)
	, m_ClaheClipLimit(2)
	, m_ClaheWidth(8)
	, m_ClaheHeight(8)
	, m_testDDXInt(0)
	, m_applyCanny(FALSE)
	, m_applyGaussian(FALSE)
	, m_applyClahe(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COpenCvWithMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CANNY_THRESHOLD_1, m_CannyThreshold1);
	DDV_MinMaxInt(pDX, m_CannyThreshold1, 0, 200);
	DDX_Text(pDX, IDC_EDIT_CANNY_THRESHOLD_2, m_CannyThreshold2);
	DDV_MinMaxInt(pDX, m_CannyThreshold2, 0, 200);
	DDX_Text(pDX, IDC_EDIT_GAUSSIAN_KERNEL_SIZE_1, m_KernelSize1);
	DDV_MinMaxInt(pDX, m_KernelSize1, 1, 101);
	DDX_Text(pDX, IDC_EDIT_GAUSSIAN_KERNEL_SIZE_2, m_KernelSize2);
	DDV_MinMaxInt(pDX, m_KernelSize2, 1, 101);
	DDX_Text(pDX, IDC_EDIT_RESIZE_FRAME_WIDTH, m_ResizeFrameWidth);
	DDV_MinMaxInt(pDX, m_ResizeFrameWidth, 640, 1920);
	DDX_Text(pDX, IDC_EDIT_RESIZE_FRAME_HEIGHT, m_ResizeFrameHeight);
	DDV_MinMaxInt(pDX, m_ResizeFrameHeight, 480, 1080);
	DDX_Text(pDX, IDC_EDIT_CLAHE_CLIP_LIMIT, m_ClaheClipLimit);
	DDV_MinMaxDouble(pDX, m_ClaheClipLimit, 1, 40);
	DDX_Text(pDX, IDC_EDIT_CLAHE_WIDTH, m_ClaheWidth);
	DDV_MinMaxInt(pDX, m_ClaheWidth, 1, 100);
	DDX_Text(pDX, IDC_EDIT_CLAHE_HEIGHT, m_ClaheHeight);
	DDV_MinMaxInt(pDX, m_ClaheHeight, 1, 100);
	DDX_Text(pDX, IDC_EDIT15, m_testDDXInt);
	DDV_MinMaxInt(pDX, m_testDDXInt, 1, 10);
	DDX_Check(pDX, IDC_CHECK_APPLY_CANNY, m_applyCanny);
	DDX_Check(pDX, IDC_CHECK_APPLY_GAUSSIAN, m_applyGaussian);
	DDX_Check(pDX, IDC_CHECK_APPLY_CLAHE, m_applyClahe);

	DDX_Control(pDX, IDC_PICTURE_CONTROL_CAMERA, m_cameraPictureControl);
}

BEGIN_MESSAGE_MAP(COpenCvWithMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START, &COpenCvWithMFCDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &COpenCvWithMFCDlg::OnBnClickedButtonStop)
	ON_WM_TIMER()
	ON_MESSAGE(WM_USER + 1, &COpenCvWithMFCDlg::OnUpdatePicture)
	ON_EN_UPDATE(IDC_EDIT_CANNY_THRESHOLD_1, &COpenCvWithMFCDlg::OnEnUpdateEditCannyThreshold1)
	ON_EN_UPDATE(IDC_EDIT_CANNY_THRESHOLD_2, &COpenCvWithMFCDlg::OnEnUpdateEditCannyThreshold2)
	ON_EN_UPDATE(IDC_EDIT_GAUSSIAN_KERNEL_SIZE_1, &COpenCvWithMFCDlg::OnEnUpdateEditGaussianKernelSize1)
	ON_EN_UPDATE(IDC_EDIT_GAUSSIAN_KERNEL_SIZE_2, &COpenCvWithMFCDlg::OnEnUpdateEditGaussianKernelSize2)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_PICTURE, &COpenCvWithMFCDlg::OnBnClickedButtonSavePicture)
	ON_BN_CLICKED(IDC_BUTTON_COMPARE_FRAME, &COpenCvWithMFCDlg::OnBnClickedButtonCompareFrame)
	ON_EN_UPDATE(IDC_EDIT_RESIZE_FRAME_WIDTH, &COpenCvWithMFCDlg::OnEnUpdateEditResizeFrameWidth)
	ON_EN_UPDATE(IDC_EDIT_RESIZE_FRAME_HEIGHT, &COpenCvWithMFCDlg::OnEnUpdateEditResizeFrameHeight)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_DIALOG, &COpenCvWithMFCDlg::OnBnClickedButtonOpenDialog)
	ON_EN_UPDATE(IDC_EDIT_CLAHE_CLIP_LIMIT, &COpenCvWithMFCDlg::OnEnUpdateEditClaheClipLimit)
	ON_EN_UPDATE(IDC_EDIT_CLAHE_WIDTH, &COpenCvWithMFCDlg::OnEnUpdateEditClaheWidth)
	ON_EN_UPDATE(IDC_EDIT_CLAHE_HEIGHT, &COpenCvWithMFCDlg::OnEnUpdateEditClaheHeight)
	ON_EN_KILLFOCUS(IDC_EDIT15, &COpenCvWithMFCDlg::OnEnKillfocusEdit15)
	ON_BN_CLICKED(IDC_CHECK_APPLY_CANNY, &COpenCvWithMFCDlg::OnBnClickedCheckApplyCanny)
	ON_BN_CLICKED(IDC_CHECK_APPLY_GAUSSIAN, &COpenCvWithMFCDlg::OnBnClickedCheckApplyGaussian)
	ON_BN_CLICKED(IDC_CHECK_APPLY_CLAHE, &COpenCvWithMFCDlg::OnBnClickedCheckApplyClahe)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_PICTURE, &COpenCvWithMFCDlg::OnBnClickedButtonClearPicture)
END_MESSAGE_MAP()


// COpenCvWithMFCDlg message handlers

BOOL COpenCvWithMFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	SetDlgItemInt(IDC_EDIT_CANNY_THRESHOLD_1, m_CannyThreshold1);
	SetDlgItemInt(IDC_EDIT_CANNY_THRESHOLD_2, m_CannyThreshold2);
	SetDlgItemInt(IDC_EDIT_GAUSSIAN_KERNEL_SIZE_1, m_KernelSize1);
	SetDlgItemInt(IDC_EDIT_GAUSSIAN_KERNEL_SIZE_2, m_KernelSize2);
	SetDlgItemInt(IDC_EDIT_RESIZE_FRAME_WIDTH, m_ResizeFrameWidth);
	SetDlgItemInt(IDC_EDIT_RESIZE_FRAME_HEIGHT, m_ResizeFrameHeight);
	SetDlgItemInt(IDC_EDIT_CLAHE_CLIP_LIMIT, m_ClaheClipLimit);
	SetDlgItemInt(IDC_EDIT_CLAHE_WIDTH, m_ClaheWidth);
	SetDlgItemInt(IDC_EDIT_CLAHE_HEIGHT, m_ClaheHeight);
	SetDlgItemInt(IDC_EDIT15, 5);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void COpenCvWithMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void COpenCvWithMFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR COpenCvWithMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void COpenCvWithMFCDlg::OnBnClickedButtonStart()
{
	if (!CAP.isOpened())
	{
		CAP.open(0);
	}

	if (!CAP.isOpened())
	{
		MessageBox(_T("Не удалось открыть камеру!"));
		return;
	}


	if (!CAP.set(cv::CAP_PROP_FRAME_WIDTH, DEFAULT_CAMERA_FRAME_WIDTH))
	{
		MessageBox(_T("Не удалось задать ширину кадра камеры!"));
		return;
	}

	if (!CAP.set(cv::CAP_PROP_FRAME_HEIGHT, DEFAULT_CAMERA_FRAME_HEIGHT))
	{
		MessageBox(_T("Не удалось задать высоту кадра камеры!"));
		return;
	}
	
	stopThread = false;
	AfxBeginThread(CameraCaptureThread, this);
}


void COpenCvWithMFCDlg::OnBnClickedButtonStop()
{
	/*KillTimer(TIMER_ID); // Останавливаем таймер
	cap.release();       // Освобождаем камеру*/

	stopThread = true;
	CAP.release();     
}


void COpenCvWithMFCDlg::OnTimer(UINT_PTR nIDEvent)
{

}

afx_msg LRESULT COpenCvWithMFCDlg::OnUpdatePicture(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

void COpenCvWithMFCDlg::OnEnUpdateEditCannyThreshold1()
{
	m_CannyThreshold1 = GetDlgItemInt(IDC_EDIT_CANNY_THRESHOLD_1);
}


void COpenCvWithMFCDlg::OnEnUpdateEditCannyThreshold2()
{
	m_CannyThreshold2 = GetDlgItemInt(IDC_EDIT_CANNY_THRESHOLD_2);
}


void COpenCvWithMFCDlg::OnEnUpdateEditGaussianKernelSize1()
{
	m_KernelSize1 = (GetDlgItemInt(IDC_EDIT_GAUSSIAN_KERNEL_SIZE_1) % 2 != 0) ? GetDlgItemInt(IDC_EDIT_GAUSSIAN_KERNEL_SIZE_1) : 1;
}


void COpenCvWithMFCDlg::OnEnUpdateEditGaussianKernelSize2()
{
	m_KernelSize2 = (GetDlgItemInt(IDC_EDIT_GAUSSIAN_KERNEL_SIZE_2) % 2 != 0) ? GetDlgItemInt(IDC_EDIT_GAUSSIAN_KERNEL_SIZE_2) : 1;
}


void COpenCvWithMFCDlg::OnBnClickedButtonSavePicture()
{
	if(cv::imwrite(COMPARE_IMG, SAVED_FRAME))
	{
		MessageBox(_T("Изображение сохранено!"), _T("Сохранение"));
	}
}


void COpenCvWithMFCDlg::OnBnClickedButtonCompareFrame()
{
	// TODO: compare with ethalon, which is saved by ROI select

	int keypoints = 500;
	float scaleFactor = 1.2f;
	int pyramidLevels = 10;
	int windowSize = 10;
	int wtakPoints = 2;
	int patchSize = 18;
	int fastThreshold = 50;

	cv::Ptr<cv::ORB> orb = cv::ORB::create(
		        		keypoints,			        // Максимальное количество ключевых точек
		        		1.2f,						// Масштабный фактор пирамиды (Этот параметр определяет, насколько изображение уменьшается на каждом уровне пирамиды - уменьшение ускоряет обработку)
		        		scaleFactor,				// Количество уровней пирамиды	(Этот параметр определяет глубину пирамиды. Чем больше уровней, тем больше требуется вычислений)
		        		pyramidLevels,				// Размер окна	(Этот параметр задаёт размер области для поиска ключевых точек. Большие значения полезны для больших изображений)
		        		0,							// Первый уровень пирамиды
		        		wtakPoints,					// WTA_K - параметр определяет количество точек в окрестности ключевой точки, которые сравниваются для генерации одного бита дескриптора
		        		cv::ORB::FAST_SCORE,		// Метод оценки
		        		patchSize,					// Радиус граничной области	(Этот параметр определяет размер патча вокруг каждой ключевой точки, используемого для вычисления дескрипторов - чем больше, тем больше обрабатывается деталей)
		        		fastThreshold				// Порог отклика -  это численная величина, характеризующая "выразительность" точки
		        	);
		    

	cv::Ptr<cv::AKAZE> akaze = cv::AKAZE::create();
	akaze->setThreshold(0.001f);                            // Уровень порога для подавления слабых ключевых точек
	akaze->setNOctaves(4);                                  // Количество октав (уровней пирамиды)
	akaze->setNOctaveLayers(4);                             // Количество слоёв в каждой октаве
	akaze->setDescriptorSize(0);                            // Размер дескриптора (по умолчанию максимальный)
	akaze->setDescriptorType(cv::AKAZE::DESCRIPTOR_MLDB);   // Тип дескриптора
}


void COpenCvWithMFCDlg::OnEnUpdateEditResizeFrameWidth()
{
	m_ResizeFrameWidth = GetDlgItemInt(IDC_EDIT_RESIZE_FRAME_WIDTH) < 640 ? 640 : GetDlgItemInt(IDC_EDIT_RESIZE_FRAME_WIDTH);
}


void COpenCvWithMFCDlg::OnEnUpdateEditResizeFrameHeight()
{
	m_ResizeFrameHeight = GetDlgItemInt(IDC_EDIT_RESIZE_FRAME_HEIGHT) < 480 ? 480 : GetDlgItemInt(IDC_EDIT_RESIZE_FRAME_HEIGHT);
}

void COpenCvWithMFCDlg::OnBnClickedButtonOpenDialog()
{
	TestDialog dlg;
	dlg.DoModal();
}

void COpenCvWithMFCDlg::OnEnUpdateEditClaheClipLimit()
{
	m_ClaheClipLimit = GetDlgItemInt(IDC_EDIT_CLAHE_CLIP_LIMIT);
}


void COpenCvWithMFCDlg::OnEnUpdateEditClaheWidth()
{
	m_ClaheWidth = GetDlgItemInt(IDC_EDIT_CLAHE_WIDTH);
}


void COpenCvWithMFCDlg::OnEnUpdateEditClaheHeight()
{
	m_ClaheHeight = GetDlgItemInt(IDC_EDIT_CLAHE_HEIGHT);
}

// По умолчанию проверка на значение работатет по нажатию на Enter. Для запуска события необходимо запустить UpdateData(TRUE) 
void COpenCvWithMFCDlg::OnEnKillfocusEdit15()
{
	UpdateData(TRUE);
}


void COpenCvWithMFCDlg::OnBnClickedCheckApplyCanny()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}


void COpenCvWithMFCDlg::OnBnClickedCheckApplyGaussian()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}


void COpenCvWithMFCDlg::OnBnClickedCheckApplyClahe()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}


void COpenCvWithMFCDlg::OnBnClickedButtonClearPicture()
{
	m_cameraPictureControl.ClearControl();
}
