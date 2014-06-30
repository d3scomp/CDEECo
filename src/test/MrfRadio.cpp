/*
 * MrfRadio.cpp
 *
 *  Created on: 24. 6. 2014
 *      Author: Vladimír Matěna
 */

#include "MrfRadio.h"

MrfRadio::MrfRadio(uint8_t channel, uint16_t panId, uint16_t sourceAddress) {
	mrf.setRecvListener(receiverListenerStatic, this);
	mrf.setBroadcastCompleteListener(broadcastCompleteListenerStatic, this);

	mrf.setChannel(channel);

	mrf.setPANId((uint8_t*) &panId);
	mrf.setSAddr((uint8_t*) &sourceAddress);

	mrf.reset();
}

void MrfRadio::broadcastFragment(const CDEECO::KnowledgeFragment fragment) {
	txBuffer.put(fragment);
}

void MrfRadio::broadcastCompleteListenerStatic(void *data, const bool success) {
	static_cast<MrfRadio*>(data)->broadcastCompleteListener(success);
}

void MrfRadio::broadcastCompleteListener(const bool success) {
	txSem.giveFromISR();
}

void MrfRadio::receiverListenerStatic(void *data) {
	static_cast<MrfRadio*>(data)->receiveListener();
}

void MrfRadio::receiveListener() {
	Packet packet;
	packet.valid = mrf.recvPacket(packet.data.data, packet.size, packet.srcPanId, packet.srcSAddr, packet.fcs,
			packet.lqi, packet.rssi);
	if(packet.valid)
		rxBuffer.put(packet);
}
