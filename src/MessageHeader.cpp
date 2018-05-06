//
// Created by Haluk AKGUNDUZ on 23/03/18.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//


#include "MessageHeader.h"
#include "MessageTypes.h"

MessageHeader::MessageHeader() {

}

int MessageHeader::getSize() {

    return sizeof(MessageHeader);
}

void MessageHeader::setType(MSG_TYPE type) {

    this->type = (int)type;
}

MSG_TYPE MessageHeader::getType() {

    return (MSG_TYPE)type;
}

ComponentObject MessageHeader::getOwner() {

    return ComponentObject((COMPONENT)(owner >> 32), (int)(owner & 0xFFFFFFFF), ownerAddress);
}

void MessageHeader::setOwner(ComponentObject owner) {

    this->owner = (((long)owner.getType()) << 32) | owner.getID();
    this->ownerAddress = owner.getAddress();
}

//long MessageHeader::getOwnerAddress() {
//
//    return ownerAddress;
//}
//
//void MessageHeader::setOwnerAddress(long ownerAddress) {
//
//    this->ownerAddress = ownerAddress;
//}

long MessageHeader::getVariant(int id) {

    if (id < MAX_VARIANT) {
        return variant[id];
    }

    return 0;
}

void MessageHeader::setVariant(int id, long variant) {

    if (id >= MAX_VARIANT) {
        return;
    }

    this->variant[id] = variant;
}

int MessageHeader::getPriority() {

    return priority;
}

int MessageHeader::iteratePriority() {

    if (priority > 1) {
        priority--;
    }

    return priority;
}

void MessageHeader::setPriority(int priority) {

    this->priority = priority;
}

void MessageHeader::normalizePriority() {

    priority *= PRIORITY_COEFFICIENT;
}

bool MessageHeader::set(const uint8_t* buffer) {

    type = ntohl(*((int *) buffer)); buffer += 4;
    priority = ntohl(*((int *) buffer)); buffer += 4;
    owner = ntohll(*((long *) buffer)); buffer += 8;
    ownerAddress = ntohll(*((long *) buffer)); buffer += 8;
    for (int i = 0; i < MAX_VARIANT; i++) {
        variant[i] = ntohll(*((long *) buffer)); buffer += 8;
    }

    return true;
}

bool MessageHeader::extract(uint8_t *buffer) {

    *((int *) buffer) = htonl(type); buffer += 4;
    *((int *) buffer) = htonl(priority); buffer += 4;
    *((long *) buffer) = htonll(owner); buffer += 8;
    *((long *) buffer) = htonll(ownerAddress); buffer += 8;
    for (int i = 0; i < MAX_VARIANT; i++) {
        *((long *) buffer) = htonll(variant[i]); buffer += 8;
    }

    return true;
}
