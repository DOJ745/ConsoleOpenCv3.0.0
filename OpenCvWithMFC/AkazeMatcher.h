#pragma once
#include "FeatureMatcher.h"

/*
	Рекомендуемые параметры:

	- threshold (0.001–0.005) (0.003 — старт) - чувствительность к деталям (меньше => больше фичей) 

	- nOctaves (4–5) - Масштабная устойчивость 

	- descriptorSize (0 [Bit]) — означает максимальный

	- nOctaveLayers большее значение позволяет детектору обнаруживать более мелкие детали, увеличивая чувствительность 

	- m_descriptorChannels определяет, сколько каналов информации используется при построении дескриптора (от 1 до 3-х)
		Значение 1: Используется только один канал, что снижает размер дескриптора и ускоряет обработку, но может уменьшить дискриминативность.
		Значение 3: Используются три канала, увеличивая объем информации и потенциально повышая точность сопоставления, но также увеличивая размер дескриптора и время обработки.

	- m_descriptorType влияет на устойчивость и скорость; DESCRIPTOR_MLDB_UPRIGHT — если устойчивость важнее ориентации.
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

	// Геттеры
	float getThreshold() const { return m_threshold; }
	int getNOctaves() const { return m_nOctaves; }
	int getNOctaveLayers() const { return m_nOctaveLayers; }
	int getDescriptorSize() const { return m_descriptorSize; }
	int getDescriptorChannels() const { return m_descriptorChannels; }
	int getDescriptorType() const { return m_descriptorType; }

	// Сеттеры
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
		{	// 0 означает максимальный размер
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
	Метод фильтрации совпадений, который помогает отсеять ложные совпадения между изображениями. 
	Идея заключается в том, чтобы сопоставить ключевые точки в одной картинке с несколькими точками на другой, 
	и затем отфильтровать те, которые имеют слишком близкие расстояния 
	(по Лоу это совпадения с расстоянием, которое больше чем 0.75 расстояния до второго ближайшего соседа)
	*/
	void matchFeaturesLowe()
	{
		if (!m_goodMatches.empty())
		{
			m_goodMatches.clear();
		}

		// Инициализация матчера с перекрестной проверкой (crossCheck)
		cv::BFMatcher matcher(cv::NORM_HAMMING, false);

		// Поиск двух ближайших совпадений для каждой точки
		matcher.knnMatch(m_descriptorsMainImg, m_descriptorsCompareImg, m_knnMatches, 2);

		const float loweRatio = 0.95f;
		std::vector<cv::DMatch> good_matches;

		for (size_t i = 0; i < m_knnMatches.size(); i++) 
		{
			if (m_knnMatches[i].size() >= 2) 
			{
				const cv::DMatch& bestMatch = m_knnMatches[i][0];
				const cv::DMatch& betterMatch = m_knnMatches[i][1];

				// Проверяем индексы на соответствие массивам ключевых точек
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

