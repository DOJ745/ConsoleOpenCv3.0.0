// ConsoleOpenCv3.0.0.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "opencv2/opencv.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include "GetCpuTime.h"

//IplImage* image = 0;
//IplImage* templ = 0;
//
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
//		std::cerr << "������ �������� �����������" << std::endl;
//		return -1;
//	}
//
//	if (templateImage.empty())
//	{
//		std::cerr << "������ �������� �������" << std::endl;
//		return -1;
//	}
//
//	if (pngImage.empty())
//	{
//		std::cerr << "������ �������� png �����������" << std::endl;
//		return -1;
//	}
//
//	cv::Ptr<cv::AKAZE> akaze = cv::AKAZE::create();
//
//	if (akaze.empty())
//	{
//		std::cerr << "������: ������ AKAZE �� ������" << std::endl;
//		return -1;
//	}
//
//  // WTA_K = 2: �����������, �������
//  // WTA_K > 2: ����� ���������� � ���� � ����������
//  // ����� �������:
//  // - ���� ������ �������� ����� ���� ���������� ��������, ��� ������������.
//  // - ����� ������� ����� �������� � �������� ����� �������� �����, �� ��� ����� ����� "��������������".
//	cv::Ptr<cv::ORB> orb = cv::ORB::create(
//		2500,					// ������������ ���������� �������� �����
//		1.4f,					// ���������� ������ �������� (���� �������� ����������, ��������� ����������� ����������� �� ������ ������ �������� - ���������� �������� ���������)
//		6,						// ���������� ������� ��������	(���� �������� ���������� ������� ��������. ��� ������ �������, ��� ������ ��������� ����������)
//		100,					// ������ ����	(���� �������� ����� ������ ������� ��� ������ �������� �����. ������� �������� ������� ��� ������� �����������)
//		0,						// ������ ������� ��������
//		2,						// WTA_K - �������� ���������� ���������� ����� � ����������� �������� �����, ������� ������������ ��� ��������� ������ ���� �����������
//		cv::ORB::FAST_SCORE,	// ����� ������
//		64,						// ������ ��������� �������	(���� �������� ���������� ������ ����� ������ ������ �������� �����, ������������� ��� ���������� ������������ - ��� ������, ��� ������ �������������� �������)
//		20						// ����� ������� -  ��� ��������� ��������, ��������������� "���������������" �����
//	);
//
//	if (orb.empty())
//	{
//		std::cerr << "������: ������ ORB �� ������" << std::endl;
//		return -1;
//	}
//	
//	// ����������� �������� ����� � ���������� ������������
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
//		std::cout << "�������� �������� image: [" << minVal << ", " << maxVal << "]\n";
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
//		std::cout << "�������� �������� templateImage: [" << minVal << ", " << maxVal << "]\n";
//	}
//
//	// ����� ������� ���������� ��� getCPUTime() 
//	double startTime, endTime;
//
//	startTime = getCPUTime();
//	akaze->detectAndCompute(image, emptyMask, keypointsImage, descriptorsImage);
//	endTime = getCPUTime();
//
//	std::cout << "����� �� AKAZE image (�� getCPUTime): " << endTime - startTime << " ���.\n";
//
//	startTime = getCPUTime();
//	akaze->detectAndCompute(templateImage, emptyMask, keypointsTemplate, descriptorsTemplate);
//	endTime = getCPUTime();
//
//	std::cout << "����� �� AKAZE templateImage (�� getCPUTime): " << endTime - startTime << " ���.\n";
//
//	if (keypointsImage.empty() || keypointsTemplate.empty()) 
//	{
//		std::cerr << "������: ������������ �������� ����� ��� �������������" << std::endl;
//		return -1;
//	}
//
//	if (descriptorsImage.empty() || descriptorsTemplate.empty()) 
//	{
//		std::cerr << "������: ����������� �� �������" << std::endl;
//		return -1;
//	}
//
//	startTime = getCPUTime();
//	orb->detectAndCompute(image, emptyMask, keypointsImageOrb, descriptorsImageOrb);
//	endTime = getCPUTime();
//
//	std::cout << "����� �� ORB image (�� getCPUTime): " << endTime - startTime << " ���.\n";
//
//	startTime = getCPUTime();
//	orb->detectAndCompute(templateImage, emptyMask, keypointsTemplateOrb, descriptorsTemplateOrb);
//	endTime = getCPUTime();
//
//	std::cout << "����� �� ORB templateImage (�� getCPUTime): " << endTime - startTime << " ���.\n";
//
//	if (keypointsImageOrb.empty() || keypointsTemplateOrb.empty())
//	{
//		std::cerr << "������: ������������ �������� ����� ��� �������������" << std::endl;
//		//return -1;
//	}
//
//	if (descriptorsImageOrb.empty() || descriptorsTemplateOrb.empty())
//	{
//		std::cerr << "������: ����������� �� �������" << std::endl;
//		//return -1;
//	}
//
//	// ������������� ������������ (Brute-Force)
//	cv::BFMatcher matcher(cv::NORM_HAMMING, true);
//	std::vector<cv::DMatch> matches;
//	matcher.match(descriptorsTemplate, descriptorsImage, matches);
//	
//	// ���������� ���������� �� ����������
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
//	// ���������� ����������
//	const double maxDistance = 50.0; // ����� ���������� ��� ���������� (50.0)
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
//	// ������� �������� ����������
//
//	double matchPercentage = (double)goodMatches.size() / keypointsTemplate.size() * 100.0;
//	std::cout << "\n======= AKAZE =======\n\n";
//	std::cout << "������� ���������� AKAZE: " << matches.size() << "\n";
//	std::cout << "����� ���������� �������� ����� � ������� AKAZE: " << keypointsTemplate.size() << "\n";
//	std::cout << "���������� ������� ���������� AKAZE: " << goodMatches.size() << "\n";
//	std::cout << "������� ���������� AKAZE: " << matchPercentage << "%" << "\n";
//
//	double matchPercentageOrb = (double)goodMatchesOrb.size() / keypointsTemplateOrb.size() * 100.0;
//	std::cout << "\n======= ORB =======\n\n";
//	std::cout << "������� ���������� ORB: " << matchesOrb.size() << "\n";
//	std::cout << "����� ���������� �������� ����� � ������� ORB: " << keypointsTemplateOrb.size() << "\n";
//	std::cout << "���������� ������� ���������� ORB: " << goodMatchesOrb.size() << "\n";
//	std::cout << "������� ���������� ORB: " << matchPercentageOrb << "%" << "\n";
//
//	// ������������ ����������
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
//	// ����� ����������
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




