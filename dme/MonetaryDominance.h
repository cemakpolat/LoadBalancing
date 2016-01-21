/*
 * MonetaryDominance.h
 *
 *  Created on: Feb 20, 2013
 *      Author: cem
 */

#ifndef MONETARYDOMINANCE_H_
#define MONETARYDOMINANCE_H_

#include<string>
#include "CreditTrack.h"
class MonetaryDominance {
public:
	static std::string className;
	double K_monetary;
	double A_monetary;
	double B_monetary;
	double k_monetary;
	double normalizationFactor;
	CreditTrack* creditTrack;
	std::string debugMonetaryDominance;
	std::string debugTimer;

	MonetaryDominance();
	virtual ~MonetaryDominance();
	double importancePi(double ownCredit);
	double getMonetaryComponent(double suggestedIncentive,double ownCredit);

};

#endif /* MONETARYDOMINANCE_H_ */
