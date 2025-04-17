#pragma once
#include "FeatureMatcher.h"

/*
	–екомендуемые параметры:

	- nFeatures Ч сколько фичей искать (чем больше, тем дольше).

	- scaleFactor Ч масштаб между уровн€ми пирамиды (1.2 Ч ок).

	- nLevels Ч уровни масштабов (больше Ч более устойчиво, но медленно).

	- edgeThreshold Ч отступ от границы изображени€ (чтобы фичи не упирались в кра€).

	- firstLevel Ч с какого уровн€ начинать.

	- WTA_K Ч сколько точек участвует в тесте BRIEF (2 или 4, больше Ч точнее, но медленнее).

	- scoreType Ч метод оценки: HARRIS_SCORE (устойчивее), FAST_SCORE (быстрее).

	- patchSize Ч размер патча вокруг точки.

	- fastThreshold Ч порог FAST-детектора (меньше Ч больше фичей).
*/

class OrbMatcher : public FeatureMatcher
{
private:
	cv::Ptr<cv::ORB> m_orb;
	int m_nFeatures;
	float m_scaleFactor;
	int m_nLevels;
	int m_edgeThreshold;
	int m_firstLevel;
	int m_WTA_K;
	int m_scoreType;
	int m_patchSize;
	int m_fastThreshold;

protected:
	void detectAndComputeMainImg() override
	{
		cv::Mat emptyMask;
		clock_t start = clock();
		m_orb->detectAndCompute(m_mainImage, emptyMask, m_keypointsMainImg, m_descriptorsMainImg);
		clock_t end = clock();

		double time = (double)(end - start) / CLOCKS_PER_SEC;
		TRACE("ORB Detection time MAIN IMG: %.6f sec\n", time);
	}

	void detectAndComputeCompareImg() override
	{
		cv::Mat emptyMask;
		clock_t start = clock();
		m_orb->detectAndCompute(m_compareImage, emptyMask, m_keypointsCompareImg, m_descriptorsCompareImg);
		clock_t end = clock();

		double time = (double)(end - start) / CLOCKS_PER_SEC;
		TRACE("ORB Detection time COMPARE IMG: %.6f sec\n", time);
	}

public:
	OrbMatcher(): FeatureMatcher(), m_orb(cv::ORB::create())
	{
	};

	OrbMatcher(const std::string& templatePath, const std::string& currentPath, double distance)
		: FeatureMatcher(templatePath, currentPath, distance), m_orb(cv::ORB::create())
	{

	}

	void updateDetector()
	{
		m_orb = cv::ORB::create(m_nFeatures
			, m_scaleFactor
			, m_nLevels
			, m_edgeThreshold
			, m_firstLevel
			, m_WTA_K
			, m_scoreType
			, m_patchSize
			, m_fastThreshold);
	}

	// √еттеры
	int getNFeatures() const { return m_nFeatures; }
	float getScaleFactor() const { return m_scaleFactor; }
	int getNLevels() const { return m_nLevels; }
	int getEdgeThreshold() const { return m_edgeThreshold; }
	int getFirstLevel() const { return m_firstLevel; }
	int getWTAK() const { return m_WTA_K; }
	int getScoreType() const { return m_scoreType; }
	int getPatchSize() const { return m_patchSize; }
	int getFastThreshold() const { return m_fastThreshold; }

	// —еттеры
	void setNFeatures(int value)
	{
		m_nFeatures = (value > 0) ? value : 500;
		updateDetector();
	}

	void setScaleFactor(float value) 
	{ 
		m_scaleFactor = (value > 0.0f) ? value : 1.2f;
		updateDetector();
	}

	void setNLevels(int value) 
	{ 
		m_nLevels = (value > 0) ? value : 10;
		updateDetector();
	}

	void setEdgeThreshold(int value) 
	{ 
		m_edgeThreshold = (value > 0) ? value : 10;
		updateDetector();
	}

	void setFirstLevel(int value) 
	{ 
		m_firstLevel = (value >= 0) ? value : 0;
		updateDetector();
	}

	void setWTAK(int value) 
	{ 
		m_WTA_K = (value == 2 || value == 3 || value == 4) ? value : 2;
		updateDetector();
	}

	void setScoreType(int value) 
	{ 
		m_scoreType = (value == cv::ORB::HARRIS_SCORE || value == cv::ORB::FAST_SCORE) ? value : cv::ORB::HARRIS_SCORE;
		updateDetector();
	}

	void setPatchSize(int value) 
	{ 
		m_patchSize = (value > 0) ? value : 31;
		updateDetector();
	}

	void setFastThreshold(int value) 
	{ 
		m_fastThreshold = (value > 0) ? value : 20;
		updateDetector();
	}

	void createDetector(int nFeatures = 500
		, float scaleFactor = 1.2f
		, int nLevels = 10
		, int edgeThreshold = 10
		, int firstLevel = 0
		, int WTA_K = 2
		, int scoreType = cv::ORB::FAST_SCORE
		, int patchSize = 18
		, int fastThreshold = 50)
	{
			m_orb = cv::ORB::create(nFeatures
				, scaleFactor
				, nLevels
				, edgeThreshold
				, firstLevel
				, WTA_K
				, scoreType
				, patchSize
				, fastThreshold);
	};

	void detectAndComputeAll()
	{
		detectAndComputeMainImg();
		detectAndComputeCompareImg();
	};

	void performAll()
	{
		createDetector();
		detectAndComputeMainImg();
		detectAndComputeCompareImg();
		matchFeatures();
		visualizeMatches();
		TRACE("ORB match percentage: %.2f%%\n", calculateMatchPercentage());
		setWindowName("ORB Matches");
		showResult();
	}

	virtual ~OrbMatcher(void)
	{
		TRACE("======>[ORB] destroying object...\n");
	};
};

