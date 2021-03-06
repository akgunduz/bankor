//
// Created by Haluk AKGUNDUZ on 01.08.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_COMPONENTOBJECT_H
#define DICON_COMPONENTOBJECT_H

#include "ComponentUnit.h"

class ComponentObject {

    TypeComponentUnit assigned{};

public:

    TypeComponentUnit& getAssigned();
    void setAssigned(COMPONENT, ARCH, long, TypeAddress);
    void setAssigned(const TypeComponentUnit&);

};

#endif //DICON_COMPONENTOBJECT_H
