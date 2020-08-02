//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DISTRIBUTOR_H
#define	DISTRIBUTOR_H

#include "Component.h"
#include "Node.h"
#include "Connector.h"
#include "Util.h"
#include "NodeManager.h"
#include "CollectorManager.h"
#include "DistributorObject.h"

class Distributor : public Component {
private:

    static Distributor *instance;

    volatile bool runCollThread = true;

    volatile bool runPollThread = true;

	NodeManager *nodeManager;

	CollectorManager *collectorManager;

    std::thread collThread;
    std::thread pollThread;

    explicit Distributor(const char *, int, int, bool);

    static void pollProcessCB(Distributor *);
	void pollProcess();

    static void collProcessCB(Distributor *);
    void collProcess();

    bool processCollectorAliveMsg(ComponentUnit&, Message *);
    bool processCollectorIDMsg(ComponentUnit&, Message *);
    bool processCollectorNodeMsg(ComponentUnit&, Message *);
    bool processCollectorReadyMsg(ComponentUnit&, Message *);

    bool processNodeAliveMsg(ComponentUnit&, Message *);
    bool processNodeIDMsg(ComponentUnit&, Message *);
    bool processNodeBusyMsg(ComponentUnit&, Message *);
    bool processNodeReadyMsg(ComponentUnit&, Message *);

	bool send2CollectorWakeupMsg(ComponentUnit&);
	bool send2CollectorIDMsg(ComponentUnit&, long);
	bool send2CollectorNodeMsg(ComponentUnit&, TypeComponentList&);

    bool send2NodeWakeupMsg(ComponentUnit&);
    bool send2NodeIDMsg(ComponentUnit&, long);
	bool send2NodeProcessMsg(ComponentUnit&);

public:


	~Distributor() override;
    static Distributor* newInstance(const char*, int, int, bool);

    bool sendWakeupMessage(COMPONENT);
    bool sendWakeupMessagesAll(bool);

    bool clear();
    CollectorManager* getCollectors() const;
    NodeManager* getNodes() const;
};

#endif	/* DISTRIBUTOR_H */
