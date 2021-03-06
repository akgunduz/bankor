//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include <cassert>
#include "NodeManager.h"
#include "Log.h"

NodeManager::NodeManager(TypeHostUnit& _host, bool _protect)
    : ComponentManager(_host, _protect) {
};

TypeNodeUnit NodeManager::getIdle(const TypeComponentUnit& assigned) {

    TypeNodeUnit leastUsedNode = nullptr;

    for (const auto& nodePair : get()) {

        auto node = std::static_pointer_cast<NodeUnit>(nodePair.second);

        if (node->getState() != NODESTATE_IDLE) {
            continue;
        }

        if (leastUsedNode == nullptr) {
            leastUsedNode = node;
            continue;
        }

        if (node->getUsage() < leastUsedNode->getUsage()) {
            leastUsedNode = node;
        }
    }

    if (leastUsedNode != nullptr) {

        leastUsedNode->iterateUsage(true);
        leastUsedNode->setState(NODESTATE_PREBUSY);
        leastUsedNode->setAssigned(assigned);
    }

    assert(leastUsedNode != nullptr);

    return leastUsedNode;
}

size_t NodeManager::getIdleCount() {

    size_t count = 0;

    for (auto &component : get()) {

        auto node = std::static_pointer_cast<NodeUnit>(component.second);

        if (node->getState() == NODESTATE_IDLE) {
            count++;
        }
    }

    return count;
}

TypeNodeUnit NodeManager::getBusyDead() {

    for (const auto& component : get(true)) {

        auto node = std::static_pointer_cast<NodeUnit>(component.second);

        if (node->getState() == NODESTATE_DEAD) {
            continue;
        }

        node->setState(NODESTATE_DEAD);

        return node;
    }

    return nullptr;
}

size_t NodeManager::getBusyDeadCount() {

    size_t count = 0;

    for (const auto &component : get(true)) {

        auto node = std::static_pointer_cast<NodeUnit>(component.second);

        if (node->getState() != NODESTATE_DEAD) {
            count++;
        }
    }

    return count;
}

TypeAddress NodeManager::getAddress(TypeID id) {

    auto node = get(id);

    if (!node) {

        LOGP_E("Node[%d] is not exist, can not get its address ", id);

        return nullptr;
    }

    return node->getAddress();
}

NODESTATES NodeManager::getState(TypeID id) {

    auto node = std::static_pointer_cast<NodeUnit>(get(id));

    if (!node) {

        LOGP_E("Node[%d] is not exist, can not get its state ", id);

        return NODESTATE_MAX;
    }

    return node->getState();
}

void NodeManager::setState(TypeID id, NODESTATES state) {

    auto node = std::static_pointer_cast<NodeUnit>(get(id));

    if (!node) {

        LOGP_E("Node[%d] is not exist, can not set its state to %s", id, NodeState::getName(state));

        return;
    }

    node->setState(state);
}

TypeComponentUnit NodeManager::getAssigned(TypeID id) {

    auto node = std::static_pointer_cast<NodeUnit>(get(id));

    if (!node) {

        LOGP_E("Node[%d] is not exist, can not get assigned Collector ID", id);

        return nullptr;
    }

    return node->getAssigned();
}

void NodeManager::setAssigned(TypeID id, TypeComponentUnit& assigned) {

    auto node = std::static_pointer_cast<NodeUnit>(get(id));

    if (!node) {

        LOGP_E("Node[%d] is not exist, can not assign Collector[%d]", id, assigned->getID());

        return;
    }

    node->setAssigned(assigned);
}

void NodeManager::setAssigned(TypeID id, ARCH assignedArch, TypeID assignedID, TypeAddress& assignedAddress) {

    auto node = std::static_pointer_cast<NodeUnit>(get(id));

    if (!node) {

        LOGP_E("Node[%d] is not exist, can not assign Collector[%d]", id, assignedID);

        return;
    }

    node->setAssigned(COMP_COLLECTOR, assignedArch, assignedID, assignedAddress);
}

void NodeManager::releaseAssigned(TypeID id) {

    auto node = std::static_pointer_cast<NodeUnit>(get(id));

    if (!node) {

        LOGP_E("Node[%d] is not exist, can not release assigned Collector", id);

        return;
    }

    node->setAssigned(nullptr);
}

TypeComponentUnit NodeManager::createUnit(ARCH arch, TypeID id, TypeAddress& address) {

    return std::make_shared<NodeUnit>(arch, id, address);
}
