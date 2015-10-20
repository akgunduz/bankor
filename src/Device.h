//
// Created by Haluk Akgunduz on 10/12/15.
//

#ifndef BANKOR_CONNECTINTERFACE_H
#define BANKOR_CONNECTINTERFACE_H

#include "Common.h"

class Device {
public:
    char name[50];
    INTERFACES type;

    long address;
    long helper;
    bool multicastEnabled = false;

    static std::vector<Device>deviceList;

    Device(const char* _name, INTERFACES _type);
    Device(const char* _name, long _ipAddress, long _helper, bool _multicastEnabled);

    const char* getName();
    INTERFACES getType();
    long getAddress();
    long getHelper();
    bool isMulticastEnabled();

    static std::vector<Device> getInterfaces();
    static int getCount();
    static Device* getDevice(int);
    static std::string getName(int);
    static INTERFACES getType(int);
    static long getAddress(int);
    static long getHelper(int);
    static bool isMulticastEnabled(int);
};


#endif //BANKOR_CONNECTINTERFACE_H