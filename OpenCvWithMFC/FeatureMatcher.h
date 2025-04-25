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
	
	double calculateMatchPercentage() const;
	void showResult();
	void loadMainImage(const std::string& filename);
	void loadCompareImage(const std::string& filename);

	virtual void performAllDistance() = 0;
	virtual void performAllLowe() = 0;
	virtual void performAllCrossCheck() = 0;

	void setMaxDistance(int value)
	{
		m_maxDistance = value;
	}

	void setWindowName(const std::string& windowName)
	{
		m_windowName = windowName;
	}

	void destroyWindow()
	{
		cv::destroyWindow(m_windowName);
	}
};

