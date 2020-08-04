//
// Created by Haluk AKGUNDUZ on 11.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_MESSAGEDATA_H
#define DICON_MESSAGEDATA_H

#include "ProcessItem.h"

class MessageData {

    int streamFlag;

    char jobName[PATH_MAX];

    std::tuple<long, TypeProcessFileList> fileList;
    TypeProcessList processList;

    TypeComponentList componentList;

public:

    int getStreamFlag() const;
    void setStreamFlag(int);

    ProcessFile& getFile(int);
    long getFileProcess();
    void setFileProcess(long);
    TypeProcessFileList& getFileList();
    void addFile(ProcessFile&);
    void addFileList(long, TypeProcessFileList&);
    long getFileCount();

    ProcessItem* getProcess(int);
    void addProcess(ProcessItem*);
    long getProcessCount();

    char* getJobName();
    void setJobName(const char*);

    TypeComponentList& getComponentList();
    void setComponentList(TypeComponentList&);
};

#endif //DICON_MESSAGEDATA_H
