//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_NET_H
#define DICON_NET_H

#include "Common.h"
#include "Scheduler.h"
#include "Interface.h"
#include "Message.h"
#include "Component.h"
#include "InterfaceType.h"

#define LOOPBACK_RANGE 256
#define MULTICAST_ADDRESS 0xE9010101
#define DEFAULT_PORT 61001
#define DEFAULT_MULTICAST_PORT 62001

class Net : public Interface {

	int netSocket{};
	int multicastSocket{};

	bool initTCP();
	bool initMulticast();
    bool runReceiver() override;
	bool runSender(ComponentUnit, TypeMessage) override;
    bool runMulticastSender(ComponentUnit, TypeMessage) override;

public :

	Net(TypeDevice&, const InterfaceSchedulerCB*, const InterfaceHostCB*);
	~Net() override;

    INTERFACE getType() override;
	bool isSupportMulticast() override;

    TypeAddressList getAddressList() override;

    static TypeReadCB getReadCB(ComponentUnit&);
    static TypeWriteCB getWriteCB(ComponentUnit&);
};

#endif //DICON_NET_H
