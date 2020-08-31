//
// Created by Haluk AKGUNDUZ on 11.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_MESSAGEDATA_H
#define DICON_MESSAGEDATA_H

#include "ProcessItem.h"

class MessageData {

    int streamType{STREAM_NONE};

    std::string jobName;

    TypeProcessItem process;

    TypeFileItem file;

    TypeComponentUnitList componentList;

public:

    explicit MessageData(const TypeHostUnit&);
    ~MessageData() = default;

    int getStreamType();
    void setStreamType(int);

    const TypeProcessItem& getProcess();
    void setProcess(const TypeProcessItem&);
    void setProcess(long, const TypeProcessFileList&);
    void setProcess(long, const TypeProcessFile&);

    const TypeFileItem& getFile();
    void setFile(const TypeFileItem&);

    std::string& getJobName();
    void setJobName(const std::string&);

    TypeComponentUnitList& getComponentList();
    void setComponentList(TypeComponentUnitList&);
};

#endif //DICON_MESSAGEDATA_H
