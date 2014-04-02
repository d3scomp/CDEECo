class SensorDataAggregation: Ensamble<GroupLeaderKnowedge, GroupMemberKnowledge> {
public:
	SensorDataAggregation() {
		PERIODIC_MEMBER_TASK(membership, 1000ms);
		KNOWLEDGE_EXCHANGE(knowladgeexchange);
	}

	bool membership(GroupLeaderKnowledge *coord, GroupMemberKnowledge *member) {
		return member.teamId.getval() == (coord.teamId.getVal()
				&& member.temperature.getVal() != 0;
	}

	void map(GroupLeaderKnowledge *coord, GroupMemberKnowledge *member) {
		coord->temperatures.putVal(member->id, member->temperature);
		coord->positions.putVal(member->id, member->position);
	}
}
