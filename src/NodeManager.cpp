//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "NodeManager.h"

int NodeManager::idCounter = 1;

NodeManager::NodeManager() {

};

NodeManager::~NodeManager() {

}

long NodeManager::getIdle() {

    NodeObject leastUsedNode(IDLE, 0x7FFFFFFF, 0);
    long leastUsedAddress = 0;

    int idleCount = 0;

    for (auto i = nodes.begin(); i != nodes.end(); i++) {
        NodeObject node = i->second;
        if (node.getState() != IDLE) {
            continue;
        }

        idleCount++;

        if (node.getUsage() < leastUsedNode.getUsage()) {
            leastUsedNode = node;
            leastUsedAddress = i->first;
        }
    }

    if (leastUsedAddress != 0) {

        setState(leastUsedAddress, PREBUSY);

        return leastUsedAddress;

    }

    return 0;
}

bool NodeManager::add(long address) {

    auto search = nodes.find(address);
    if (search == nodes.end()) {

        mutex.lock();

        nodes[address] = NodeObject(AddressHelper::getID(address));

        mutex.unlock();

        return true;
    }

	return false;
}

void NodeManager::clear() {

    mutex.lock();

    nodes.clear();

    mutex.unlock();
}

bool NodeManager::setState(long address, NODE_STATES state) {

    auto search = nodes.find(address);
    if (search == nodes.end()) {
        return false;
    }

    NodeObject node = search->second;

    mutex.lock();

    switch(state) {

        case IDLE:
            nodes[address] = NodeObject(IDLE, node.getUsage(), node.getID());
            break;

        case PREBUSY:
            nodes[address] = NodeObject(PREBUSY, node.getUsage(), node.getID());
            break;

        case BUSY:
            nodes[address] = NodeObject(BUSY, node.iterateUsage(true), node.getID());
            break;

        default:
            break;
    }

    mutex.unlock();

    return true;
}

int NodeManager::getFreeID() {

    return idCounter++;
}
