//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#include "CollectorObject.h"

CollectorObject::CollectorObject(COLLSTATES _state, long id, long address) :
        state(_state), ComponentObject(COMP_COLLECTOR, id, address) {
}

CollectorObject::CollectorObject(long id, long address) :
        CollectorObject(COLLSTATE_IDLE, id, address) {
}

CollectorObject::CollectorObject(const char* rootPath) :
        state(COLLSTATE_IDLE), ComponentObject(COMP_COLLECTOR, rootPath)  {
}

CollectorObject::CollectorObject() :
        CollectorObject(0, 0) {
}

CollectorObject::CollectorObject(const CollectorObject &copy) :
        CollectorObject(copy.state, copy.getID(), copy.getAddress()) {
}

CollectorObject::CollectorObject(const ComponentObject &copy) :
        CollectorObject(copy.getID(), copy.getAddress()) {
}

CollectorObject::~CollectorObject() {

}

COLLSTATES CollectorObject::getState() {

    return state;
}

void CollectorObject::setState(COLLSTATES _state) {

    this->state = _state;
}
