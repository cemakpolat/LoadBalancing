/*
 * AuthenticationPlatformHAPD.h
 *
 *  Created on: Oct 17, 2012
 *      Author: ufit
 */

#ifndef AUTHENTICATIONPLATFORMHAPD_H_
#define AUTHENTICATIONPLATFORMHAPD_H_


#include "PracticalSocket.h"
#include "writer.h"
#include "elements.h"
#include "reader.h"
#include "UPMonitoring.h"
using namespace std;
#include <vector>
struct AssObj{
	string mac;
	string userId;
};
struct DHCPObj{
	string mac;
	string IP;
	string ts;
};
class AuthenticationPlatformHAPD{

private:
	static const string AUTHENTICATED;
	static const string ASSOCIATED;
	static const string DISASSOCIATED;
	static const  string DEAUTHENTICATED;
	static const int MESSAGEBUFFERSIZE=1000;
public:
	//TODO:declare here a vector in which user object are kept.
	//static vector<AssObj> associatedUserList;

	//vector<UPMonitoring*> upmonitors;
	static void *run(void *context);
	AuthenticationPlatformHAPD();
	~AuthenticationPlatformHAPD();
	//string getMessage(TCPSocket *sock);
	json::Object getMessage(TCPSocket *sock);
	bool checkUserInAuthenticatedList(string& mac);
	UserAuthListObject jsonToUserObject(json::Object& jsonObj);
	UserAuthListObject* jsonHostapdToUserObject(Object& jsonObj);
	void handleTerminatedThreadObject(string& mac);
	void authenticationInvestigator();
	string getIPFromDHCPTable(string& mac);
	vector<DHCPObj> readDHCPFile();
	void getAuthenticatedUser(string& mac,UserAuthListObject* user);
	bool userInList(string& mac);
	static void *getIP(void *obj);//for IP address from ARP File
	void setIPTablesConfiguration_1();
	void setIPTablesConfiguration_2();
	void removeIPTablesConf_1();
	void removeIPTablesConf_2();
};



#endif /* AUTHENTICATIONPLATFORMHAPD_H_ */
