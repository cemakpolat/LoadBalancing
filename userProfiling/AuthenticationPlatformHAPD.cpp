
/*
 * AuthenticationPlatformHAPD.cpp
 *
 *  Created on:  Oct 17, 2012
 *      Author: cem and Mursel
 * Authentication Message Format:
 * {
 * 		"userId"="",
 * 		"IP"="",
 * 		"MAC"="",
 * 		"Token"="",
 * 		"State"=""-> authenticated or deauthenticated
 * }
 */

#include <iostream>
#include <string>
#include <sstream>
#include <pthread.h>
#include "UPMain.h"
#include "AuthenticationPlatformHAPD.h"
//#include "writer.h"
//#include "elements.h"
//#include "reader.h"
#include <unistd.h>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include "Utilities.h"
#include <memory>
//#include "ConfigParameters.h"

const string AuthenticationPlatformHAPD::ASSOCIATED="Association";
const string AuthenticationPlatformHAPD::DISASSOCIATED="Disassociation";
const string AuthenticationPlatformHAPD::AUTHENTICATED="Authentication";
const string AuthenticationPlatformHAPD::DEAUTHENTICATED="Deauthentication";
 AuthenticationPlatformHAPD::AuthenticationPlatformHAPD(){

}
 AuthenticationPlatformHAPD::~AuthenticationPlatformHAPD(){

}

bool AuthenticationPlatformHAPD::checkUserInAuthenticatedList(string& mac){
	bool state=false;
	try{
		usleep(1*1000000);
		pthread_mutex_lock(&UPMain::Instance()->mutex1);
		for (unsigned int i = 0; i < UPMain::Instance()->userAuthList.size(); i++) {
			cout << "AuthAssoBlock: Client mac  " << mac << " current MAC"<<UPMain::Instance()->userAuthList[i].Mac
					<<"active state: "<<UPMain::Instance()->userAuthList[i].active << endl;

			if (UPMain::Instance()->userAuthList[i].Mac==mac && UPMain::Instance()->userAuthList[i].active ) {
				state=true;
				break;
			}
		}
		pthread_mutex_unlock(&UPMain::Instance()->mutex1);
	}catch(exception& e){e.what();}
	return state;
}
/*
 *
void *ThreadMain(void *clntSock) {
  // Guarantees that thread resources are deallocated upon return
  pthread_detach(pthread_self());

  // Extract socket file descriptor from argument
  HandleTCPClient((TCPSocket *) clntSock);

  delete (TCPSocket *) clntSock;
  return NULL;
}
 */
