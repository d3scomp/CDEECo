/**
 * \ingroup example
 * @file MrfRadio.cpp
 *
 * Implementation of the MRF24J40 adapter for CDEECo++ radio interface
 *
 * \date 24. 6. 2014
 * \author Vladimír Matěna <vlada@mattty.cz>
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

void MrfRadio::RxThread::run() {
	console.print(Info, ">>>> Radio RX thread started\n");
	while(true) {
		Packet packet = radio.rxBuffer.get();
		if(packet.valid && packet.data.fragment.length() == packet.size)
			radio.receiveFragment(packet.data.fragment, packet.lqi);
	}
}

void MrfRadio::TxThread::run() {
	console.print(Info, ">>>> Radio TX thread started\n");
	while(true) {
		CDEECO::KnowledgeFragment fragment = radio.txBuffer.get();
		radio.txSem.take();
		taskDISABLE_INTERRUPTS();
		mrf.broadcastPacket((uint8_t*) &fragment, (uint8_t) fragment.length());
		taskENABLE_INTERRUPTS();
	}
}

