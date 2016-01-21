/*
 * QoE.cpp
 *
 *  Created on: 08.08.2012
 *      Author: cem
 */

#include "QoE.h"
#include <vector>
#include <stdio.h>
#include "QoETracker.h"
#include "BlackBoard.h"
#include "Utilities.h"
//initiate list
std::vector<qoePacketObject> QoE::qoeList;
QoE::QoE(){
	this->averageDelayNormalizationFactor = 0.2;
	this->packetLossRateNormalizationFactor = 10;
	this->jitterNormalizationFactor = 0.2;
	this->bitRateNormalizationFactor = 700;

}
QoE::~QoE(){

}
void* QoE::run(void* object) {
	QoE *qoe=(QoE*)object;
	qoe->qoeMain();
}
void QoE::qoeMain(){
	printf("QoS Component started\n");
	mutexQoE = PTHREAD_MUTEX_INITIALIZER;
	mutexTimeQoE=PTHREAD_MUTEX_INITIALIZER;
	pthread_t qoeTracker,ditg;
	QoETracker qT;
	ITGChecker itg; //TODO : can be pointer

	int rc1,rc2;
	//start QoETracker
	if ((rc1 = pthread_create(&qoeTracker, NULL, &QoETracker::run, &qT))) {
		printf("QoS Block: QoS Tracker thread creation failed: %d\n", rc1);
		int attempt = 0;
		while (attempt < 3) {
			bool success = true;
			Utilities::randomBackOfftime(7);
			if ((rc1 = pthread_create(&qoeTracker, NULL, &QoETracker::run, &qT))) {
				success = false;
				printf("QoS Block: QoS Tracker thread creation failed: %d\n", rc1);
				if (attempt == 2) {
					printf(
							"QoS Block:QoS Tracker thread creation failed,please re-start the program. \n");
				}
			}
			if (success == true) {
				attempt = 3;
			}
		}
	}
		//start ITG
	if ((rc2 = pthread_create(&ditg, NULL, &ITGChecker::run, &itg))) {
		printf("ITG Checker thread creation failed: %d\n", rc2);
		int attempt = 0;
		while (attempt < 3) {
			bool success = true;
			Utilities::randomBackOfftime(7);
			if ((rc2 = pthread_create(&ditg, NULL, &ITGChecker::run, &itg))) {
				success = false;
				printf("QoS Block:ITG Checker thread creation failed: %d\n", rc2);
				if (attempt == 2) {
					printf(
							"QoS Block: ITG Checker thread creation failed,please re-start the program. \n");
				}
			}
			if (success == true) {
				attempt = 3;
			}
		}

	}
	pthread_join(qoeTracker,NULL);
	pthread_join(ditg,NULL);
	printf("QoS Block ended");
}

//singleton pattern required
QoE* QoE::m_qoeInstance=NULL;
QoE* QoE::Instance()
{
   if (!m_qoeInstance)   // Only allow one instance of class to be generated.
	   m_qoeInstance = new QoE();
   return m_qoeInstance;
}

//----------------------Functionalities for QoE----------------------//

int QoE::getAverageDelay(){
	std::string result = "NULL"; //in case of nothing it will return NULL or may be ""
	double maximumAverageDelay = 0;
	pthread_mutex_lock(&QoE::Instance()->mutexQoE);
	if(	QoE::Instance()->qoeList.size()>8){
		int listSize=QoE::Instance()->qoeList.size();
		for(int i=listSize-1;i>=(listSize-8);i--){
			json::Object objRoot=QoE::Instance()->qoeList[i].qoeMeasurement;
			if(!objRoot.Empty()){
				//cout<<"Object"<<String(objRoot)<<endl;
				const json::Array& array = objRoot["Flow Result"];
				json::Object obj2 = array[0];
				json::String& avDelstr=obj2["Average Delay"];
				double temp=atoi(avDelstr.Value().c_str())*1000;

				BlackBoard::writeConsole("QoS Block: "+Utilities::convertDoubleToString(temp));
				if( temp > maximumAverageDelay){
					BlackBoard::writeConsole("QoS Block: Congestion Occured" );
							maximumAverageDelay = temp;
				}
			}

		}
	}
	pthread_mutex_unlock(&QoE::Instance()->mutexQoE);
	int tempInt = (int) maximumAverageDelay;
	return tempInt;
}
//getCurrentUserMACs(){} this function should be called by UPMain

