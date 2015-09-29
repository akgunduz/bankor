//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Distributor.h"

Distributor::Distributor(uint32_t collInterfaceIndex,
		uint32_t clientInterfaceIndex, const std::string &rootPath, double backupRate) {

	callback = new InterfaceCallback(receiveCB, this);

	try {

		collectorConnector = new Connector(collInterfaceIndex, callback, rootPath);

	} catch (const std::runtime_error e) {

		LOG_E("Connector Init failed!!!");

		delete callback;

		throw std::runtime_error("Distributor : Connector Init failed!!!");
	}

	if (collInterfaceIndex != clientInterfaceIndex) {

		try {

			clientConnector = new Connector(clientInterfaceIndex, callback, rootPath);

		} catch (const std::runtime_error e) {

			LOG_E("Connector Init failed!!!");

			delete collectorConnector;
			delete callback;

			throw std::runtime_error("Distributor : Connector Init failed!!!");
		}

	} else {

		clientConnector = collectorConnector;

	}

	LOG_U(UI_UPDATE_DIST_ADDRESS, getAddress(HOST_COLLECTOR), getAddress(HOST_CLIENT));

	this->rootPath = rootPath;

	clientManager = new ClientManager(clientConnector,
			timeoutCallback, sendWakeupMessage, backupRate);
	clientManager->initClientChecker();

	LOG_I("Instance is created!!!");
};

Distributor::~Distributor() {

	delete clientManager;

	if (collectorConnector != clientConnector) {
		delete clientConnector;
	}

	delete collectorConnector;

	delete callback;
}

bool Distributor::receiveCB(void *arg, long address, Message *msg) {

	Distributor *dist = (Distributor *) arg;

	switch(msg->getOwner()) {

		case HOST_CLIENT:
			if (dist->clientConnector->getInterfaceType() == Address::getInterface(address)) {
				dist->processClientMsg(address, msg);
			}
			break;

		case HOST_COLLECTOR:
			if (dist->collectorConnector->getInterfaceType() == Address::getInterface(address)) {
				dist->processCollectorMsg(address, msg);
			}
			break;

		default:
			LOG_W("Wrong message received : %d from %s, disgarding", msg->getType(), Address::getString(address).c_str());
			delete msg;
			break;
	}

	return true;
}

INTERFACES Distributor::getInterfaceType(HOST host) {

	if (host == HOST_COLLECTOR) {
		return collectorConnector->getInterfaceType();
	} else {
		return clientConnector->getInterfaceType();
	}

}

long Distributor::getAddress(HOST host) {

	if (host == HOST_COLLECTOR) {
		return collectorConnector->getAddress();
	} else {
		return clientConnector->getAddress();
	}

}

bool Distributor::processCollectorMsg(long address, Message *msg) {

	bool status = false;

	switch(msg->getType()) {

		case MSGTYPE_READY:
			LOG_U(UI_UPDATE_DIST_COLL_LIST, address, (uint64_t)0L);
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"READY\" msg from collector: %s", Address::getString(address).c_str());
			break;

		case MSGTYPE_CLIENT:
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"CLIENT\" msg from collector: %s", Address::getString(address).c_str());

			status = send2CollectorMsg(address, MSGTYPE_CLIENT);
			break;

		case MSGTYPE_TIME:
			collStartTime.start();
			clientManager->resetDiffTimes();
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"TIME\" msg from collector: %s", Address::getString(address).c_str());
			break;

		default:
			break;
	}

	delete msg;
	return status;

}