UserAuthListObject* AuthenticationPlatformHAPD::jsonHostapdToUserObject(Object& jsonObj){
		try{
			String userId=jsonObj["userId"];
			String mac=jsonObj["MAC"];
			String token=jsonObj["Token"];
			//std::auto_ptr<UserAuthListObject> client(new UserAuthListObject(userId.Value(),"",mac.Value(),token.Value()));
			UserAuthListObject *client=new UserAuthListObject(userId.Value(),"",mac.Value(),token.Value());
			client->active=false;
			return client;
		}catch(exception& e){
			cerr<<"AuthAssoBlock: "<<e.what()<<endl;
			try{
			//std::auto_ptr<UserAuthListObject> client(new UserAuthListObject("","","",""));
			UserAuthListObject *client=new UserAuthListObject("","","","");
			client->active=false;
			return client;
			}catch(json::Exception& e){
				cerr<<"AuthAssoBlock: "<<e.what()<<endl;
				//std::auto_ptr<UserAuthListObject> client(new UserAuthListObject("","","",""));
				UserAuthListObject *client=new UserAuthListObject("","","","");
				return client;
			}
		}

}
/*UserAuthListObject AuthenticationPlatformHAPD::jsonToUserObject(
		Object& jsonObj) {
	try {
		String& userId = jsonObj["userId"]; //TODO:checkPointer
		String& ip = jsonObj["IP"];
		String& mac = jsonObj["MAC"];
		String& token = jsonObj["Token"];
		std::auto_ptr<UserAuthListObject> client(
				new UserAuthListObject(userId.Value(), ip.Value(), mac.Value(),
						token.Value()));
		//UserAuthListObject *client=new UserAuthListObject(userId.Value(),ip.Value(),mac.Value(),token.Value());
		client->active = true;
		return *client;
	} catch (json::Exception& e) {
		e.what();
		std::auto_ptr<UserAuthListObject> client(
				new UserAuthListObject("", "", "", ""));
		return *client;
	}
}*/
struct composedObject{
	AuthenticationPlatformHAPD* aut;
	UserAuthListObject* newUser;
};
void *AuthenticationPlatformHAPD::run(void *object) {
	int portNumber = 5011;
	printf("AuthAssoBlock:AuthenticationPlatformHAPD started\n");
	usleep(Utilities::second(2));
	AuthenticationPlatformHAPD *authObj = (AuthenticationPlatformHAPD*) object;
	//Set configuration
	vector<pthread_t> plist;
	pthread_t newClient;
	pthread_t IPGetter;
	UserAuthListObject *newUser;
	try {
		TCPServerSocket servSock(portNumber);

		while (true) {
			Object message = authObj->getMessage(servSock.accept());
			String state = String("");

			state = message["State"];

			if (state.Value() == AuthenticationPlatformHAPD::ASSOCIATED) {
				cout << "AuthAssoBlock: Client in  " << state.Value() << " state" << endl;

				String& userId = message["userId"];
				//UserAuthListObject newUser = authObj->jsonHostapdToUserObject(message);

				newUser= authObj->jsonHostapdToUserObject(message); //temporary code
				pthread_mutex_lock(&UPMain::Instance()->mutex1);
				bool userExist = false;
				for (unsigned int i = 0;i < UPMain::Instance()->userAuthList.size(); i++) {
					if (userId == UPMain::Instance()->userAuthList[i].userId) {
						userExist = true;
					}
				}
				if (userExist != true) {
					UPMain::Instance()->userAuthList.push_back(*newUser);
				}
				pthread_mutex_unlock(&UPMain::Instance()->mutex1);

			} else if (state.Value()== AuthenticationPlatformHAPD::AUTHENTICATED) {
				cout << "AuthAssoBlock: Client in  " << state.Value() << " state" << endl;
				try {
					//String& userId = message["userId"];
					String mac = message["MAC"];
					//cout<<"AuthAssoBlock: Mac address: "<< mac.Value()<<endl;
					if(authObj->userInList(mac)){
					if (!authObj->checkUserInAuthenticatedList(mac) ) {
						//pthread_t newClient;

						int rc;
						//UserAuthListObject newUser;// =authObj->getAuthenticatedUser(mac);
						authObj->getAuthenticatedUser(mac,newUser);
						newUser->ip = authObj->getIPFromDHCPTable(mac);
						cout<<"AuthAssoBlock: Mac address 1: "<< newUser->Mac << " "<<newUser->userId<<" "<<newUser->ip<<endl;
						if (newUser->ip == "0.0.0.0") {
							cout << "AuthAssoBlock:IP address received as 0.0.0.0"
									<< endl;
							usleep(Utilities::second(2));

							int rc1;
							//pthread_t IPGetter;
							composedObject composedObj; //can be new class instead of struct so that we can deallocate
							composedObj.aut = authObj;
							composedObj.newUser = newUser;
							if ((rc1 = pthread_create(&IPGetter, NULL,
									authObj->getIP, &composedObj))) { //upm is already pointer no need to write address like &upm
								printf("AuthAssoBlock:Thread creation failed: %d\n", rc1);
							}

						} else {
							newUser->active = true;
							cout<<"AuthAssoBlock: Mac address: "<< newUser->Mac << newUser->userId<<endl;
							UPMonitoring *upm = new UPMonitoring(newUser);

							pthread_mutex_lock(&UPMain::Instance()->mutex1);
							for (unsigned int i = 0;
									i < UPMain::Instance()->userAuthList.size();
									i++) {
								if (newUser->userId
										== UPMain::Instance()->userAuthList[i].userId) {
									//delete UPMain::Instance()->userAuthList[i];
									UPMain::Instance()->userAuthList[i] =
											*newUser;
								}
							}
							pthread_mutex_unlock(&UPMain::Instance()->mutex1);
							cout<<"AuthAssoBlock: Mac address: "<< upm->Mac<<endl;
							if ((rc = pthread_create(&newClient, NULL,
									&UPMonitoring::run, upm)) && upm!=NULL) {
								//upm is already pointer no need to write address like &upm
								printf("AuthAssoBlock:Thread creation failed: %d\n", rc);
								int attempt = 0;
								while (attempt < 3) {
									bool success = true;
									Utilities::randomBackOfftime(7);
									if ((rc = pthread_create(&newClient, NULL,
											&UPMonitoring::run, upm))) {
										success = false;
										printf(
												"AuthAssoBlock:Thread creation failed: %d\n",
												rc);
										if (attempt == 2) {
											printf(
													"AuthAssoBlock:Thread creation failed,please re-start the program. \n");
										}
									}
									if (success == true) {
										attempt = 3;
									}
								}
							}
							plist.push_back(newClient);
							//authObj->upmonitors.push_back(upm);
						}

					} else {
						cout
								<< "AuthAssoBlock: New authentication message arrived for an already authenticated user!"
								<< endl;
					}
				}else{
					cout<< "AuthAssoBlock: User is not authenticated!"<< endl;
				}
				}catch (SocketException &e) {
					cerr <<"AuthAssoBlock: "<< e.what() << "socket" <<endl;
					usleep(Utilities::second(5));
					AuthenticationPlatformHAPD::run(object);
				}
				catch (json::Exception& e) {
					e.what();
					cout << "AuthAssoBlock:Authentication process failed" << endl;
					AuthenticationPlatformHAPD::run(object);
				}
			} else if (state.Value()== AuthenticationPlatformHAPD::DEAUTHENTICATED
					|| state.Value()== AuthenticationPlatformHAPD::DISASSOCIATED) {
				try {
					String& mac = message["MAC"];
					cout << "AuthAssoBlock: Client in " << state.Value() << " state"
							<< endl;
					cout << "AuthAssoBlock: Authenticated User Count: "
													<< UPMain::Instance()->userAuthList.size()
													<< endl;

					if (authObj->checkUserInAuthenticatedList(mac)) {

						pthread_mutex_lock(&UPMain::Instance()->mutex1);
						cout << "AuthAssoBlock: Authenticated User Count: "
								<< UPMain::Instance()->userAuthList.size()<< endl;
						int index;
						for (unsigned int i = 0;
								i < UPMain::Instance()->userAuthList.size();
								i++) {
							if (mac== UPMain::Instance()->userAuthList[i].Mac) {
								index = i;
							}
						}
						UPMain::Instance()->userAuthList.erase(
								UPMain::Instance()->userAuthList.begin()
										+ index);
						cout << "AuthAssoBlock:Authenticated User Count: "
								<< UPMain::Instance()->userAuthList.size()
								<< endl;
						pthread_mutex_unlock(&UPMain::Instance()->mutex1);
						pthread_join(plist[0],NULL);
						//authObj->handleTerminatedThreadObject(mac);
//						if(int rc =pthread_join(plist[0])){
//							printf("AuthAssoBlock:Thread creation failed: %d\n", rc);
//						}
//
						plist.clear();
					} else {

						cout
								<< "AuthAssoBlock: Client has already left the connection area "
								<< endl;
					}
				} catch (json::Exception& e) {
					e.what();
					cout << "AuthAssoBlock:Deauthentication or Disassociation failed" << endl;
				}catch (SocketException &e) {
					cerr <<"AuthAssoBlock: "<< e.what() << "socket" <<endl;
					usleep(Utilities::second(5));
					AuthenticationPlatformHAPD::run(object);
				}
			} else {
				cout << "AuthAssoBlock: Unknown message" << endl;
			}
		}
	} catch (SocketException &e) {
		cerr <<"AuthAssoBlock: "<< e.what() << "socket" <<endl;
		AuthenticationPlatformHAPD::run(object);
		usleep(Utilities::second(2));
	} catch (exception& e) {
		cerr <<"AuthAssoBlock: "<< e.what()<<endl;
		//usleep(Utilities::second(5));
		AuthenticationPlatformHAPD::run(object);
	}

	pthread_exit(NULL); //exit from thread, but this object will take place in memory.
}
bool AuthenticationPlatformHAPD::userInList(string& mac){
	bool userExist = false;
	pthread_mutex_lock(&UPMain::Instance()->mutex1);
	for (unsigned int i = 0;i < UPMain::Instance()->userAuthList.size(); i++) {
		if (mac == UPMain::Instance()->userAuthList[i].Mac) {
			userExist = true;
		}
	}
	pthread_mutex_unlock(&UPMain::Instance()->mutex1);
	return userExist;


}
void *AuthenticationPlatformHAPD::getIP(void *obj){
	int loopCount=0;
	composedObject *comp=(composedObject*)obj;
	bool state=false;
	string mac=comp->newUser->Mac;
	string userId=comp->newUser->userId;
	string token=comp->newUser->token;

	string ip;
	while(loopCount<10){
		ip=comp->aut->getIPFromDHCPTable(comp->newUser->Mac);
		if(ip=="0.0.0.0"){
			cout<<"AuthAssoBlock: IP could not be fetched, there could be any record in dhcp.lease \n file related to this user"<<endl;
			usleep(Utilities::second(2));
			state=false;
		}else{
			state=true;

			cout<<"AuthAssoBlock: IP address is received: "<<ip<<endl;
			UserAuthListObject newUser;
			newUser.Mac=mac;
			newUser.ip=ip;
			newUser.active=true;
			newUser.userId=userId;
			newUser.token=token;
			//cout<<" "<<ip<<" "<< userId<<" " <<mac <<endl;
			UPMonitoring *upm = new UPMonitoring(newUser);

			//cout<<"AuthAssoBlock: IP address 3.1"<<endl;
			pthread_mutex_lock(&UPMain::Instance()->mutex1);
			for(unsigned int i=0;i<UPMain::Instance()->userAuthList.size();i++){
				if(comp->newUser->userId==UPMain::Instance()->userAuthList[i].userId){
					UPMain::Instance()->userAuthList[i]=(newUser);
				}
			}
			pthread_mutex_unlock(&UPMain::Instance()->mutex1);
			int rc;pthread_t newClient;
			if ((rc = pthread_create(&newClient, NULL, &UPMonitoring::run, upm))&& upm!=NULL) {
				printf("AuthAssoBlock:Thread creation failed: %d\n", rc);
				int attempt = 0;
				while (attempt < 3) {
					bool success = true;
					Utilities::randomBackOfftime(7);
					if ((rc = pthread_create(&newClient, NULL,
							&UPMonitoring::run, upm))) {
						success = false;
						printf("AuthAssoBlock:Thread creation failed: %d\n", rc);
						if (attempt == 2) {
							printf(
									"AuthAssoBlock:Thread creation failed,please re-start the program. \n");
						}
					}
					if (success == true) {
						attempt = 3;
					}
				}
			}
			//comp->aut->upmonitors.push_back(upm);
			loopCount=11;
		}
		loopCount++;
	}
	if(state==false){
		cout<<"AuthAssoBlock: IP could not be fetched, there could be any valid record in dhcp.lease \n "
				"	file related to this user or DHCP record is too old for this user"<<endl;
		pthread_mutex_lock(&UPMain::Instance()->mutex1);
		int index;
		for (unsigned int i = 0; i < UPMain::Instance()->userAuthList.size();
				i++) {
			if (comp->newUser->Mac == UPMain::Instance()->userAuthList[i].Mac) {
				index = i;
			}
		}
		UPMain::Instance()->userAuthList.erase(UPMain::Instance()->userAuthList.begin() + index);
		cout << "AuthAssoBlock:Authenticated User is being removed."<< endl;
		pthread_mutex_unlock(&UPMain::Instance()->mutex1);
	}
	pthread_exit(NULL);
}
/**UserAuthListObject
 * After receiving authenticated message from hostapd,  user information is returned by this function
 * User is not totally authenticated, although this message is being received.
 */
