//
// Created by Haluk Akgunduz on 24/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//
#ifndef __Interface_H_
#define __Interface_H_

#include "Scheduler.h"
#include "Message.h"
#include "Address.h"

class Interface;

enum NOTIFIER_TYPE {
	NOTIFIER_READ,
	NOTIFIER_WRITE
};

class Argument {

public:
	union {
		int acceptSocket;
		Message *msg;
	} var;

	long address;

	Interface *_interface;

	Argument(Interface *c) : _interface(c){

	}
};
/*
class InitArgument {
public:
//	uint32_t interfaceIndex;
	std::string rootPath;
	InitArgument(uint32_t index, std::string path) : interfaceIndex(index), rootPath(path) {}
};
*/
class Interface {
private :
	static void* runReceiver(void *);
	static void* runSender(void *);
	static bool senderCB(void *, long, Message *);
protected :
	bool initialized = false;
	long address;
	Scheduler *scheduler;
	pthread_t thread;
	int notifierPipe[2];
	virtual bool init(uint32_t) = 0;
	virtual void runReceiver() = 0;
	virtual void runSender(long, Message *) = 0;

	bool initThread();
	void end();
	Interface(INTERFACES type, const InterfaceCallback *, const std::string &);
	virtual void setAddress(uint32_t) = 0;

public :
	std::string rootPath;
	bool push(MESSAGE_DIRECTION, long, Message *);
	int getNotifier(NOTIFIER_TYPE type);
	virtual INTERFACES getType() = 0;
	virtual long getAddress();
	virtual std::vector<long> getAddressList() = 0;
	virtual ~Interface();
};


#endif //__Connector_H_
