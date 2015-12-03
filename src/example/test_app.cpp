/**
 * @file test_app.cpp
 * 
 * This configures an example DEECo application.
 * 
 */

#include <main.h>

#include "MrfRadio.h"
#include "TestComponent.h"
#include "PortableSensor.h"
#include "Alarm.h"
#include "TempExchange.h"


/**
 * Setups the example CDEECo application
 */
void cdeecoSetup(const uint32_t uniqId) {
	//// System setup
	auto radio = new MrfRadio(0, uniqId, uniqId);
	auto system = new CDEECO::System<3, 64>(*radio);

	// Test component
	new TestComponent::Component(*system, uniqId);

	///// Temperature monitoring system
	// Components
	auto sensor = new PortableSensor::Component(*system, uniqId);
	auto alarm = new Alarm::Component(*system, uniqId);

	// Caches
	auto sensorCache = new CDEECO::KnowledgeCache<PortableSensor::Component::Type, PortableSensor::Knowledge, 10>();
	auto alarmCache = new CDEECO::KnowledgeCache<Alarm::Component::Type, Alarm::Knowledge, 10>();
	system->registerCache(sensorCache);
	system->registerCache(alarmCache);

	// Ensembles
	new TempExchange::Ensemble(*alarm, *sensorCache);
	new TempExchange::Ensemble(*sensor, *alarmCache);
}

