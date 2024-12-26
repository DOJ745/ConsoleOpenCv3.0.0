// ConsoleOpenCv3.0.0.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <Windows.h>
#include "opencv2/opencv.hpp"
#include "opencv2/features2d.hpp"
#include <opencv2/flann.hpp>
#include "GetCpuTime.h"

IplImage* image = 0;
IplImage* templ = 0;

//// ===== Image detection =====
//int _tmain(int argc, _TCHAR* argv[])
//{
//	const char* windowNameOriginalImage = "Original image";
//	const char* windowNameTemplateImage = "Template image";
//	const char* windowNameFoundMatch = "Found Match";
//	const char* windowNameComapreResult = "Compare result";
//	const char* windowNameNormalizedCompareResult = "Normalized Compare result";
//
//	// имя картинки задаётся первым параметром
//	//char* filename = argc >= 2 ? argv[1] : "Image0.jpg";
//	char* filename = "OriginalImage.jpg";
//	// получаем картинку
//	image = cvLoadImage(filename, CV_LOAD_IMAGE_COLOR);
//
//	printf("[i] image: %s\n", filename);
//	assert(image != 0);
//
//	// шаблон
//	//char* filename2 = argc >= 3 ? argv[2] : "eye.jpg";
//	char* filename2 = "EyeImage.jpg";
//	printf("[i] template: %s\n", filename2);
//
//	templ = cvLoadImage(filename2, CV_LOAD_IMAGE_COLOR);
//	assert(templ != 0);
//
//	cvNamedWindow(windowNameOriginalImage, CV_WINDOW_AUTOSIZE);
//	cvNamedWindow(windowNameTemplateImage, CV_WINDOW_AUTOSIZE);
//	cvNamedWindow(windowNameFoundMatch, CV_WINDOW_AUTOSIZE);
//	cvNamedWindow(windowNameComapreResult, CV_WINDOW_AUTOSIZE);
//
//	// размер шаблона
//	int width = templ->width;
//	int height = templ->height;
//
//	// оригинал и шаблон
//	cvShowImage(windowNameOriginalImage, image);
//	cvShowImage(windowNameTemplateImage, templ);
//
//	printf("Template size: %dx%d\n", templ->width, templ->height);
//	printf("Original image size: %dx%d\n", image->width, image->height);
//
//	// изображение для хранения результата сравнения
//	// размер результата: если image WxH и templ wxh, то result = (W - w + 1) x (H - h + 1)
//	IplImage* compareResult = cvCreateImage(
//		cvSize((image->width - templ->width + 1)
//		, (image->height - templ->height + 1))
//		, IPL_DEPTH_32F
//		, 1);
//
//	int methodSQDIFF = CV_TM_SQDIFF;
//	int methodCCORR_NORMED = CV_TM_CCORR_NORMED;
//	// сравнение изображения с шаблоном
//	cvMatchTemplate(image, templ, compareResult, methodSQDIFF); // methodSQDIFF
//
//	// покажем что получили
//	cvShowImage(windowNameComapreResult, compareResult);
//
//	// определение лучшее положение для сравнения
//	// (поиск минимумов и максимумов на изображении)
//	double minval, maxval;
//	CvPoint minloc, maxloc;
//	cvMinMaxLoc(compareResult, &minval, &maxval, &minloc, &maxloc, 0);
//
//	printf("Min value: %f, Max value: %f\n", minval, maxval);
//	printf("Min location: (%d; %d), Max location: (%d; %d)\n", minloc.x, minloc.y, maxloc.x, maxloc.y);
//
//	// выделим лучшее положение для сравнения прямоугольником
//	cvRectangle(
//		image
//		, cvPoint(minloc.x, minloc.y)
//		, cvPoint(maxloc.x, maxloc.y)
//		, CV_RGB(0, 255, 255)
//		, 1
//		, 8);
//
//	// нормализуем
//	cvNormalize(compareResult, compareResult, 1, 0, CV_MINMAX);
//	cvNamedWindow(windowNameNormalizedCompareResult, CV_WINDOW_AUTOSIZE);
//	cvShowImage(windowNameNormalizedCompareResult, compareResult);
//
//	// выделим распознанную область прямоугольником
//	cvRectangle(
//		image
//		, cvPoint(minloc.x, minloc.y)
//		, cvPoint(minloc.x + templ->width - 1, minloc.y + templ->height - 1)
//		, CV_RGB(255, 0, 0)
//		, 1
//		, 8);
//
//	printf("Rectangle coords: Point 1(%d; %d), Point 2(%d; %d)", 
//		minloc.x
//		, minloc.y
//		, minloc.x + templ->width - 1
//		, minloc.y + templ->height - 1);
//
//	// показываем изображение
//	cvShowImage(windowNameFoundMatch, image);
//
//	// ждём нажатия клавиши
//	cvWaitKey(0);
//
//	// освобождаем ресурсы
//	cvReleaseImage(&image);
//	cvReleaseImage(&templ);
//	cvReleaseImage(&compareResult);
//	cvDestroyAllWindows();
//
//	return 0;
//}

