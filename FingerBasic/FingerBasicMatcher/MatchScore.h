#pragma once

#include "FingerBasic.h"
#include "MinuMatch.h"

class MatchScore
{
public:
	MatchScore(MinuMatch& _matcher, vector<MinuPair>& _MinuPairs):matcher(_matcher),minu_pairs(_MinuPairs) {};
	virtual ~MatchScore(void) {};

	virtual void ComputeFeatures() = 0;
	virtual void ComputeScore() = 0;

	MinuMatch& matcher;// need to access members of MinuMatch
	vector<MinuPair>& minu_pairs;
	double score;

	//// Match features
	int pair_num;
	double descriptor_similarity;
	double minu_percentage[2];

	// binary relationship between matched minutiae
	double bi_distance;
	double bi_alpha;
	double bi_beta;
};
