/*
 * EM.h
 *
 *  Created on: Mar 2, 2013
 *      Author: cem
 */

#ifndef EM_H_
#define EM_H_

#include <string>
#include <vector>
class EM {
public:
	static std::string className;
	/*admin Lambda is a deciding parameter on the belief of BW utilization*/
	int adminLambda;
	long period;
	std::string visitorClass;
	//Generic file types
	std::string debugOutFileTaos;
	std::string debugOutFileLambdas;
	std::string debugOutFileLambdasTime;
	std::string debugOutFileNoTaos;
	std::string debugOutFileNoLambdas;
	std::string debugOutFileNoLambdasTime;

	//Files for Bandwidth Utilization
	std::string debugOutFileNegLambdas;
	std::string debugOutFileNegLambdasTime;
	std::string debugOutFileNegTaos;
	std::string debugOutFileNegotiations;
	std::string debugOutFileNegotiationsTime;
	//Files for Reputation and Client Populations
	std::string debugOutFileCoopLambdas;
	std::string debugOutFileCoopLambdasTime;
	std::string debugOutFileCoopTaos;
	std::string debugOutFileNoCoopLambdas;
	std::string debugOutFileNoCoopLambdasTime;
	std::string debugOutFileNoCoopTaos;
	std::string debugOutFileDecisions;
	std::string debugOutFileDecesionsTime;

	std::string debugOutFileProbabilities;
	std::string debugOutFileProbabilitiesTime;

	/** EM related parameters positiv*/
	int windowSize_P; //that was defines as public, it can rise an access problem.
	int sliderSize_P;
	int sliderCounter_P;
	int priority_P;
	int previousLambda_P;
	std::vector<bool> trackingWindow_P;
	std::vector<bool> numberOfOccurance_P;
	double taoNC_P;
	double taoC_P;
	double taoN_P;
	int lambdaNC_P;
	int lambdaC_P;
	int lambdaN_P;

	/** EM related parameters negative decisions*/
	int windowSize_N; //that was defines as public, it can rise an access problem.
	int sliderSize_N;
	int sliderCounter_N;
	int priority_N;
	int previousLambda_N;
	std::vector<bool> trackingWindow_N;
	std::vector<bool> numberOfOccurance_N;
	double taoNC_N;
	double taoC_N;
	double taoN_N;
	int lambdaNC_N;
	int lambdaC_N;
	int lambdaN_N;
	EM();
	EM(std::string visitorClassName);
	virtual ~EM();
	void prepareAllPath();
	void addOccuranceTrack_P(int& occurance);
	void addOccuranceTrack_N(int& occurance);
	void runEM_P();
	void runEM_N();
	void setAdminLambda(int i);
	void setPeriod(int per);
	//void calculateProbabilies();
	void fileWrite(std::string& debugOutFile, std::string& outputString);
	void updateTestInteger(int i);
	int getTestInteger();
	void runTest();

private:
	int testInteger;
	double EMStep_P();
	double EMStep_N();
	double getMax(double& rateOfChangeNC,double& rateOfChangeN,double& rateOfChangeC);
	double calculateLogFact(int& k);
};

#endif /* EM_H_ */
