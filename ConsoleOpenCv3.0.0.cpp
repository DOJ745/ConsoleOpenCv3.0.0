// ConsoleOpenCv3.0.0.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "opencv2/opencv.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include "GetCpuTime.h"

//IplImage* image = 0;
//IplImage* templ = 0;

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

//// ===== Detection Algorythms =====
//int main()
//{
//	setlocale(LC_ALL, "Russian");
//
//	int imgReadMode = cv::IMREAD_GRAYSCALE;						// IMREAD_COLOR by default
//	const char* originalImageFile = "images/OriginalImage.jpg"; // "images/OriginalImage.jpg";
//	const char* templateImageFile = "images/EyeImage.jpg";		// "images/EyeImage.jpg";
//	const char* pngImageFile = "images/test2.png";
//
//	cv::Mat image = cv::imread(originalImageFile, imgReadMode);
//	cv::Mat templateImage = cv::imread(templateImageFile, imgReadMode);
//	cv::Mat pngImage = cv::imread(pngImageFile, imgReadMode);
//
//	/*double resizeCoeff = 0.15;
//	cv::resize(image, image, cv::Size(), resizeCoeff, resizeCoeff);
//	cv::resize(templateImage, templateImage, cv::Size(), resizeCoeff, resizeCoeff);*/
//
//	if (image.empty()) 
//	{
//		std::cerr << "Ошибка загрузки изображения" << std::endl;
//		return -1;
//	}
//
//	if (templateImage.empty())
//	{
//		std::cerr << "Ошибка загрузки шаблона" << std::endl;
//		return -1;
//	}
//
//	if (pngImage.empty())
//	{
//		std::cerr << "Ошибка загрузки png изображения" << std::endl;
//		return -1;
//	}
//
//	cv::Ptr<cv::AKAZE> akaze = cv::AKAZE::create();
//
//	if (akaze.empty())
//	{
//		std::cerr << "Ошибка: объект AKAZE не создан" << std::endl;
//		return -1;
//	}
//
//  // WTA_K = 2: Стандартный, быстрый
//  // WTA_K > 2: Более устойчивый к шуму и искажениям
//  // Порог отклика:
//  // - Если отклик ключевой точки ниже порогового значения, она игнорируется.
//  // - Более высокий порог приводит к меньшему числу ключевых точек, но они будут более "выразительными".
//	cv::Ptr<cv::ORB> orb = cv::ORB::create(
//		2500,					// Максимальное количество ключевых точек
//		1.4f,					// Масштабный фактор пирамиды (Этот параметр определяет, насколько изображение уменьшается на каждом уровне пирамиды - уменьшение ускоряет обработку)
//		6,						// Количество уровней пирамиды	(Этот параметр определяет глубину пирамиды. Чем больше уровней, тем больше требуется вычислений)
//		100,					// Размер окна	(Этот параметр задаёт размер области для поиска ключевых точек. Большие значения полезны для больших изображений)
//		0,						// Первый уровень пирамиды
//		2,						// WTA_K - параметр определяет количество точек в окрестности ключевой точки, которые сравниваются для генерации одного бита дескриптора
//		cv::ORB::FAST_SCORE,	// Метод оценки
//		64,						// Радиус граничной области	(Этот параметр определяет размер патча вокруг каждой ключевой точки, используемого для вычисления дескрипторов - чем больше, тем больше обрабатывается деталей)
//		20						// Порог отклика -  это численная величина, характеризующая "выразительность" точки
//	);
//
//	if (orb.empty())
//	{
//		std::cerr << "Ошибка: объект ORB не создан" << std::endl;
//		return -1;
//	}
//	
//	// Обнаружение ключевых точек и вычисление дескрипторов
//	std::vector<cv::KeyPoint> keypointsImage, keypointsTemplate;
//	std::vector<cv::KeyPoint> keypointsImageOrb, keypointsTemplateOrb;
//
//	cv::Mat emptyMask;
//	cv::Mat descriptorsImage, descriptorsTemplate;
//	cv::Mat descriptorsImageOrb, descriptorsTemplateOrb;
//
//	double minVal, maxVal;
//
//	if (image.channels() == 3) 
//	{
//		cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);
//	}
//	else
//	{
//		cv::minMaxLoc(image, &minVal, &maxVal);
//		cv::normalize(image, image, 0, 255, cv::NORM_MINMAX);
//		std::cout << "Диапазон значений image: [" << minVal << ", " << maxVal << "]\n";
//	}
//
//	if (templateImage.channels() == 3)
//	{
//		cv::cvtColor(templateImage, templateImage, cv::COLOR_BGR2GRAY);
//	}
//	else
//	{
//		cv::minMaxLoc(templateImage, &minVal, &maxVal);
//		cv::normalize(templateImage, templateImage, 0, 255, cv::NORM_MINMAX);
//		std::cout << "Диапазон значений templateImage: [" << minVal << ", " << maxVal << "]\n";
//	}
//
//	// Замер времени выполнения для getCPUTime() 
//	double startTime, endTime;
//
//	startTime = getCPUTime();
//	akaze->detectAndCompute(image, emptyMask, keypointsImage, descriptorsImage);
//	endTime = getCPUTime();
//
//	std::cout << "Время на AKAZE image (по getCPUTime): " << endTime - startTime << " сек.\n";
//
//	startTime = getCPUTime();
//	akaze->detectAndCompute(templateImage, emptyMask, keypointsTemplate, descriptorsTemplate);
//	endTime = getCPUTime();
//
//	std::cout << "Время на AKAZE templateImage (по getCPUTime): " << endTime - startTime << " сек.\n";
//
//	if (keypointsImage.empty() || keypointsTemplate.empty()) 
//	{
//		std::cerr << "Ошибка: недостаточно ключевых точек для сопоставления" << std::endl;
//		return -1;
//	}
//
//	if (descriptorsImage.empty() || descriptorsTemplate.empty()) 
//	{
//		std::cerr << "Ошибка: дескрипторы не созданы" << std::endl;
//		return -1;
//	}
//
//	startTime = getCPUTime();
//	orb->detectAndCompute(image, emptyMask, keypointsImageOrb, descriptorsImageOrb);
//	endTime = getCPUTime();
//
//	std::cout << "Время на ORB image (по getCPUTime): " << endTime - startTime << " сек.\n";
//
//	startTime = getCPUTime();
//	orb->detectAndCompute(templateImage, emptyMask, keypointsTemplateOrb, descriptorsTemplateOrb);
//	endTime = getCPUTime();
//
//	std::cout << "Время на ORB templateImage (по getCPUTime): " << endTime - startTime << " сек.\n";
//
//	if (keypointsImageOrb.empty() || keypointsTemplateOrb.empty())
//	{
//		std::cerr << "Ошибка: недостаточно ключевых точек для сопоставления" << std::endl;
//		//return -1;
//	}
//
//	if (descriptorsImageOrb.empty() || descriptorsTemplateOrb.empty())
//	{
//		std::cerr << "Ошибка: дескрипторы не созданы" << std::endl;
//		//return -1;
//	}
//
//	// Сопоставление дескрипторов (Brute-Force)
//	cv::BFMatcher matcher(cv::NORM_HAMMING, true);
//	std::vector<cv::DMatch> matches;
//	matcher.match(descriptorsTemplate, descriptorsImage, matches);
//	
//	// Сортировка совпадений по расстоянию
//	std::sort(
//		matches.begin()
//		, matches.end()
//		, [](const cv::DMatch& a, const cv::DMatch& b) { return a.distance < b.distance; } );
//
//	std::vector<cv::DMatch> matchesOrb;
//	matcher.match(descriptorsTemplateOrb, descriptorsImageOrb, matchesOrb);
//	std::sort(
//		matchesOrb.begin()
//		, matchesOrb.end()
//		, [](const cv::DMatch& a, const cv::DMatch& b) { return a.distance < b.distance; });
//
//	// Фильтрация совпадений
//	const double maxDistance = 50.0; // Порог расстояния для фильтрации (50.0)
//	
//	std::vector<cv::DMatch> goodMatches; 
//	for (int i = 0; i < matches.size(); i++) 
//	{
//		if (matches[i].distance < maxDistance)
//		{
//			goodMatches.push_back(matches[i]);
//		}
//	}
//
//	std::vector<cv::DMatch> goodMatchesOrb;
//	for (int i = 0; i < matchesOrb.size(); i++)
//	{
//		if (matchesOrb[i].distance < maxDistance)
//		{
//			goodMatchesOrb.push_back(matchesOrb[i]);
//		}
//	}
//
//	// Рассчёт процента совпадения
//
//	double matchPercentage = (double)goodMatches.size() / keypointsTemplate.size() * 100.0;
//	std::cout << "\n======= AKAZE =======\n\n";
//	std::cout << "Найдено совпадений AKAZE: " << matches.size() << "\n";
//	std::cout << "Общее количество ключевых точек в шаблоне AKAZE: " << keypointsTemplate.size() << "\n";
//	std::cout << "Количество хороших совпадений AKAZE: " << goodMatches.size() << "\n";
//	std::cout << "Процент совпадения AKAZE: " << matchPercentage << "%" << "\n";
//
//	double matchPercentageOrb = (double)goodMatchesOrb.size() / keypointsTemplateOrb.size() * 100.0;
//	std::cout << "\n======= ORB =======\n\n";
//	std::cout << "Найдено совпадений ORB: " << matchesOrb.size() << "\n";
//	std::cout << "Общее количество ключевых точек в шаблоне ORB: " << keypointsTemplateOrb.size() << "\n";
//	std::cout << "Количество хороших совпадений ORB: " << goodMatchesOrb.size() << "\n";
//	std::cout << "Процент совпадения ORB: " << matchPercentageOrb << "%" << "\n";
//
//	// Визуализация совпадений
//	cv::Mat matchedImage;
//
//	cv::drawMatches(
//		templateImage
//		, keypointsTemplate
//		, image
//		, keypointsImage
//		, goodMatches
//		, matchedImage
//		, cv::Scalar::all(-1)
//		, cv::Scalar::all(-1)
//		, std::vector<char>()
//		, cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
//
//	cv::Mat matchedImageOrb;
//	cv::drawMatches(
//		templateImage
//		, keypointsTemplateOrb
//		, image
//		, keypointsImageOrb
//		, goodMatchesOrb
//		, matchedImageOrb
//		, cv::Scalar::all(-1)
//		, cv::Scalar::all(-1)
//		, std::vector<char>()
//		, cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
//
//	// Показ результата
//	cv::imshow("Matched Keypoints AKAZE", matchedImage);
//	cv::imshow("Matched Keypoints ORB", matchedImageOrb);
//	cv::waitKey(0);
//
//	return 0;
//}

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

