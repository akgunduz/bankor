//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Distributor.h"
#include "DeviceList.h"

Distributor *Distributor::instance = NULL;

Distributor *Distributor::newInstance() {

    if (instance) {
        return instance;
    }

    char path[PATH_MAX];
    sprintf(path, "%s/%s", getcwd(NULL, 0), DISTRIBUTOR_PATH);
    mkdir(path, 0777);

    instance = new Distributor(path);
    return instance;
}

Distributor::Distributor(const char *rootPath) :
        Component(COMP_DISTRIBUTOR, rootPath) {

    processMsg[COMP_COLLECTOR][MSGTYPE_ALIVE] = static_cast<TypeProcessComponentMsg>(&Distributor::processCollectorAliveMsg);
    processMsg[COMP_COLLECTOR][MSGTYPE_NODE] = static_cast<TypeProcessComponentMsg>(&Distributor::processCollectorNodeMsg);
    processMsg[COMP_NODE][MSGTYPE_READY] = static_cast<TypeProcessComponentMsg>(&Distributor::processNodeReadyMsg);
    processMsg[COMP_NODE][MSGTYPE_ALIVE] = static_cast<TypeProcessComponentMsg>(&Distributor::processNodeAliveMsg);
    processMsg[COMP_NODE][MSGTYPE_BUSY] = static_cast<TypeProcessComponentMsg>(&Distributor::processNodeBusyMsg);
    processMsg[COMP_NODE][MSGTYPE_ID] = static_cast<TypeProcessComponentMsg>(&Distributor::processNodeIDMsg);

    LOG_U(UI_UPDATE_DIST_ADDRESS, std::vector<long> {getInterfaceAddress(COMP_COLLECTOR), getInterfaceAddress(COMP_NODE)});

    nodeManager = new NodeManager();

    collectorManager = new CollectorManager();
};

Distributor::~Distributor() {

    delete nodeManager;

    delete collectorManager;
}

bool Distributor::processCollectorAliveMsg(ComponentObject owner, long address, Message *msg) {

    if (!collectorManager->add(address)) {

        return true;
    }

    LOG_U(UI_UPDATE_DIST_COLL_LIST, std::vector<long> {address, 0});

    LOGS_I(getHost(), "Collector at address : %s added to the list with ID : %d",
           InterfaceTypes::getAddressString(address).c_str(), nodeManager->getID(address));

    return send2CollectorIDMsg(address, collectorManager->getID(address));
}

bool Distributor::processCollectorNodeMsg(ComponentObject owner, long address, Message *msg) {

    long nodeAddress = nodeManager->getIdle();

    if (nodeAddress > 0) {

        LOG_U(UI_UPDATE_DIST_NODE_LIST, std::vector<long> {nodeAddress, PREBUSY});
        LOG_U(UI_UPDATE_DIST_COLL_LIST, std::vector<long> {address, nodeAddress});

        LOGS_I(getHost(), "Available node: %s",
               InterfaceTypes::getAddressString(nodeAddress).c_str());

        return send2CollectorNodeMsg(address, msg->getData()->getJobDir(), nodeAddress);

    } else {

        collectorManager->addWaiting(address, msg->getData()->getJobDir());

        LOGS_I(getHost(),
               "No available node, adding Collector at %s with Job : %s to Wait List with new Count %d",
               InterfaceTypes::getAddressString(address).c_str(), msg->getData()->getJobDir(),
               collectorManager->getWaitingCount());
        LOG_U(UI_UPDATE_DIST_COLL_LIST, std::vector<long> {address, 0});
    }

    return false;
}

bool Distributor::processNodeAliveMsg(ComponentObject owner, long address, Message *msg) {

    if (!nodeManager->add(address)) {

        return true;
    }

    LOG_U(UI_UPDATE_DIST_NODE_LIST, std::vector<long> {address, IDLE});

    LOGS_I(getHost(), "Node at address : %s added to the list with ID : %d",
           InterfaceTypes::getAddressString(address).c_str(), nodeManager->getID(address));

    return send2NodeIDMsg(address, nodeManager->getID(address));
}