void AuthenticationPlatformHAPD::getAuthenticatedUser(string& mac,UserAuthListObject* user){

	pthread_mutex_lock(&UPMain::Instance()->mutex1);
	for(unsigned int i=0;i<UPMain::Instance()->userAuthList.size();i++){
		//cout<<" list mac: "<<UPMain::Instance()->userAuthList[i].Mac<<endl;
		//cout<<" list mac: "<<UPMain::Instance()->userAuthList[i].userId<<endl;
		if(mac==UPMain::Instance()->userAuthList[i].Mac ){
			user->Mac=UPMain::Instance()->userAuthList[i].Mac ;
			user->ip=UPMain::Instance()->userAuthList[i].ip;
			user->token=UPMain::Instance()->userAuthList[i].token;
			user->userId=UPMain::Instance()->userAuthList[i].userId;
			user->active=UPMain::Instance()->userAuthList[i].active;
			break;
		}
	}
	pthread_mutex_unlock(&UPMain::Instance()->mutex1);
}
Object AuthenticationPlatformHAPD::getMessage(TCPSocket *sock) {
	//read Message block and return message
	int val[1];
	sock->recv(val, sizeof(val));
	char echoBuffer2[val[0]];
	sock->recv(echoBuffer2, val[0]);
	echoBuffer2[val[0]] = '\0'; //TODO: you should add '\0' end of the line so as to prevent the additional data.
	delete sock;
	Object jsonObj;
	try {
		//convert here json
		istringstream jsonString(echoBuffer2);
		Reader::Read(jsonObj, jsonString);
	}
	 catch (json::Reader::ParseException& e) {
			e.what();
			jsonObj["userId"] = String("");
			jsonObj["MAC"] = String("");
			jsonObj["State"] = String("");
			jsonObj["Token"] = String("");
	}
	catch (json::Exception& e) {
		e.what();
		jsonObj["userId"] = String("");
		jsonObj["MAC"] = String("");
		jsonObj["State"] = String("");
		jsonObj["Token"] = String("");
	}
	//return converted JSON Version
	return jsonObj;
}
int dhcpCounter=0;
string AuthenticationPlatformHAPD::getIPFromDHCPTable(string& mac){
	//read from dhcp.leases file
	vector<DHCPObj> list=readDHCPFile();
	string ip="0.0.0.0";
	int time=0;
	bool exist=false;
	for(unsigned int i=0;i<list.size();i++){
		if(mac==list[i].mac && time<atoi(list[i].ts.c_str())){
			exist=true;
			ip=list[i].IP;
			time =atoi(list[i].ts.c_str());
			//cout<<" hello true"<<endl;
		}
	}
	if(exist==false && dhcpCounter<2){
		usleep(Utilities::second(3));
		dhcpCounter++;
		//cout<<" hello"<<endl;
		getIPFromDHCPTable(mac);
	}
	dhcpCounter=0;
	return ip;
}

