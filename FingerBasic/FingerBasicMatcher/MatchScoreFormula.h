#pragma once

#include "MatchScore.h"

class MatchScoreFormula:
	public MatchScore
{
public:
	MatchScoreFormula(MinuMatch& _matcher, vector<MinuPair>& _MinuPairs):MatchScore(_matcher, _MinuPairs) {};
	virtual ~MatchScoreFormula(void) {};

	virtual void ComputeFeatures();
	virtual void ComputeScore();
};
