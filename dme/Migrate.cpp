///*
// * Migrate.cpp
// *
// *  Created on: Dec 18, 2012
// *      Author: cem
// */
//
//
//#include "Migrate.h"
////#include "Utilities.h"
//#include "BlackBoard.h"
//#include "CommLang.h"
//
//using namespace std;
//using namespace json;
//
//Migrate::Migrate() {
//
//}
//Migrate::~Migrate() {
//
//}
//void Migrate::run() {
//	int clientServerNumber = 14131;
//	int serverPortNumber = 14444;
//	//int portNumber=ConfigParameters::MeasurementPlanePortNumber;//14131
//	//usleep(Utilities::second(2));
//	try {
//		TCPServerSocket clientServSock(clientServerNumber); //port number
//	//	while (true) {
//		//add here console scan another program scan
//
//			//send packet
//			CommLang *cm = new CommLang();
//			//this->sendMessage(clientServSock.accept(),cm->createCommObject(serverPortNumber, 1, ""));
//			//Utilities::second(1);
//
//			TCPServerSocket servSock(serverPortNumber); //port number
//			//received packet
//			string pktString = this->getMessage(servSock.accept());
//			if (string::npos != pktString.find("timeout") || pktString != "") {
//				BlackBoard::writeConsole("Time out in collecting userQoEs ");
//			} else {
//				//Object pkt=Utilities::convertStringToJSON(pktString);
//				//String& content=pkt["content"];
//				//cout<<"Message Content"<<content.Value()<<endl;
//			}
//
//			delete cm;
//			//usleep(Utilities::second(10));
//			std::cout << "Migrate : " << std::endl;
//			//usleep(Utilities::second(50));
//
//		//}
//	} catch (SocketException &e) {
//		cerr << "Migrate : " << e.what() << "socket" << endl;
//		this->run ();
//
//	} catch (exception& e) {
//		cerr << "Migrate : " << e.what() << endl;
//		//usleep(Utilities::second(5));
//		this->run ();
//	}
//}
///**
// * Timer is designed for error handling. This function is used for some task
// * whether they are performed on time or not.
// */
//void* Migrate::runTimer(void* timer){
//
//}
//void* Migrate::run(void* object) {
//
//	int clientServerNumber = 14131;
//	int serverPortNumber=14444;
//	string ipAddress="192.168.1.1";
//	//int portNumber=ConfigParameters::MeasurementPlanePortNumber;//14131
//	//usleep(Utilities::second(2));
//	Migrate *mr = (Migrate*) object;
//	try {
//		TCPSocket sock(ipAddress,clientServerNumber);
//
//		TCPServerSocket clientServSock(clientServerNumber); //port number
//		TCPServerSocket servSock(serverPortNumber); //port number
//
//		while (true) {
//
//			//send packet
//			CommLang *cm=new CommLang();
//			//mr->sendMessage(clientServSock.accept(),cm->createCommObject(serverPortNumber,1,""));
//			//Utilities::second(2);
//			//received packet
//			 string pktString=mr->getMessage(servSock.accept());
//			 if (string::npos!=pktString.find("timeout") || pktString!="") {
//				 BlackBoard::writeConsole("Time out in collecting userQoEs ");
//			 } else {
//
//
//			 }
//
//			 delete cm;
//			//usleep(Utilities::second(10));
//			std::cout << "Migrate : " <<std::endl;
//		//	usleep(Utilities::second(50));
//		}
//	}catch (SocketException &e) {
//		cerr <<"Migrate : "<< e.what() << "socket" <<endl;
//		run(object);
//
//	}
//	catch (exception& e) {
//		cerr<<"Migrate : "<<e.what()<<endl;
//		//usleep(Utilities::second(5));
//		run(object);
//	}
//}
////SEND SOCKET
////------------------------------------------
//    /*TCPSocket sock("127.0.0.1", 6060);
//    // Send the string to the echo server
//    unsigned int size;
//
//    size=strlen(stream1.str().c_str());
//    	//  cout<<"size "<<size<<endl;
//    sock.send(&size, sizeof(size));
//
//    sock.send(stream1.str().c_str(), size);*/
////------------------------------------------
//void Migrate::sendMessageToClient(TCPSocket *sock){
//	// Establish connection with the echo server
//	    //TCPSocket sock(servAddress, echoServPort);
//		 // TCPSocket sock("127.0.0.1", 6060);
//
//	    // Send the string to the echo server
////		  int size;
////
////		 char *echoString="asdlfnlsdf;gd;fmdsf;lksdmf;g'ldmfglmfglkdfjg'sdfkgakdAOPMKCP[AOSKDCP[AOSJKDAPSOVFRA";
////		  size=strlen(echoString);
////		//  cout<<"size "<<size<<endl;
////		  sock->send(&size, sizeof(size));
////
////		  sock->send(echoString, strlen(echoString));
//		  delete sock;
//}
//void Migrate::sendMessage(TCPSocket *sock,string message){
//	int val[1];
//	//send size of packet
//	val[0]=message.length();
//	int errorCount=0;
//	try{
//
//		sock->send(val,sizeof(val));
//		//send packet itself
//		sock->send(message.c_str(),val[0]);
//		delete sock;
//	}
//	catch(SocketException& e){
//		cerr<<"Migrate Socket Exception:\n"<<e.what()<<endl;
//		if(errorCount<2){
//			//Utilities::randomBackOfftime(6);
//			sendMessage(sock,message);
//		}
//	}
//
//}
//string Migrate::getMessage(TCPSocket *sock) {
//	try {
//		//get size of incoming packet and reserve the required buffer place
//		int val[1];
//		sock->recv(val, sizeof(val));
//		char echoBuffer2[val[0]];
//
//		//get the packet itself and end it with '\0' in order to not receive extra characters.
//		sock->recv(echoBuffer2, val[0]);
//		echoBuffer2[val[0]] = '\0';
//
//		return echoBuffer2;
//	} catch (SocketException& e) {
//		cerr<<"Migrate Socket Exception: \n"<<e.what()<<endl;
//	}
//
//	return "";
//}
////int main(){
////	//Migrate *migrate=new Migrate();
////	//without thread
////	Migrate migrate;
////	/*
////		migrate.run();
////	*/
////	//with thread
////	pthread_t migratethread;
////	int rc1;
////	//start upmain
////		if ((rc1 = pthread_create(&migratethread, NULL, &Migrate::run, &migrate))) {
////			printf("Measurement Plane:Thread creation failed: %d\n", rc1);
////			int attempt = 0;
////			while (attempt < 3) {
////				bool success = true;
////				Utilities::randomBackOfftime(7);
////				if ((rc1 = pthread_create(&migratethread, NULL, &Migrate::run, &migrate))) {
////
////					success = false;
////					printf("Measurement Plane:Thread creation failed: %d\n", rc1);
////					if (attempt == 2) {
////						printf(
////								"Measurement Plane:Thread creation failed,please re-start the program. \n");
////					}
////				}
////				if (success == true) {
////					attempt = 3;
////				}
////			}
////		}
////	pthread_join(migratethread, NULL);
////	return 0;
////
////}
