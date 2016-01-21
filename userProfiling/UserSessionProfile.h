/*
 * UserSessionProfile.h
 *
 *  Created on: 09.08.2012
 *      Author: cem
 */

#ifndef USERSESSIONPROFILE_H_
#define USERSESSIONPROFILE_H_

#include <string>
#include "writer.h"
#include "elements.h"
#include "reader.h"
#include "UserBandwidth.h"
#include "UserSession.h"

using namespace json;
class UserSessionProfile {
private:
	void getUserModelInfo(Object& userModel);
	void firstUserSessionsDeployment(Object& userModel);
public:
	UserSessionProfile();
	~UserSessionProfile();
	UserSessionProfile(Object& userModel,String userId, String ip, int timeInterval);
	std::string userId;
	int timeInterval;
	UserBandwidth *bandwidth;//TODO:check ? definition should be bandwidth
	UserSession *session;
	void transferUserProfileInfoIntoUserModel(Object& userModel);//
};


#endif /* USERSESSIONPROFILE_H_ */
