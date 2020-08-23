//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#ifndef DICON_COMPONENTMANAGER_H
#define DICON_COMPONENTMANAGER_H

#include "Common.h"
#include "HostUnit.h"

#define ALIVE_INTERVAL 8
#define CHECK_INTERVAL ALIVE_INTERVAL + 2

class ComponentManager {

    const TypeHostUnit host;

    long idCounter;

    bool protect{false};

    volatile bool threadRun{true};

    std::mutex mutex;
    std::thread thread;

    TypeComponentMapIDList componentsMapID;
    TypeComponentUnitList componentsMapDead;

    void checkDead();

protected:

    virtual TypeComponentUnit createUnit(ARCH, long, Address&) = 0;

public:

    ComponentManager(const TypeHostUnit&, bool);
    virtual ~ComponentManager();

    TypeComponentMapIDList& get();
    TypeComponentUnitList& getDead();
    TypeComponentUnit get(long);
    TypeComponentUnit getDead(long);
    size_t size();
    void clear();

    void process();
    long add(ARCH, Address&, bool&);
};

#endif //DICON_COMPONENTMANAGER_H
