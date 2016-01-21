/*
 * MeasurementPlane.cpp
 *
 *  Created on: 08.08.2012
 *      Author: cem
 */
#include "PracticalSocket.h"
#include "MeasurementPlane.h"
//#include "BlackBoard.h"
#include "UPMain.h"
#include "QoE.h"
#include "pthread.h"
//#include <stdio.h>
#include "Utilities.h"
#include "writer.h"
#include "elements.h"
#include "reader.h"
#include "CommLang.h"
#include "Definitions.h"
//#include "MainDME.h"
#include "SessionObserver.h"

////////// SET TIME ON OWRT  by using the following command line ->date -s 2012.09.24-15:53

MeasurementPlane::MeasurementPlane() {

}
MeasurementPlane::~MeasurementPlane() {

}
MeasurementPlane* MeasurementPlane::m_mpInstance = NULL;
MeasurementPlane* MeasurementPlane::Instance() {
	if (!m_mpInstance) // Only allow one instance of class to be generated.
		m_mpInstance = new MeasurementPlane();
	return m_mpInstance;
}
void* MeasurementPlane::run(void* object) {
	using namespace json;
	//int portNumber = 14131;
	int portNumber=Definitions::MPServerPortNumber;//this works too
	usleep(Utilities::second(2));
	MeasurementPlane *mp = (MeasurementPlane*) object;

	try {
		TCPServerSocket servSock(portNumber); //port number
		while (true) {
			mp->con=new TCPConnection();
			string pktString=mp->con->getMessageLocal(servSock.accept());//TODO:Message comes from load balancing

			 if (string::npos!=pktString.find("timeout")) {
				// BlackBoard::writeConsole("Time out in collecting userQoEs ");
				 cout<<"Time out in collecting userQoEs"<<endl;
			 } else {

				 //WE RECEIVE ONLY 'REQUEST MESSAGE OBJECT' AND SENDING ONLY 'RESPONSE MESSAGE OBJECT'
				Object recObj=Utilities::convertStringToJSON(pktString);
				const unsigned int reqNum=Number(recObj["request"]);
				const String ip=String(recObj["ip"]);
				int clientSocketNum=Number(recObj["serverPort"]);

				int val=0;
				CommLang *cm=new CommLang();
				Object obj;
				//string strVal="";
				switch (reqNum) {
					case CommLang::getCurrentRemainedBandwidthUsage:
						val =UPMain::Instance()->restBandwidthCurrentSessionBW();
						mp->con->sendMessageFirstSizeInInt(ip,cm->createResponseMesObj( Utilities::convertIntToString(val)),clientSocketNum);
						break;
					case CommLang::getOnlineUsers:
						cout << "Nothing" << endl;
						break;
					case CommLang::getLastQoSResultOfKnownID:
						val = QoE::Instance()->getAverageDelay();
						mp->con->sendMessageFirstSizeInInt(ip,cm->createResponseMesObj( Utilities::convertIntToString(val)),clientSocketNum);
						break;
					case CommLang::getfirstCongestedUser:
						obj = QoE::Instance()->firstCongestedUser();
						mp->con->sendMessageFirstSizeInInt(ip,cm->createResponseMesObj( Utilities::convertJSONToString(obj)),clientSocketNum);
						break;
					case CommLang::getCongestedUsers:
						obj = QoE::Instance()->congestedusers();
						mp->con->sendMessageFirstSizeInInt(ip,cm->createResponseMesObj( Utilities::convertJSONToString(obj)),clientSocketNum);
						break;
					case CommLang::getWorstCongestedUser:
						obj = QoE::Instance()->worstCongestedUser();
						mp->con->sendMessageFirstSizeInInt(ip,cm->createResponseMesObj( Utilities::convertJSONToString(obj)),clientSocketNum);
						break;
					case CommLang::getAverageDelayTime:
						val = QoE::Instance()->averageDelay();
						mp->con->sendMessageFirstSizeInInt(ip,cm->createResponseMesObj( Utilities::convertIntToString(val)),clientSocketNum);
						break;
					case CommLang::getAverageJitter:
						val = QoE::Instance()->averageJitter();
						mp->con->sendMessageFirstSizeInInt(ip,cm->createResponseMesObj( Utilities::convertIntToString(val)),clientSocketNum);
						break;
					case CommLang::getPacketLoss:
						val = QoE::Instance()->averagePacketLoss();
						mp->con->sendMessageFirstSizeInInt(ip,cm->createResponseMesObj( Utilities::convertIntToString(val)),clientSocketNum);
						break;
					case CommLang::getBitRate:
						val = QoE::Instance()->averageBitRate();
						mp->con->sendMessageFirstSizeInInt(ip,cm->createResponseMesObj( Utilities::convertIntToString(val)),clientSocketNum);
						break;
					default:
						cout << "Undefined rule Request Number!" << endl;
						break;
				}
				delete cm;

			 }

			usleep(Utilities::second(10));
			std::cout << "Measurement Plane: "
					<< UPMain::Instance()->restBandwidthCurrentSessionBW()
					<<" B/s" <<std::endl;
			usleep(Utilities::second(50));
			delete mp->con;
		}
	}catch (SocketException &e) {
		cerr <<"Measurement Plane: "<< e.what() << "socket" <<endl;
		run(object);

	}
	catch (exception& e) {
		cerr<<"Measurement Plane: "<<e.what()<<endl;
		//usleep(Utilities::second(5));
		run(object);
	}
}

