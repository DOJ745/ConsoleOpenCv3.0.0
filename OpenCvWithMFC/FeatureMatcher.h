#pragma once
#include "stdafx.h"

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <ctime>

class FeatureMatcher
{
protected:
	cv::Mat m_compareImage;
	cv::Mat m_mainImage;
	std::vector<cv::KeyPoint> m_keypointsCompareImg;
	std::vector<cv::KeyPoint> m_keypointsMainImg;
	cv::Mat m_descriptorsCompareImg;
	cv::Mat m_descriptorsMainImg;
	double m_maxDistance;
	std::vector<cv::DMatch> m_matches;
	std::vector<cv::DMatch> m_goodMatches;
	std::vector<cv::DMatch> m_crossCheckedMatches;
	std::vector<std::vector<cv::DMatch>> m_knnMatches;
	cv::Mat m_resultImage;
	std::string m_windowName;

	void normalizeMainImage();
	void normalizeCompareImage();
	void normalizeImages();
	virtual void detectAndComputeMainImg() = 0;
	virtual void detectAndComputeCompareImg() = 0;

public:
	FeatureMatcher()
		: m_compareImage(cv::Mat::zeros(480, 640, CV_8UC3))
		, m_mainImage(cv::Mat::zeros(480, 640, CV_8UC3))
		, m_resultImage(cv::Mat::zeros(480, 640, CV_8UC3))
	{
		TRACE("======>[FeatureMatcher] empty constructor\n");
	};


	FeatureMatcher(const std::string& templatePath
		, const std::string& currentPath
		, double distance);

	virtual ~FeatureMatcher(void) = 0;
	virtual void visualizeMatches();
	virtual void visualizeMatchesCrossCheck();
	virtual void matchFeaturesDistance();
	virtual void matchFeaturesLowe() = 0;
	virtual void matchFeaturesCrossCheck();

	void matchTemplate(int compareMethod = cv::TM_CCOEFF_NORMED);
	
	double calculateMatchPercentage() const;
	void showResult();
	void loadMainImage(const std::string& filename);
	void loadCompareImage(const std::string& filename);

	virtual void performAllDistance() = 0;
	virtual void performAllLowe() = 0;
	virtual void performAllCrossCheck() = 0;
	void normalizeImage(cv::Mat& img);

	void setMaxDistance(int value)
	{
		m_maxDistance = value;
	}

	void setWindowName(const std::string& windowName)
	{
		m_windowName = windowName;
	}

	void setMainImage(cv::Mat img)
	{
		m_mainImage = img.clone();
		normalizeImage(m_mainImage);
	}

	void setCompareImage(cv::Mat img)
	{
		m_compareImage = img.clone();
		normalizeImage(m_compareImage);
	}

	cv::Mat getMainImage() const
	{
		return m_mainImage;
	}

	cv::Mat getCompareImage() const
	{
		return m_compareImage;
	}

	void changeColorScheme(cv::Mat& img, int colorScheme = cv::COLOR_RGB2GRAY)
	{
		cv::cvtColor(img, img, colorScheme);
	}

	void destroyWindow()
	{
		cv::destroyWindow(m_windowName);
	}

	void rotateAndCompareByDistance()
	{
		std::vector<cv::Point2f> points1, points2;

		for (size_t i = 0; i < m_goodMatches.size(); i++)
		{
			points1.push_back(m_keypointsCompareImg[m_goodMatches[i].queryIdx].pt);
			points2.push_back(m_keypointsMainImg[m_goodMatches[i].trainIdx].pt);
		}

		cv::Mat homography = findHomography(points1, points2, cv::RANSAC);

		if (homography.empty())
		{
			TRACE("Гомография не найдена.\n");
			return;
		}

		//Извлечение угла поворота
		double angle = atan2(homography.at<double>(1, 0), homography.at<double>(0, 0)) * 180.0 / CV_PI;

		TRACE("Угол поворота: %f.02 градусов\n", angle);

		cv::Point2f center(m_mainImage.cols / 2.0F, m_mainImage.rows / 2.0F);

		cv::Mat rotationMatrix = getRotationMatrix2D(center, angle, 1.0);

		// Определяем размер изображения после преобразования
		cv::Rect2f bbox = cv::RotatedRect(cv::Point2f(), m_mainImage.size(), angle).boundingRect();
		rotationMatrix.at<double>(0, 2) += bbox.width / 2.0 - center.x;
		rotationMatrix.at<double>(1, 2) += bbox.height / 2.0 - center.y;

		// Применяем поворот к изображению
		cv::Mat restoredImg;
		warpAffine(m_mainImage, restoredImg, rotationMatrix, bbox.size());
		imshow("FINAL rotated image", restoredImg);
	}
};