bool Distributor::processClientMsg(long address, Message *msg) {

	bool status = false;

	switch(msg->getType()) {

		case MSGTYPE_READY:
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"READY\" msg from client: %s", Address::getString(address).c_str());

			LOG_U(UI_UPDATE_DIST_CLIENT_LIST, address, IDLE);

			clientManager->setClientIdle(address, collStartTime.stop());

			if (collectorWaitingList.size() > 0) {

				long collectorAddress = collectorWaitingList.front();
				collectorWaitingList.pop_front();

				LOG_U(UI_UPDATE_DIST_LOG,
                      "Processing a collector from the waiting list: %s",
					  Address::getString(collectorAddress).c_str());

				status = send2CollectorMsg(collectorAddress, MSGTYPE_CLIENT);

			} else {
				status = true;
			}

			break;

		case MSGTYPE_ALIVE:
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"ALIVE\" msg from client: %s", Address::getString(address).c_str());

			if (!clientManager->setClientValidate(address)
					&& collectorWaitingList.size() > 0) {

                long collectorAddress = collectorWaitingList.front();
				collectorWaitingList.pop_front();

				LOG_U(UI_UPDATE_DIST_LOG,
						"Processing a collector from the waiting list: %s", Address::getString(collectorAddress).c_str());

				status = send2CollectorMsg(collectorAddress, MSGTYPE_CLIENT);
			} else {
				status = true;
			}

			LOG_U(UI_UPDATE_DIST_CLIENT_LIST, address, IDLE);

			break;

		case MSGTYPE_BUSY:
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"BUSY\" msg from client: %s", Address::getString(address).c_str());

			clientManager->setClientBusy(address);
			LOG_U(UI_UPDATE_DIST_CLIENT_LIST, address, BUSY);


			status = true;
			break;

		case MSGTYPE_TIMEOUT:
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"TIMEOUT\" msg from client: %s", Address::getString(address).c_str());

			clientManager->setClientRemove(address);
			LOG_U(UI_UPDATE_DIST_CLIENT_LIST, address, REMOVE);

			status = send2CollectorMsg(msg->getVariant(0), MSGTYPE_CLIENT);

			break;

		default :
			break;
	}

	delete msg;
	return status;

}

bool Distributor::send2ClientMsg(long address, uint8_t type) {

	Message *msg = new Message(HOST_DISTRIBUTOR, type, rootPath);

	switch(type) {

		case MSGTYPE_WAKEUP:
			msg->setPriority(PRIORITY_1);
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"WAKEUP\" msg sent to client: %s", Address::getString(address).c_str());
			break;

		default:
			delete msg;
			return false;

	}

	return clientConnector->send(address, msg);

}

bool Distributor::send2CollectorMsg(long address, uint8_t type) {

	Message *msg = new Message(HOST_DISTRIBUTOR, type, rootPath);

	switch(type) {

		case MSGTYPE_WAKEUP:
			msg->setPriority(PRIORITY_1);
			LOG_U(UI_UPDATE_DIST_LOG,
					"\"WAKEUP\" msg sent to collector: %s", Address::getString(address).c_str());
			break;

		case MSGTYPE_CLIENT: {
				msg->setPriority(PRIORITY_2);
				long clientAddress = clientManager->getIdleClient(address);

				if (clientAddress != 0) {

					LOG_U(UI_UPDATE_DIST_CLIENT_LIST, clientAddress, PREBUSY);
					LOG_U(UI_UPDATE_DIST_LOG,
							"\"CLIENT\" msg sent to collector: %s with available client: %s",
						  Address::getString(address).c_str(),
						  Address::getString(clientAddress).c_str());

                    msg->setVariant(0, clientAddress);

                    LOG_U(UI_UPDATE_DIST_COLL_LIST, address, clientAddress);

				} else {
					collectorWaitingList.push_back(address);

					LOG_U(UI_UPDATE_DIST_LOG,
							"\"CLIENT\" msg sent to collector: %s with no available client",
						  Address::getString(address).c_str());

                    msg->setVariant(0, 0);

                    LOG_U(UI_UPDATE_DIST_COLL_LIST, address, (uint64_t)0L);
				}

			}
			break;

		default:
			delete msg;
			return false;
	}

	return collectorConnector->send(address, msg);

}

std::string Distributor::getRootPath() {

	return rootPath;

}

bool Distributor::sendWakeupMessagesAll() {

	sendWakeupMessage(clientConnector);
	if (collectorConnector != clientConnector) {
		sendWakeupMessage(collectorConnector);
	}
	return true;
}

bool Distributor::reset() {

	clientManager->clear();
	collectorWaitingList.clear();
	collStartTime.reset();
	return true;

}

bool Distributor::timeoutCallback(Connector *connector,
								  long address, long collectorAddress) {

	Message *msg = new Message(HOST_CLIENT, MSGTYPE_TIMEOUT, connector->getRootPath());
	msg->setVariant(0, collectorAddress);
	connector->getInterface()->
			push(MESSAGE_RECEIVE, address, msg);

	return true;
}

bool Distributor::sendWakeupMessage(Connector *connector) {

	std::vector<long> list = connector->getAddressList();

	for (int i = 0; i < list.size(); i++) {

		Message *msg = new Message(HOST_DISTRIBUTOR, MSGTYPE_WAKEUP, connector->getRootPath());
		msg->setPriority(PRIORITY_1);
		connector->send(list[i], msg);

	}

	LOG_U(UI_UPDATE_DIST_LOG,
			"\"WAKEUP\" messages sent to network");

	return true;
}
