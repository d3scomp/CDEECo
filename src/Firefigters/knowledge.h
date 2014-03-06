/**
 *
 * Sample system code used for knowledge backed implementation
 *
 */

class Knowledge {
public:
	string name;
	Knowledge *parent;

	Knowledge(string name, Knowledge *parent): name(name), parent(parent);

	void registerTrigger(Trigger trigger);
private:
	Set<Trigger> triggers;
	void triggerChange() {
		// Run triggers
		parent->triggerChange();
	}
};

template<class T>
class SimpleKnowledge: Knowledge {
public:
	SimpleKnowledge(string name, Knowledge *parent): Knowledge(name, parent) {}

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
	SimpleKnowledge *x, *y;

	Position(string name, Knowledge *parent): Knowledge(name, aprent) {
		x = new SimpleKnowledge<int>("x", this);
		y = new SimpleKnowledge<int>("y", this);
	}
};