/*multipe or one congested user*/
json::Object QoE::firstCongestedUser(){
	double averaeDelay=20;
	json::Object congestedUser;
	pthread_mutex_lock(&QoE::Instance()->mutexQoE);
	for(unsigned int i=0;i<QoE::Instance()->qoeList.size();i++){
		json::Object objRoot=QoE::Instance()->qoeList[i].qoeMeasurement;
		if(!objRoot.Empty()){
			const json::Array& array = objRoot["Flow Result"];
			json::Object obj2 = array[0];
			json::String& avDelstr=obj2["Average Delay"];
			double temp=atoi(avDelstr.Value().c_str())*1000;
			if(temp>averaeDelay){
					congestedUser["congestedUserIP"]=json::String(QoE::Instance()->qoeList[i].ip);
					i=QoE::Instance()->qoeList.size();
			}
		}
	}
	pthread_mutex_unlock(&QoE::Instance()->mutexQoE);
	return congestedUser;
}
json::Object QoE::worstCongestedUser(){
	double temp1=0;	json::Object congestedUser;
	pthread_mutex_lock(&QoE::Instance()->mutexQoE);
	int placeOfWorstAvDelay=0;
	for(unsigned int i=0;i<QoE::Instance()->qoeList.size();i++){
			json::Object objRoot=QoE::Instance()->qoeList[i].qoeMeasurement;
			if(!objRoot.Empty()){
				const json::Array& array = objRoot["Flow Result"];
				json::Object obj2 = array[0];
				json::String& avDelstr=obj2["Average Delay"];
				double temp=atoi(avDelstr.Value().c_str())*1000;
				if(temp>temp1){
					temp1=temp;
					placeOfWorstAvDelay=i;
				}
			}
		}
	if(QoE::Instance()->qoeList.size()>0){
		congestedUser["congestedUserIP"]=json::String(QoE::Instance()->qoeList[placeOfWorstAvDelay].ip);
	}
	pthread_mutex_unlock(&QoE::Instance()->mutexQoE);
	return congestedUser;
}
json::Object QoE::congestedusers(){
	double averaeDelay=20;
	json::Array congestedUser;
	pthread_mutex_lock(&QoE::Instance()->mutexQoE);
	for(unsigned int i=0;i<QoE::Instance()->qoeList.size();i++){
			json::Object objRoot=QoE::Instance()->qoeList[i].qoeMeasurement;
			if(!objRoot.Empty()){
				const json::Array& array = objRoot["Flow Result"];
				json::Object obj2 = array[0];
				json::String& avDelstr=obj2["Average Delay"];
				double temp=atoi(avDelstr.Value().c_str())*1000;
				if(temp>averaeDelay){
					json::Object congUser;
					congUser[QoE::Instance()->qoeList[i].ip]=json::String(QoE::Instance()->qoeList[i].ip);
					congestedUser.Insert(congUser);
				}
			}
		}
	pthread_mutex_unlock(&QoE::Instance()->mutexQoE);
	json::Object obj;obj["congestedUsers"]=congestedUser;
	return obj;
}
//CreditTRACK
vector<qoePacketObject> QoE::getQoEList(){
	vector<qoePacketObject> list;
	pthread_mutex_lock(&QoE::Instance()->mutexQoE);
	list=QoE::Instance()->qoeList;
	pthread_mutex_unlock(&QoE::Instance()->mutexQoE);
	return list;
}
double QoE::averageBitRate() {
	return this->averageBitRate2(this->getQoEList());
}
double QoE::averageBitRate2(std::vector<qoePacketObject> list) {//TODO:receive please reference
	double averageBitRate=0;
	try{
		if(list.size()>8){
			for(int i=list.size()-1;i>=(list.size()-8);i--){
					json::Object objRoot=QoE::Instance()->qoeList[i].qoeMeasurement;
					if(!objRoot.Empty()){
						//cout<<"Object"<<String(objRoot)<<endl;
						const json::Array& array = objRoot["Flow Result"];
						json::Object obj2 = array[0];
						json::String& avDelstr=obj2["Average Bitrate"];
						double temp=atoi(avDelstr.Value().c_str());
						BlackBoard::writeConsole("QoS Block: "+Utilities::convertDoubleToString(temp));
						if( temp > averageBitRate){
							averageBitRate = temp;
						}
					}

				}
		}

	}catch(json::Exception& e){
		cerr<<"QoE: "<<e.what()<<endl;
	}
	return ((10*averageBitRate)/this->bitRateNormalizationFactor);
}

