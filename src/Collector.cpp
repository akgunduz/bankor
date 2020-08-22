//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Collector.h"
#include "CollectorHost.h"

Collector::Collector(int interfaceOther, int interfaceNode) {

    host = std::make_unique<CollectorHost>();

    addProcessHandler(COMP_DISTRIBUTOR, MSGTYPE_WAKEUP, static_cast<TypeProcessComponentMsg>(&Collector::processDistributorWakeupMsg));
    addProcessHandler(COMP_DISTRIBUTOR, MSGTYPE_NODE, static_cast<TypeProcessComponentMsg>(&Collector::processDistributorNodeMsg));
    addProcessHandler(COMP_DISTRIBUTOR, MSGTYPE_ID, static_cast<TypeProcessComponentMsg>(&Collector::processDistributorIDMsg));
    addProcessHandler(COMP_DISTRIBUTOR, MSGTYPE_REPLACE, static_cast<TypeProcessComponentMsg>(&Collector::processDistributorReplaceMsg));

    addProcessHandler(COMP_NODE, MSGTYPE_INFO, static_cast<TypeProcessComponentMsg>(&Collector::processNodeFileInfoMsg));
    addProcessHandler(COMP_NODE, MSGTYPE_BINARY, static_cast<TypeProcessComponentMsg>(&Collector::processNodeFileBinaryMsg));

    initInterfaces(COMP_COLLECTOR, interfaceOther, interfaceNode);
}

Collector::~Collector() {

    LOGS_T(getHost(), "Deallocating Collector");
}

bool Collector::processDistributorWakeupMsg(ComponentUnit& owner, TypeMessage msg) {

    setDistributor(owner);

    return send2DistributorAliveMsg(owner);
}

bool Collector::processDistributorIDMsg(ComponentUnit& owner, TypeMessage msg) {

    if (!setID(msg->getHeader().getVariant(0))) {

        return false;
    }

    LOGC_I(getHost(), owner, MSGDIR_RECEIVE, "New ID : %d is assigned by Distributor", getHost()->getID());

    return send2DistributorIDMsg(owner);
}

bool Collector::processDistributorNodeMsg(ComponentUnit& owner, TypeMessage msg) {

    TypeComponentUnitList& nodes = msg->getData().getComponentList();

    LOGC_I(getHost(), owner, MSGDIR_RECEIVE, "Distributor assigned %d node", nodes.size());

    if (nodes.empty()) {
        LOGC_W(getHost(), owner, MSGDIR_RECEIVE, "No available node is came from Distributor", nodes.size());
        return false;
    }

    for (auto &node : nodes) {
        auto process = job->assignNode(node->getID());
        send2NodeProcessMsg(*node, process);
    }

    return true;
}

bool Collector::processDistributorReplaceMsg(ComponentUnit& owner, TypeMessage msg) {

    TypeComponentUnitList& nodes = msg->getData().getComponentList();

    LOGC_I(getHost(), owner, MSGDIR_RECEIVE, "Distributor reassigned %d node", nodes.size() / 2);

    if (nodes.size() < 2) {
        LOGC_E(getHost(), owner, MSGDIR_RECEIVE, "At least two node info should came, old and new node IDs");
        return false;
    }

    for (size_t i = 0; i < nodes.size(); i = i + 2) {

        auto process = job->reAssignNode(nodes[i]->getID(), nodes[i + 1]->getID());
        if (process) {
            send2NodeProcessMsg(*nodes[i + 1], process);
        }
    }

    return true;
}

bool Collector::processNodeFileInfoMsg(ComponentUnit& owner, TypeMessage msg) {

    LOGC_I(getHost(), owner, MSGDIR_RECEIVE, "Node[%d]:Process[%d] requested %d missing files",
            owner.getID(), msg->getData().getFileProcess(), msg->getData().getFileCount());

    auto processItem = job->getProcessByID(msg->getData().getFileProcess());

    if (!processItem) {

        LOGC_E(getHost(), owner, MSGDIR_RECEIVE, "Process[%d] could not find in the Job!",
               msg->getData().getFileProcess());

        return false;
    }

    TypeProcessFileList newList;

    for (auto file : msg->getData().getFileList()) {

        newList.emplace_back(processItem->getFile(file));
    }

    return send2NodeBinaryMsg(owner, processItem->getID(), newList);
}

