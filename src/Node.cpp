//
// Created by Haluk AKGUNDUZ on 29/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Node.h"
#include "CollectorObject.h"

#define PROCESS_SLEEP_TIME 200

Node *Node::newInstance(const char* path) {

    return new Node(path);
}

Node::Node(const char *rootPath) :
        Component(rootPath), distributor(getRootPath()){

    host = new NodeObject(getRootPath());

    processMsg[COMP_DISTRIBUTOR][MSGTYPE_WAKEUP] = static_cast<TypeProcessComponentMsg>(&Node::processDistributorWakeupMsg);
    processMsg[COMP_DISTRIBUTOR][MSGTYPE_ID] = static_cast<TypeProcessComponentMsg>(&Node::processDistributorIDMsg);
    processMsg[COMP_DISTRIBUTOR][MSGTYPE_PROCESS] = static_cast<TypeProcessComponentMsg>(&Node::processDistributorProcessMsg);
    processMsg[COMP_COLLECTOR][MSGTYPE_JOB] = static_cast<TypeProcessComponentMsg>(&Node::processCollectorJobMsg);
    processMsg[COMP_COLLECTOR][MSGTYPE_BINARY] = static_cast<TypeProcessComponentMsg>(&Node::processCollectorBinaryMsg);
    processMsg[COMP_COLLECTOR][MSGTYPE_READY] = static_cast<TypeProcessComponentMsg>(&Node::processCollectorReadyMsg);

    initInterfaces(COMP_NODE);
}

Node::~Node() = default;

bool Node::processDistributorWakeupMsg(const ComponentObject& owner, Message *msg) {

    setDistributor((DistributorObject &) owner);

    return send2DistributorAliveMsg(owner);
}

bool Node::processDistributorIDMsg(const ComponentObject& owner, Message *msg) {

    setID((int)msg->getHeader()->getVariant(0));

    notifyUI();
    LOGS_I(getHost(), "New ID : %d is assigned by Distributor", getHost().getID());

    return send2DistributorIDMsg(owner);
}

bool Node::processCollectorJobMsg(const ComponentObject& owner, Message *msg) {

    ((NodeObject&)getHost()).setState(NODESTATE_BUSY);
    ((NodeObject&)getHost()).setAssigned(owner.getID(), owner.getAddress());

    ((NodeObject&)getHost()).getProcessInfo().setAssigned(owner.getID());
    ((NodeObject&)getHost()).getProcessInfo().setState(PROCESS_STATE_STARTED);
    ((NodeObject&)getHost()).getProcessInfo().setJobID(msg->getData()->getJobDir());
    ((NodeObject&)getHost()).getProcessInfo().setID(msg->getData()->getExecutorID());
    ((NodeObject&)getHost()).getProcessInfo().get().setParsedExec(msg->getData()->getExecutor());
    ((NodeObject&)getHost()).getProcessInfo().get().setFileList(msg->getData()->getFileList());

    LOGS_I(getHost(), "Process[%d] request is received from Collector[%d]",
           msg->getData()->getExecutorID(), owner.getID());

    return send2DistributorBusyMsg(getDistributor(), owner.getID());
}

bool Node::processJob(const ComponentObject& owner, Message *msg) {

    LOGS_I(getHost(), "Process[%d] from Collector[%d] is executing",
           ((NodeObject&)getHost()).getProcessInfo().getID(),
           ((NodeObject&)getHost()).getAssigned().getID());

    ProcessInfo &processInfo = ((NodeObject&)getHost()).getProcessInfo();

    processList.emplace_back(processInfo);

    notifyUI();

    int result = processCommand(((NodeObject&)getHost()).getProcessInfo().getID(),
                   ((NodeObject&)getHost()).getProcessInfo().get().getParsedExec());

    if (!result) {
        return false;
    }

    TypeFileInfoList outputList = FileInfo::getFileList(((NodeObject&)getHost()).getProcessInfo().get().getFileList(), true);
    FileInfo::setFileListState(outputList, false);

    LOGS_I(getHost(), "Process[%d] from Collector[%d] is executed, sending back output data",
           ((NodeObject&)getHost()).getProcessInfo().getID(),
           ((NodeObject&)getHost()).getAssigned().getID());

    //TODO will update with md5s including outputs
    //TODO will update with actual binary to collector including outputs
    return send2CollectorBinaryMsg(CollectorObject(((NodeObject&)getHost()).getAssigned().getID(),
                                                   ((NodeObject&)getHost()).getAssigned().getAddress()),
                                   ((NodeObject&)getHost()).getProcessInfo().getJobID().c_str(),
                                   ((NodeObject&)getHost()).getProcessInfo().getID(),
                                   ((NodeObject&)getHost()).getProcessInfo().get().getParsedExec(),
                                   outputList);
}

