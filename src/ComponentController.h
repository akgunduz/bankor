//
// Created by Haluk AKGUNDUZ on 3.05.2018.
//

#ifndef DICON_COMPONENTCONTROLLER_H
#define DICON_COMPONENTCONTROLLER_H

#include "Distributor.h"
#include "Collector.h"
#include "Node.h"

class ComponentController {

    Distributor *distributor = nullptr;
    std::vector<Collector*> collectors;
    std::vector<Node*> nodes;

    int interface[2]{};

    static ComponentController *instance;

    ComponentController(int*);

public:

    static ComponentController* newInstance(int*);
    ~ComponentController();

    Distributor* getDistributor();
    long getCollectorCount();
    Collector* getCollector(long);
    std::vector<Collector*>& getCollectors();
    long getNodeCount();
    Node* getNode(long);
    std::vector<Node*>& getNodes();

    bool startDistributor(bool);
    bool startCollector(int);
    Collector* startCollector();
    bool startNode(int);
    Node* startNode();
};

#endif //DICON_COMPONENTCONTROLLER_H
