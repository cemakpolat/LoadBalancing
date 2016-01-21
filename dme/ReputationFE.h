/*
 * ReputationFE.h
 *
 *  Created on: Feb 20, 2013
 *      Author: cem
 */

#ifndef REPUTATIONFE_H_
#define REPUTATIONFE_H_
#include<string>
#include "EM.h"
class ReputationFE {
public:
	static std::string className;
	EM *em;
	int nonCooperativeProbabInPercent;
	int cooperativeProbabInPercent;
	int normalCooperativeProbabInPercent;
	ReputationFE();
	virtual ~ReputationFE();
	static void* run(void*);
	void calculateProbabilities();


	int getNonCooperativeProbabilityInPercent();
	int getCooperativeProbabilityInPercent();
	int getNormalCooperativeProbabilityInPercent();
	void addPositiveDecision(bool occurance);
	void addNegativeDecision(bool occurance);
};

#endif /* REPUTATIONFE_H_ */
