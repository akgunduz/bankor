//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef COLLECTOR_H
#define	COLLECTOR_H

#include "Component.h"
#include "Connector.h"
#include "Message.h"
#include "Job.h"
#include "Jobs.h"

class Collector : public Component {

	Jobs jobs;
	long distributorAddress;

    Collector(const char *rootPath);

	long getDistributorAddress();
	bool setDistributorAddress(long);

	bool processDistributorWakeupMsg(ComponentObject, long, Message *);
	bool processDistributorIDMsg(ComponentObject, long, Message *);
	bool processDistributorNodeMsg(ComponentObject, long, Message *);
	bool processNodeInfoMsg(ComponentObject, long, Message *);
	bool processNodeBinaryMsg(ComponentObject, long, Message *);

	bool send2DistributorAliveMsg(long);
	bool send2DistributorNodeMsg(long, const char*, TypeMD5List*);
	bool send2NodeJobMsg(long, const char*, long, const char*, TypeFileInfoList*);
	bool send2NodeBinaryMsg(long, const char*, long, const char*, TypeFileInfoList*);
	bool send2NodeReadyMsg(long);

public:


    ~Collector();
    static Collector* newInstance(const char*);

    bool processJob(int);
    bool processJobs();
	Jobs* getJobs();

};

#endif	/* COLLECTOR_H */

