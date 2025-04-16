#include "StdAfx.h"
#include "FeatureMatcher.h"


FeatureMatcher::FeatureMatcher(const std::string& templatePath
	, const std::string& currentPath
	, double distance)
	: m_maxDistance(distance)
	, m_windowName("MATCHED IMAGE")
{
		m_templateImage = cv::imread(templatePath, cv::IMREAD_GRAYSCALE);
		m_currentImage = cv::imread(currentPath, cv::IMREAD_GRAYSCALE);

		normalizeImages();
}

void FeatureMatcher::normalizeImages() 
{
	double minVal = 0;
	double maxVal = 0;

	if (m_templateImage.channels() == 3)
	{
		cv::cvtColor(m_templateImage, m_templateImage, cv::COLOR_BGR2GRAY);
	}
		
	if (m_currentImage.channels() == 3)
	{
		cv::cvtColor(m_currentImage, m_currentImage, cv::COLOR_BGR2GRAY);
	}
		
	cv::minMaxLoc(m_templateImage, &minVal, &maxVal);
	cv::normalize(m_templateImage, m_templateImage, 0, 255, cv::NORM_MINMAX);

	cv::minMaxLoc(m_currentImage, &minVal, &maxVal);
	cv::normalize(m_currentImage, m_currentImage, 0, 255, cv::NORM_MINMAX);
}

void FeatureMatcher::matchFeatures() 
{
	cv::BFMatcher matcher(cv::NORM_HAMMING);

	matcher.match(m_descriptorsTemplate, m_descriptorsCurrent, m_matches);

	std::sort(m_matches.begin(), m_matches.end(), 
		[](const cv::DMatch& a, const cv::DMatch& b) 
	{ 
			return a.distance < b.distance; 
	});

	for (int i = 0; i < m_matches.size(); i++) 
	{
		if (m_matches[i].queryIdx < m_keypointsTemplate.size() && 
			m_matches[i].trainIdx < m_keypointsCurrent.size() &&
			m_matches[i].distance < m_maxDistance) 
		{
				m_goodMatches.push_back(m_matches[i]);
		}
	}
}

void FeatureMatcher::visualizeMatches() 
{
	cv::drawMatches(m_templateImage
		, m_keypointsTemplate
		, m_currentImage
		, m_keypointsCurrent
		, m_goodMatches
		, m_resultImage
		, cv::Scalar::all(-1)
		, cv::Scalar::all(-1)
		, std::vector<char>()
		, cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
}

double FeatureMatcher::calculateMatchPercentage() const 
{
	return (m_keypointsTemplate.empty()) ? 0.0 : 
		(static_cast<double>(m_goodMatches.size()) / m_keypointsTemplate.size()) * 100.0;
}

void FeatureMatcher::showResult() 
{
	cv::imshow(m_windowName, m_resultImage);
}


FeatureMatcher::~FeatureMatcher()
{
	destroyWindow();
}