vector<DHCPObj> AuthenticationPlatformHAPD::readDHCPFile(){
	vector<DHCPObj> templist;
	std::ifstream infile("/var/dhcp.leases");
	try {
		//int firstLine=0;
		std::string line;
		while (std::getline(infile, line)) {
			std::istringstream iss(line);
			string ts, mac, ip;
			if ((iss >> ts >> mac >> ip)) {
				if(Utilities::timeCompareWithCurrentTime(200,ts)){
					DHCPObj obj;
					obj.ts=ts;
					obj.IP = ip;
					obj.mac = mac;
					//cout<<obj.IP<< " "<<obj.mac<<endl;
					templist.push_back(obj);
				}
			/*	else{
					cout<<"AuthAssoBlock: DHCP record is too old!"<<endl;//false
				}*/
			}
		}
		return templist;
	} catch (exception& e) {
		e.what();
		cout << "AuthAssoBlock:DHCP.leases file could not be red" << endl;
		readDHCPFile();
	}
	return templist;
}
//!!!!!!We need here another thread which ought to deal with only removing the object
void AuthenticationPlatformHAPD::handleTerminatedThreadObject(string& mac) {
	/*for (unsigned int i = 0; i < this->upmonitors.size(); i++) {
		if (this->upmonitors[i]->Mac == mac) {
			int time = 0;
			while (time < 20) {
				if (this->upmonitors[i]->left == true) {
					cout << "AuthAssoBlock: \" " << this->upmonitors[i]->userId
							<< "\" userId has left the connection area, its thread terminated."
							<< endl;
					delete this->upmonitors[i];
					this->upmonitors.erase(this->upmonitors.begin() + i);
					time=21;
				} else {
					cout<<"AuthAssoBlock: UPM list : user's thread didn't still leave the connection area."<<endl;
					time = time + 1;
					usleep(Utilities::second(5));
				}
			}
		}

	}*/
}
/*void AuthenticationPlatformHAPD::setIPTablesConfiguration_1(){
	Total Chain output+input together
	system("iptables -N UBM_TRAFFIC_ACCT");
	system("iptables -I FORWARD -j UBM_TRAFFIC_ACCT");

	system("iptables -A UBM_TRAFFIC_ACCT -p tcp");
	system("iptables -A UBM_TRAFFIC_ACCT -p udp");
	system("iptables -A UBM_TRAFFIC_ACCT -p icmp");
}*/
/*void AuthenticationPlatformHAPD::setIPTablesConfiguration_2(){
	INPUT CHAIN
	system("iptables -X UBM_TRAFFIC_ACCT_IN");
	system("iptables -X UBM_TRAFFIC_ACCT_OUT");
	system("iptables -N UBM_TRAFFIC_ACCT_IN");
	system("iptables -N UBM_TRAFFIC_ACCT_OUT");
	system("iptables -I FORWARD -i br-lan -j UBM_TRAFFIC_ACCT_IN");
	system("iptables -I FORWARD -o br-lan -j UBM_TRAFFIC_ACCT_OUT");

	system("iptables -A UBM_TRAFFIC_ACCT_IN -p tcp");
	system("iptables -A UBM_TRAFFIC_ACCT_IN -p udp");
	system("iptables -A UBM_TRAFFIC_ACCT_IN -p icmp");
	OUTPUT CHAIN

	system("iptables -A UBM_TRAFFIC_ACCT_OUT -p tcp");
	system("iptables -A UBM_TRAFFIC_ACCT_OUT -p udp");
	system("iptables -A UBM_TRAFFIC_ACCT_OUT -p icmp");
	wlan0 IN/OUT interface

}*/
/*void AuthenticationPlatformHAPD::removeIPTablesConf_1(){
	system("iptables -F UBM_TRAFFIC_ACCT");
	system("iptables -X UBM_TRAFFIC_ACCT");
}*/
/*void AuthenticationPlatformHAPD::removeIPTablesConf_2(){
	system("iptables -F UBM_TRAFFIC_ACCT_IN");
		system("iptables -F UBM_TRAFFIC_ACCT_OUT");
		system("iptables -X UBM_TRAFFIC_ACCT_IN");
		system("iptables -X UBM_TRAFFIC_ACCT_OUT");

}*/


