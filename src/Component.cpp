//
// Created by Haluk Akgunduz on 10/14/15.
//

#include "Component.h"

Component::Component(long index, const char* rootPath) {

    memset(connectors, 0, sizeof(connectors));

    callback = new InterfaceCallback(receiveCB, this);

    int indexDist = (int)(index & 0xFFFF);
    int indexCollector = (int)((index >> 16) & 0xFFFF);
    int indexNode = (int)((index >> 32) & 0xFFFF);

    if (indexDist != 0xFFFF) {

        try {

            connectors[HOST_DISTRIBUTOR] = new Connector(indexDist, callback, rootPath);
            this->rootPath = connectors[HOST_DISTRIBUTOR]->getRootPath();

        } catch (const std::runtime_error e) {

            LOG_E("Connector Init failed!!!");

            delete callback;

            throw std::runtime_error("Distributor : Connector Init failed!!!");
        }
    }

    if (indexCollector != 0xFFFF) {

        if (indexCollector != indexDist) {

            try {

                connectors[HOST_COLLECTOR] = new Connector(indexCollector, callback, rootPath);
                this->rootPath = connectors[HOST_COLLECTOR]->getRootPath();

            } catch (const std::runtime_error e) {

                LOG_E("Connector Init failed!!!");

                if (connectors[HOST_DISTRIBUTOR] != nullptr) {
                    delete connectors[HOST_DISTRIBUTOR];
                }
                delete callback;

                throw std::runtime_error("Distributor : Connector Init failed!!!");
            }

        } else {

            connectors[HOST_COLLECTOR] = connectors[HOST_DISTRIBUTOR];

        }
    }

    if (indexNode != 0xFFFF) {

        if (indexNode != indexDist && indexNode != indexCollector) {

            try {

                connectors[HOST_NODE] = new Connector(indexCollector, callback, rootPath);
                this->rootPath = connectors[HOST_NODE]->getRootPath();

            } catch (const std::runtime_error e) {

                LOG_E("Connector Init failed!!!");

                if (connectors[HOST_DISTRIBUTOR] != nullptr) {
                    delete connectors[HOST_DISTRIBUTOR];
                }
                if (connectors[HOST_COLLECTOR] != nullptr) {
                    delete connectors[HOST_DISTRIBUTOR];
                }
                delete callback;

                throw std::runtime_error("Distributor : Connector Init failed!!!");
            }

        } else {

            connectors[HOST_NODE] = indexDist != 0xFFFF ? connectors[HOST_DISTRIBUTOR] : connectors[HOST_COLLECTOR];

        }
    }
}

Component::~Component() {

    if (connectors[HOST_NODE] != connectors[HOST_DISTRIBUTOR] &&
            connectors[HOST_NODE] != connectors[HOST_COLLECTOR]) {
        delete connectors[HOST_NODE];
    }

    if (connectors[HOST_COLLECTOR] != connectors[HOST_DISTRIBUTOR]) {
        delete connectors[HOST_COLLECTOR];
    }

    delete connectors[HOST_DISTRIBUTOR];

    delete callback;
}

const char* Component::getRootPath() {

    return rootPath;

}

bool Component::receiveCB(void *arg, long address, Message *msg) {

    Component *component = (Component *) arg;

    return component->onReceive(address, msg);
}

bool Component::onReceive(long address, Message *msg) {

    if (connectors[msg->getOwner()]->getInterfaceType() != Address::getInterface(address)) {
        LOG_W("Wrong message received : %d from %s, disgarding", msg->getType(), Address::getString(address).c_str());
        delete msg;
        return false;
    }

    switch(msg->getOwner()) {

        case HOST_DISTRIBUTOR:
            return processDistributorMsg(address, msg);

        case HOST_NODE:
            return processClientMsg(address, msg);

        case HOST_COLLECTOR:
            return processCollectorMsg(address, msg);

        default:
            LOG_W("Wrong message received : %d from %s, disgarding", msg->getType(), Address::getString(address).c_str());
            delete msg;
            return false;
    }
}

long Component::generateIndex(int indexFirst, int indexSecond, int indexThird) {

    return ((long)indexFirst) | ((long)indexSecond << 16) | ((long)indexThird << 32) ;
}

INTERFACES Component::getInterfaceType(HOST host) {

    if (connectors[host] != nullptr) {
        return connectors[host]->getInterfaceType();
    }

    return INTERFACE_MAX;
}

long Component::getAddress(HOST host) {

    if (connectors[host] != nullptr) {
        return connectors[host]->getAddress();
    }

    return 0;
}


