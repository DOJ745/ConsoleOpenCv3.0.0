// ConsoleOpenCv3.0.0.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "opencv2/opencv.hpp"
#include <opencv2/highgui.hpp>
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
//	// ��� �������� ������� ������ ����������
//	//char* filename = argc >= 2 ? argv[1] : "Image0.jpg";
//	char* filename = "OriginalImage.jpg";
//	// �������� ��������
//	image = cvLoadImage(filename, CV_LOAD_IMAGE_COLOR);
//
//	printf("[i] image: %s\n", filename);
//	assert(image != 0);
//
//	// ������
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
//	// ������ �������
//	int width = templ->width;
//	int height = templ->height;
//
//	// �������� � ������
//	cvShowImage(windowNameOriginalImage, image);
//	cvShowImage(windowNameTemplateImage, templ);
//
//	printf("Template size: %dx%d\n", templ->width, templ->height);
//	printf("Original image size: %dx%d\n", image->width, image->height);
//
//	// ����������� ��� �������� ���������� ���������
//	// ������ ����������: ���� image WxH � templ wxh, �� result = (W - w + 1) x (H - h + 1)
//	IplImage* compareResult = cvCreateImage(
//		cvSize((image->width - templ->width + 1)
//		, (image->height - templ->height + 1))
//		, IPL_DEPTH_32F
//		, 1);
//
//	int methodSQDIFF = CV_TM_SQDIFF;
//	int methodCCORR_NORMED = CV_TM_CCORR_NORMED;
//	// ��������� ����������� � ��������
//	cvMatchTemplate(image, templ, compareResult, methodSQDIFF); // methodSQDIFF
//
//	// ������� ��� ��������
//	cvShowImage(windowNameComapreResult, compareResult);
//
//	// ����������� ������ ��������� ��� ���������
//	// (����� ��������� � ���������� �� �����������)
//	double minval, maxval;
//	CvPoint minloc, maxloc;
//	cvMinMaxLoc(compareResult, &minval, &maxval, &minloc, &maxloc, 0);
//
//	printf("Min value: %f, Max value: %f\n", minval, maxval);
//	printf("Min location: (%d; %d), Max location: (%d; %d)\n", minloc.x, minloc.y, maxloc.x, maxloc.y);
//
//	// ������� ������ ��������� ��� ��������� ���������������
//	cvRectangle(
//		image
//		, cvPoint(minloc.x, minloc.y)
//		, cvPoint(maxloc.x, maxloc.y)
//		, CV_RGB(0, 255, 255)
//		, 1
//		, 8);
//
//	// �����������
//	cvNormalize(compareResult, compareResult, 1, 0, CV_MINMAX);
//	cvNamedWindow(windowNameNormalizedCompareResult, CV_WINDOW_AUTOSIZE);
//	cvShowImage(windowNameNormalizedCompareResult, compareResult);
//
//	// ������� ������������ ������� ���������������
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
//	// ���������� �����������
//	cvShowImage(windowNameFoundMatch, image);
//
//	// ��� ������� �������
//	cvWaitKey(0);
//
//	// ����������� �������
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
	// �������� �����������
	int imgReadMode = cv::IMREAD_COLOR; // IMREAD_COLOR by default
	cv::Mat image = cv::imread("OriginalImage.jpg", imgReadMode);
	cv::Mat templateImage = cv::imread("EyeImage.jpg", imgReadMode);
	cv::Mat pngImage = cv::imread("test.png", cv::IMREAD_GRAYSCALE);

	if (image.empty()) 
	{
		std::cerr << "������ �������� �����������" << std::endl;
		return -1;
	}

	if (templateImage.empty())
	{
		std::cerr << "������ �������� �������" << std::endl;
		return -1;
	}

	if (pngImage.empty())
	{
		std::cerr << "������ �������� png �����������" << std::endl;
		return -1;
	}

	// ����������� � �������� ������
	cv::Mat grayImage; 
	cv::Mat grayTemplate;

	cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY, 1);
	cv::imshow("COLOR_BGR2GRAY grayImage", grayImage);

	if (grayImage.empty() || grayImage.type() != CV_8UC1)
	{
		std::cerr << "������: grayImage ������!\n";
		return -1;
	}

	if (grayImage.type() != CV_8UC1)
	{
		std::cerr << "������: grayImage ����� �������� ������!\n";
		return -1;
	}

	cv::cvtColor(templateImage, grayTemplate, cv::COLOR_BGR2GRAY, 1);
	cv::imshow("COLOR_BGR2GRAY grayTemplate", grayTemplate);

	if (grayTemplate.empty())
	{
		std::cerr << "������: grayTemplate ������!\n";
		return -1;
	}

	if (grayTemplate.type() != CV_8UC1)
	{
		std::cerr << "������: grayTemplate ����� �������� ������!\n";
		return -1;
	}

	cv::Ptr<cv::AKAZE> akaze = cv::AKAZE::create();

	if (akaze.empty())
	{
		std::cerr << "������: ������ AKAZE �� ������" << std::endl;
		return -1;
	}

	
	// ����������� �������� ����� � ���������� ������������
	std::vector<cv::KeyPoint> keypointsImage;
	std::vector<cv::KeyPoint> keypointsTemplate;

	cv::Mat emptyMask;
	cv::Mat descriptorsImage;
	cv::Mat descriptorsTemplate;

	cv::normalize(grayImage, grayImage, 0, 255, cv::NORM_MINMAX);
	cv::normalize(grayTemplate, grayTemplate, 0, 255, cv::NORM_MINMAX);

	cv::imshow("NORMALIZED grayImage", grayImage);
	cv::imshow("NORMALIZED grayTemplate", grayTemplate);

	//cv::Ptr<cv::ORB> orb = cv::ORB::create();
	//orb->detectAndCompute(pngImage, cv::noArray(), keypointsImage, descriptorsImage);

	// ����� ������� ���������� ��� getCPUTime() 
	double startTime, endTime;

	startTime = getCPUTime();
	akaze->detectAndCompute(grayImage, emptyMask, keypointsImage, descriptorsImage);
	endTime = getCPUTime();

	std::cout << "����� �� akaze grayImage (�� getCPUTime): " << endTime - startTime << " ���.\n";

	startTime = getCPUTime();
	akaze->detectAndCompute(grayTemplate, emptyMask, keypointsTemplate, descriptorsTemplate);
	endTime = getCPUTime();

	std::cout << "����� �� akaze grayTemplate (�� getCPUTime): " << endTime - startTime << " ���.\n";

	if (keypointsImage.empty() || keypointsTemplate.empty()) 
	{
		std::cerr << "������: ������������ �������� ����� ��� �������������" << std::endl;
		return -1;
	}

	if (descriptorsImage.empty() || descriptorsTemplate.empty()) 
	{
		std::cerr << "������: ����������� �� �������" << std::endl;
		return -1;
	}

	// ������������� ������������ (Brute-Force)
	cv::BFMatcher matcher(cv::NORM_HAMMING, true);
	std::vector<cv::DMatch> matches;
	matcher.match(descriptorsTemplate, descriptorsImage, matches);

	// ���������� ���������� �� ����������
	std::sort(
		matches.begin()
		, matches.end()
		, [](const cv::DMatch& a, const cv::DMatch& b) { return a.distance < b.distance; } );

	// ���������� ����������
	const double maxDistance = 50.0; // ����� ���������� ��� ����������
	std::vector<cv::DMatch> goodMatches;
	
	for (int i = 0; i < matches.size(); i++) 
	{
		if (matches[i].distance < maxDistance)
		{
			goodMatches.push_back(matches[i]);
		}
	}

	// ������� �������� ����������
	double matchPercentage = (double)goodMatches.size() / keypointsTemplate.size() * 100.0;

	std::cout << "������� ����������: " << matches.size() << "\n";
	std::cout << "����� ���������� �������� ����� � �������: " << keypointsTemplate.size() << "\n";
	std::cout << "���������� ������� ����������: " << goodMatches.size() << "\n";
	std::cout << "������� ����������: " << matchPercentage << "%" << "\n";

	// ������������ ����������
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

	// ����� ����������
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
//	// ����� ����� ��� ������ ������
//	CvPoint pt = cvPoint(height / 8, width / 2);
//
//	// ������ 8-������, 3-��������� ��������
//	IplImage* cvImage = cvCreateImage(cvSize(height, width), 8, 3);
//
//	// �������� �������� ����� ������
//	cvSet(cvImage, cvScalar(255, 255, 255));
//
//	// ������������� ������
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
//	// ��������� ����� ������� �� �������� �����
//	cvPutText(cvImage, "ABCDEFGHIJKLMNOPQRSTYVWXYZ", pt, &font, CV_RGB(255, 0, 150) );
//
//	// ������ ������
//	cvNamedWindow("Hello World", 0);
//	// ���������� �������� � ��������� ����
//	cvShowImage("Hello World", cvImage);
//	// ��� ������� �������
//	cvWaitKey(0);
//
//	// ����������� �������
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