// // ===== MANUAL SET KEY POINTS =====
//int main() 
//{
//    cv::Mat image = cv::imread("images/HD-CHIP.jpg", cv::IMREAD_GRAYSCALE);
//
//    if (image.empty()) 
//    {
//        std::cerr << "Ошибка: не удалось загрузить изображение.\n";
//        return -1;
//    }
//
//    std::vector<cv::KeyPoint> keypoints;
//    keypoints.push_back(cv::KeyPoint(100, 150, 10)); 
//    keypoints.push_back(cv::KeyPoint(200, 250, 15));
//    keypoints.push_back(cv::KeyPoint(300, 350, 20));
//
//    cv::Mat descriptors;
//    cv::Ptr<cv::ORB> orb = cv::ORB::create();
//    orb->compute(image, keypoints, descriptors);
//
//    // Визуализация ключевых точек
//    cv::Mat outputImage;
//    cv::drawKeypoints(image, keypoints, outputImage, cv::Scalar(0, 255, 0), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
//    cv::imshow("Keypoints", outputImage);
//    cv::waitKey(0);
//
//    return 0;
//}

//// ===== Keypoints select and compute =====
//
//cv::Mat templateImg, mainImg;
//std::vector<cv::Point2f> selectedPoints;
//
//void onMouse(int event, int x, int y, int, void*) 
//{
//    if (event == cv::EVENT_LBUTTONDOWN) 
//    {
//        selectedPoints.emplace_back(cv::Point2f(x, y));
//        std::cout << "Выбрана точка: (" << x << ", " << y << ")\n";
//
//        cv::circle(templateImg, cv::Point(x, y), 5, cv::Scalar(0, 255, 0), -1);
//        cv::imshow("Select Points", templateImg);
//    }
//}
//
//int main() 
//{
//    setlocale(LC_ALL, "Russian");
//
//    int imgReadMode = cv::IMREAD_GRAYSCALE;
//    const char* templateImgFile = "images/HD-CHIP-SURFACE.jpg";
//    const char* mainImgFile = "images/HD-CHIP-SURFACE-TURN.jpg";//"images/HD-CHIP.jpg";
//    
//    templateImg = cv::imread(templateImgFile, imgReadMode);
//    mainImg = cv::imread(mainImgFile, imgReadMode);
//
//    const double resizeTemplate = 0.5;
//    const double resizeMain = 0.5;
//
//    cv::resize(templateImg, templateImg, cv::Size(), resizeTemplate, resizeTemplate, cv::INTER_AREA);
//    cv::resize(mainImg, mainImg, cv::Size(), resizeMain, resizeMain, cv::INTER_AREA);
//
//    if (templateImg.empty()) 
//    {
//        std::cerr << "Ошибка: не удалось загрузить изображение.\n";
//        return -1;
//    }
//
//    cv::imshow("Select Points", templateImg);
//    cv::setMouseCallback("Select Points", onMouse);
//
//    std::cout << "Выберите точки на изображении. Нажмите клавишу для завершения.\n";
//    while (true) 
//    {
//        char key = cv::waitKey(0);
//        
//        if (key) 
//        {
//            break;
//        }
//    }
//
//    std::cout << "Выбранные точки:\n";
//
//    for (int i = 0; i < selectedPoints.size(); i++)
//    {
//        std::cout << "(" << selectedPoints[i].x << ", " << selectedPoints[i].y << ")\n";
//    }
//
//    // Преобразуем выбранные точки в KeyPoint для дальнейшего использования
//    std::vector<cv::KeyPoint> templateKeypoints, mainImgKeypoints;
//
//    for (int i = 0; i < selectedPoints.size(); i++)
//    {
//        templateKeypoints.emplace_back(cv::KeyPoint(selectedPoints[i].x, selectedPoints[i].y, 10)); // Указываем координаты и размер
//    }
//
//    cv::Mat templateDescriptors, mainImgDescriptors;
//    cv::Mat mask;
//    cv::Ptr<cv::ORB> orb = cv::ORB::create(
//        		500,					// Максимальное количество ключевых точек
//        		1.4f,					// Масштабный фактор пирамиды (Этот параметр определяет, насколько изображение уменьшается на каждом уровне пирамиды - уменьшение ускоряет обработку)
//        		10,						// Количество уровней пирамиды	(Этот параметр определяет глубину пирамиды. Чем больше уровней, тем больше требуется вычислений)
//        		10,					    // Размер окна	(Этот параметр задаёт размер области для поиска ключевых точек. Большие значения полезны для больших изображений)
//        		0,						// Первый уровень пирамиды
//        		2,						// WTA_K - параметр определяет количество точек в окрестности ключевой точки, которые сравниваются для генерации одного бита дескриптора
//        		cv::ORB::FAST_SCORE,	// Метод оценки
//        		18,						// Радиус граничной области	(Этот параметр определяет размер патча вокруг каждой ключевой точки, используемого для вычисления дескрипторов - чем больше, тем больше обрабатывается деталей)
//        		50						// Порог отклика -  это численная величина, характеризующая "выразительность" точки
//        	);
//    
//    cv::Ptr<cv::AKAZE> akaze = cv::AKAZE::create();
//    akaze->setThreshold(0.001f);                            // Уровень порога для подавления слабых ключевых точек
//    akaze->setNOctaves(4);                                  // Количество октав (уровней пирамиды)
//    akaze->setNOctaveLayers(4);                             // Количество слоёв в каждой октаве
//    akaze->setDescriptorSize(0);                            // Размер дескриптора (по умолчанию максимальный)
//    akaze->setDescriptorType(cv::AKAZE::DESCRIPTOR_MLDB);   // Тип дескриптора
//
//    //orb->compute(templateImg, templateKeypoints, templateDescriptors);
//    //akaze->compute(templateImg, templateKeypoints, templateDescriptors);
//    
//    try
//    {
//        //orb->detectAndCompute(templateImg, mask, templateKeypoints, templateDescriptors);
//        akaze->detectAndCompute(templateImg, mask, templateKeypoints, templateDescriptors);
//    }
//    catch (const std::exception& e)
//    {
//        std::cerr << "Исключение: " << e.what() << "\n";
//    }
//
//    try
//    {
//        //orb->detectAndCompute(mainImg, mask, mainImgKeypoints, mainImgDescriptors);
//        akaze->detectAndCompute(mainImg, mask, mainImgKeypoints, mainImgDescriptors);
//    }
//    catch (const std::exception& e)
//    {
//        std::cerr << "Исключение: " << e.what() << "\n";
//    }
//
//    // Визуализация ключевых точек
//    cv::Mat templateImgKeypoints;
//    cv::drawKeypoints(templateImg, templateKeypoints, templateImgKeypoints, cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
//    cv::imshow("Template Keypoints", templateImgKeypoints);
//
//	cv::BFMatcher matcher(cv::NORM_HAMMING, true);
//	std::vector<cv::DMatch> matches;
//	matcher.match(templateDescriptors, mainImgDescriptors, matches);
//	
//	// Сортировка совпадений по расстоянию
//	std::sort(
//		matches.begin()
//		, matches.end()
//		, [](const cv::DMatch& a, const cv::DMatch& b) { return a.distance < b.distance; } );
//
//    // Фильтрация совпадений
//	const double maxDistance = 50.0; // Порог расстояния для фильтрации (50.0)
//	
//	std::vector<cv::DMatch> goodMatches; 
//	for (int i = 0; i < matches.size(); i++) 
//	{
//		if (matches[i].distance < maxDistance)
//		{
//			goodMatches.push_back(matches[i]);
//		}
//	}
//
//    // Визуализация совпадений
//	cv::Mat matchedImage;
//
//	cv::drawMatches(
//        templateImg
//		, templateKeypoints
//		, mainImg
//		, mainImgKeypoints
//		, goodMatches
//		, matchedImage
//		, cv::Scalar::all(-1)
//		, cv::Scalar::all(-1)
//		, std::vector<char>()
//		, cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
//
//    cv::imshow("Matched Keypoints ORB", matchedImage);
//
//    double matchPercentage = (double)goodMatches.size() / (double)templateKeypoints.size() * 100.0;
//	std::cout << "Найдено совпадений: " << matches.size() << "\n";
//	std::cout << "Общее количество ключевых точек в шаблоне: " << templateKeypoints.size() << "\n";
//	std::cout << "Количество хороших совпадений: " << goodMatches.size() << "\n";
//	std::cout << "Процент совпадения: " << matchPercentage << "%" << "\n";
//
//	cv::Mat result;
//	cv::matchTemplate(mainImg, templateImg, result, cv::TM_CCOEFF_NORMED);
//
//	double minVal, maxVal;
//	cv::Point minLoc, maxLoc;
//	cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
//
//    std::cout << "Шаблонное совпадение (максимальная корреляция): " << maxVal << "\n";
//
//    // Пороговое значение корреляции
//	if (maxVal > 0.8) 
//	{ 
//		std::cout << "Совпадение подтверждено через шаблонное сравнение." << "\n";
//	}
//	else 
//	{
//        std::cout << "Совпадение не подтверждено.\n";
//	}
//
//    // Размер шаблона
//    cv::Size templateSize = templateImg.size();
//
//    // Рисуем прямоугольник на оригинальном изображении
//    cv::rectangle(
//        mainImg,
//        maxLoc,                                                                     // Верхний левый угол
//        cv::Point(maxLoc.x + templateSize.width, maxLoc.y + templateSize.height),   // Нижний правый угол
//        cv::Scalar(0, 0, 255),                                                      
//        2                                                                           // Толщина линии
//    );
//
//    cv::imshow("Detected Area", mainImg);
//
//    cv::waitKey(0);
//
//    return 0;
//}

