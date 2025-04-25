#pragma once
#include "FeatureMatcher.h"

/*
	������������� ���������:

	- threshold (0.001�0.005) (0.003 � �����) - ���������������� � ������� (������ => ������ �����) 

	- nOctaves (4�5) - ���������� ������������ 

	- descriptorSize (0 [Bit]) � �������� ������������

	- nOctaveLayers ������� �������� ��������� ��������� ������������ ����� ������ ������, ���������� ���������������� 

	- m_descriptorChannels ����������, ������� ������� ���������� ������������ ��� ���������� ����������� (�� 1 �� 3-�)
		�������� 1: ������������ ������ ���� �����, ��� ������� ������ ����������� � �������� ���������, �� ����� ��������� ������������������.
		�������� 3: ������������ ��� ������, ���������� ����� ���������� � ������������ ������� �������� �������������, �� ����� ���������� ������ ����������� � ����� ���������.

	- m_descriptorType ������ �� ������������ � ��������; DESCRIPTOR_MLDB_UPRIGHT � ���� ������������ ������ ����������.
*/


class AkazeMatcher : public FeatureMatcher
{
private:
   cv::Ptr<cv::AKAZE> m_akaze;
   float m_threshold;
   int m_nOctaves;
   int m_nOctaveLayers;
   int m_descriptorSize;
   int m_descriptorChannels;
   int m_descriptorType;

protected:
	void detectAndComputeMainImg() override
	{
		cv::Mat emptyMask;
		clock_t start = clock();
		m_akaze->detectAndCompute(m_mainImage, emptyMask, m_keypointsMainImg, m_descriptorsMainImg);
		clock_t end = clock();

		double time = (double)(end - start) / CLOCKS_PER_SEC;
		TRACE("AKAZE Detection time MAIN IMG: %.6f sec\n", time);
	}

	void detectAndComputeCompareImg() override
	{
		cv::Mat emptyMask;
		clock_t start = clock();
		m_akaze->detectAndCompute(m_compareImage, emptyMask, m_keypointsCompareImg, m_descriptorsCompareImg);
		clock_t end = clock();

		double time = (double)(end - start) / CLOCKS_PER_SEC;
		TRACE("AKAZE Detection time COMPARE IMG: %.6f sec\n", time);
	}


public:
	AkazeMatcher(): FeatureMatcher(), m_akaze(cv::AKAZE::create())
	{
	};

	AkazeMatcher(const std::string& templatePath, const std::string& currentPath, double distance)
		: FeatureMatcher(templatePath, currentPath, distance), m_akaze(cv::AKAZE::create()) 
	{
	};

	void updateDetector() 
	{
		m_akaze = cv::AKAZE::create(m_descriptorType,
			m_descriptorSize,
			m_descriptorChannels,
			m_threshold,
			m_nOctaves,
			m_nOctaveLayers);
	}

	// �������
	float getThreshold() const { return m_threshold; }
	int getNOctaves() const { return m_nOctaves; }
	int getNOctaveLayers() const { return m_nOctaveLayers; }
	int getDescriptorSize() const { return m_descriptorSize; }
	int getDescriptorChannels() const { return m_descriptorChannels; }
	int getDescriptorType() const { return m_descriptorType; }

	// �������
	void AkazeMatcher::setThreshold(float value) 
	{
		if (value > 0.0f) 
		{
			m_threshold = value;
			updateDetector();
		}
	}

	void AkazeMatcher::setNOctaves(int value) 
	{
		if (value > 0) 
		{
			m_nOctaves = value;
			updateDetector();
		}
	}

	void AkazeMatcher::setNOctaveLayers(int value) 
	{
		if (value > 0) 
		{
			m_nOctaveLayers = value;
			updateDetector();
		}
	}

	void AkazeMatcher::setDescriptorSize(int value) 
	{
		if (value >= 0) 
		{	// 0 �������� ������������ ������
			m_descriptorSize = value;
			updateDetector();
		}
	}

	void AkazeMatcher::setDescriptorChannels(int value) 
	{
		if (value == 1 || value == 2 || value == 3) 
		{
			m_descriptorChannels = value;
			updateDetector();
		}
	}

