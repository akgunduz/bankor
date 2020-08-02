//
// Created by Haluk Akgunduz on 10/14/15.
//

#ifndef DICON_COMPONENT_H
#define DICON_COMPONENT_H

#include "Connector.h"
#include "DeviceList.h"
#include "MessageTypes.h"
#include "HostUnit.h"
#include "StopWatch.h"

enum NOTIFYSTATE {
    NOTIFYSTATE_PASSIVE,
    NOTIFYSTATE_ACTIVE,
    NOTIFYSTATE_ONCE,
    NOTIFYSTATE_TRANSPARENT,
    NOTIFYSTATE_MAX,
};

class Component;

typedef bool (Component::*TypeProcessComponentMsg)(ComponentUnit&, Message *);
typedef bool (*TypeStaticProcessComponentMsg)(Component*, ComponentUnit&, Message *);

typedef std::map<const MSG_TYPE, TypeProcessComponentMsg> TypeProcessMsgMap;
typedef std::map<const MSG_TYPE, TypeStaticProcessComponentMsg> TypeStaticProcessMsgMap;

typedef bool (*TypeNotifyCB)(void*, COMPONENT, NOTIFYSTATE);

class Component {

    char rootPath[PATH_MAX]{};

    Interface *interfaces[COMP_MAX]{};

    static void *notifyContext;
    static TypeNotifyCB notifyCB;

protected :

    HostUnit *host{};

    TypeProcessMsgMap processMsg[COMP_MAX];
    TypeStaticProcessMsgMap processStaticMsg[COMP_MAX];

    const InterfaceSchedulerCB *schedulerCB;
    const InterfaceHostCB *hostCB;

    StopWatch componentWatch;

    static HostUnit& getHostCB(void*);

    bool initInterfaces(COMPONENT type, int, int);

    void notifyUI(NOTIFYSTATE);

public:

    explicit Component(const char* rootPath);
    virtual ~Component();

    HostUnit& getHost();

    const char* getRootPath();

    Device* getDevice(COMPONENT);
    Address& getInterfaceAddress(COMPONENT);
    Address& getInterfaceMulticastAddress(COMPONENT);
    TypeAddressList getInterfaceAddressList(COMPONENT);
    INTERFACE getInterfaceType(COMPONENT);
    bool isSupportMulticast(COMPONENT);
    static bool receiveCB(void *, SchedulerItem*);
    bool onReceive(ComponentUnit&, MSG_TYPE, Message *);

    bool send(ComponentUnit&, Message*);

    bool defaultProcessMsg(ComponentUnit&, Message *);

    static void registerNotify(void*, TypeNotifyCB);

    void setID(long);

    bool addProcessHandler(COMPONENT, MSG_TYPE, TypeProcessComponentMsg);
    bool addStaticProcessHandler(COMPONENT, MSG_TYPE, TypeStaticProcessComponentMsg);

};


#endif //DICON_COMPONENT_H
