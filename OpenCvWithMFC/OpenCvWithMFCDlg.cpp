
// OpenCvWithMFCDlg.cpp : implementation file
//

#include "stdafx.h"
#include "OpenCvWithMFC.h"
#include "OpenCvWithMFCDlg.h"
#include "afxdialogex.h"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <afxwin.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

cv::VideoCapture cap; // Глобальный объект захвата камеры
cv::Mat frame;
cv::Mat edges;

UINT_PTR TIMER_ID = 1;
UINT TIMER_INTERVAL_MS = 1;
volatile bool stopThread = false; // Флаг для остановки потока

/*HBITMAP MatToHBITMAP(const cv::Mat& mat)
{
	// Проверяем, что изображение действительно существует
	if (mat.empty()) 
	{
		return nullptr;
	}

	// Преобразуем изображение в формат с 3 каналами (BGR)
	cv::Mat temp;

	if (mat.channels() == 1)
	{
		cv::cvtColor(mat, temp, cv::COLOR_GRAY2BGR);
	}
	else
	{
		temp = mat.clone();
	}

	// Создаем Bitmap-объект
	BITMAPINFO bInfo;
	memset(&bInfo, 0, sizeof(BITMAPINFO));
	bInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bInfo.bmiHeader.biWidth = temp.cols;
	bInfo.bmiHeader.biHeight = -temp.rows;		// Отрицательная высота для переворота изображения
	bInfo.bmiHeader.biPlanes = 1;
	bInfo.bmiHeader.biBitCount = 24;			// 24 бита на пиксель
	bInfo.bmiHeader.biCompression = BI_RGB;

	void* data = nullptr;
	HBITMAP hBitmap = CreateDIBSection(nullptr, &bInfo, DIB_RGB_COLORS, &data, nullptr, 0);

	if (!hBitmap)
	{
		return nullptr;
	}

	// Копируем пиксели
	memcpy(data, temp.data, temp.total() * temp.elemSize());
	return hBitmap;
} */

// Отображение изображения в Picture Control
/*void DisplayImageInPictureControl(CWnd* pWnd, const cv::Mat& mat)
{
	// Получаем HBITMAP
	HBITMAP hBitmap = MatToHBITMAP(mat);

	if (!hBitmap)
	{
		return;
	}

	// Получаем указатель на Picture Control
	CStatic* pPictureControl = (CStatic*)pWnd->GetDlgItem(IDC_PICTURE_CONTROL);

	if (pPictureControl)
	{
		// Устанавливаем изображение
		pPictureControl->SetBitmap(hBitmap);

		// Удаляем предыдущее изображение
		DeleteObject(hBitmap);
	}
}*/

void DrawFrameToPictureControl(CWnd* pWnd, const cv::Mat& img)
{
	if (img.empty())
	{
		return;
	}

	CWnd* pPictureCtrl = pWnd->GetDlgItem(IDC_PICTURE_CONTROL);

	if (!pPictureCtrl)
	{
		return;
	}

	// Получаем размеры Picture Control
	CRect rect;
	pPictureCtrl->GetClientRect(&rect);

	// Преобразуем изображение в формат BGR24 (если требуется)
	cv::Mat bgrImage;

	if (img.channels() == 1)
	{
		cv::cvtColor(img, bgrImage, cv::COLOR_GRAY2BGR);
	}
	else
	{
		bgrImage = img;
	}

	// Получаем HDC для Picture Control
	HDC hdc = ::GetDC(pPictureCtrl->m_hWnd);

	// Настраиваем BITMAPINFO
	BITMAPINFO bInfo;
	memset(&bInfo, 0, sizeof(BITMAPINFO));
	bInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bInfo.bmiHeader.biWidth = bgrImage.cols;
	bInfo.bmiHeader.biHeight = -bgrImage.rows; // Отрицательная высота для переворота
	bInfo.bmiHeader.biPlanes = 1;
	bInfo.bmiHeader.biBitCount = 24;           // 24 бита на пиксель
	bInfo.bmiHeader.biCompression = BI_RGB;    // Без компрессии

	// Отрисовка в контексте устройства
	::SetDIBitsToDevice(
		hdc,
		0, 0,                               // Координаты верхнего левого угла
		rect.Width(), rect.Height(),        // Размеры изображения
		0, 0,                               // Начальные координаты
		0, bgrImage.rows,                   // Линии для вывода
		bgrImage.data,                      // Данные изображения
		&bInfo,
		DIB_RGB_COLORS);

	::ReleaseDC(pPictureCtrl->m_hWnd, hdc); // Освобождаем HDC
}

