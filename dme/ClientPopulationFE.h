/*
 * ClientPopulationFE.h
 *
 *  Created on: Feb 20, 2013
 *      Author: cem
 */
#ifndef CLIENTPOPULATIONFE_H_
#define CLIENTPOPULATIONFE_H_
#include <string>
#include "EM.h"
class ClientPopulationFE {
public:
	static std::string className;
	EM *em;
	int criticalPopulationProbInPercent;
	int desolatePopulationProbInPercent;
	int normalPopulationProbInPercent;
	ClientPopulationFE();
	virtual ~ClientPopulationFE();
	static void* run(void*);
	void calculateProbabilities();
	int getCriticalPopulationProbabilityInPercent();
	int getDesolatePopulationProbabilityInPercent();
	int getNormalPopulationProbabilityInPercent();
	void addClientDeparture(bool occurance);
	void addClientArrival(bool occurance);
};

#endif /* CLIENTPOPULATIONFE_H_ */
