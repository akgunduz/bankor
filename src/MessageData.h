//
// Created by Haluk AKGUNDUZ on 11.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_MESSAGEDATA_H
#define DICON_MESSAGEDATA_H

#include "ProcessItem.h"

class MessageData {

    int streamFlag;

    TypeMD5List md5List;
    TypeFileInfoList fileList;

    int processID;
    char process[PATH_MAX];

    char jobName[PATH_MAX];

    std::vector<ComponentObject> componentList;

public:

    int getStreamFlag();
    void setStreamFlag(int);

    Md5* getMD5(int);
    TypeMD5List* getMD5List();
    void addMD5(Md5);
    void addMD5List(TypeMD5List*);
    unsigned long getMD5Count();

    FileItem* getFile(int);
    bool isOutput(int);
    TypeFileInfoList& getFileList();
    void addFile(FileInfo);
    void addFileList(const TypeFileInfoList&);
    unsigned long getFileCount();

    char* getProcess();
    int getProcessID();
    void setProcessID(int);
    void setProcess(int, const char*);

    char* getJobName();
    void setJobName(const char*);

    std::vector<ComponentObject>& getComponentList();
    void setComponentList(std::vector<ComponentObject>&);
};


#endif //DICON_MESSAGEDATA_H
