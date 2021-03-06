//
// Created by Haluk AKGUNDUZ on 25.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include "CollectorManager.h"
#include "Log.h"

CollectorManager::CollectorManager(const TypeHostUnit& _host, bool _protect)
    : ComponentManager(_host, _protect) {
};

CollectorManager::~CollectorManager() = default;

COLLSTATES CollectorManager::getState(TypeID id) {

    auto coll = std::static_pointer_cast<CollectorUnit>(get(id));

    if (coll) {

        return coll->getState();
    }

    return COLLSTATE_MAX;
}

void CollectorManager::setState(TypeID id, COLLSTATES state) {

    auto coll = std::static_pointer_cast<CollectorUnit>(get(id));

    if (!coll) {
        LOGP_E("Collector[%d] is not exist, can not set its state to %s", id, CollState::getName(state));
        return;
    }

    coll->setState(state);
}

TypeComponentUnit CollectorManager::createUnit(ARCH arch, TypeID id, TypeAddress& address) {

    return std::make_shared<CollectorUnit>(arch, id, address);
}

bool CollectorManager::addRequest(TypeID id, uint32_t reqNodeCount) {

    auto coll = std::static_pointer_cast<CollectorUnit>(get(id));

    if (coll) {

        coll->setState(COLLSTATE_BUSY);

        collReqList.emplace_back(CollectorRequest(id, reqNodeCount));
    }

    return true;
}

CollectorRequest* CollectorManager::getRequest() {

    CollectorRequest* request = nullptr;

    if (!collReqList.empty()) {

        request = &collReqList.front();
    }

    return request;
}

void CollectorManager::updateRequest(size_t reqCount) {

    collReqList.front().reqCount = reqCount;
}

void CollectorManager::removeRequest() {

    collReqList.pop_front();
}