// ===== Detection Algorythms =====
int main()
{
	setlocale(LC_ALL, "Russian");
	// Загрузка изображений
	int imgReadMode = cv::IMREAD_UNCHANGED; // IMREAD_COLOR by default
	cv::Mat image = cv::imread("OriginalImage.jpg", imgReadMode);
	cv::Mat templateImage = cv::imread("EyeImage.jpg", imgReadMode);

	if (image.empty()) 
	{
		std::cerr << "Ошибка загрузки изображения" << std::endl;
		return -1;
	}

	if (templateImage.empty())
	{
		std::cerr << "Ошибка загрузки шаблона" << std::endl;
		return -1;
	}

	// Конвертация в градации серого
	cv::Mat grayImage, grayTemplate;

	cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);

	if (grayImage.empty())
	{
		std::cerr << "Ошибка: grayImage пустое" << std::endl;
		return -1;
	}

	cv::cvtColor(templateImage, grayTemplate, cv::COLOR_BGR2GRAY);

	if (grayTemplate.empty()) 
	{
		std::cerr << "Ошибка: grayTemplate пустое" << std::endl;
		return -1;
	}

	const int keyPoints = 250;

	// Создание объекта ORB
	cv::Ptr<cv::ORB> orb = cv::ORB::create(keyPoints);

	if (orb.empty()) 
	{
		std::cerr << "Ошибка: объект ORB не создан" << std::endl;
		return -1;
	}

	cv::Ptr<cv::AKAZE> akaze = cv::AKAZE::create();

	if (akaze.empty())
	{
		std::cerr << "Ошибка: объект AKAZE не создан" << std::endl;
		return -1;
	}

	
	// Обнаружение ключевых точек и вычисление дескрипторов
	std::vector<cv::KeyPoint> keypointsImage, keypointsTemplate;
	cv::Mat descriptorsImage, descriptorsTemplate;

	cv::normalize(grayImage, grayImage, 0, 255, cv::NORM_MINMAX);
	cv::normalize(grayTemplate, grayTemplate, 0, 255, cv::NORM_MINMAX);

	//orb->detectAndCompute(grayImage, cv::Mat(), keypointsImage, descriptorsImage);
	//orb->detectAndCompute(grayTemplate, cv::Mat(), keypointsTemplate, descriptorsTemplate);

	// Замер времени выполнения для getCPUTime() 
	double startTime, endTime;

	startTime = getCPUTime();
	std::clock_t start = std::clock();
	akaze->detectAndCompute(grayImage, cv::Mat(), keypointsImage, descriptorsImage);
	std::clock_t end = std::clock();
	endTime = getCPUTime();

	double elapsed_secs = double(end - start) / CLOCKS_PER_SEC;
	std::cout << "Время на akaze grayImage (по std::clock()): " << elapsed_secs << " сек.\n";
	std::cout << "Время на akaze grayImage (по getCPUTime): " << endTime - startTime << " сек.\n";

	startTime = getCPUTime();
	start = std::clock();
	akaze->detectAndCompute(grayTemplate, cv::Mat(), keypointsTemplate, descriptorsTemplate);
	end = std::clock();
	endTime = getCPUTime();

	elapsed_secs = double(end - start) / CLOCKS_PER_SEC;
	std::cout << "Время на akaze grayTemplate (по std::clock()): " << elapsed_secs << " сек.\n";
	std::cout << "Время на akaze grayTemplate (по getCPUTime): " << endTime - startTime << " сек.\n";

	if (keypointsImage.empty() || keypointsTemplate.empty()) 
	{
		std::cerr << "Ошибка: недостаточно ключевых точек для сопоставления" << std::endl;
		return -1;
	}

	if (descriptorsImage.empty() || descriptorsTemplate.empty()) 
	{
		std::cerr << "Ошибка: дескрипторы не созданы" << std::endl;
		return -1;
	}

	// Сопоставление дескрипторов (Brute-Force)
	cv::BFMatcher matcher(cv::NORM_HAMMING, true);
	std::vector<cv::DMatch> matches;
	matcher.match(descriptorsTemplate, descriptorsImage, matches);

	// Сортировка совпадений по расстоянию
	std::sort(
		matches.begin()
		, matches.end()
		, [](const cv::DMatch& a, const cv::DMatch& b) { return a.distance < b.distance; } );

	// Фильтрация совпадений
	const double maxDistance = 50.0; // Порог расстояния для фильтрации
	std::vector<cv::DMatch> goodMatches;
	
	for (int i = 0; i < matches.size(); i++) 
	{
		if (matches[i].distance < maxDistance)
		{
			goodMatches.push_back(matches[i]);
		}
	}

	// Рассчёт процента совпадения
	double matchPercentage = (double)goodMatches.size() / keypointsTemplate.size() * 100.0;

	std::cout << "Найдено совпадений: " << matches.size() << "\n";
	std::cout << "Общее количество ключевых точек в шаблоне: " << keypointsTemplate.size() << "\n";
	std::cout << "Количество хороших совпадений: " << goodMatches.size() << "\n";
	std::cout << "Процент совпадения: " << matchPercentage << "%" << "\n";

	// Визуализация совпадений
	cv::Mat matchedImage;
	cv::drawMatches(
		templateImage
		, keypointsTemplate
		, image
		, keypointsImage
		, goodMatches
		, matchedImage
		, cv::Scalar::all(-1)
		, cv::Scalar::all(-1)
		, std::vector<char>()
		, cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	// Показ результата
	cv::imshow("Matched Keypoints", matchedImage);
	cv::waitKey(0);
	
	cvDestroyAllWindows();

	return 0;
}

