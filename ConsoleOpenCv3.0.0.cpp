// ConsoleOpenCv3.0.0.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "opencv2/opencv.hpp"
#include <Windows.h>

int _tmain(int argc, _TCHAR* argv[])
{
	int keyCode = VK_ESCAPE;
	std::cout << "Hello, world!\n";

	cv::VideoCapture cap(0); // open the default camera

	if(!cap.isOpened())		// check if we succeeded
	{
		return -1;
	}	

	cv::Mat edges;

	cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, 960);

	cv::Mat frame;

	for(;;)
	{
		cap >> frame;		// get a new frame from camera

		if (frame.empty()) 
		{
			std::cerr << "Failed to capture frame!" << std::endl;
			break;
		}

		cv::cvtColor(frame, edges, cv::COLOR_BGR2GRAY);
		//cv::GaussianBlur(edges, edges, cv::Size(7,7), 1.5, 1.5, cv::BorderTypes::BORDER_DEFAULT);  // default parameters
		//cv::Canny(edges, edges, 0, 30, 3);														 // default parameters

		cv::GaussianBlur(edges, edges, cv::Size(7,7), 1.5, 1.5, cv::BorderTypes::BORDER_DEFAULT); 
		cv::Canny(edges, edges, 50, 90, 3);

		imshow("Camera edges", edges);
		imshow("Camera clear image", frame);

		if (cv::waitKey(keyCode) >= 0) 
		{
			break;
		}
	}

	// Create simple text

	//int height = 800;
	//int width = 600;
	//// задаЄм точку дл€ вывода текста
	//CvPoint pt = cvPoint(height / 8, width / 2);

	//// —оздаЄм 8-битную, 3-канальную картинку
	//IplImage* cvImage = cvCreateImage(cvSize(height, width), 8, 3);

	//// заливаем картинку белым цветом
	//cvSet(cvImage, cvScalar(255, 255, 255));

	//// инициализаци€ шрифта
	//CvFont font;

	//double fontHScale = 1.0;
	//double fontVScale = 1.0;
	//int fontThickness = 1;
	//double fontSheer = 0.0;
	//int fontLineType = 32;
	//cvInitFont(
	//	&font
	//	, CV_FONT_HERSHEY_DUPLEX
	//	, fontHScale
	//	, fontVScale
	//	, fontSheer
	//	, fontThickness
	//	, fontLineType);
	//// использу€ шрифт выводим на картинку текст
	//cvPutText(cvImage, "ABCDEFGHIJKLMNOPQRSTYVWXYZ", pt, &font, CV_RGB(255, 0, 150) );

	//// создаЄм окошко
	//cvNamedWindow("Hello World", 0);
	//// показываем картинку в созданном окне
	//cvShowImage("Hello World", cvImage);
	//// ждЄм нажати€ клавиши
	//cvWaitKey(0);

	//// освобождаем ресурсы
	//cvReleaseImage(&cvImage);
	//cvDestroyWindow("Hello World");

	return 0;
}