double QoE::averagePacketLoss() {
	return this->averagePacketLoss2(this->getQoEList());
}
double QoE::averagePacketLoss2(std::vector<qoePacketObject> list){
double maximumAveragePacketLoss=0;
try{
		if(list.size()>8){
			for(int i=list.size()-1;i>=(list.size()-8);i--){
					json::Object objRoot=QoE::Instance()->qoeList[i].qoeMeasurement;
					if(!objRoot.Empty()){
						//cout<<"Object"<<String(objRoot)<<endl;
						const json::Array& array = objRoot["Flow ResultcongestedUser"];
						json::Object obj2 = array[0];
						json::String& avDelstr=obj2["Packet Dropped"];
						string tempString=avDelstr.Value();
						tempString=Utilities::ltrim(tempString);
						vector<string> tempStringArray=Utilities::split(tempString,' ');
						//string a=tempStringArray[0];//TODO: failure
						double temp=Utilities::convertStringDouble(tempStringArray[0]);
						BlackBoard::writeConsole("QoS Block: "+Utilities::convertDoubleToString(temp));
						if( temp > maximumAveragePacketLoss){
							maximumAveragePacketLoss = temp;
						}
					}

				}
		}

	}catch(json::Exception& e){
		cerr<<"QoE: "<<e.what()<<endl;
	}
	return ((10*maximumAveragePacketLoss)/this->packetLossRateNormalizationFactor);

}
double QoE::averageDelay(){
	//return this->averageDelay2(this->getQoEList());
	return this->averageDelayFirstDelay3(this->getQoEList());
}
double QoE::averageDelay2(std::vector<qoePacketObject> list){
	double maximumAverageDelay=0;
	try{
		if(list.size()>8){
			for(int i=list.size()-1;i>=(list.size()-8);i--){
					json::Object objRoot=QoE::Instance()->qoeList[i].qoeMeasurement;
					if(!objRoot.Empty()){
						//cout<<"Object"<<String(objRoot)<<endl;
						const json::Array& array = objRoot["Flow Result"];
						json::Object obj2 = array[0];
						json::String& avDelstr=obj2["Average Bitrate"];
						double temp=atoi(avDelstr.Value().c_str());
						BlackBoard::writeConsole("QoS Block: "+Utilities::convertDoubleToString(temp));
						if( temp > maximumAverageDelay){
							maximumAverageDelay = temp;
						}
					}

				}
		}

	}catch(json::Exception& e){
		cerr<<"QoE: "<<e.what()<<endl;
	}
	return ((10*maximumAverageDelay)/this->averageDelayNormalizationFactor);
}
double QoE::averageDelayFirstDelay3(std::vector<qoePacketObject> list){
	double maximumAverageDelay=0;
	try{
		if(list.size()>8){
			for(int i=list.size()-1;i>=(list.size()-8);i--){
					json::Object objRoot=QoE::Instance()->qoeList[i].qoeMeasurement;
					if(!objRoot.Empty()){
						//cout<<"Object"<<String(objRoot)<<endl;
						const json::Array& array = objRoot["Flow Result"];
						json::Object obj2 = array[0];
						json::String& avDelstr=obj2["Average Delay"];
						double temp=atoi(avDelstr.Value().c_str());
						BlackBoard::writeConsole("QoS Block: "+Utilities::convertDoubleToString(temp));
						if( temp > maximumAverageDelay){
							maximumAverageDelay = temp;
						}
					}

				}
		}

	}catch(json::Exception& e){
		cerr<<"QoE: "<<e.what()<<endl;
	}
	return ((10*maximumAverageDelay)/this->averageDelayNormalizationFactor);
}
double QoE::averageJitter(){
	return this->averageJitter2(this->getQoEList());
}
double QoE::averageJitter2(std::vector<qoePacketObject> list){
	double maximumAverageJitter=0;
	try{
		if(list.size()>8){
			for(int i=list.size()-1;i>=(list.size()-8);i--){
					json::Object objRoot=QoE::Instance()->qoeList[i].qoeMeasurement;
					if(!objRoot.Empty()){
						//cout<<"Object"<<String(objRoot)<<endl;
						const json::Array& array = objRoot["Flow Result"];
						json::Object obj2 = array[0];
						json::String& avDelstr=obj2["Average Jitter"];
						double temp=atoi(avDelstr.Value().c_str());
						BlackBoard::writeConsole("QoS Block: "+Utilities::convertDoubleToString(temp));
						if( temp > maximumAverageJitter){
							maximumAverageJitter = temp;
						}
					}
				}
		}
	}catch(json::Exception& e){
		cerr<<"QoE: "<<e.what()<<endl;
	}
	return ((10*maximumAverageJitter)/this->jitterNormalizationFactor);

}


//----------------------End of Functionalities for QoE----------------------//











