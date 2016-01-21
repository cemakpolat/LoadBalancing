/*
 * BandwidthUtilizationFE.h
 *
 *  Created on: Feb 20, 2013
 *      Author: cem
 */

#ifndef BANDWIDTHUTILIZATIONFE_H_
#define BANDWIDTHUTILIZATIONFE_H_
#include <string>
#include <vector>
#include <iostream>
#include "EM.h"

using namespace std;
class BandwidthUtilizationFE {
public:
	static string className;
	EM *em;

	BandwidthUtilizationFE();
	virtual ~BandwidthUtilizationFE();
	static void* run(void*);
	int getHighBWProbabilityInPercent();
	void addNegotiationDecision(bool occurance);
	void calculateProbabilities();
	int getHighBWProbabInPercent();
	int getNormalBWProbabInPercent();
	int getEconomicalBWProbabInPercent();


private:
	int economicalBWUtilProbabInPercent;
	int normalBWUtilProbabInPercent;
	int highBWUtilProbabInPercent;

};

#endif /* BANDWIDTHUTILIZATIONFE_H_ */
