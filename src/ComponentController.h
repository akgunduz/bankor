//
// Created by Haluk AKGUNDUZ on 3.05.2018.
//

#ifndef DICON_COMPONENTCONTROLLER_H
#define DICON_COMPONENTCONTROLLER_H


#include "Distributor.h"
#include "Collector.h"

class ComponentController {

    Distributor *distributor = nullptr;
    std::vector<Collector*> collectors;
    std::vector<Node*> nodes;

    static ComponentController *instance;

    ComponentController(int, int);

public:

    static ComponentController* newInstance(int, int);
    ~ComponentController();

    Distributor* getDistributor();
    long getCollectorCount();
    Collector* getCollector(int);
    long getNodeCount();
    Node* getNode(int);

    bool startDistributor();
    bool startCollector(int);
    bool startNode(int);

    static bool setInterfaces(int, int);
};


#endif //DICON_COMPONENTCONTROLLER_H
