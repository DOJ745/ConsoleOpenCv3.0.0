#pragma once
#include "stdafx.h"

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <ctime>

class FeatureMatcher
{
protected:
	cv::Mat m_templateImage;
	cv::Mat m_currentImage;
	std::vector<cv::KeyPoint> m_keypointsTemplate;
	std::vector<cv::KeyPoint> m_keypointsCurrent;
	cv::Mat m_descriptorsTemplate;
	cv::Mat m_descriptorsCurrent;
	double m_maxDistance;
	std::vector<cv::DMatch> m_matches;
	std::vector<cv::DMatch> m_goodMatches;
	cv::Mat m_resultImage;
	std::string m_windowName;

	void normalizeImages();
	virtual void detectAndComputeCurrent() = 0;
	virtual void detectAndComputeTemplate() = 0;

public:
	FeatureMatcher(const std::string& templatePath
		, const std::string& currentPath
		, double distance);

	virtual ~FeatureMatcher(void) = 0;
	virtual void matchFeatures();
	virtual void visualizeMatches();
	double calculateMatchPercentage() const;
	void showResult();

	virtual void performAll() = 0;

	void setWindowName(const std::string& windowName)
	{
		m_windowName = windowName;
	}

	void destroyWindow()
	{
		cv::destroyWindow(m_windowName);
	}
};

