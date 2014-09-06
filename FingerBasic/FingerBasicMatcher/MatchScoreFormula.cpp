#include "MatchScoreFormula.h"

void MatchScoreFormula::ComputeFeatures()
{
	// number of matched minutiae
	pair_num = minu_pairs.size();

	// mean similarity of matched minutiae descriptors

	// percentage of matched minutiae in common area

	// binary relationship between minutiae
}

void MatchScoreFormula::ComputeScore()
{
	score = pair_num * minu_percentage[0] * minu_percentage[1];
}

