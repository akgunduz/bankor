//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_DISTRIBUTOR_H
#define	DICON_DISTRIBUTOR_H

#include "Component.h"
#include "Node.h"
#include "InterfaceFactory.h"
#include "Util.h"
#include "NodeManager.h"
#include "CollectorManager.h"
#include "DistributorObject.h"

#define WAKEUP_INTERVAL 5

typedef std::map<long, TypeComponentUnitList> TypeComponentReplaceIDList;

class Distributor : public Component {

    static Distributor *instance;

    volatile bool runCollThread = true;

    volatile bool runPollThread = true;

	NodeManager *nodeManager;

	CollectorManager *collectorManager;

    std::thread collThread;
    std::thread pollThread;

    explicit Distributor(const char *, int, int, bool);

	void pollProcess();
    void collProcess();

    bool processCollectorAliveMsg(ComponentUnit&, TypeMessage);
    bool processCollectorIDMsg(ComponentUnit&, TypeMessage);
    bool processCollectorNodeMsg(ComponentUnit&, TypeMessage);
    bool processCollectorReadyMsg(ComponentUnit&, TypeMessage);

    bool processNodeAliveMsg(ComponentUnit&, TypeMessage);
    bool processNodeIDMsg(ComponentUnit&, TypeMessage);
    bool processNodeBusyMsg(ComponentUnit&, TypeMessage);
    bool processNodeReadyMsg(ComponentUnit&, TypeMessage);

	bool send2CollectorWakeupMsg(ComponentUnit&);
	bool send2CollectorIDMsg(ComponentUnit&, long);
	bool send2CollectorNodeMsg(ComponentUnit&, TypeComponentUnitList&);
	bool send2CollectorReplaceMsg(ComponentUnit&, TypeComponentUnitList&);

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

typedef std::unique_ptr<Distributor> TypeDistributor;

#endif //DICON_DISTRIBUTOR_H