bool Node::processDistributorProcessMsg(const ComponentObject& owner, Message *msg) {

    LOGS_I(getHost(), "Execution order is received from distributor for Collector[%d]'s Process[%d]",
           ((NodeObject&)getHost()).getAssigned().getID(),
           ((NodeObject&)getHost()).getProcessInfo().getID());

    TypeFileInfoList requiredList = FileInfo::getReqFileList(getHost(),
            ((NodeObject&)getHost()).getProcessInfo().get().getFileList());

    if (!requiredList.empty()) {

        LOGS_I(getHost(), "Missing files for Process[%d] is requested from Collector[%d]",
               ((NodeObject&)getHost()).getProcessInfo().getID(),
               ((NodeObject&)getHost()).getAssigned().getID());

        return send2CollectorInfoMsg(CollectorObject(((NodeObject&)getHost()).getAssigned().getID(),
                                                     ((NodeObject&)getHost()).getAssigned().getAddress()),
                                     ((NodeObject&)getHost()).getProcessInfo().getJobID().c_str(),
                                     ((NodeObject&)getHost()).getProcessInfo().getID(),
                                     ((NodeObject&)getHost()).getProcessInfo().get().getParsedExec(),
                                     requiredList);

    } else {

        return processJob(owner, msg);
    }
}


bool Node::processCollectorBinaryMsg(const ComponentObject& owner, Message *msg) {

    LOGS_I(getHost(), "Process[%d] binaries are received from Collector[%d]",
           msg->getData()->getExecutorID(), owner.getID());

    return processJob(owner, msg);
}

bool Node::processCollectorReadyMsg(const ComponentObject& owner, Message *msg) {

    ((NodeObject&)getHost()).setState(NODESTATE_IDLE);

    notifyUI();

    return send2DistributorReadyMsg(getDistributor());
}

bool Node::send2DistributorReadyMsg(const ComponentObject& target) {

	auto *msg = new Message(getHost(), MSGTYPE_READY);

	return send(target, msg);
}

bool Node::send2DistributorAliveMsg(const ComponentObject& target) {

    auto *msg = new Message(getHost(), MSGTYPE_ALIVE);

    return send(target, msg);
}

bool Node::send2DistributorIDMsg(const ComponentObject& target) {

    auto *msg = new Message(getHost(), MSGTYPE_ID);

    return send(target, msg);
}

bool Node::send2DistributorBusyMsg(const ComponentObject& target, int collID) {

    auto *msg = new Message(getHost(), MSGTYPE_BUSY);

    msg->getHeader()->setVariant(0, collID);

    return send(target, msg);
}

bool Node::send2CollectorInfoMsg(const ComponentObject& target, const char* jobDir, int executorID,
                                 const char* executor, TypeFileInfoList &fileList) {

	auto *msg = new Message(getHost(), MSGTYPE_INFO);

    msg->getData()->setStreamFlag(STREAM_INFO);
    msg->getData()->setJobDir(jobDir);
    msg->getData()->setExecutor(executorID, executor);
    msg->getData()->addFileList(fileList);

	return send(target, msg);
}

bool Node::send2CollectorBinaryMsg(const ComponentObject& target, const char* jobDir, int executorID,
                                   const char* executor, TypeFileInfoList &fileList) {

    auto *msg = new Message(getHost(), MSGTYPE_BINARY);

    msg->getData()->setStreamFlag(STREAM_BINARY);
    msg->getData()->setJobDir(jobDir);
    msg->getData()->setExecutor(executorID, executor);
    msg->getData()->addFileList(fileList);

    return send(target, msg);
}

ComponentObject Node::getDistributor() {

    return distributor;
}

void Node::setDistributor(const DistributorObject& _distributor) {

    this->distributor = _distributor;
}

void Node::parseCommand(char *cmd, char **argv) {

    while (*cmd != '\0') {

        while (*cmd == ' ' || *cmd == '\t' || *cmd == '\n') {
            *cmd++ = '\0';
        }

        *argv++ = cmd;

        while (*cmd != '\0' && *cmd != ' ' && *cmd != '\t' && *cmd != '\n') {
            cmd++;
        }
    }
    *argv = nullptr;
}

bool Node::processCommand(int processID, const char *cmd) {

    char fullCmd[PATH_MAX];

    strcpy(fullCmd, Util::parsePath(getHost().getRootPath(), cmd).c_str());

    LOGS_T(getHost(), "Process[%d] is started", processID);
    LOGS_I(getHost(), "Command : %s", fullCmd);

    char childOut[256];
    int tryCount = 3;
    while(--tryCount) {

        FILE *fdProcess = popen(fullCmd, "r");
        if (fdProcess == nullptr) {
            LOGS_E(getHost(), "Can not execute command", fullCmd);
            return false;
        }

        while (fgets(childOut, 256, fdProcess)) {
            LOGS_I(getHost(), "Child Output : %s", childOut);
        }

        int res = pclose(fdProcess);
        if (res == 0) {
            LOGS_T(getHost(), "Process[%d] is ended successfully", processID);
            return true;
        }

        LOGS_E(getHost(), "Process[%d] has execution problem retrying", processID);

        std::this_thread::sleep_for(std::chrono::milliseconds(PROCESS_SLEEP_TIME));

    }

    LOGS_E(getHost(), "Process[%d] is ended with error", processID);

    return false;
}

std::vector<ProcessInfo> &Node::getProcessList() {

    return processList;
}
