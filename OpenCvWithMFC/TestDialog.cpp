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

// Глобальные переменные для использования в потоке
cv::VideoCapture TEST_CAP;				// Для захвата кадров
cv::Mat TEST_FRAME;						// Для хранения изображения
volatile bool stopTestThread = false;	// Для управления потоком

// Обработка кликов мыши
void MouseCallback(int event, int x, int y, int flags, void* userdata)
{
	if (event == cv::EVENT_LBUTTONDOWN) 
	{
		cv::Mat* img = reinterpret_cast<cv::Mat*>(userdata);
		circle(*img, cv::Point(x, y), 10, cv::Scalar(0, 0, 255), -1);
		imshow("OpenCV Window", *img);
	}
}

// Функция потока для отображения кадров
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

		// Завершаем по нажатию ESC
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

// Функция для привязки окна OpenCV к области MFC
void AttachOpenCVWindowToMFC(HWND hwndParent)
{
	//cv::namedWindow("OpenCV Window", cv::WINDOW_NORMAL);

	RECT parentRect;               // Глобальные координаты окна
	RECT clientRect;               // Размеры клиентской области
	POINT clientOffset = {0, 0};   // Смещение клиентской области относительно окна

	// Получаем глобальные координаты окна MFC
	GetWindowRect(hwndParent, &parentRect);

	// Получаем размеры клиентской области окна MFC
	GetClientRect(hwndParent, &clientRect);

	// Преобразуем верхний левый угол клиентской области в глобальные координаты
	ClientToScreen(hwndParent, &clientOffset);

	// Рассчитываем смещение из-за границ и заголовка
	int borderOffsetX = clientOffset.x - parentRect.left;
	int borderOffsetY = clientOffset.y - parentRect.top;

	// Найти окно OpenCV
	HWND hwndOpenCV = FindWindow(NULL, L"OpenCV Window");

	if (hwndOpenCV) 
	{
		// Ограничиваем размеры окна OpenCV в пределах клиентской области MFC
		int maxWidth = clientRect.right - clientRect.left;
		int maxHeight = clientRect.bottom - clientRect.top;

		int width = std::min(640, maxWidth);  // Ширина окна OpenCV не больше доступного пространства
		int height = std::min(480, maxHeight); // Высота окна OpenCV не больше доступного пространства

		int offsetX = 10; // Сдвиг от левого края клиентской области
		int offsetY = 10; // Сдвиг от верхнего края клиентской области

		// Вычисляем позицию OpenCV окна относительно клиентской области
		int posX = clientOffset.x + offsetX;
		int posY = clientOffset.y + offsetY;

		// Устанавливаем положение окна OpenCV относительно MFC
		SetWindowPos(hwndOpenCV,
			HWND_TOP,
			posX,
			posY,
			width,
			height,
			SWP_NOZORDER | SWP_SHOWWINDOW);

		// Убираем заголовок и предотвращаем перемещение
		LONG style = GetWindowLong(hwndOpenCV, GWL_STYLE);
		style &= ~WS_CAPTION;
		SetWindowLong(hwndOpenCV, GWL_STYLE, style);

		// Устанавливаем окно OpenCV как дочернее для окна MFC
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

	// Инициализация камеры
	if (!TEST_CAP.open(0)) 
	{
		AfxMessageBox(L"Failed to open camera!");
		return FALSE;
	}

	const int defaultFrameWidth = 1280;
	const int defaultFrameHeight = 1024;

	if (!TEST_CAP.set(cv::CAP_PROP_FRAME_WIDTH, defaultFrameWidth))
	{
		MessageBox(_T("Не удалось задать ширину кадра камеры!"));
		return FALSE;
	}

	if (!TEST_CAP.set(cv::CAP_PROP_FRAME_HEIGHT, defaultFrameHeight))
	{
		MessageBox(_T("Не удалось задать высоту кадра камеры!"));
		return FALSE;
	}

	// Создаем окно OpenCV
	cv::namedWindow("OpenCV Window", cv::WINDOW_NORMAL);

	// Устанавливаем MouseCallback
	setMouseCallback("OpenCV Window", MouseCallback, &TEST_FRAME);

	// Закрепляем окно OpenCV
	AttachOpenCVWindowToMFC(GetSafeHwnd());

	// Запуск потока отображения
	AfxBeginThread(VideoThread, NULL);

	return TRUE;
}

void TestDialog::OnDestroy()
{
	stopTestThread = true; // Останавливаем поток при завершении диалога
	CDialogEx::OnDestroy();
}

BEGIN_MESSAGE_MAP(TestDialog, CDialogEx)
END_MESSAGE_MAP()


// TestDialog message handlers
