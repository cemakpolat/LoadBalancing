/*
 * QoETracker.cpp
 *
 *  Created on: 08.08.2012
 *      Author: cem
 */

#include "QoETracker.h"
#include "QoE.h"
//#include "string"
#include "time.h"
#include <pthread.h>
#include "BlackBoard.h"
#include <stdlib.h>
#include "DataAccess.h"
#include "UPMain.h"
#include <iostream>
#include "Utilities.h"

//#include "ConfigParameters.h"

QoETracker::QoETracker(){}
QoETracker::~QoETracker(){}
void* QoETracker::run(void* object) {
	//int portNumber=ConfigParameters::QoEPortNumber;//13132
	int portNumber = 13132;
	usleep(Utilities::second(2));
	printf("QoS Tracker started \n");
	QoETracker *qoe = (QoETracker*) object;
	TCPServerSocket servSock(portNumber);
	try {
		qoe->lastQoETime=Utilities::getCurrentTimeAsSecond();
		//cout<<"QOE: LastTime qoe: "<<qoe->lastQoETime<<endl;
		int rc;
		pthread_t shifter;
		if ((rc = pthread_create(&shifter, NULL, &QoETracker::qoeShifter,
				qoe))) {
			printf("QoS Tracker:QoS Shifter thread creation failed: %d\n", rc);
			int attempt = 0;
			while (attempt < 3) {
				bool success = true;
				Utilities::randomBackOfftime(7);
				if ((rc = pthread_create(&shifter, NULL,
						&QoETracker::qoeShifter, (void*) &qoe))) {
					success = false;
					printf("QoS Tracker:QoS Shifter thread  creation failed: %d\n", rc);
					if (attempt == 2) {
						printf(
								"QoS Tracker: QoS Shifter thread  creation failed,please re-start the program. \n");
					}
				}
				if (success == true) {
					attempt = 3;
				}
			}
		}

		while(true){

			if(UPMain::Instance()->getAuthenticatedUserCount()>0){
				std::string receivedMessage=qoe->getQoEMeasurementFromClientsInString(servSock.accept());
				//if not found, found is equal to npos, else  it returns the position of the searched term.
				if (string::npos!=receivedMessage.find("timeout")) {
					BlackBoard::writeConsole("QoS Tracker:Time out in collecting userQoEs ");
				} else {
					//IP Address
					if (receivedMessage!="NULL") {
						json::Object obj = Utilities::convertStringToJSON(receivedMessage);//TODO:pointer error may be

						if (UPMain::Instance()->checkUserAuthenticityByIP(qoe->userIP)) {
							BlackBoard::writeConsole("User IP:"+qoe->userIP);
							//BlackBoard::writeConsole("QoS Tracker: QoS Measurement from IP:"+qoe->userIP +" \n"+receivedMessage);
							string userId=UPMain::Instance()->getUserNameByIP(qoe->userIP);
							qoe->userMac=UPMain::Instance()->getUserMAC(qoe->userIP);
							qoe->shiftQoEFromClientsToLeft(userId,qoe->userIP,qoe->userMac,Utilities::getCurrentDateAndTimeAsString(),obj);

							pthread_mutex_lock(&QoE::Instance()->mutexTimeQoE);
							qoe->lastQoETime=Utilities::getCurrentTimeAsSecond();
							pthread_mutex_unlock(&QoE::Instance()->mutexTimeQoE);
						}
					} else {
						Utilities::randomBackOfftime();
						BlackBoard::writeConsole("QoS:Authenticated user number is 0 and waiting for RBT");
					}
				}
			} else {
				Utilities::randomBackOfftime();
				BlackBoard::writeConsole("QoS:Authenticated user number is 0 and waiting for RBT");
			}
	}
	} catch (SocketException &e) {
		cerr <<"QoS Tracker:" <<e.what() << endl;
		QoETracker::run(object);
		usleep(Utilities::second(2));
	} catch (exception& e) {
		cerr <<"QoS Tracker:" <<e.what() << endl;
		QoETracker::run(object);
	}

	pthread_exit(NULL); //exit from thread, but this object will take place in memory.
}
void* QoETracker::qoeShifter(void* object){
	QoETracker *qoe=(QoETracker*)object;

	try{
		while(true){
			usleep(Utilities::second(20));
			time_t currentTime=Utilities::getCurrentTimeAsSecond();
			pthread_mutex_lock(&QoE::Instance()->mutexTimeQoE);
			time_t oldTime=qoe->lastQoETime;
			pthread_mutex_unlock(&QoE::Instance()->mutexTimeQoE);
			long  diff=(currentTime-oldTime);
			//cout<<"QOE: Time diff "<<diff<<endl;
			if(diff>=20){
				json::Object qoEEmpty;
				qoEEmpty["QoE"]=String("\"Empty\"");
				qoe->shiftQoEFromClientsToLeft("","","","",qoEEmpty);
			}
		}
		pthread_exit(NULL); //exit from thread, but this object will take place in memory.
	}catch(json::Exception& e){
		e.what();
		QoETracker::qoeShifter(object);
	}catch(std::exception& e){
		e.what();
		QoETracker::qoeShifter(object);
	}
}