//// ===== Basic example with Canny and Gauss filters =====
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
//	const char* wndName = "OpenCV";
//	int width = 640;
//	int height = 480;
//
//	cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
//	cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
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
//		//cv::GaussianBlur(edges, edges, cv::Size(7,7), 1.5, 1.5, cv::BorderTypes::BORDER_DEFAULT); 
//		//cv::Canny(edges, edges, 50, 90, 3);
//
//		//imshow("Camera edges", edges);
//		cv::namedWindow(wndName, cv::WINDOW_AUTOSIZE);
//		imshow(wndName, frame);
//
//		if (cv::waitKey(27) >= 0) 
//		{
//			break;
//		}
//	}
//
//	return 0;
//}



//
// // ===== MANUAL SET KEY POINTS =====
//int main() 
//{
//    cv::Mat image = cv::imread("images/HD-CHIP.jpg", cv::IMREAD_GRAYSCALE);
//
//    if (image.empty()) 
//    {
//        std::cerr << "������: �� ������� ��������� �����������.\n";
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
//    // ������������ �������� �����
//    cv::Mat outputImage;
//    cv::drawKeypoints(image, keypoints, outputImage, cv::Scalar(0, 255, 0), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
//    cv::imshow("Keypoints", outputImage);
//    cv::waitKey(0);
//
//    return 0;
//}




// ===== Apply Rotation by homography =====

cv::Mat templateImg, mainImg, copyMainImg;
std::vector<cv::Point2f> selectedPoints;

