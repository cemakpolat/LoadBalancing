/*
 * MeasurementPlane.h
 *
 *  Created on: 08.08.2012
 *      Author: cem
 */

#ifndef MEASUREMENTPLANE_H_
#define MEASUREMENTPLANE_H_


#include "TCPConnection.h"

class MeasurementPlane {
private:
	static MeasurementPlane* m_mpInstance;
protected:
public:
	TCPConnection *con;
	static MeasurementPlane* Instance();
	static void* run(void* object);
	string getMessage(TCPSocket *sock);
	void sendMessage(TCPSocket *sock,string message);

	MeasurementPlane();
	~MeasurementPlane();
};


#endif /* MEASUREMENTPLANE_H_ */