bool Collector::processNodeFileBinaryMsg(ComponentUnit& owner, TypeMessage msg) {

    auto& collectorHost = reinterpret_cast<TypeCollectorHost&>(host);

    LOGC_I(getHost(), owner, MSGDIR_RECEIVE, "Node[%d] sent %d File output binaries", owner.getID(), msg->getData().getFileCount());

    int totalCount = 0;
    int readyCount = job->updateDependency(msg->getData().getFileProcess(), totalCount);

    send2NodeReadyMsg(owner);

    if (readyCount) {

        return send2DistributorNodeMsg(distributor, readyCount);
    }

    if (totalCount == 0) {

        collectorHost->setState(COLLSTATE_IDLE);

        job->setDuration(componentWatch.stop());

        notifyUI(NOTIFYSTATE_PASSIVE);

        return send2DistributorReadyMsg(distributor);
    }

    return true;
}

bool Collector::send2DistributorAliveMsg(ComponentUnit& target) {

    auto msg = std::make_unique<Message>(getHost(), target, MSGTYPE_ALIVE);

    return send(target, std::move(msg));
}

bool Collector::send2DistributorIDMsg(ComponentUnit& target) {

    auto msg = std::make_unique<Message>(getHost(), target, MSGTYPE_ID);

    return send(target, std::move(msg));
}

bool Collector::send2DistributorNodeMsg(ComponentUnit& target, long readyProcessCount) {

    auto msg = std::make_unique<Message>(getHost(), target, MSGTYPE_NODE);

    msg->getHeader().setVariant(0, readyProcessCount);

    return send(target, std::move(msg));
}

bool Collector::send2DistributorReadyMsg(ComponentUnit& target) {

    auto msg = std::make_unique<Message>(getHost(), target, MSGTYPE_READY);

    return send(target, std::move(msg));
}

bool Collector::send2NodeProcessMsg(ComponentUnit& target, const TypeProcessItem& processItem) {

    auto msg = std::make_unique<Message>(getHost(), target, MSGTYPE_PROCESS);

    msg->getData().setStreamFlag(STREAM_PROCESS);
    msg->getData().addProcess(processItem);

    LOGC_I(getHost(), target, MSGDIR_SEND, "Node[%d]:Process[%d] is triggered ", target.getID(), processItem->getID());

    return send(target, std::move(msg));
}

bool Collector::send2NodeBinaryMsg(ComponentUnit& target, long processID, TypeProcessFileList &fileList) {

    auto msg = std::make_unique<Message>(getHost(), target, MSGTYPE_BINARY);

    msg->getData().setStreamFlag(STREAM_FILEBINARY);
    msg->getData().addFileList(processID, fileList);

    return send(target, std::move(msg));
}

bool Collector::send2NodeReadyMsg(ComponentUnit& target) {

    auto msg = std::make_unique<Message>(getHost(), target, MSGTYPE_READY);

    return send(target, std::move(msg));
}

void Collector::setDistributor(const ComponentUnit& _distributor) {

    distributor = _distributor;
}

bool Collector::processJob() {

    auto& collectorHost = reinterpret_cast<TypeCollectorHost&>(host);

    if (job->getStatus() != JOBSTATUS_OK) {

        return false;
    }

    componentWatch.start();

    notifyUI(NOTIFYSTATE_ACTIVE);

    collectorHost->setState(COLLSTATE_BUSY);

    return send2DistributorNodeMsg(distributor, job->getProcessCount(PROCESS_STATE_READY));
}

TypeJobItem& Collector::loadJob(const std::filesystem::path& zipFile) {

    job = std::make_shared<JobItem>(getHost(), zipFile, JobItem::jobID++);

    notifyUI(NOTIFYSTATE_ONCE);

    return job;
}

TypeJobItem& Collector::getJob() {

    return job;
}
