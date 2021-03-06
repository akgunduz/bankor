//
// Created by Haluk AKGUNDUZ on 27.06.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#include <UtilNet.h>
#include <DistributorHost.h>
#include "ComponentFactory.h"
#include "NodeManager.h"
#include "CollectorManager.h"
#include "WebApp.h"

bool WebApp::distHandler(struct mg_connection *conn, const char * uri) {

    const struct mg_request_info *ri = mg_get_request_info(conn);

    if (0 == strcmp(ri->request_method, "GET")) {

        if (0 == strcmp(uri, "/state")) {

            return distStateHandler(conn);
        }

        if (0 == strcmp(uri, "/poll")) {

            return distPollHandler(conn);
        }

        if (0 == strcmp(uri, "/addcoll")) {

            return distAddCollHandler(conn);
        }

        if (0 == strcmp(uri, "/addnode")) {

            return distAddNodeHandler(conn);
        }
    }

    return false;
}

bool WebApp::distPollHandler(struct mg_connection *conn) {

    auto &distributor = componentFactory->getDistributor();

    distributor->sendWakeupMessagesAll(false);

    sendOK(distributor->getHost(), conn, "Polling is Executed");

    return true;
}

bool WebApp::distAddCollHandler(struct mg_connection *conn) {

    auto &distributor = componentFactory->getDistributor();

    auto &collector = componentFactory->startCollector();

    collector->setAllUIAddress(webPort);

    sendOK(distributor->getHost(), conn, "New Collector is Added");

    return true;
}

bool WebApp::distAddNodeHandler(struct mg_connection *conn) {

    auto &distributor = componentFactory->getDistributor();

    auto &node = componentFactory->startNode();

    node->setAllUIAddress(webPort);

    sendOK(distributor->getHost(), conn, "New Node is Added");

    return true;
}

bool WebApp::distStateHandler(struct mg_connection *conn) {

    auto &distributor = componentFactory->getDistributor();
    if (!distributor) {
        sendError(conn, "Can not find the distributor !!!");
        return false;
    }

    auto distributorHost = std::static_pointer_cast<DistributorHost>(distributor->getHost());

    nlohmann::json distItem;

    distItem["_id"] = distributorHost->getID();
    distItem["_arch"] = distributorHost->getArch();
    distItem["_address"] = UtilNet::getIPPortString(distributorHost->getAddress(COMP_NODE)->get());
    distItem["_webAddress"] = UtilNet::getIPPortString(distributorHost->getAddress(COMP_NODE)->getUI());
    distItem["_autoWake"] = distributor->getAutoWake();

    nlohmann::json collList;

    for (auto& collectorPair : componentFactory->getDistributor()->getCollectors()->get()) {

        auto collector = std::static_pointer_cast<CollectorUnit>(collectorPair.second);

        nlohmann::json collItem;

        collItem["_id"] = collector->getID();
        collItem["_arch"] = collector->getArch();
        collItem["_state"] = collector->getState();
        collItem["_address"] = UtilNet::getIPPortString(collector->getAddress()->get());
        collItem["_webAddress"] = UtilNet::getIPPortString(collector->getAddress()->getUI());

        collList += collItem;

    }

    nlohmann::json nodeList;

    for (auto& nodePair : componentFactory->getDistributor()->getNodes()->get()) {

        auto node = std::static_pointer_cast<NodeUnit>(nodePair.second);

        nlohmann::json nodeItem;

        nodeItem["_id"] = node->getID();
        nodeItem["_arch"] = node->getArch();
        nodeItem["_state"] = node->getState();
        nodeItem["_address"] = UtilNet::getIPPortString(node->getAddress()->get());
        nodeItem["_webAddress"] = UtilNet::getIPPortString(node->getAddress()->getUI());

        nodeList += nodeItem;

    }

    nlohmann::json componentList;

    componentList["_distItem"] = distItem;
    componentList["_collList"] = collList;
    componentList["_nodeList"] = nodeList;

    sendStr(conn, componentList.dump());

    return true;
}
