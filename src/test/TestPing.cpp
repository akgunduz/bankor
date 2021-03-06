//
// Created by Haluk AKGUNDUZ on 23.07.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//


#include "TestApp.h"

void sendPing(const TypeComponent& owner, const TypeComponentUnit& target) {

    auto msg = std::make_unique<Message>(owner->getHost(), target->getType(), (MSG_TYPE)MSG_TYPE_TEST_PING);

    owner->send(target, std::move(msg));
}

bool processPingMsg(const TypeComponent& component, const TypeComponentUnit& owner, TypeMessage msg) {

    LOGS_I(component->getHost(), "Message Ping has came from : %s, replying back",
           ComponentType::getName(owner->getType()));

    sendPing(component, owner);

    return true;
}

void TestApp::testPing(TypeDistributor& distributor, TypeCollector& collector, TypeNode& node) {

    MessageType::addMsg(MSG_TYPE_TEST_PING, "TEST_PING");

    distributor->addStaticProcessHandler(COMP_NODE, (MSG_TYPE)MSG_TYPE_TEST_PING, processPingMsg);
    node->addStaticProcessHandler(COMP_DISTRIBUTOR, (MSG_TYPE)MSG_TYPE_TEST_PING, processPingMsg);

    auto target = std::make_shared<ComponentUnit>(COMP_NODE, node->getHost()->getArch(), node->getHost()->getID(),
                                                  node->getHost()->getAddress(COMP_DISTRIBUTOR));

    sendPing((TypeComponent &)distributor, target);
}
