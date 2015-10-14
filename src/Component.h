//
// Created by Haluk Akgunduz on 10/14/15.
//

#ifndef BANKOR_COMPONENT_H
#define BANKOR_COMPONENT_H

#include "Connector.h"

class Component {

protected :

    InterfaceCallback *callback;
    Connector *connectors[HOST_MAX];

    const char* rootPath;

public:
    Component(long index, const char* rootPath);
    ~Component();
    const char* getRootPath();
    INTERFACES getInterfaceType(HOST host);
    long getAddress(HOST host);
    static bool receiveCB(void *, long, Message *);
    bool onReceive(long, Message *);
    static long generateIndex(int indexFirst, int indexSecond, int indexThird);

    virtual bool processDistributorMsg(long, Message *) = 0;
    virtual bool processCollectorMsg(long, Message *) = 0;
    virtual bool processClientMsg(long, Message *) = 0;

};


#endif //BANKOR_COMPONENT_H