// Потоковая функция для захвата кадров
UINT CameraCaptureThread(LPVOID pParam)
{
	COpenCvWithMFCDlg* dlg = (COpenCvWithMFCDlg*)pParam;
	CWnd* pWnd = dlg;

	while (!stopThread)
	{
		cap >> frame;

		if (frame.empty())
		{
			continue;
		}

		// Обработка изображения
		cv::cvtColor(frame, edges, cv::COLOR_BGR2GRAY);
		cv::GaussianBlur(edges, edges, cv::Size(dlg->m_KernelSize1, dlg->m_KernelSize2), 1.5, 1.5);
		cv::Canny(edges, edges, dlg->m_CannyThreshold1, dlg->m_CannyThreshold2);

		DrawFrameToPictureControl(pWnd, edges);
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
	/*if (!cap.isOpened())
	{
		cap.open(0);
	}

	if (!cap.isOpened())
	{
		MessageBox(_T("Не удалось открыть камеру!"));
		return;
	}

	cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, 960);

	SetTimer(TIMER_ID, TIMER_INTERVAL_MS, nullptr);*/

	if (!cap.isOpened())
	{
		cap.open(0);
	}

	if (!cap.isOpened())
	{
		MessageBox(_T("Не удалось открыть камеру!"));
		return;
	}

	cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, 960);

	stopThread = false;
	AfxBeginThread(CameraCaptureThread, this); // Запускаем поток
}


void COpenCvWithMFCDlg::OnBnClickedButtonStop()
{
	/*KillTimer(TIMER_ID); // Останавливаем таймер
	cap.release();       // Освобождаем камеру*/

	stopThread = true;
	cap.release();     
}


void COpenCvWithMFCDlg::OnTimer(UINT_PTR nIDEvent)
{
	/*if (nIDEvent == TIMER_ID)
	{
		cap >> frame;

		if (frame.empty())
		{
			KillTimer(TIMER_ID);
			MessageBox(_T("Камера отключена или кадры не доступны!"));
			return;
		}

		cv::cvtColor(frame, edges, cv::COLOR_BGR2GRAY);
		cv::GaussianBlur(edges, edges, cv::Size(7, 7), 1.5, 1.5);
		cv::Canny(edges, edges, 50, 90);

		DisplayImageInPictureControl(this, edges);
	}

	CDialogEx::OnTimer(nIDEvent);*/
}

afx_msg LRESULT COpenCvWithMFCDlg::OnUpdatePicture(WPARAM wParam, LPARAM lParam)
{
	//DisplayImageInPictureControl(this, edges);
	//DrawFrameToPictureControl(pWnd, edges);
	return 0;
}

void COpenCvWithMFCDlg::OnEnUpdateEditCannyThreshold1()
{
	m_CannyThreshold1 = GetDlgItemInt(IDC_EDIT_CANNY_THRESHOLD_1);
	UpdateData(FALSE);
}


void COpenCvWithMFCDlg::OnEnUpdateEditCannyThreshold2()
{
	m_CannyThreshold2 = GetDlgItemInt(IDC_EDIT_CANNY_THRESHOLD_2);
	UpdateData(FALSE);
}


void COpenCvWithMFCDlg::OnEnUpdateEditGaussianKernelSize1()
{
	m_KernelSize1 = (GetDlgItemInt(IDC_EDIT_GAUSSIAN_KERNEL_SIZE_1) % 2 != 0) ? GetDlgItemInt(IDC_EDIT_GAUSSIAN_KERNEL_SIZE_1) : 1;
	UpdateData(FALSE);
}


void COpenCvWithMFCDlg::OnEnUpdateEditGaussianKernelSize2()
{
	m_KernelSize2 = (GetDlgItemInt(IDC_EDIT_GAUSSIAN_KERNEL_SIZE_2) % 2 != 0) ? GetDlgItemInt(IDC_EDIT_GAUSSIAN_KERNEL_SIZE_2) : 1;
	UpdateData(FALSE);
}
