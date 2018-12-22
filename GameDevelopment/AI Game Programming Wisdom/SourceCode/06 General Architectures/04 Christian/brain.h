// call ie::getBrain(name) to get a brain

#define IE_START(name)\
	ie->start(name);  // adds brain to the list

#define GOAL(name)\
	ie->makeGoal(name); // startGoal ends previous goal

#define IF(cond)\
	ie->makeRule(cond);

#define GOTO(goal)\
	ie->addRuleGoto(goal);

#define GOSUB(goal)\
	ie->addRuleGosub(goal);

#define RETURN\
	ie->addRuleReturn(goal);

#define IE_END\
	ie->end(); // ends last goal and brain

