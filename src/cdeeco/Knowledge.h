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

	Knowledge(Knowledge *parent): parent(parent), trigger(NULL) {};

	void setTrigger(Trigger trigger);

private:
	Trigger *trigger;

protected:
	void triggerChange() {
		// Run triggers
		if(trigger)
			(*trigger)();
		if(parent)
			parent->triggerChange();
	};
};

template<class T>
class SimpleKnowledge: Knowledge {
public:
	SimpleKnowledge(Knowledge *parent): Knowledge(parent), value(0) {}

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
	SimpleKnowledge<int> x;
	SimpleKnowledge<int> y;

	Position(Knowledge *parent): Knowledge(parent), x(this), y(this) { }
};
