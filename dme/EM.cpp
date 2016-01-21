/*
 * EM.cpp
 *
 *  Created on: Mar 2, 2013
 *      Author: cem
 */

#include "EM.h"
#include "Utilities.h"
#include "math.h"


std::string EM::className="EM";
EM::EM(){

}
EM::EM(std::string visitorClassName) {
	//Utilities::errorCatcher("EM:"+visitorClassName, 1);
	this->visitorClass=visitorClassName;
	this->adminLambda = 1;
	this->period = 10 * 1000;
	//EM Related Parameters for Positive
	this->windowSize_P = 20;
	this->sliderSize_P = 2;
	this->sliderCounter_P = 0;

	this->taoNC_P = 0.33;
	this->taoC_P = 0.33;
	this->taoN_P = 0.34;
	this->lambdaNC_P = this->windowSize_P * 2 / 10;
	this->lambdaC_P = this->windowSize_P * 8 / 10;
	this->lambdaN_P = this->windowSize_P * 5 / 10;

	for (int i = 0; i < this->windowSize_P; i++) {
		this->trackingWindow_P.push_back(false);
	}
	for (int i = 0; i < this->windowSize_P; i++) {
		this->numberOfOccurance_P.push_back(0);
	}
	if(this->visitorClass!="BandwidthUtilizationFE"){
		//EM Related Parameters for Negative
		this->windowSize_N = 20;
		this->sliderSize_N = 2;
		this->sliderCounter_N = 0;

		this->taoNC_N = 0.33;
		this->taoC_N = 0.33;
		this->taoN_N = 0.34;
		this->lambdaNC_N = this->windowSize_N * 2 / 10;
		this->lambdaC_N = this->windowSize_N * 8 / 10;
		this->lambdaN_N = this->windowSize_N * 5 / 10;

		for (int i = 0; i < this->windowSize_N; i++) {
			this->trackingWindow_N.push_back(false);
		}
		for (int i = 0; i < this->windowSize_N; i++) {
			this->numberOfOccurance_N.push_back(0);
		}
	}
}

EM::~EM() {
	// TODO Auto-generated destructor stub
}