////===== Create simple text =====
//int main()
//{
//	int height = 800;
//	int width = 600;
//	// задаём точку для вывода текста
//	CvPoint pt = cvPoint(height / 8, width / 2);
//
//	// Создаём 8-битную, 3-канальную картинку
//	IplImage* cvImage = cvCreateImage(cvSize(height, width), 8, 3);
//
//	// заливаем картинку белым цветом
//	cvSet(cvImage, cvScalar(255, 255, 255));
//
//	// инициализация шрифта
//	CvFont font;
//
//	double fontHScale = 1.0;
//	double fontVScale = 1.0;
//	int fontThickness = 1;
//	double fontSheer = 0.0;
//	int fontLineType = 32;
//	cvInitFont(
//		&font
//		, CV_FONT_HERSHEY_DUPLEX
//		, fontHScale
//		, fontVScale
//		, fontSheer
//		, fontThickness
//		, fontLineType);
//	// используя шрифт выводим на картинку текст
//	cvPutText(cvImage, "ABCDEFGHIJKLMNOPQRSTYVWXYZ", pt, &font, CV_RGB(255, 0, 150) );
//
//	// создаём окошко
//	cvNamedWindow("Hello World", 0);
//	// показываем картинку в созданном окне
//	cvShowImage("Hello World", cvImage);
//	// ждём нажатия клавиши
//	cvWaitKey(0);
//
//	// освобождаем ресурсы
//	cvReleaseImage(&cvImage);
//	cvDestroyWindow("Hello World");
//
//	 return 0;
//}

// ===== Basic example =====
//int _tmain(int argc, _TCHAR* argv[])
//{
//	std::cout << "Hello, world!\n";
//
//	cv::VideoCapture cap(0); // open the default camera
//
//	if(!cap.isOpened())		// check if we succeeded
//	{
//		return -1;
//	}	
//
//	cv::Mat edges;
//
//	cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
//	cap.set(cv::CAP_PROP_FRAME_HEIGHT, 960);
//
//	cv::Mat frame;
//
//	for(;;)
//	{
//		cap >> frame;		// get a new frame from camera
//
//		if (frame.empty()) 
//		{
//			std::cerr << "Failed to capture frame!" << std::endl;
//			break;
//		}
//
//		cv::cvtColor(frame, edges, cv::COLOR_BGR2GRAY);
//		//cv::GaussianBlur(edges, edges, cv::Size(7,7), 1.5, 1.5, cv::BorderTypes::BORDER_DEFAULT);  // default parameters
//		//cv::Canny(edges, edges, 0, 30, 3);														 // default parameters
//
//		cv::GaussianBlur(edges, edges, cv::Size(7,7), 1.5, 1.5, cv::BorderTypes::BORDER_DEFAULT); 
//		cv::Canny(edges, edges, 50, 90, 3);
//
//		imshow("Camera edges", edges);
//		imshow("Camera clear image", frame);
//
//		if (cv::waitKey(27) >= 0) 
//		{
//			break;
//		}
//	}
//
//	return 0;
//}