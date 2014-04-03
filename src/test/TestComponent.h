/*
 * TestComponent.h
 *
 *  Created on: 28. 3. 2014
 *      Author: Vladimír Matěna
 *
 *  This file contains cdeeco++ component implementation for testing early system.
 *
 */

#ifndef TESTCOMPONENT_H_
#define TESTCOMPONENT_H_

#include "cdeeco/Component.h"

/**
 * Test component knowledge
 *
 * Holds integer and float values named "id" and "value".
 *
 */
class TestKnowledge: Knowledge {
public:
	SimpleKnowledge<int> id;
	SimpleKnowledge<float> value;

	TestKnowledge():Knowledge(NULL), id(this), value(this) {}
};

/**
 * Test component task
 *
 * Uses periodic scheduling.
 * It processes whole component knowledge and outputs whole component knowledge.
 *
 * The task blinks the green LED and increases "id" value in the knowledge.
 */
class TestTask: public PeriodicTask<TestKnowledge, TestKnowledge> {
public:
	// Test task initialization
	TestTask(TestKnowledge *in, TestKnowledge *out): PeriodicTask(250, in, out), state(false) {
		Console::log("TestTask");

		// Init green led
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

		GPIO_InitTypeDef  gpioInitStruct = {
			GPIO_Pin_12, GPIO_Mode_OUT, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_UP
		};
		GPIO_Init(GPIOD, &gpioInitStruct);
	};

	bool state;

	// Test task code
	TestKnowledge run(TestKnowledge in) {
		Console::log("TaskRun");
		// Visualize knowlage value
		int id = in.id.getVal();
		char num[17] = "0000000000000000";
		for(int i = 0; i < 16; i++)
			if(1 << i & id)
				num[15 - i] = '1';
		Console::log(num);

		if(!state)
			GPIOD->BSRRL = GPIO_Pin_12;
		else
			GPIOD->BSRRH = GPIO_Pin_12;

		state = !state;

		// TODO: Do something with the "in" knowledge return the "out" knowledge
		TestKnowledge out;

		// Increase ID and copy value
		out.id.setVal(in.id.getVal() + 1);
		out.value.setVal(in.value.getVal());

		return out;
	}
};

/**
 * Test component container
 *
 * Defines one periodic task.
 */
class TestComponent: public Component<TestKnowledge> {
public:
	TestTask task;

	TestComponent(): task(&this->knowledge, &this->knowledge) {}
};

#endif /* TESTCOMPONENT_H_ */