void EM::prepareAllPath(){
	if(this->visitorClass=="BandwidthUtilizationFE"){
			this->debugOutFileLambdas=this->debugOutFileNegLambdas;
			this->debugOutFileTaos=this->debugOutFileNegTaos;
			this->debugOutFileLambdasTime=this->debugOutFileNegLambdasTime;
	}else if(this->visitorClass=="ClientPopulationFE" || this->visitorClass=="ReputationFE"){
		this->debugOutFileLambdas = this->debugOutFileCoopLambdas;
		this->debugOutFileTaos = this->debugOutFileCoopTaos;
		this->debugOutFileLambdasTime = this->debugOutFileCoopLambdasTime;
		this->debugOutFileNoLambdas = this->debugOutFileNoCoopLambdas;
		this->debugOutFileNoTaos = this->debugOutFileNoCoopTaos;
		this->debugOutFileNoLambdasTime = this->debugOutFileNoCoopLambdasTime;

	}
}
void EM::addOccuranceTrack_P(int& occurance){
	this->numberOfOccurance_P.push_back(occurance);
	this->numberOfOccurance_P.erase(this->numberOfOccurance_P.begin());
}
void EM::addOccuranceTrack_N(int& occurance){
	this->numberOfOccurance_N.push_back(occurance);
	this->numberOfOccurance_N.erase(this->numberOfOccurance_N.begin());
}
double EM::EMStep_P(){

	double maximumBasedOnTao = 0;
			double newTaoNC = 0;
			double newTaoN = 0;
			double newTaoC = 0;

			double previousTaoNC = this->taoNC_P;
			double previousTaoN = this->taoN_P;
			double previousTaoC = this->taoC_P;

			double startTaoNC = 0.01;
			double finishTaoNC = 0.99;
			if ( (this->taoNC_P - 0.20) > 0  ){
				startTaoNC = (this->taoNC_P - 0.20);
			}
			if ( (this->taoNC_P + 0.20) < 1 ) {
				finishTaoNC = (this->taoNC_P + 0.20);
			}

			double startTaoN = 0.01;
			if ( (this->taoN_P - 0.20) > 0  ){
				startTaoN = (this->taoN_P - 0.20);
			}

			for (double taoNC = startTaoNC; taoNC < finishTaoNC ; taoNC = taoNC + 0.02){

				double finishTaoN = 0.99;
				if ( (this->taoN_P + 0.20) < (1 - taoNC) ) {
					finishTaoN = (this->taoN_P + 0.20);
				}else {
					finishTaoN = (1 - taoNC);
				}

				for (double taoN = startTaoN; taoN < finishTaoN ; taoN = taoN + 0.02){

					double taoC = 1 - taoN - taoNC;

					double Expectation = 0;
					for (int i=0; i < windowSize_P ; i++) {
						int k = numberOfOccurance_P[i];
						double nominatorNC = taoNC * exp(-this->lambdaNC_P) * pow(this->lambdaNC_P, k);
						double nominatorC = taoC * exp(-this->lambdaC_P) * pow(this->lambdaC_P, k);
						double nominatorN = taoN * exp(-this->lambdaN_P) * pow(this->lambdaN_P, k);
						double denominator = nominatorN + nominatorNC + nominatorC;
						double portionNC = nominatorNC / denominator;
						double portionC = nominatorC / denominator;
						double portionN = nominatorN / denominator;
						double tempDouble = this->calculateLogFact(k);
						double logPartNC = log(taoNC) - this->lambdaNC_P + k * log(this->lambdaNC_P) - tempDouble;
						double logPartC = log(taoC) - this->lambdaC_P + k * log(this->lambdaC_P) - tempDouble;
						double logPartN = log(taoN) - this->lambdaN_P + k * log(this->lambdaN_P) - tempDouble;
						Expectation = portionNC * logPartNC + portionC * logPartC + portionN * logPartN + Expectation;
					}

					if( (taoNC == startTaoNC) && (taoN == startTaoN) ) {
						//CommonFunctions.writeConsoleSecond("firstly equationg maximum NC to Expectation");
						//writeConsole("firstly equationg maximum NC to Expectation " + Expectation);
						maximumBasedOnTao = Expectation;
						newTaoNC = taoNC;
						newTaoC = taoC;
						newTaoN = taoN;
					}
					if ( Expectation > maximumBasedOnTao){
						maximumBasedOnTao = Expectation;
						maximumBasedOnTao = Expectation;
						newTaoNC = taoNC;
						newTaoC = taoC;
						newTaoN = taoN;
					}

				}
			}

			this->taoNC_P = newTaoNC;
			this->taoN_P = newTaoN;
			this->taoC_P = newTaoC;
			//writeConsole("\n\nNegotiation Initiation Decision data: New values for tao s: \nTaoNC is "
			//		+ this.taoNC_P + "\nTaoN is " + this.taoN_P + "\nTaoC is " + this.taoC_P );

			std::string toWrite=Utilities::convertDoubleToString(this->taoNC_P)+" "+
						Utilities::convertDoubleToString(this->taoN_P)+" "+
						Utilities::convertDoubleToString(this->taoC_P);
			//this->fileWrite( this->debugOutFieNegTaos, toWrite);
			this->fileWrite( this->debugOutFileTaos, toWrite);
			int startLambdaNC = 0;
			int startLambdaN = 0;
			int startLambdaC = 0;
			int finishLambdaC = 0;
			double maximumBasedOnLambda = 0;
			int newLambdaNC = this->lambdaNC_P;
			int newLambdaN = this->lambdaN_P;
			int newLambdaC = this->lambdaC_P;

			if ( (this->lambdaC_P + sliderSize_P) > windowSize_P ) {
				finishLambdaC = windowSize_P + 1;
			}else {
				finishLambdaC = (this->lambdaC_P + 5);
			}

			if ( (this->lambdaC_P - sliderSize_P) > 1 ) {
				startLambdaC = (this->lambdaC_P - sliderSize_P);
			}else {
				startLambdaC = 1;
			}

			if ( (this->lambdaNC_P - sliderSize_P) > 1 ) {
				startLambdaNC = (this->lambdaNC_P - sliderSize_P);
			}else {
				startLambdaNC = 1;
			}

			if ( (this->lambdaN_P - sliderSize_P) > 1 ) {
				startLambdaN = (this->lambdaN_P - sliderSize_P);
			}else {
				startLambdaN = 1;
			}

			for (int lambdaC = startLambdaC; lambdaC < finishLambdaC ; lambdaC++){
				for (int lambdaN = startLambdaN; lambdaN < lambdaC ; lambdaN++){
					for (int lambdaNC = startLambdaNC; lambdaNC < lambdaN ; lambdaNC++){
						double Expectation = 0;
						for (int i=0; i < windowSize_P ; i++) {
							int k = numberOfOccurance_P[i];
							double nominatorNC = this->taoNC_P * exp(-lambdaNC) * pow(lambdaNC, k);
							double nominatorC = this->taoC_P * exp(-lambdaC) * pow(lambdaC, k);
							double nominatorN = this->taoN_P * exp(-lambdaN) * pow(lambdaN, k);
							double denominator = nominatorN + nominatorNC + nominatorC;
							double portionNC = nominatorNC / denominator;
							double portionC = nominatorC / denominator;
							double portionN = nominatorN / denominator;
							double tempDouble = calculateLogFact(k);
							double logPartNC = log(this->taoNC_P) - lambdaNC + k * log(lambdaNC) - tempDouble;
							double logPartC = log(this->taoC_P) - lambdaC + k * log(lambdaC) - tempDouble;
							double logPartN = log(this->taoN_P) - lambdaN + k * log(lambdaN) - tempDouble;
							Expectation = portionNC * logPartNC + portionC * logPartC + portionN * logPartN + Expectation;
						}

						if( (lambdaNC == startLambdaNC) && (lambdaN == startLambdaN) && (lambdaC == startLambdaC) ) {
							//CommonFunctions.writeConsoleSecond("firstly equationg maximum NC to Expectation");
							//writeConsole("firstly equationg maximum Lambda based to Expectation " + Expectation);
							maximumBasedOnLambda = Expectation;
							newLambdaNC = lambdaNC;
							newLambdaN = lambdaN;
							newLambdaC = lambdaC;
						}
						if ( Expectation > maximumBasedOnLambda){
							maximumBasedOnLambda = Expectation;
							newLambdaNC = lambdaNC;
							newLambdaN = lambdaN;
							newLambdaC = lambdaC;
						}
					}
				}
			}

			this->lambdaC_P = newLambdaC;
			this->lambdaNC_P = newLambdaNC;
			this->lambdaN_P = newLambdaN;
			//writeConsole("Negotiation Initiation Decision data: EM MESSAGE, new values for lambdas \nlambdaNC = " + this.lambdaNC_P +
			//		" \nlambdaN = " + this.lambdaN_P + "\nlambdaC = " + this.lambdaC_P);

			double rateOfChangeNC = this->taoNC_P - previousTaoNC;
			double rateOfChangeN = this->taoN_P - previousTaoN;
			double rateOfChangeC = this->taoC_P - previousTaoC;

			double maxRateOfChange = getMax(rateOfChangeNC, rateOfChangeN, rateOfChangeC);
			/*
			 * Comparison of rate of changes
			 */
			int lambdaForMMAP = this->previousLambda_P;
			if ( (rateOfChangeNC == 0) && (rateOfChangeN == 0) && (rateOfChangeC == 0)){
				if ( this->taoNC_P > this->taoN_P ){
					if ( this->taoNC_P > this->taoC_P ){
						lambdaForMMAP = this->lambdaNC_P;
						//writeConsole("MMAP MESSAGE, rate of changes 0 for " + this.className);
					}else {
						lambdaForMMAP = this->lambdaC_P;
						//writeConsole("MMAP MESSAGE, rate of changes 0 for " + this.className);
					}
				}else{
					if ( this->taoN_P > this->taoC_P ){
						lambdaForMMAP = this->lambdaN_P;
						//writeConsole("MMAP MESSAGE, rate of changes 0 for " + this.className);
					}else {
						lambdaForMMAP = this->lambdaC_P;
						//writeConsole("MMAP MESSAGE, rate of changes 0 for " + this.className);
					}
				}
			}
			else if ( rateOfChangeNC == maxRateOfChange ){
				lambdaForMMAP = this->lambdaNC_P;
				//writeConsole("MMAP MESSAGE, max rateofChange in NC " + this.className + " " + rateOfChangeNC);
				//writeConsole("MMAP MESSAGE, previous taoNC " + previousTaoNC + " now " + this.taoNC_P);
			} else if ( rateOfChangeN == maxRateOfChange ){
				//writeConsole("MMAP MESSAGE, max rateofChange in N " + this.className + " " + rateOfChangeN);
				//writeConsole("MMAP MESSAGE, previous taoN " + previousTaoN + " now " + this.taoN_P);
				lambdaForMMAP = this->lambdaN_P;
			} else if ( rateOfChangeC == maxRateOfChange ){
				//writeConsole("MMAP MESSAGE, max rateofChange in C " + this.className + " " + rateOfChangeC);
				//writeConsole("MMAP MESSAGE, previous taoC " + previousTaoC + " now " + this.taoC_P);
				lambdaForMMAP = this->lambdaC_P;
			}
			this->previousLambda_P = lambdaForMMAP;

			/*
			double probabilityCriticalNom = 3 * Math.pow(this.adminLambda,2) + 3 * this.adminLambda * lambdaForMMAP + Math.pow(lambdaForMMAP, 2);
			double probabilityCriticalDe = Math.pow(  (3* this.adminLambda + lambdaForMMAP ), 2);
			double probabilityCritical = probabilityCriticalNom / probabilityCriticalDe;
			this.priority_P = (int) ( 100 * probabilityCritical); */
			std::cout<<" MMAP MESSAGE, new lambda is " << this->previousLambda_P <<" \n\n"<<std::endl;
			toWrite=this->previousLambda_P;
			//this->fileWrite( this->debugOutFileNegLambdas,toWrite );
			this->fileWrite( this->debugOutFileLambdas,toWrite );
			toWrite=Utilities::longToTimeString(Utilities::getCurrentTimeAsSecond());
			//this->fileWrite( this->debugOutFileNegLamdasTime, toWrite );
			this->fileWrite( this->debugOutFileLambdasTime, toWrite );
			return 0;




}
double EM::EMStep_N(){

	double maximumBasedOnTao = 0;
			double newTaoNC = 0;
			double newTaoN = 0;
			double newTaoC = 0;

			double previousTaoNC = this->taoNC_N;
			double previousTaoN = this->taoN_N;
			double previousTaoC = this->taoC_N;

			double startTaoNC = 0.01;
			double finishTaoNC = 0.99;
			if ( (this->taoNC_N - 0.20) > 0  ){
				startTaoNC = (this->taoNC_N - 0.20);
			}
			if ( (this->taoNC_N + 0.20) < 1 ) {
				finishTaoNC = (this->taoNC_N + 0.20);
			}

			double startTaoN = 0.01;
			if ( (this->taoN_N - 0.20) > 0  ){
				startTaoN = (this->taoN_N - 0.20);
			}

			for (double taoNC = startTaoNC; taoNC < finishTaoNC ; taoNC = taoNC + 0.02){

				double finishTaoN = 0.99;
				if ( (this->taoN_N + 0.20) < (1 - taoNC) ) {
					finishTaoN = (this->taoN_N + 0.20);
				}else {
					finishTaoN = (1 - taoNC);
				}

				for (double taoN = startTaoN; taoN < finishTaoN ; taoN = taoN + 0.02){

					double taoC = 1 - taoN - taoNC;

					double Expectation = 0;
					for (int i=0; i < windowSize_N ; i++) {
						int k = numberOfOccurance_N[i];
						double nominatorNC = taoNC * exp(-this->lambdaNC_N) * pow(this->lambdaNC_N, k);
						double nominatorC = taoC * exp(-this->lambdaC_N) * pow(this->lambdaC_N, k);
						double nominatorN = taoN * exp(-this->lambdaN_N) * pow(this->lambdaN_N, k);
						double denominator = nominatorN + nominatorNC + nominatorC;
						double portionNC = nominatorNC / denominator;
						double portionC = nominatorC / denominator;
						double portionN = nominatorN / denominator;
						double tempDouble = this->calculateLogFact(k);
						double logPartNC = log(taoNC) - this->lambdaNC_N + k * log(this->lambdaNC_N) - tempDouble;
						double logPartC = log(taoC) - this->lambdaC_N + k * log(this->lambdaC_N) - tempDouble;
						double logPartN = log(taoN) - this->lambdaN_N + k * log(this->lambdaN_N) - tempDouble;
						Expectation = portionNC * logPartNC + portionC * logPartC + portionN * logPartN + Expectation;
					}

					if( (taoNC == startTaoNC) && (taoN == startTaoN) ) {
						//CommonFunctions.writeConsoleSecond("firstly equationg maximum NC to Expectation");
						//writeConsole("firstly equationg maximum NC to Expectation " + Expectation);
						maximumBasedOnTao = Expectation;
						newTaoNC = taoNC;
						newTaoC = taoC;
						newTaoN = taoN;
					}
					if ( Expectation > maximumBasedOnTao){
						maximumBasedOnTao = Expectation;
						maximumBasedOnTao = Expectation;
						newTaoNC = taoNC;
						newTaoC = taoC;
						newTaoN = taoN;
					}

				}
			}

			this->taoNC_N = newTaoNC;
			this->taoN_N = newTaoN;
			this->taoC_N = newTaoC;
			//writeConsole("\n\nNegotiation Initiation Decision data: New values for tao s: \nTaoNC is "
			//		+ this.taoNC_P + "\nTaoN is " + this.taoN_P + "\nTaoC is " + this.taoC_P );

			std::string toWrite=Utilities::convertDoubleToString(this->taoNC_N)+" "+
						Utilities::convertDoubleToString(this->taoN_N)+" "+
						Utilities::convertDoubleToString(this->taoC_N);
			//this->fileWrite( this->debugOutFileNoCoopTaos, toWrite);
			this->fileWrite( this->debugOutFileNoTaos, toWrite);
			int startLambdaNC = 0;
			int startLambdaN = 0;
			int startLambdaC = 0;
			int finishLambdaC = 0;
			double maximumBasedOnLambda = 0;
			int newLambdaNC = this->lambdaNC_N;
			int newLambdaN = this->lambdaN_N;
			int newLambdaC = this->lambdaC_N;

			if ( (this->lambdaC_N + sliderSize_N) > windowSize_N ) {
				finishLambdaC = windowSize_N + 1;
			}else {
				finishLambdaC = (this->lambdaC_N + 5);
			}

			if ( (this->lambdaC_N - sliderSize_N) > 1 ) {
				startLambdaC = (this->lambdaC_N - sliderSize_N);
			}else {
				startLambdaC = 1;
			}

			if ( (this->lambdaNC_N - sliderSize_N) > 1 ) {
				startLambdaNC = (this->lambdaNC_N - sliderSize_N);
			}else {
				startLambdaNC = 1;
			}

			if ( (this->lambdaN_N - sliderSize_N) > 1 ) {
				startLambdaN = (this->lambdaN_N - sliderSize_N);
			}else {
				startLambdaN = 1;
			}

			for (int lambdaC = startLambdaC; lambdaC < finishLambdaC ; lambdaC++){
				for (int lambdaN = startLambdaN; lambdaN < lambdaC ; lambdaN++){
					for (int lambdaNC = startLambdaNC; lambdaNC < lambdaN ; lambdaNC++){
						double Expectation = 0;
						for (int i=0; i < windowSize_N ; i++) {
							int k = numberOfOccurance_N[i];
							double nominatorNC = this->taoNC_N * exp(-lambdaNC) * pow(lambdaNC, k);
							double nominatorC = this->taoC_N * exp(-lambdaC) * pow(lambdaC, k);
							double nominatorN = this->taoN_N * exp(-lambdaN) * pow(lambdaN, k);
							double denominator = nominatorN + nominatorNC + nominatorC;
							double portionNC = nominatorNC / denominator;
							double portionC = nominatorC / denominator;
							double portionN = nominatorN / denominator;
							double tempDouble = calculateLogFact(k);
							double logPartNC = log(this->taoNC_N) - lambdaNC + k * log(lambdaNC) - tempDouble;
							double logPartC = log(this->taoC_N) - lambdaC + k * log(lambdaC) - tempDouble;
							double logPartN = log(this->taoN_N) - lambdaN + k * log(lambdaN) - tempDouble;
							Expectation = portionNC * logPartNC + portionC * logPartC + portionN * logPartN + Expectation;
						}

						if( (lambdaNC == startLambdaNC) && (lambdaN == startLambdaN) && (lambdaC == startLambdaC) ) {
							//CommonFunctions.writeConsoleSecond("firstly equationg maximum NC to Expectation");
							//writeConsole("firstly equationg maximum Lambda based to Expectation " + Expectation);
							maximumBasedOnLambda = Expectation;
							newLambdaNC = lambdaNC;
							newLambdaN = lambdaN;
							newLambdaC = lambdaC;
						}
						if ( Expectation > maximumBasedOnLambda){
							maximumBasedOnLambda = Expectation;
							newLambdaNC = lambdaNC;
							newLambdaN = lambdaN;
							newLambdaC = lambdaC;
						}
					}
				}
			}

			this->lambdaC_N = newLambdaC;
			this->lambdaNC_N = newLambdaNC;
			this->lambdaN_N = newLambdaN;
			//writeConsole("Negotiation Initiation Decision data: EM MESSAGE, new values for lambdas \nlambdaNC = " + this.lambdaNC_P +
			//		" \nlambdaN = " + this.lambdaN_P + "\nlambdaC = " + this.lambdaC_P);

			double rateOfChangeNC = this->taoNC_N - previousTaoNC;
			double rateOfChangeN = this->taoN_N - previousTaoN;
			double rateOfChangeC = this->taoC_N - previousTaoC;

			double maxRateOfChange = getMax(rateOfChangeNC, rateOfChangeN, rateOfChangeC);
			/*
			 * Comparison of rate of changes
			 */
			int lambdaForMMAP = this->previousLambda_N;
			if ( (rateOfChangeNC == 0) && (rateOfChangeN == 0) && (rateOfChangeC == 0)){
				if ( this->taoNC_N > this->taoN_N ){
					if ( this->taoNC_N > this->taoC_N ){
						lambdaForMMAP = this->lambdaNC_P;
						//writeConsole("MMAP MESSAGE, rate of changes 0 for " + this.className);
					}else {
						lambdaForMMAP = this->lambdaC_N;
						//writeConsole("MMAP MESSAGE, rate of changes 0 for " + this.className);
					}
				}else{
					if ( this->taoN_N > this->taoC_N ){
						lambdaForMMAP = this->lambdaN_N;
						//writeConsole("MMAP MESSAGE, rate of changes 0 for " + this.className);
					}else {
						lambdaForMMAP = this->lambdaC_N;
						//writeConsole("MMAP MESSAGE, rate of changes 0 for " + this.className);
					}
				}
			}
			else if ( rateOfChangeNC == maxRateOfChange ){
				lambdaForMMAP = this->lambdaNC_N;
				//writeConsole("MMAP MESSAGE, max rateofChange in NC " + this.className + " " + rateOfChangeNC);
				//writeConsole("MMAP MESSAGE, previous taoNC " + previousTaoNC + " now " + this.taoNC_P);
			} else if ( rateOfChangeN == maxRateOfChange ){
				//writeConsole("MMAP MESSAGE, max rateofChange in N " + this.className + " " + rateOfChangeN);
				//writeConsole("MMAP MESSAGE, previous taoN " + previousTaoN + " now " + this.taoN_P);
				lambdaForMMAP = this->lambdaN_N;
			} else if ( rateOfChangeC == maxRateOfChange ){
				//writeConsole("MMAP MESSAGE, max rateofChange in C " + this.className + " " + rateOfChangeC);
				//writeConsole("MMAP MESSAGE, previous taoC " + previousTaoC + " now " + this.taoC_P);
				lambdaForMMAP = this->lambdaC_N;
			}
			this->previousLambda_N = lambdaForMMAP;

			/*
			double probabilityCriticalNom = 3 * Math.pow(this.adminLambda,2) + 3 * this.adminLambda * lambdaForMMAP + Math.pow(lambdaForMMAP, 2);
			double probabilityCriticalDe = Math.pow(  (3* this.adminLambda + lambdaForMMAP ), 2);
			double probabilityCritical = probabilityCriticalNom / probabilityCriticalDe;
			this.priority_P = (int) ( 100 * probabilityCritical); */
			std::cout<<"MMAP MESSAGE, new lambda is " << this->previousLambda_N <<" \n\n"<<std::endl;
			toWrite=this->previousLambda_P;
			//this->fileWrite( this->debugOutFileNoCoopLambdas,toWrite );
			this->fileWrite( this->debugOutFileNoLambdas,toWrite );
			toWrite=Utilities::longToTimeString(Utilities::getCurrentTimeAsSecond());
			//this->fileWrite( this->debugOutFileNoCoopLambdasTime, toWrite );
			this->fileWrite( this->debugOutFileNoLambdasTime, toWrite );
			return 0;




}
void EM::runEM_P(){
	if(this->sliderCounter_P>=this->sliderSize_P){
		this->sliderCounter_P=0;
		int occuranceN=0;
		for(int i=0;i<this->windowSize_P;i++){
			if(this->trackingWindow_P[i])
			occuranceN=occuranceN+1;
		}
		this->addOccuranceTrack_P(occuranceN);
		this->EMStep_P();
	}else{
		this->sliderCounter_P=this->sliderCounter_P+1;
	}
}
void EM::runEM_N(){
	if(this->sliderCounter_N>=this->sliderSize_N){
		this->sliderCounter_N=0;
		int occuranceN=0;
		for(int i=0;i<this->windowSize_N;i++){
			if(this->trackingWindow_N[i])
			occuranceN=occuranceN+1;
		}
		this->addOccuranceTrack_N(occuranceN);
		this->EMStep_N();
	}else{
		this->sliderCounter_N=this->sliderCounter_N+1;
	}
}
void EM::setAdminLambda(int i){
	this->adminLambda=i;
}
void EM::setPeriod(int per){
	this->period=per*1000;
}
double EM::getMax(double& rateOfChangeNC, double& rateOfChangeN,double& rateOfChangeC){
	double temp=rateOfChangeNC;
	if(temp<rateOfChangeN){
		temp=rateOfChangeN;
	}
	if(temp<rateOfChangeC){
		temp=rateOfChangeC;
	}
	return temp;
}
double EM::calculateLogFact(int& k){
	double temp=0;
	for(int i=1;i<=k;i++){
		temp=temp+log(i);
	}
	return temp;
}
void EM::fileWrite(std::string& debugOutFile,
		std::string& outputString) {
	try {
		std::ofstream file(debugOutFile.c_str());
		if (file.is_open()) {
			file << outputString;
			file << "\n";
			file.close();
		}
	} catch (std::exception& e) {
		std::cerr << className << " " << e.what() << std::endl;
	}
}
void EM::updateTestInteger(int i){
	this->testInteger=i;
}
int EM::getTestInteger(){
	return this->testInteger;
}
void EM::runTest(){
	while(true){
			try{
			sleep(1);
			this->testInteger++;
			}catch(std::exception &e){
				std::cerr<<this->className<<" "<<e.what()<<std::endl;
			}

		}
}