//// ===== Select ROI by mouse =====
//bool isDrawing = false;
//cv::Mat templateImg, mainImg, tempImg; 
//cv::Point startPoint, centerPoint, endPoint;
//std::vector<cv::Point2f> selectedPoints;
//
//void onMouse(int event, int x, int y, int, void*) 
//{
//    switch (event) 
//    {
//    case cv::EVENT_LBUTTONDOWN:
//        isDrawing = true;
//        startPoint = cv::Point(x, y);
//        cv::circle(mainImg, startPoint, 5, cv::Scalar(0, 255, 0), -1);
//        break;
//
//    case cv::EVENT_MOUSEMOVE:
//
//        if (isDrawing)
//        {
//            tempImg = mainImg.clone();
//            endPoint = cv::Point(x, y);
//            cv::rectangle(tempImg, startPoint, endPoint, cv::Scalar(0, 255, 0), 2);
//        }
//        break;
//
//    case cv::EVENT_LBUTTONUP:
//        isDrawing = false;
//        endPoint = cv::Point(x, y);
//        cv::rectangle(mainImg, startPoint, endPoint, cv::Scalar(0, 255, 0), 2);
//        cv::circle(mainImg, endPoint, 5, cv::Scalar(0, 255, 0), -1);
//
//        centerPoint = cv::Point((startPoint.x + endPoint.x) / 2, (startPoint.y + endPoint.y) / 2);
//
//        cv::circle(mainImg, centerPoint, 15, cv::Scalar(255, 0, 0), -1);
//        break;
//    }
//}
//
//int main()
//{
//    setlocale(LC_ALL, "Russian");
//
//    int imgReadMode = cv::IMREAD_GRAYSCALE;
//    const char* templateImgFile = "images/HD-CHIP-SURFACE.jpg";
//    const char* mainImgFile = "images/HD-CHIP.jpg";
//
//    templateImg = cv::imread(templateImgFile, imgReadMode);
//
//    if (templateImg.empty())
//    {
//        std::cerr << "Ошибка: не удалось загрузить изображение " << templateImgFile << ".\n";
//        return -1;
//    }
//
//    mainImg = cv::imread(mainImgFile, imgReadMode);
//
//    if (mainImg.empty())
//    {
//        std::cerr << "Ошибка: не удалось загрузить изображение " << mainImgFile << ".\n";
//        return -1;
//    }
//
//    const double resizeTemplate = 0.20;
//    const double resizeMain = 0.20;
//
//    cv::resize(templateImg, templateImg, cv::Size(), resizeTemplate, resizeTemplate, cv::INTER_AREA);
//    cv::resize(mainImg, mainImg, cv::Size(), resizeMain, resizeMain, cv::INTER_AREA);
//
//    // Преобразуем изображение в 3-канальный формат (BGR) для отрисовки RGB палитры
//    // Серый цвет останется таким же, потому что значения для каждого канала будут одинаковы
//    cv::cvtColor(mainImg, mainImg, cv::COLOR_GRAY2BGR);
//
//    tempImg = mainImg.clone();
//    cv::namedWindow("Select ROI");
//    cv::setMouseCallback("Select ROI", onMouse);
//
//    while (true)
//    {
//        cv::imshow("Select ROI", isDrawing ? tempImg : mainImg);
//
//        char key = cv::waitKey(1);
//
//        if (key == 'q' || key == 27) 
//        {
//            break;
//        }
//    }
//
//    return 0;
//}

