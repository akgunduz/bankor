//
// Created by Haluk AKGUNDUZ on 29.04.2018.
//

#ifndef BANKOR_EXECUTORINFO_H
#define BANKOR_EXECUTORINFO_H


#include "ExecutorItem.h"

enum PROCESS_STATE {
    PROCESS_STATE_NOTSTARTED,
    PROCESS_STATE_STARTED,
    PROCESS_STATE_ENDED,
    PROCESS_STATE_MAX,
};

class ExecutorInfo {

    ExecutorItem *item;
    long id;
    PROCESS_STATE state;

public:

    ExecutorInfo(long, ExecutorItem *);
    ExecutorItem* get();
    long getID();
    PROCESS_STATE getState();
    void setState(PROCESS_STATE);
};


#endif //BANKOR_EXECUTORINFO_H