int main() 
{
    setlocale(LC_ALL, "Russian");

    int imgReadMode = cv::IMREAD_GRAYSCALE;
    const char* templateImgFile = "images/HD-CHIP-SURFACE.jpg";
    const char* mainImgFile = "images/HD-CHIP-SURFACE-TURN.jpg";//"images/HD-CHIP.jpg";
    
    templateImg = cv::imread(templateImgFile, imgReadMode);

    if (templateImg.empty())
    {
        std::cerr << "������: �� ������� ��������� �����������.\n";
        return -1;
    }

    mainImg = cv::imread(mainImgFile, imgReadMode);

    if (mainImg.empty())
    {
        std::cerr << "������: �� ������� ��������� �����������.\n";
        return -1;
    }

    const double resizeTemplate = 0.5;
    const double resizeMain = 0.5;

    cv::resize(templateImg, templateImg, cv::Size(), resizeTemplate, resizeTemplate, cv::INTER_AREA);
    cv::resize(mainImg, mainImg, cv::Size(), resizeMain, resizeMain, cv::INTER_AREA);

    copyMainImg = mainImg.clone();

    cv::imshow("Select Points", mainImg);

	std::vector<cv::KeyPoint> templateKeypoints, mainImgKeypoints;
    cv::Mat templateDescriptors, mainImgDescriptors;

    cv::Ptr<cv::ORB> orb = cv::ORB::create(
        		500,			        // ������������ ���������� �������� �����
        		1.2f,					// ���������� ������ �������� (���� �������� ����������, ��������� ����������� ����������� �� ������ ������ �������� - ���������� �������� ���������)
        		10,						// ���������� ������� ��������	(���� �������� ���������� ������� ��������. ��� ������ �������, ��� ������ ��������� ����������)
        		10,					    // ������ ����	(���� �������� ����� ������ ������� ��� ������ �������� �����. ������� �������� ������� ��� ������� �����������)
        		0,						// ������ ������� ��������
        		2,						// WTA_K - �������� ���������� ���������� ����� � ����������� �������� �����, ������� ������������ ��� ��������� ������ ���� �����������
        		cv::ORB::FAST_SCORE,	// ����� ������
        		18,						// ������ ��������� �������	(���� �������� ���������� ������ ����� ������ ������ �������� �����, ������������� ��� ���������� ������������ - ��� ������, ��� ������ �������������� �������)
        		50						// ����� ������� -  ��� ��������� ��������, ��������������� "���������������" �����
        	);
    
    cv::Ptr<cv::AKAZE> akaze = cv::AKAZE::create();
    akaze->setThreshold(0.001f);                            // ������� ������ ��� ���������� ������ �������� �����
    akaze->setNOctaves(4);                                  // ���������� ����� (������� ��������)
    akaze->setNOctaveLayers(4);                             // ���������� ���� � ������ ������
    akaze->setDescriptorSize(0);                            // ������ ����������� (�� ��������� ������������)
    akaze->setDescriptorType(cv::AKAZE::DESCRIPTOR_MLDB);   // ��� �����������
    
    try
    {
        orb->detectAndCompute(templateImg, cv::Mat(), templateKeypoints, templateDescriptors);
        //akaze->detectAndCompute(templateImg, cv::Mat(), templateKeypoints, templateDescriptors);

        if (templateKeypoints.empty())
        {
            std::cerr << "������: ������ �������� ����� templateKeypoints ����!\n";
            return -1;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "����������: " << e.what() << "\n";
    }

    try
    {
        orb->detectAndCompute(mainImg, cv::Mat(), mainImgKeypoints, mainImgDescriptors);
        //akaze->detectAndCompute(mainImg, cv::Mat(), mainImgKeypoints, mainImgDescriptors);
        
        if (mainImgKeypoints.empty())
        {
            std::cerr << "������: ������ �������� ����� mainImgKeypoints ����!\n";
            return -1;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "����������: " << e.what() << "\n";
    }

    // ������������ �������� �����, ��������� �� ���������
    cv::Mat templateImgFoundKeypoints;

    cv::drawKeypoints(
        templateImg
        , templateKeypoints
        , templateImgFoundKeypoints
        , cv::Scalar(0, 0, 255)
        , cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

    cv::imshow("Found keypoints on template image", templateImgFoundKeypoints);

    // ���������� ���������� �� ����������
	cv::BFMatcher matcher(cv::NORM_HAMMING, true);
	std::vector<cv::DMatch> matches;
	matcher.match(templateDescriptors, mainImgDescriptors, matches);

    std::sort(
        matches.begin()
        , matches.end()
        , [](const cv::DMatch& a, const cv::DMatch& b) { 
            return a.distance < b.distance; 
        });

    std::vector<cv::DMatch> goodMatches;

    // ���������� ����������
    const double maxDistance = 50.0; // ����� ���������� ��� ���������� �� ����������

    for (int i = 0; i < matches.size(); i++)
    {
        if (matches[i].distance < maxDistance)
        {
            goodMatches.push_back(matches[i]);
        }
    }

    // ���������� ���������� �� Lowe's Ratio Test
    // Lowe's Ratio Test ���������, 
    // �������� �� ���������� �� ������� ���������� 
    // ����������� ������ ���������� �� ������� ���������� ����������
    cv::BFMatcher knnMatcher(cv::NORM_HAMMING);
    std::vector<std::vector<cv::DMatch>> knnMatches;
    knnMatcher.knnMatch(templateDescriptors, mainImgDescriptors, knnMatches, 2);

    // ���� ��������� ���������� ������ ��������� ������ 
    // (������ 0.7 ��� 0.75), ���������� ��������� �������
    const float ratioThresh = 0.75f; // 0.7f - default

    std::vector<cv::DMatch> goodMatchesKnn;

    for (int i = 0; i < knnMatches.size(); i++)
    {
        // ���������, ���� �� ���� �� ��� ����������
        if (knnMatches[i].size() >= 2) 
        { 
            const cv::DMatch& bestMatch = knnMatches[i][0];
            const cv::DMatch& secondBestMatch = knnMatches[i][1];

            // ��������� ������� �� ������������ �������� �������� �����
            if (bestMatch.queryIdx >= 0 
                && bestMatch.queryIdx < templateKeypoints.size() 
                && bestMatch.trainIdx >= 0 
                && bestMatch.trainIdx < mainImgKeypoints.size()) 
            {
                if (bestMatch.distance < ratioThresh * secondBestMatch.distance)
                {
                    goodMatchesKnn.push_back(bestMatch);
                }
            }
        }
    }

    int debugSizeKnn = goodMatchesKnn.size();
    int debugKeypoints1Size = templateKeypoints.size();
    int debugKeypoints2Size = mainImgKeypoints.size();
    int debugSizeDistance = goodMatches.size();

    // ������������ ����������
	cv::Mat matchedImage, matchedImageKnn;

	cv::drawMatches(
        templateImg
		, templateKeypoints
		, mainImg
		, mainImgKeypoints
		, goodMatches
		, matchedImage
		, cv::Scalar::all(-1)
		, cv::Scalar::all(-1)
		, std::vector<char>()
		, cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

    cv::imshow("Matched Keypoints ORB (Distance)", matchedImage);

    //double matchPercentage = (double)goodMatches.size() / (double)templateKeypoints.size() * 100.0;

    double matchPercentage = (double)goodMatches.size() / (double)mainImgKeypoints.size() * 100.0;

    std::cout << "����� ���������� �������� ����� � �������: " << templateKeypoints.size() << "\n";
    std::cout << "����� ���������� �������� ����� � ������������ �����������: " << mainImgKeypoints.size() << "\n";
	std::cout << "������� ���������� (���������� �� ����������): " << matches.size() << "\n";
	std::cout << "���������� ������� ����������: " << goodMatches.size() << "\n";
	std::cout << "������� ����������: " << matchPercentage << "%" << "\n";

    cv::drawMatches(
        templateImg
        , templateKeypoints
        , mainImg
        , mainImgKeypoints
        , goodMatchesKnn
        , matchedImageKnn
        , cv::Scalar::all(-1)
        , cv::Scalar::all(-1)
        , std::vector<char>()
        , cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

    cv::imshow("Matched Keypoints ORB (KNN)", matchedImageKnn);

    //double matchPercentageKnn = (double)goodMatchesKnn.size() / (double)templateKeypoints.size() * 100.0;
    
    double matchPercentageKnn = (double)goodMatchesKnn.size() / (double)mainImgKeypoints.size() * 100.0;

    std::cout << "������� ���������� (���������� �� KNN): " << knnMatches.size() << "\n";
    std::cout << "���������� ������� ����������: " << goodMatchesKnn.size() << "\n";
    std::cout << "������� ����������: " << matchPercentageKnn << "%" << "\n";

	cv::Mat result;
	cv::matchTemplate(mainImg, templateImg, result, cv::TM_CCOEFF_NORMED);

	double minVal, maxVal;
	cv::Point minLoc, maxLoc;
	cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

    std::cout << "��������� ���������� (������������ ����������): " << maxVal << "\n";

    // ��������� �������� ����������
	if (maxVal > 0.8) 
	{ 
		std::cout << "���������� ������������ ����� ��������� ���������." << "\n";
	}
	else 
	{
        std::cout << "���������� �� ������������.\n";
	}

    // ������ �������
    cv::Size templateSize = templateImg.size();

    // ������ ������������� �� ������������ �����������
    cv::rectangle(
        mainImg,
        maxLoc,                                                                     // ������� ����� ����
        cv::Point(maxLoc.x + templateSize.width, maxLoc.y + templateSize.height),   // ������ ������ ����
        cv::Scalar(0, 0, 255),                                                      
        2                                                                           // ������� �����
    );

    cv::imshow("Detected Template Area", mainImg);

    std::vector<cv::Point2f> points1, points2;

    for (size_t i = 0; i < goodMatches.size(); i++)
    {
        points1.push_back(templateKeypoints[goodMatches[i].queryIdx].pt);
        points2.push_back(mainImgKeypoints[goodMatches[i].trainIdx].pt);
    }

    cv::Mat homography = findHomography(points1, points2, cv::RANSAC);

    if (homography.empty())
    {
        std::cout << "���������� �� �������.\n";
        return -1;
    }

    // 5. ���������� ���� ��������
    double angle = atan2(homography.at<double>(1, 0), homography.at<double>(0, 0)) * 180.0 / CV_PI;

    std::cout << "���� ��������: " << angle << " ��������\n";

    cv::Point2f center(copyMainImg.cols / 2.0F, copyMainImg.rows / 2.0F);

    cv::Mat rotationMatrix = getRotationMatrix2D(center, angle, 1.0);

    // ���������� ������ ����������� ����� ��������������
    cv::Rect2f bbox = cv::RotatedRect(cv::Point2f(), copyMainImg.size(), angle).boundingRect();
    rotationMatrix.at<double>(0, 2) += bbox.width / 2.0 - center.x;
    rotationMatrix.at<double>(1, 2) += bbox.height / 2.0 - center.y;

    // ��������� �������
    cv::Mat restoredImg;
    warpAffine(copyMainImg, restoredImg, rotationMatrix, bbox.size());
    imshow("FINAL rotated image", restoredImg);

	cv::Mat resultRotation;
	cv::matchTemplate(restoredImg, templateImg, resultRotation, cv::TM_CCOEFF_NORMED);

	double minValRotation, maxValRotation;
	cv::Point minLocRotation, maxLocRotation;
	cv::minMaxLoc(resultRotation, &minValRotation, &maxValRotation, &minLocRotation, &maxLocRotation);

	std::cout << "��������� ���������� (������������ ����������) ����� ��������: " << maxValRotation << "\n";

	// ������ �������
	cv::Size templateSizeRotation = templateImg.size();

	// ������ ������������� �� �������������� �����������
	cv::rectangle(
		restoredImg,
		maxLocRotation,																								// ������� ����� ����
		cv::Point(maxLocRotation.x + templateSizeRotation.width, maxLocRotation.y + templateSizeRotation.height),   // ������ ������ ����
		cv::Scalar(50, 100, 180),                                                      
		3																											// ������� �����
		);


	cv::Mat tempRotationMatrix = getRotationMatrix2D(center, -angle, 1.0);
	// ���������� ������ ����������� ����� ��������������
	cv::Rect2f bboxTemp = cv::RotatedRect(cv::Point2f(), restoredImg.size(), angle).boundingRect();
	tempRotationMatrix.at<double>(0, 2) += bboxTemp.width / 2.0 - center.x;
	tempRotationMatrix.at<double>(1, 2) += bboxTemp.height / 2.0 - center.y;

	warpAffine(restoredImg, restoredImg, tempRotationMatrix, bbox.size());

	cv::imshow("Detected Template Area Rotation (rotated back)", restoredImg);

    cv::waitKey(0);

    return 0;
}




//// ===== Select ROI by mouse =====
//
//void setPointInImage(const int width, const int height, int& x, int& y)
//{
//    if (x > width)
//    {
//        x = width;
//    }
//
//    if (x < 0)
//    {
//        x = 0;
//    }
//
//    if (y > height)
//    {
//        y = height;
//    }
//
//    if (y < 0)
//    {
//        y = 0;
//    }
//}
//
//bool isDrawing = false;
//cv::Mat templateImg, mainImg, tempImg; 
//cv::Point startPoint, centerPoint, endPoint;
//
//void onMouse(int event, int x, int y, int, void*) 
//{
//    switch (event) 
//    {
//    case cv::EVENT_LBUTTONDOWN:
//        isDrawing = true;
//        startPoint = cv::Point(x, y);
//        cv::circle(tempImg, startPoint, 5, cv::Scalar(0, 255, 0), -1);
//        break;
//
//    case cv::EVENT_MOUSEMOVE:
//
//        if (isDrawing)
//        {
//            tempImg = mainImg.clone();
//            
//            setPointInImage(tempImg.cols, tempImg.rows, x, y);
//
//            endPoint = cv::Point(x, y);
//
//            cv::rectangle(tempImg, startPoint, endPoint, cv::Scalar(0, 255, 0), 2);
//        }
//        break;
//
//    case cv::EVENT_LBUTTONUP:
//        isDrawing = false;
//
//        setPointInImage(tempImg.cols, tempImg.rows, x, y);
//        endPoint = cv::Point(x, y);
//
//        cv::rectangle(tempImg, startPoint, endPoint, cv::Scalar(0, 255, 0), 2);
//        cv::circle(tempImg, endPoint, 5, cv::Scalar(0, 255, 0), -1);
//
//        centerPoint = cv::Point((startPoint.x + endPoint.x) / 2, (startPoint.y + endPoint.y) / 2);
//
//        cv::circle(tempImg, centerPoint, 15, cv::Scalar(255, 0, 0), -1);
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
//        std::cerr << "������: �� ������� ��������� ����������� " << templateImgFile << ".\n";
//        return -1;
//    }
//
//    mainImg = cv::imread(mainImgFile, imgReadMode);
//
//    if (mainImg.empty())
//    {
//        std::cerr << "������: �� ������� ��������� ����������� " << mainImgFile << ".\n";
//        return -1;
//    }
//
//    const double resizeTemplate = 0.20;
//    const double resizeMain = 0.20;
//
//    cv::resize(templateImg, templateImg, cv::Size(), resizeTemplate, resizeTemplate, cv::INTER_AREA);
//    cv::resize(mainImg, mainImg, cv::Size(), resizeMain, resizeMain, cv::INTER_AREA);
//
//    // ����������� ����������� � 3-��������� ������ (BGR) ��� ��������� RGB �������
//    // ����� ���� ��������� ����� ��, ������ ��� �������� ��� ������� ������ ����� ���������
//    cv::cvtColor(mainImg, mainImg, cv::COLOR_GRAY2BGR);
//
//    tempImg = mainImg.clone();
//
//	const char* selectRoiWindow = "Select ROI RECT";
//
//    cv::namedWindow(selectRoiWindow);
//    cv::setMouseCallback(selectRoiWindow, onMouse);
//
//    while (true)
//    {
//        cv::imshow(selectRoiWindow, tempImg);
//
//        char key = cv::waitKey(1);
//
//        if (key == 'q' || key == 27) 
//        {
//            break;
//        }
//    }
//
//    cv::setMouseCallback(selectRoiWindow, NULL);
//
//    // Min ����� ������� ����� ���� ��������������
//    int x1 = std::min(startPoint.x, endPoint.x);
//    int y1 = std::min(startPoint.y, endPoint.y);
//    int x2 = std::max(startPoint.x, endPoint.x);
//    int y2 = std::max(startPoint.y, endPoint.y);
//
//    cv::Rect roiRect(x1, y1, x2 - x1, y2 - y1);
//    cv::Mat imageRoi = mainImg(roiRect);
//
//    cv::imshow("ROI image", imageRoi);
//
//    //const int offset = 0;
//    //cv::Rect imageRoiRect(offset, offset, imageRoi.size().width - offset, imageRoi.size().height - offset);
//
//	// >===< Old style
//
//	//IplImage* mainImgPtrCopy = cvLoadImage(mainImgFile, 1);
//
//	//cvSetImageROI(mainImgPtrCopy, imageRoiRect);
//	//cvAddS(mainImgPtrCopy, cvScalar(200), mainImgPtrCopy);
//	//// ���������� ROI
//	//cvResetImageROI(mainImgPtrCopy);
//	//// ���������� �����������
//	//cvShowImage("ROI colored region", mainImgPtrCopy);
//
//	// >===<
//
//    cv::imshow("Main image", mainImg);
//
//    cv::waitKey(0);
//
//    cv::imwrite("ROI.jpg", imageRoi);
//
//    return 0;
//}



//// ===== Testing scaling =====
//int main() 
//{
//    setlocale(LC_ALL, "Russian");
//
//    const int defaultWidth = 1920;
//    const int defaultHeight = 1080;
//    const int targetWidth = 1024;
//    const int targetHeight = 768;
//    const int figureOffset = 50;
//
//    cv::Mat frame = cv::Mat::zeros(defaultHeight, defaultWidth, CV_8UC3);
//
//    // ������� �������� ����������� (����� ������������� ������ ������ �������)
//    cv::rectangle(frame
//        , cv::Point(figureOffset, figureOffset)
//        , cv::Point(defaultWidth - figureOffset, defaultHeight - figureOffset)
//        , cv::Scalar(255, 0, 0)
//        , -1);
//
//    cv::imshow("Initial image", frame);
//
//    cv::Size targetSize(targetWidth, targetHeight);
//
//    cv::Mat resizedFrameSimpleResize, resizedFrameSaveProportions;
//
//    int frameWidth = frame.cols;
//    int frameHeight = frame.rows;
//
//    double scaleWidth = static_cast<double>(targetSize.width) / frameWidth;
//    double scaleHeight = static_cast<double>(targetSize.height) / frameHeight;
//    double scale = std::min(scaleWidth, scaleHeight);
//
//    // ����� ������ ����������� � ����������� ���������
//    int newWidth = static_cast<int>(frameWidth * scale);
//    int newHeight = static_cast<int>(frameHeight * scale);
//
//    cv::resize(frame, resizedFrameSaveProportions, cv::Size(newWidth, newHeight), 0, 0, cv::INTER_LINEAR);
//
//    // Letterbox
//    cv::Mat outputFrame(targetSize, frame.type(), cv::Scalar(128, 64, 128));
//    int xOffset = (targetSize.width - newWidth) / 2;
//    int yOffset = (targetSize.height - newHeight) / 2;
//
//    // ��������� ���������������� ����������� � ����� �������
//    resizedFrameSaveProportions.copyTo(outputFrame(cv::Rect(xOffset, yOffset, newWidth, newHeight)));
//
//    resizedFrameSaveProportions = outputFrame;
//
//    cv::imshow("Resized Frame with Letterbox (saved proportions)", resizedFrameSaveProportions);
//
//    cv::resize(frame, resizedFrameSimpleResize, targetSize, 0, 0, cv::INTER_LINEAR);
//
//    cv::imshow("Resized Frame without saving proportions", resizedFrameSimpleResize);
//
//    cv::waitKey(0);
//
//    return 0;
//}



//// Different filters
//int main()
//{
//	setlocale(LC_ALL, "Russian");
//	const char* imgFile = "images/HD-CHIP-SURFACE.jpg";
//
//	cv::Mat src = cv::imread(imgFile, cv::IMREAD_GRAYSCALE);
//
//	if (src.empty())
//	{
//		std::cerr << "������: �� ������� ��������� ����������� " << imgFile << ".\n";
//		return -1;
//	}
//
//	// 1. ��������� ������
//	cv::Mat median_filtered;
//	cv::medianBlur(src, median_filtered, 3); // ������ ���� ������ ���� ��������
//
//	// 2. ������������� ������
//	cv::Mat bilateral_filtered;
//	cv::bilateralFilter(src, bilateral_filtered, 9, 75, 75);
//
//	// 3. ���������� ����������� ������� ����
//	cv::Mat otsu_thresholded;
//	double otsu_thresh_val = cv::threshold(src, otsu_thresholded, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
//	std::cout << "�����, ������������ ������� ����: " << otsu_thresh_val << std::endl;
//
//	// 4. ���������� �����������
//	cv::Mat adaptive_thresholded;
//	cv::adaptiveThreshold(src, adaptive_thresholded, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 11, 2);
//
//	cv::Mat adaptive_thresholded_gauss;
//	cv::adaptiveThreshold(src, adaptive_thresholded_gauss, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 11, 2);
//
//	// ����������� �����������
//	cv::imshow("��������", src);
//	cv::imshow("��������� ������", median_filtered);
//	cv::imshow("������������� ������", bilateral_filtered);
//	cv::imshow("����", otsu_thresholded);
//	cv::imshow("���������� �����������", adaptive_thresholded);
//	cv::imshow("���������� ����������� (�����)", adaptive_thresholded_gauss);
//
//	cv::waitKey(0);
//	return 0;
//}


//// ===== New Filters =====
//int main() 
//{
//	setlocale(LC_ALL, "Russian");
//	const char* imgFile = "images/HD-CHIP-SURFACE.jpg";
//	// �������� �����������
//	cv::Mat image = cv::imread(imgFile, cv::IMREAD_GRAYSCALE);
//
//	if (image.empty()) 
//	{
//		std::cerr << "������: �� ������� ��������� �����������!" << std::endl;
//		return -1;
//	}
//
//	// 1. ���������� ������� �������
//	cv::Mat laplacian, absLaplacian;
//	cv::Laplacian(image, laplacian, CV_16S, 3); // ������ ����: 3x3
//	cv::convertScaleAbs(laplacian, absLaplacian); // �������������� � 8-������ �����������
//
//	// 2. ���������� ������� ������
//	cv::Mat gradX, gradY, sobelCombined;
//	cv::Sobel(image, gradX, CV_16S, 1, 0); // �������� �� ��� X
//	cv::Sobel(image, gradY, CV_16S, 0, 1); // �������� �� ��� Y
//	cv::convertScaleAbs(gradX, gradX); // �������������� � 8-������ �����������
//	cv::convertScaleAbs(gradY, gradY);
//
//	// 3. ������ � ���������
//	cv::Mat erodeImg, dilateImg;
//	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
//	cv::erode(image, erodeImg, kernel);
//	cv::dilate(image, dilateImg, kernel);
//
//	// ����������� ����������
//	cv::addWeighted(gradX, 0.5, gradY, 0.5, 0, sobelCombined);
//
//	// ����������� �����������
//	cv::imshow("Original Image", image);
//	cv::imshow("Laplacian Filter", absLaplacian);
//	cv::imshow("Sobel Filter", sobelCombined);
//	cv::imshow("Erode Filter", erodeImg);
//	cv::imshow("Dilate Filter", dilateImg);
//
//	cv::waitKey(0);
//	return 0;
//}




//// ===== Select ROI by mouse  MODIFIED=====
//
//void setPointInImage(const int width, const int height, int& x, int& y)
//{
//    if (x > width)
//    {
//        x = width;
//    }
//
//    if (x < 0)
//    {
//        x = 0;
//    }
//
//    if (y > height)
//    {
//        y = height;
//    }
//
//    if (y < 0)
//    {
//        y = 0;
//    }
//}
//
//bool isDrawing = false;
//cv::Mat templateImg, mainImg, tempImg;
//cv::Point startPoint, centerPoint, endPoint;
//const char* selectRoiWindow = "Select ROI RECT";
//
//void onMouse(int event, int x, int y, int, void* data) 
//{
//    switch (event) 
//    {
//    case cv::EVENT_LBUTTONDOWN:
//        isDrawing = true;
//        startPoint = cv::Point(x, y);
//        cv::circle(tempImg, startPoint, 5, cv::Scalar(0, 255, 0), -1);
//
//        break;
//
//    case cv::EVENT_MOUSEMOVE:
//		tempImg = mainImg.clone();
//
//        if (isDrawing)
//        {
//            setPointInImage(tempImg.cols, tempImg.rows, x, y);
//
//            endPoint = cv::Point(x, y);
//
//            cv::rectangle(tempImg, startPoint, endPoint, cv::Scalar(0, 255, 0), 2);
//        }
//		cv::imshow(selectRoiWindow, tempImg);
//        break;
//
//    case cv::EVENT_LBUTTONUP:
//        isDrawing = false;
//
//        setPointInImage(tempImg.cols, tempImg.rows, x, y);
//        endPoint = cv::Point(x, y);
//
//        cv::rectangle(tempImg, startPoint, endPoint, cv::Scalar(0, 255, 0), 2);
//        cv::circle(tempImg, endPoint, 5, cv::Scalar(0, 255, 0), -1);
//
//        centerPoint = cv::Point((startPoint.x + endPoint.x) / 2, (startPoint.y + endPoint.y) / 2);
//
//        cv::circle(tempImg, centerPoint, 15, cv::Scalar(255, 0, 0), -1);
//		cv::imshow(selectRoiWindow, tempImg);
//        break;
//    }
//}
//
//int main()
//{
//    setlocale(LC_ALL, "Russian");
//	cv::VideoCapture capture;
//
//	cv::namedWindow(selectRoiWindow);
//	cv::setMouseCallback(selectRoiWindow, onMouse);
//
//	if (!capture.open(0))
//	{
//		return -1;
//	}
//
//	capture.open(0);
//
//    while (true)
//    {
//		capture >> mainImg;
//		cv::cvtColor(mainImg, mainImg, cv::COLOR_BGR2RGB);
//		tempImg = mainImg.clone();
//		
//        cv::imshow(selectRoiWindow, tempImg);
//
//        char key = cv::waitKey(1);
//
//        if (key == 'q' || key == 27) 
//        {
//            break;
//        }
//    }
//
//	cv::setMouseCallback(selectRoiWindow, NULL);
//
//    // Min ����� ������� ����� ���� ��������������
//    int x1 = std::min(startPoint.x, endPoint.x);
//    int y1 = std::min(startPoint.y, endPoint.y);
//    int x2 = std::max(startPoint.x, endPoint.x);
//    int y2 = std::max(startPoint.y, endPoint.y);
//
//    cv::Rect roiRect(x1, y1, x2 - x1, y2 - y1);
//    cv::Mat imageRoi = mainImg(roiRect);
//
//    cv::imshow("ROI image", imageRoi);
//
//    cv::imshow("Main image", mainImg);
//
//	cv::imshow("Temp image", tempImg);
//
//    cv::waitKey(0);
//
//    return 0;
//}


//// Blob detection algorythms
//int main()
//{
//    setlocale(LC_ALL, "Russian");
//
//	const char* imgFile = "images/HD-CHIP-SURFACE.jpg";
//	double startTime, endTime;
//
//	cv::Mat img = cv::imread(imgFile, cv::IMREAD_GRAYSCALE);
//
//	if (img.empty()) 
//	{
//		std::cerr << "������: �� ������� ��������� �����������!" << std::endl;
//		return -1;
//	}
//
//	cv::normalize(img, img, 0, 255, cv::NORM_MINMAX);
//
//	// ���������� LoG �������
//	cv::Mat LoG;
//	cv::Laplacian(img, LoG, CV_32F);
//	cv::GaussianBlur(LoG, LoG, cv::Size(5, 5), 0);
//
//	// ���������� DoG �������
//	cv::Mat DoG1 = cv::Mat::zeros(img.size(), CV_32F);
//	cv::Mat DoG2 = cv::Mat::zeros(img.size(), CV_32F);
//	cv::GaussianBlur(img, DoG1, cv::Size(3, 3), 0);
//	cv::GaussianBlur(img, DoG2, cv::Size(7, 7), 0);
//	DoG1 -= DoG2;
//
//	cv::GaussianBlur(img, DoG1, cv::Size(5, 5), 0);
//	cv::GaussianBlur(img, DoG2, cv::Size(11, 11), 0);
//	DoG2 -= DoG1;
//
//	// ���������� DoH �������
//	cv::Mat DoH;
//	cv::GaussianBlur(img, DoH, cv::Size(5, 5), 0);
//
//	cv::Mat Dxx, Dyy, Dxy;
//	cv::Sobel(DoH, Dxx, CV_64F, 2, 0);
//	cv::Sobel(DoH, Dyy, CV_64F, 0, 2);
//	cv::Sobel(DoH, Dxy, CV_64F, 1, 1);
//	DoH = Dxx.mul(Dyy) - Dxy.mul(Dxy);
//
//	// ��������� ���������� ��������� ������
//	cv::SimpleBlobDetector::Params params;
//	params.filterByArea = true;
//	params.minArea = 10;
//	params.filterByCircularity = false;
//	params.filterByConvexity = false;
//	params.filterByInertia = false;
//
//	// �������� ��������� ������
//	auto detector = cv::SimpleBlobDetector::create(params);
//
//	cv::normalize(LoG, LoG, 0, 255, cv::NORM_MINMAX);
//	LoG.convertTo(LoG, CV_8U);
//	//cv::adaptiveThreshold(LoG, LoG, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 11, 10);
//	cv::threshold(LoG, LoG, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
//	cv::imshow("Binary LoG", LoG);
//
//	cv::normalize(DoH, DoH, 0, 255, cv::NORM_MINMAX);
//	DoH.convertTo(DoH, CV_8U);
//	//cv::threshold(DoH, DoH, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
//	cv::imshow("Binary DoH", DoH);
//
//	// ����������� �������� �����
//	std::vector<cv::KeyPoint> keypoints_LoG, keypoints_DoG1, keypoints_DoG2, keypoints_DoH;
//
//	startTime = getCPUTime();
//	detector->detect(LoG, keypoints_LoG);
//	endTime	= getCPUTime();
//	std::cout << "����� �� LoG (�� getCPUTime): " << endTime - startTime << " ���.\n";
//
//	startTime = getCPUTime();
//	detector->detect(DoG1, keypoints_DoG1);
//	endTime	= getCPUTime();
//	std::cout << "����� �� DoG1 (�� getCPUTime): " << endTime - startTime << " ���.\n";
//
//	startTime = getCPUTime();
//	detector->detect(DoG2, keypoints_DoG2);
//	endTime	= getCPUTime();
//	std::cout << "����� �� DoG2 (�� getCPUTime): " << endTime - startTime << " ���.\n";
//
//	startTime = getCPUTime();
//	detector->detect(DoH, keypoints_DoH);
//	endTime	= getCPUTime();
//	std::cout << "����� �� DoH (�� getCPUTime): " << endTime - startTime << " ���.\n";
//
//	// ��������� ������������ ������ �� �������� �����������
//	cv::Mat img_with_keypoints_LoG, img_with_keypoints_DoG1, img_with_keypoints_DoG2, img_with_keypoints_DoH;
//	cv::drawKeypoints(img, keypoints_LoG, img_with_keypoints_LoG, cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
//	cv::drawKeypoints(img, keypoints_DoG1, img_with_keypoints_DoG1, cv::Scalar(0, 255, 0), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
//	cv::drawKeypoints(img, keypoints_DoG2, img_with_keypoints_DoG2, cv::Scalar(255, 0, 0), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
//	cv::drawKeypoints(img, keypoints_DoH, img_with_keypoints_DoH, cv::Scalar(0, 255, 255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
//
//	// ����������� �����������
//	cv::imshow("LoG", img_with_keypoints_LoG);
//	cv::imshow("DoG1", img_with_keypoints_DoG1);
//	cv::imshow("DoG2", img_with_keypoints_DoG2);
//	cv::imshow("DoH", img_with_keypoints_DoH);
//
//	cv::waitKey(0);
//	cv::destroyAllWindows();
//
//	return 0;
//}

//// ���������� ����� ����� ������� �� ������ ������
//
//// ������� ��� �������� ���������� ��������� � �����������
//cv::Point3f pixelToMetric(int x, int y, float distance, float focalLength) 
//{
//	float X = static_cast<float>(x) * distance / focalLength;
//	float Y = static_cast<float>(y) * distance / focalLength;
//	return cv::Point3f(X, Y, distance);
//}
//
//int main() 
//{
//	// ��������� ������
//	float focalLength = 800.0f;		// �������� ���������� � ��������
//	float distanceToObject1 = 0.5f; // ���������� �� ������� �� ������ ����� (� ������)
//	float distanceToObject2 = 0.5f; // ���������� �� ������� �� ������ ����� (� ������)
//
//	// ���������� ����� �� ������ � ������ ������ (� ��������)
//	cv::Point point1(0, 0);		// ����� �� ������ �����
//	cv::Point point2(0, 800);	// ����� �� ������ �����
//
//	// ������� ���������� ��������� � �����������
//	cv::Point3f metricPoint1 = pixelToMetric(point1.x, point1.y, distanceToObject1, focalLength);
//	cv::Point3f metricPoint2 = pixelToMetric(point2.x, point2.y, distanceToObject2, focalLength);
//
//	// ���������� ���������� ����� ������� � ������������
//	float deltaX = metricPoint2.x - metricPoint1.x;
//	float deltaY = metricPoint2.y - metricPoint1.y;
//	float deltaZ = metricPoint2.z - metricPoint1.z;
//
//	float distanceBetweenPoints = std::sqrt(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ);
//
//	// ����� ����������
//	std::cout << "Distance between points: " << distanceBetweenPoints << " meters" << std::endl;
//
//	system("pause");
//
//	return 0;
//}



//// ===== Image from camera =====
//int main()
//{
//	std::cout << "Hello, world!\n";
//
//	cv::VideoCapture cap(0);
//
//	if(!cap.isOpened())			
//	{
//		std::cerr << "Failed to open device!\n";
//		return -1;
//	}
//
//	const char* wndName = "OpenCV Camera";
//	int width = 1280;
//	int height = 1024;
//
//	cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
//	cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
//
//	cv::Mat frame;
//
//	cv::namedWindow(wndName, cv::WINDOW_AUTOSIZE);
//
//	while(true)
//	{
//		cap >> frame;		
//
//		if (frame.empty()) 
//		{
//			std::cerr << "Failed to capture frame!\n";
//			break;
//		}
//
//		imshow(wndName, frame);
//
//		int key = cv::waitKey(1);
//
//		if (key >= 0) 
//		{
//			break;
//		}
//	}
//
//	return 0;
//}