	void AkazeMatcher::setDescriptorType(int value) 
	{
		if (value == cv::AKAZE::DESCRIPTOR_KAZE ||
			value == cv::AKAZE::DESCRIPTOR_KAZE_UPRIGHT ||
			value == cv::AKAZE::DESCRIPTOR_MLDB ||
			value == cv::AKAZE::DESCRIPTOR_MLDB_UPRIGHT) 
		{
				m_descriptorType = value;
				updateDetector();
		}
	}

	void createDetector(float threshold = 0.001f
		, int nOctaves = 3
		, int nOctaveLayers = 4
		, int descriptorSize = 0
		, int descriptorChannels = 3
		, int descriptorType = cv::AKAZE::DESCRIPTOR_MLDB) 
	{
		m_akaze = cv::AKAZE::create(descriptorType
			, descriptorSize
			, descriptorChannels
			, threshold
			, nOctaves
			, nOctaveLayers);
	}

	void detectAndComputeAll()
	{
		detectAndComputeMainImg();
		detectAndComputeCompareImg();
	};

	void performAllDistance() override
	{
		createDetector();
		detectAndComputeMainImg();
		detectAndComputeCompareImg();
		matchFeaturesDistance();
		visualizeMatches();
		setWindowName("[AKAZE] Distance Matches");
		showResult();
		CString tempMsg;
		tempMsg.Format(L"AKAZE match percentage DISTANCE: %.2f%%", calculateMatchPercentage());
		AfxMessageBox(tempMsg, MB_OK | MB_ICONINFORMATION);
	}

	void performAllLowe() override
	{
		createDetector();
		detectAndComputeMainImg();
		detectAndComputeCompareImg();
		matchFeaturesLowe();
		visualizeMatches();
		TRACE("AKAZE match percentage LOWE: %.2f%%\n", calculateMatchPercentage());
		setWindowName("[AKAZE] Lowe Matches");
		showResult();
	}

	void performAllCrossCheck() override
	{
		clock_t start = clock();
		createDetector();
		detectAndComputeMainImg();
		detectAndComputeCompareImg();
		matchFeaturesDistance();
		matchFeaturesCrossCheck();
		clock_t end = clock();

		double time = (double)(end - start) / CLOCKS_PER_SEC;
		TRACE("AKAZE perform all CrossCheck duration: %.6f sec\n", time);

		visualizeMatchesCrossCheck();
		TRACE("AKAZE match percentage DISTANCE (CROSS CHECK): %.2f%%\n", calculateMatchPercentage());
		setWindowName("[AKAZE] Distance cross-checked Matches");
		showResult();
	}

	virtual ~AkazeMatcher(void)
	{
		TRACE("======>[AKAZE] destroying object...\n");
	}

	/*
	����� ���������� ����������, ������� �������� ������� ������ ���������� ����� �������������. 
	���� ����������� � ���, ����� ����������� �������� ����� � ����� �������� � ����������� ������� �� ������, 
	� ����� ������������� ��, ������� ����� ������� ������� ���������� 
	(�� ��� ��� ���������� � �����������, ������� ������ ��� 0.75 ���������� �� ������� ���������� ������)
	*/
	void matchFeaturesLowe()
	{
		if (!m_goodMatches.empty())
		{
			m_goodMatches.clear();
		}

		// ������������� ������� � ������������ ��������� (crossCheck)
		cv::BFMatcher matcher(cv::NORM_HAMMING, false);

		// ����� ���� ��������� ���������� ��� ������ �����
		matcher.knnMatch(m_descriptorsMainImg, m_descriptorsCompareImg, m_knnMatches, 2);

		const float loweRatio = 0.95f;
		std::vector<cv::DMatch> good_matches;

		for (size_t i = 0; i < m_knnMatches.size(); i++) 
		{
			if (m_knnMatches[i].size() >= 2) 
			{
				const cv::DMatch& bestMatch = m_knnMatches[i][0];
				const cv::DMatch& betterMatch = m_knnMatches[i][1];

				// ��������� ������� �� ������������ �������� �������� �����
				if (bestMatch.queryIdx >= 0 
					&& bestMatch.queryIdx < m_keypointsCompareImg.size() 
					&& bestMatch.trainIdx >= 0 
					&& bestMatch.trainIdx < m_keypointsMainImg.size()) 
				{
					if (bestMatch.distance < loweRatio * betterMatch.distance)
					{
						m_goodMatches.push_back(bestMatch);
					}
				}
			}
		}
	}
};

