//
// Created by Haluk AKGUNDUZ on 29/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_NODE_H
#define DICON_NODE_H

#include <atomic>
#include <mutex>
#include <condition_variable>
#include "Component.h"

class Node : public Component {

    TypeComponentUnit distributor;

    std::thread processThread;

    volatile bool runProcessThread = true;

    TypeProcessItem processItem;

    TypeProcessList processList;

    uv_loop_t processLoop{};

    std::mutex mutex;

    std::condition_variable cond;

    std::atomic<bool> processReady{false};

    void spawnProcess();

	void setDistributor(const TypeComponentUnit&);

    bool processDistributorWakeupMsg(const TypeComponentUnit&, TypeMessage);
    bool processDistributorIDMsg(const TypeComponentUnit&, TypeMessage);
    bool processDistributorProcessMsg(const TypeComponentUnit&, TypeMessage);

    bool processCollectorProcessMsg(const TypeComponentUnit&, TypeMessage);
    bool processCollectorBinaryMsg(const TypeComponentUnit&, TypeMessage);
    bool processCollectorReadyMsg(const TypeComponentUnit&, TypeMessage);

    bool send2DistributorReadyMsg(const TypeComponentUnit&);
    bool send2DistributorAliveMsg(const TypeComponentUnit&);
    bool send2DistributorIDMsg(const TypeComponentUnit&);
    bool send2DistributorBusyMsg(const TypeComponentUnit&, TypeID);

    bool send2CollectorInfoMsg(const TypeComponentUnit&, long, const TypeProcessFileList&);
	bool send2CollectorBinaryMsg(const TypeComponentUnit&, long, const TypeProcessFileList&);

    bool executeJob(const TypeComponentUnit&, TypeMessage);
    static bool onProcessSuccess(void*);
    void parseCommand(char *cmd, char **argv);

public:

    explicit Node(int);
	~Node() override;

    TypeProcessList& getProcessList();
};

typedef std::shared_ptr<Node> TypeNode;

#endif //DICON_NODE_H