int main() {

	pthread_t qoeThread, upmainThread, mpThread;
	MeasurementPlane *mp = MeasurementPlane::Instance();
	QoE *qoe = QoE::Instance();
	UPMain *upmain = UPMain::Instance();
	int rc1, rc2, rc3;

	//start upmain
	if ((rc1 = pthread_create(&upmainThread, NULL, &UPMain::run, upmain))) {
		printf("Measurement Plane:Thread creation failed: %d\n", rc1);
		int attempt = 0;
		while (attempt < 3) {
			bool success = true;
			Utilities::randomBackOfftime(7);
			if ((rc1 = pthread_create(&upmainThread, NULL, &UPMain::run, upmain))) {

				success = false;
				printf("Measurement Plane:Thread creation failed: %d\n", rc1);
				if (attempt == 2) {
					printf(
							"Measurement Plane:Thread creation failed,please re-start the program. \n");
				}
			}
			if (success == true) {
				attempt = 3;
			}
		}
	}
	//start QoETracker
	if ((rc2 = pthread_create(&qoeThread, NULL, &QoE::run, qoe))) {
		printf("Measurement Plane:Thread creation failed: %d\n", rc2);
		int attempt = 0;
		while (attempt < 3) {
			bool success = true;
			Utilities::randomBackOfftime(7);
			if ((rc2 = pthread_create(&qoeThread, NULL, &QoE::run, qoe))) {
				success = false;
				printf("Measurement Plane:Thread creation failed: %d\n", rc2);
				if (attempt == 2) {
					printf(
							"Measurement Plane:Thread creation failed,please re-start the program. \n");
				}
			}
			if (success == true) {
				attempt = 3;
			}
		}
	}
	//start Listener
	if ((rc3 = pthread_create(&mpThread, NULL, &MeasurementPlane::run, mp))) {
		printf("Measurement Plane:Thread creation failed: %d\n", rc3);

		int attempt = 0;
		while (attempt < 3) {
			bool success = true;
			Utilities::randomBackOfftime(7);
			if ((rc3 = pthread_create(&mpThread, NULL, &MeasurementPlane::run,
					mp))) {

				success = false;
				printf("Measurement Plane:Thread creation failed: %d\n", rc3);
				if (attempt == 2) {
					printf(
							"Measurement Plane:Thread creation failed,please re-start the program. \n");
				}
			}
			if (success == true) {
				attempt = 3;
			}
		}
	}
	pthread_t sessionObserver;
	SessionObserver *so = SessionObserver::Instance();

	//start MDME
	if ((rc3 = pthread_create(&sessionObserver, NULL, &SessionObserver::run, so))) {
			printf("Measurement Plane:Thread creation failed: %d\n", rc3);
		}

//	pthread_t mainDMEThread;
//	MainDME *mdme = MainDME::Instance();
//
//	//start MDME
//		if ((rc3 = pthread_create(&mainDMEThread, NULL, &MainDME::run, mdme))) {
//			printf("Measurement Plane:Thread creation failed: %d\n", rc3);
//
//			int attempt = 0;
//			while (attempt < 3) {
//				bool success = true;
//				Utilities::randomBackOfftime(7);
//				if ((rc3 = pthread_create(&mpThread, NULL, &MeasurementPlane::run,
//						mp))) {
//
//					success = false;
//					printf("Measurement Plane:Thread creation failed: %d\n", rc3);
//					if (attempt == 2) {
//						printf(
//								"Measurement Plane:Thread creation failed,please re-start the program. \n");
//					}
//				}
//				if (success == true) {
//					attempt = 3;
//				}
//			}
//		}
	//TODO:NEW ADDED PART
//	pthread_t changeTP;
//	ChangeTransmissionPower *ctp = ChangeTransmissionPower::Instance();
//
//	//start Listener
//		if ((rc3 = pthread_create(&changeTP, NULL, &ChangeTransmissionPower::run, ctp))) {
//			printf("Measurement Plane:Thread creation failed: %d\n", rc3);
//
//			int attempt = 0;
//			while (attempt < 3) {
//				bool success = true;
//				Utilities::randomBackOfftime(7);
//				if ((rc3 = pthread_create(&changeTP, NULL, &ChangeTransmissionPower::run, ctp))) {
//
//					success = false;
//					printf("Measurement Plane:Thread creation failed: %d\n", rc3);
//					if (attempt == 2) {
//						printf(
//								"Measurement Plane:Thread creation failed,please re-start the program. \n");
//					}
//				}
//				if (success == true) {
//					attempt = 3;
//				}
//			}
//		}

	pthread_join(upmainThread, NULL);
	pthread_join(qoeThread, NULL);
	pthread_join(mpThread, NULL);
	pthread_join(sessionObserver, NULL);
	//pthread_join(mainDMEThread, NULL);
	//pthread_join(changeTP,NULL);
	return 0;
}
