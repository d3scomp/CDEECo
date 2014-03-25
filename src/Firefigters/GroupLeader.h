DEFINE_COMPOSED_KNOWLEDGE("Position",
		Int, "x",
		Int, "y");

DEFINE_MAP_KNOWLEDGE("Temps",
		Int, String);

DEFINE_MAP_KNOWLEDGE("Poss",
		Position, String);

DEFINE_SET_KNOLEDGE("FFs",
		String);

DEFINE_COMPOSED_KNOWLEDGE("GroupLeaderKnowledge",
		Int, "teamId",
		Int, "id",
		Temps, "temperatures",
		Poss, "positions",
		FFs, "FFsInDangerInTeam",
		Float, "temperatureThreshold",
		Position, "leaderPosition",
		Float , "spectrum");


class GroupLeader: Component<GroupLeaderKnowledge> {
public:
	GroupLeader(String id, String team_id) {
		// Setup instance specific knowledge
		knowledge.id.setVal(id);
		knowledge.teamId.setVal(team_id);

		PERIODIC_TASK(processSensorData, 2000ms);
		TRIGGERED_TASK(outputGMsInDanger, &GroupLeaderKnowledge.FFsInDangerInTeam);
	}

	void processSensorData() {
		std::cout << GLId << ": Processing sensor data..." << std::endl;
		knoledge.FFsInDangerInTeam.clear();
		std::cout << "Temperatures map holds " <<
				knowledge.temperatures.size() << " items" << std::endl;
	}

	public static void outputGMsInDanger() {
		std::cout << "Firefighters in danger: " << std::endl;
		for_each(knowledge.FFsInDangerInTeam.begin(), knowledge.FFsInDangerInTeam.end(), [](string &name) {
			System.out.print(name + ", ");
		});
	}
}
