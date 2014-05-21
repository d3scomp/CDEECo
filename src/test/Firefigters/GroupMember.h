DEFINE_COMPOSED_KNOWLEDGE("Position",
		Int, "x",
		Int, "y");

DEFINE_COMPOSED_KNOWLEDGE("GroupMemberKnowledge",
			Float, "temparature"
			Position, "position",
			Bool isMoving,
			Int, "teamId",
			Int, "id");


class GroupMember: DeecoComponent<GroupMemberKnowledge> {
public:
	GroupMember(String id, String team_id) {
		// Setup instance specific knowledge
		knowledge.temperature.setVal(25.0f);
		knowledge.position.x.setVal(300);
		knowledge.position.y.setVal(300);

		PERIODIC_TASK(measureTemperature, 500ms);
	}

	void measureTemperature() {
		knowledge.temperature.setVal(RandomFloat * 100)
		std::cout << id << " new temp: " << Math.round(temperature.value) << "C" << std::endl;
	}
}
