#pragma once
#include "FeatureMatcher.h"

/*
	–екомендуемые параметры:

	- threshold (0.001Ц0.005) (0.003 Ч старт) - чувствительность к детал€м (меньше => больше фичей) 

	- nOctaves (4Ц5) - ћасштабна€ устойчивость 

	- descriptorSize (0 [Bit]) Ч означает максимальный

	- nOctaveLayers большее значение позвол€ет детектору обнаруживать более мелкие детали, увеличива€ чувствительность 

	- m_descriptorChannels определ€ет, сколько каналов информации используетс€ при построении дескриптора (от 1 до 3-х)
		«начение 1: »спользуетс€ только один канал, что снижает размер дескриптора и ускор€ет обработку, но может уменьшить дискриминативность.
		«начение 3: »спользуютс€ три канала, увеличива€ объем информации и потенциально повыша€ точность сопоставлени€, но также увеличива€ размер дескриптора и врем€ обработки.

	- m_descriptorType вли€ет на устойчивость и скорость; DESCRIPTOR_MLDB_UPRIGHT Ч если устойчивость важнее ориентации.
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

	// √еттеры
	float getThreshold() const { return m_threshold; }
	int getNOctaves() const { return m_nOctaves; }
	int getNOctaveLayers() const { return m_nOctaveLayers; }
	int getDescriptorSize() const { return m_descriptorSize; }
	int getDescriptorChannels() const { return m_descriptorChannels; }
	int getDescriptorType() const { return m_descriptorType; }

	// —еттеры
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

	void performAll()
	{
		createDetector();
		detectAndComputeMainImg();
		detectAndComputeCompareImg();
		matchFeatures();
		visualizeMatches();
		TRACE("AKAZE match percentage: %.2f%%\n", calculateMatchPercentage());
		setWindowName("AKAZE Matches");
		showResult();
	}

	virtual ~AkazeMatcher(void)
	{
		TRACE("======>[AKAZE] destroying object...\n");
	}
};

