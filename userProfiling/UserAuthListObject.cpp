/*
 * UserAuthListObject.cpp
 *
 *  Created on: 27.07.2012
 *      Author: cem
 */
#include <string>
#include "UserAuthListObject.h"

UserAuthListObject::UserAuthListObject(std::string userId,std::string ip,std::string Mac,std::string token){
	this->Mac=Mac;
	this->userId=userId;
	this->token=token;
	this->ip=ip;
	//this line added for hostapd, if this object is not activ, then its packets will not be considered.
	this->active=false;
	//this->state=state;
}
UserAuthListObject::UserAuthListObject(){

}
UserAuthListObject::~UserAuthListObject(){

}
