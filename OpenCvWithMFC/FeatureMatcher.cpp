#include "StdAfx.h"
#include "FeatureMatcher.h"


FeatureMatcher::FeatureMatcher(const std::string& templatePath
	, const std::string& currentPath
	, double distance)
	: m_maxDistance(distance)
	, m_windowName("MATCHED IMAGE")
{
		m_compareImage = cv::imread(templatePath, cv::IMREAD_GRAYSCALE);

		if (m_compareImage.empty())
		{
			TRACE("======>[!] Compare image is empty!");
		}

		m_mainImage = cv::imread(currentPath, cv::IMREAD_GRAYSCALE);

		if (m_mainImage.empty())
		{
			TRACE("======>[!] Main image is empty!");
		}

		normalizeImages();
}

void FeatureMatcher::normalizeMainImage()
{
	double minVal = 0;
	double maxVal = 0;

	if (m_mainImage.channels() == 3)
	{
		cv::cvtColor(m_mainImage, m_mainImage, cv::COLOR_BGR2GRAY);
	}

	cv::minMaxLoc(m_mainImage, &minVal, &maxVal);
	cv::normalize(m_mainImage, m_mainImage, 0, 255, cv::NORM_MINMAX);
}

void FeatureMatcher::normalizeCompareImage()
{
	double minVal = 0;
	double maxVal = 0;

	if (m_compareImage.channels() == 3)
	{
		cv::cvtColor(m_compareImage, m_compareImage, cv::COLOR_BGR2GRAY);
	}

	cv::minMaxLoc(m_compareImage, &minVal, &maxVal);
	cv::normalize(m_compareImage, m_compareImage, 0, 255, cv::NORM_MINMAX);
}

void FeatureMatcher::normalizeImages() 
{
	normalizeMainImage();
	normalizeCompareImage();
}

void FeatureMatcher::matchFeatures() 
{
	cv::BFMatcher matcher(cv::NORM_HAMMING);

	matcher.match(m_descriptorsCompareImg, m_descriptorsMainImg, m_matches);

	std::sort(m_matches.begin(), m_matches.end(), 
		[](const cv::DMatch& a, const cv::DMatch& b) 
	{ 
			return a.distance < b.distance; 
	});

	for (int i = 0; i < m_matches.size(); i++) 
	{
		if (m_matches[i].queryIdx < m_keypointsCompareImg.size() && 
			m_matches[i].trainIdx < m_keypointsMainImg.size() &&
			m_matches[i].distance < m_maxDistance) 
		{
				m_goodMatches.push_back(m_matches[i]);
		}
	}
}

void FeatureMatcher::visualizeMatches() 
{
	cv::drawMatches(m_compareImage
		, m_keypointsCompareImg
		, m_mainImage
		, m_keypointsMainImg
		, m_goodMatches
		, m_resultImage
		, cv::Scalar::all(-1)
		, cv::Scalar::all(-1)
		, std::vector<char>()
		, cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
}

double FeatureMatcher::calculateMatchPercentage() const 
{
	return (m_keypointsCompareImg.empty()) ? 0.0 : 
		(static_cast<double>(m_goodMatches.size()) / m_keypointsCompareImg.size()) * 100.0;
}

void FeatureMatcher::showResult() 
{
	cv::imshow(m_windowName, m_resultImage);
}


FeatureMatcher::~FeatureMatcher()
{
	destroyWindow();
}

void FeatureMatcher::loadMainImage(const std::string& filename)
{
	m_mainImage = cv::imread(filename, cv::IMREAD_GRAYSCALE);

	if (m_mainImage.empty())
	{
		TRACE("======>[!] Main image is empty!");
	}
	else
	{
		normalizeMainImage();
	}
}

void FeatureMatcher::loadCompareImage(const std::string& filename)
{
	m_compareImage = cv::imread(filename, cv::IMREAD_GRAYSCALE);

	if (m_compareImage.empty())
	{
		TRACE("======>[!] Compare image is empty!");
	}
	else
	{
		normalizeCompareImage();
	}
}
