//
// Created by Haluk Akgunduz on 02/09/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include <cstring>
#include <Util.h>
#include <NodeHost.h>
#include "ConsoleApp.h"
#include "ComponentFactory.h"

bool ConsoleApp::nodeStateHandler(long id) {

    auto &node = componentFactory->getNode(id);
    if (!node) {
        LOGP_E("Can not find the node with ID : %d !!!", id);
        return false;
    }

    auto nodeObj = (NodeHost&) node->getHost();

    LOGP_I("Node[%d] Status", id);

    LOGP_I("State", nodeObj.getState());

    LOGP_I("Processes");

    for (auto &process : node->getProcessList()) {

        std::string processCommand = process->getParsedProcess();
        Util::replaceStr(processCommand, ROOT_SIGN, "");

        LOGP_I("ID : %d, Assigned Collector : %d, Job ID : %ld, Process : %s",
              process->getID(),
              process->getAssigned(),
              process->getAssignedJob(),
              processCommand.c_str());
    }

    return true;
}
