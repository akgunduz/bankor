//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_COMMTCP_H
#define DICON_COMMTCP_H

#include "Common.h"
#include "Scheduler.h"
#include "CommInterface.h"
#include "Message.h"
#include "Component.h"
#include "CommInterfaceType.h"

#define LOOPBACK_RANGE 256
#define MULTICAST_ADDRESS 0xE9010101
#define DEFAULT_TCP_PORT 61001
#define DEFAULT_UDP_PORT 62001
#define TRY_COUNT 10

class CommTCP : public CommInterface {

    static inline int lastFreeTCPPort = DEFAULT_TCP_PORT;
    static inline int lastFreeUDPPort = DEFAULT_UDP_PORT;

    uv_tcp_t *tcpServer{};
    uv_udp_t *udpServer{};

	bool initTCP();
	bool initUDP();

	static bool onReceive(uv_handle_t*, ssize_t, const uv_buf_t *);
    bool onServerConnect();

    static bool onTCPSendCB(const TypeComponentUnit&, const uint8_t*, size_t);
    static bool onUDPSendCB(const TypeComponentUnit&, const uint8_t*, size_t);

	bool onTCPSend(const TypeComponentUnit&, TypeMessage);
    bool onUDPSend(const TypeComponentUnit&, TypeMessage);
    bool onSend(const TypeComponentUnit&, TypeMessage) override;

protected:

    bool initInterface() override;

public :

	CommTCP(const TypeHostUnit&, const TypeDevice&,
            const CommInterfaceCB *);
	~CommTCP() override;

    COMM_INTERFACE getType() override;
	bool isSupportMulticast() override;

    TypeAddressList getAddressList() override;
};

#endif //DICON_COMMTCP_H