bool Distributor::processNodeReadyMsg(ComponentObject owner, long address, Message *msg) {

    if (!nodeManager->setState(address, IDLE)) {
        LOGS_I(getHost(),
               "Could not found a node with address : %s",
               InterfaceTypes::getAddressString(address).c_str());
        return false;
    }

    bool status = false;

    TypeWaitingCollector collector = collectorManager->getWaiting();

    if (collector.first > 0) {

        LOGS_I(getHost(),
               "Processing a collector %s with Job : %s from the waiting list with new Count %d",
               InterfaceTypes::getAddressString(collector.first).c_str(),
               collector.second.c_str(), collectorManager->getWaitingCount());

        status = send2CollectorNodeMsg(collector.first, collector.second.c_str(), address);

    } else {

        status = true;
    }

    LOG_U(UI_UPDATE_DIST_NODE_LIST, std::vector<long> {address, IDLE});

    return status;
}

bool Distributor::processNodeIDMsg(ComponentObject owner, long address, Message *msg) {

    TypeWaitingCollector collector = collectorManager->getWaiting();

    if (collector.first) {

        LOGS_I(getHost(),
               "Processing a collector %s with Job : %s from the waiting list with new Count %d",
               InterfaceTypes::getAddressString(collector.first).c_str(),
               collector.second.c_str(), collectorManager->getWaitingCount());

        return send2CollectorNodeMsg(collector.first, collector.second.c_str(), address);

    }

    return true;
}

bool Distributor::processNodeBusyMsg(ComponentObject owner, long address, Message *msg) {

    nodeManager->setState(address, BUSY);

    LOG_U(UI_UPDATE_DIST_NODE_LIST, std::vector<long> {address, BUSY});

    LOGS_I(getHost(),
           "Node at address : %s switch to state : %s",
           InterfaceTypes::getAddressString(address).c_str(), NodeState::getName(BUSY));

    return true;
}

bool Distributor::send2CollectorWakeupMsg(long address) {

    auto *msg = new Message(getHost(), MSGTYPE_WAKEUP);

    return send(COMP_COLLECTOR, address, msg);
}

bool Distributor::send2CollectorIDMsg(long address, int id) {

    auto *msg = new Message(getHost(), MSGTYPE_ID);

    msg->getHeader()->setVariant(0, id);

    return send(COMP_COLLECTOR, address, msg);
}

bool Distributor::send2CollectorNodeMsg(long address, const char *jobDir, long nodeAddress) {

    auto *msg = new Message(getHost(), MSGTYPE_NODE);

    msg->getData()->setStreamFlag(STREAM_JOB);
    msg->getHeader()->setVariant(0, nodeAddress);
    msg->getData()->setJobDir(jobDir);

    return send(COMP_COLLECTOR, address, msg);
}

bool Distributor::send2NodeWakeupMsg(long address) {

    auto *msg = new Message(getHost(), MSGTYPE_WAKEUP);

    return send(COMP_NODE, address, msg);
}

bool Distributor::send2NodeIDMsg(long address, int id) {

    auto *msg = new Message(getHost(), MSGTYPE_ID);

    msg->getHeader()->setVariant(0, id);

    return send(COMP_NODE, address, msg);
}

bool Distributor::sendWakeupMessage(COMPONENT component) {

    if (isSupportMulticast(component)) {

        auto *msg = new Message(getHost(), MSGTYPE_WAKEUP);
        send(component, msg);

    } else {

        std::vector<long> list = getAddressList(component);

        for (int i = 0; i < list.size(); i++) {

            auto *msg = new Message(getHost(), MSGTYPE_WAKEUP);

            send(component, list[i], msg);
        }
    }

    return true;
}

bool Distributor::sendWakeupMessagesAll() {

    sendWakeupMessage(COMP_NODE);

    if (DeviceList::getInstance()->isActiveDifferent()) {
        sendWakeupMessage(COMP_COLLECTOR);
    }

    return true;
}

bool Distributor::reset() {

    nodeManager->clear();

    collectorManager->clear();

    return true;

}