// ===== Testing scaling =====
void resizeAndDisplay(cv::Mat& inputFrame, cv::Size targetSize, bool keepAspectRatio) 
{
    cv::Mat resizedFrame;

    if (keepAspectRatio) 
    {
        double scaleWidth = static_cast<double>(targetSize.width) / inputFrame.cols;
        double scaleHeight = static_cast<double>(targetSize.height) / inputFrame.rows;
        double scale = std::min(scaleWidth, scaleHeight);

        // Новый размер изображения с сохранением пропорций
        int newWidth = static_cast<int>(inputFrame.cols * scale);
        int newHeight = static_cast<int>(inputFrame.rows * scale);

        cv::resize(inputFrame, resizedFrame, cv::Size(newWidth, newHeight), 0, 0, cv::INTER_LINEAR);

        // Создаем рамку (если требуется)
        cv::Mat outputFrame(targetSize, inputFrame.type(), cv::Scalar(128, 64, 128));
        int xOffset = (targetSize.width - newWidth) / 2;
        int yOffset = (targetSize.height - newHeight) / 2;

        // Вставляем масштабированное изображение в центр области
        resizedFrame.copyTo(outputFrame(cv::Rect(xOffset, yOffset, newWidth, newHeight)));

        resizedFrame = outputFrame;
    }
    else 
    {
        // Масштабируем без сохранения пропорций
        cv::resize(inputFrame, resizedFrame, targetSize, 0, 0, cv::INTER_LINEAR);
    }

    cv::imshow("Resized Frame", resizedFrame);
    cv::waitKey(0);
}

int main() 
{
    setlocale(LC_ALL, "Russian");

    const int defaultWidth = 1280;
    const int defaultHeight = 1024;
    const int targetWidth = 640;
    const int targetHeight = 480;

    // Создаем тестовое изображение размером 1280 x 1024
    cv::Mat frame = cv::Mat::zeros(defaultHeight, defaultWidth, CV_8UC3);
    // Синий прямоугольник внутри изображения
    cv::rectangle(frame, cv::Point(100, 100), cv::Point(defaultWidth - 100, defaultHeight - 100), cv::Scalar(255, 0, 0), -1); 

    cv::Size targetSize(targetWidth, targetHeight);

    std::cout << "Сохранение пропорций...\n";
    resizeAndDisplay(frame, targetSize, true);

    std::cout << "Без сохранения пропорций...\n";
    resizeAndDisplay(frame, targetSize, false);

    return 0;
}