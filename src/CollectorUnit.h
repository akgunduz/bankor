//
// Created by Haluk AKGUNDUZ on 1.08.2020.
//

#ifndef DICON_COLLECTORUNIT_H
#define DICON_COLLECTORUNIT_H

#include "Address.h"
#include "CollectorState.h"
#include "ComponentUnit.h"
#include "CollectorObject.h"

class CollectorUnit : public ComponentUnit, public CollectorObject {

public:
    CollectorUnit(COLLSTATES, ARCH, long, TypeAddress);
    CollectorUnit(ARCH, long, TypeAddress);
    CollectorUnit(const CollectorUnit &);

    ~CollectorUnit() override;
};

typedef std::shared_ptr<CollectorUnit> TypeCollectorUnit;
typedef std::vector<TypeCollectorUnit> TypeCollectorList;

#endif //DICON_COLLECTORUNIT_H
