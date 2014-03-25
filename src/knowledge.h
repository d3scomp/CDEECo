/**
 *
 * Sample system code used for knowledge backed implementation
 *
 */

// For now trigger is a function pointer
typedef void (*Trigger)(void);

// Basic knowledge class
class Knowledge {
public:
	Knowledge *parent;

	Knowledge(Knowledge *parent): parent(parent) {};

	void setTrigger(Trigger trigger);
private:
	Trigger *trigger;
	void triggerChange() {
		// Run triggers
		if(trigger)
			(*trigger)();
		parent->triggerChange();
	};
};

template<class T>
class SimpleKnowledge: Knowledge {
public:
	SimpleKnowledge(Knowledge *parent): Knowledge(parent) {}

	T getVal() {
		return value;
	}

	void setVal(T value) {
		this->value = value;
		triggerChange();
	}

private:
	T value;
};


class Position: Knowledge {
public:
	/*SimpleKnowledge<int> x(NULL);
	SimpleKnowledge<int> y((Knowledge*)NULL);*/

	SimpleKnowledge<int> *x;
	SimpleKnowledge<int> *y;

	Position(Knowledge *parent): Knowledge(parent) {
		/*x.parent = parent;
		y.parent = parent;*/

		x = new SimpleKnowledge<int>(this);
		y = new SimpleKnowledge<int>(this);
	}
};