std::string QoETracker::getQoEMeasurementFromClientsInString(TCPSocket *sock){
	//BlackBoard::writeConsole("QOE: getPacket");
	  using namespace std;
	 // std::string result="NULL";
	  const int RCVBUFSIZE = 1000000;
	  //get message
	  char echoBuffer2[RCVBUFSIZE];
	  //echoBuffer2="NULL";
	  sock->recv(echoBuffer2, RCVBUFSIZE);
	  echoBuffer2[RCVBUFSIZE]='\0';//TODO: you should add '\0' end of the line so as to prevent the addtional data.
	  this->userIP=sock->getForeignAddress();
	 // std::cout<<"QOE: User IP: "<<userIP<<std::endl;
	  delete sock;
	  if(echoBuffer2!=""){
		  return echoBuffer2;
	  }
	  else{
		  return "NULL";
	  }
}

void QoETracker::shiftQoEFromClientsToLeft(string userId,string ip,string mac,string time,json::Object& measurement ){
	//BlackBoard::writeConsole("QOE: shifting fuction is called");
	struct qoePacketObject packet; //TODO: can be defined as a Class instead of struct in order to manage dynamically with memory.
	packet.ip = ip;
	packet.mac = mac;
	packet.userId = userId;
	packet.time = time;
	packet.qoeMeasurement = measurement;
	pthread_mutex_lock(&QoE::Instance()->mutexQoE); //TODO:handling mutex exception

	cout<<"QOE List size " <<QoE::Instance()->qoeList.size()<<endl;
	if (QoE::Instance()->qoeList.size() < 50) {
		QoE::Instance()->qoeList.push_back(packet);
	} else {
		QoE::Instance()->qoeList.erase(QoE::Instance()->qoeList.begin() + 0);
		QoE::Instance()->qoeList.push_back(packet);
	}
	pthread_mutex_unlock(&QoE::Instance()->mutexQoE);

	bufferingNumber = bufferingNumber + 1;
	if (bufferingNumber == 50) {
		bufferingNumber = 0;
		cout<<"Buffering number reached 5"<<endl;

		storeInformationOfQoE();
	}



}

void QoETracker::storeInformationOfQoE(){//TODO: some codes are not correctly implemented.
	DataAccess *da=new DataAccess();
	vector<qoePacketObject> list;
	pthread_mutex_lock(&QoE::Instance()->mutexQoE); //TODO:handling mutex exception
	for(unsigned int i=0;i<QoE::Instance()->qoeList.size();i++){
		list.push_back(QoE::Instance()->qoeList[i]);
	}
	cout<<"QOE List size " <<list.size()<<endl;
	QoE::Instance()->qoeList.clear();
	pthread_mutex_unlock(&QoE::Instance()->mutexQoE);
	BlackBoard::writeConsole("QoE Measurement are being storing...");
	for(unsigned int i=0;i<list.size();i++){
	//	json::String& contentQoE=Utilities::convertJSONToString(list[i].qoeMeasurement);//TODO:false
		if(list[i].userId!=""){
			//cout<<"QoE Measurement are being storing..."<<endl;

			da->storeQoEInfo(list[i].time,list[i].qoeMeasurement,list[i].userId);
		}
			//list.push_back(QoE::Instance()->qoeList[i]);
	}
	delete da;
}




