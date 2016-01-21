/*
 * QoE.h
 *
 *  Created on: 08.08.2012
 *      Author: cem
 */

#ifndef QOE_H_
#define QOE_H_
#include <vector>
#include <pthread.h>
#include "QoETracker.h"
#include "ITGChecker.h"
#include "writer.h"
#include "elements.h"
#include "reader.h"

struct qoePacketObject {
	std::string userId;
	std::string ip;
	std::string time;
	json::Object qoeMeasurement;//change to QoS Object
	std::string mac;
};

class QoE {
private:
	static QoE* m_qoeInstance;
	ITGChecker *ditg;
	QoETracker *qoeTracker;
	double averageDelayNormalizationFactor;
	double packetLossRateNormalizationFactor;
	double jitterNormalizationFactor;
	double bitRateNormalizationFactor;


public:
	pthread_mutex_t mutexQoE; //TODO: check the necessity of this mutex!
	pthread_mutex_t mutexTimeQoE;
	QoE();
	~QoE();
	void qoeMain();
	static std::vector<qoePacketObject> qoeList;
	static QoE* Instance();
	static void* run(void*);
	int getAverageDelay();
	json::Object congestedusers();
	json::Object worstCongestedUser();
	json::Object firstCongestedUser();
	//int averageDelay();
	double averageBitRate();
	double averageBitRate2(std::vector<qoePacketObject> list);

	double averagePacketLoss();
	double averagePacketLoss2(std::vector<qoePacketObject> list);

	double averageDelay();
	double averageDelay2(std::vector<qoePacketObject> list);
	double averageDelayFirstDelay3(std::vector<qoePacketObject> list);

	double averageJitter();
	double averageJitter2(std::vector<qoePacketObject> list);

	std::vector<qoePacketObject> getQoEList();
};

#endif /* QOE_H_ */
