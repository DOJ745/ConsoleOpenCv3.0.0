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

void FeatureMatcher::matchFeaturesDistance() 
{
	if (!m_goodMatches.empty())
	{
		m_goodMatches.clear();
	}

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

// ����������, ������� ���������, ��������� �� ����� � ����� ������ (�� ������� ����������� � �� �������) ���������
// ���������� �������������� ������ ���� �������
void FeatureMatcher::matchFeaturesCrossCheck()
{
	if (!m_goodMatches.empty())
	{
		for (size_t i = 0; i < m_goodMatches.size(); i++) 
		{
			// ��������, ��� �������� ���������� ����� ����������
			int queryIdx = m_goodMatches[i].queryIdx;
			int trainIdx = m_goodMatches[i].trainIdx;

			if (std::find_if(m_goodMatches.begin(), m_goodMatches.end(), [trainIdx](const cv::DMatch& m) 
			{
				return m.queryIdx == trainIdx;
			}) != m_goodMatches.end()) 
			{
				m_crossCheckedMatches.push_back(m_goodMatches[i]); // ���� ���������� �������, ���������
			}
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

void FeatureMatcher::visualizeMatchesCrossCheck() 
{
	cv::drawMatches(m_compareImage
		, m_keypointsCompareImg
		, m_mainImage
		, m_keypointsMainImg
		, m_crossCheckedMatches
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

void FeatureMatcher::matchTemplate(int compareMethod /*= cv::TM_CCOEFF_NORMED*/)
{
	// �������� ��������������� ������� ��� �������� ����������� �������������
	cv::Mat result;
	/*int result_cols = m_mainImage.cols - m_compareImage.cols + 1;
	int result_rows = m_mainImage.rows - m_compareImage.rows + 1;
	result.create(result_rows, result_cols, CV_32FC1);*/

	// ���������� ������������� ��������
	//int compareMethod = cv::TM_CCOEFF_NORMED;
	//cv::TM_CCORR_NORMED;
	//cv::TM_SQDIFF_NORMED; // 0,01 - 0,05 - good
	//cv::TM_CCOEFF_NORMED; // traditional from 0.01 to 1.00
	cv::matchTemplate(m_mainImage, m_compareImage, result, compareMethod);

	// ����� ������������� ����������
	double minVal, maxVal;
	cv::Point minLoc, maxLoc;
	cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

	// ��������� �������������� ������ ���������� ����������
	cv::rectangle(m_mainImage, maxLoc, cv::Point(maxLoc.x + m_compareImage.cols, maxLoc.y + m_compareImage.rows), cv::Scalar(0, 255, 0), 2);
	cv::rectangle(m_mainImage, minLoc, cv::Point(minLoc.x + m_compareImage.cols, minLoc.y + m_compareImage.rows), cv::Scalar(15, 100, 130), 2);
}

void FeatureMatcher::normalizeImage(cv::Mat& img)
{
	double minVal = 0;
	double maxVal = 0;

	if (img.channels() == 3)
	{
		changeColorScheme(img, cv::COLOR_BGR2GRAY);
	}

	cv::minMaxLoc(img, &minVal, &maxVal);
	cv::normalize(img, img, 0, 255, cv::NORM_MINMAX);
}
