//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef __UnixSocket_H_
#define __UnixSocket_H_

#include "Common.h"
#include "Scheduler.h"
#include "Interface.h"
#include "Message.h"

#define MAX_SIMUL_CLIENTS 10

class UnixSocket : public Interface {
private :
	int unixSocket;
	bool initUnixSocket();
	void runReceiver(Unit host);
	static void *runAccepter(void *);
	void runSender(long, Message *);
    void runMulticastSender(Message *);
	void setAddress(int);
	INTERFACES getType();

	static std::vector<Device>deviceList;

public :
	static uint16_t gOffset;
	UnixSocket(const Unit, Device*, const InterfaceCallback *);
	std::vector<long> getAddressList();
	~UnixSocket();

	static bool createDevices();
    static std::vector<Device>* getDevices();
};

#endif //__UnixSocket_H_
