package bb;

import java.util.*;

/**
This is our main blackboard class. 

The blackboard is really just a hashtable of open missions. The key is the name of the skill
required for the mission (e.g. "ATTACK_CITY") and the value is the mission object itself
which contains specific mission parameters.

It also happens that this blackboard is where we do the arbitration. When agents apply for a
mission, the blackboard compiles the list of applications and grants it to the top N most 
relevant agents, where N is determined by the mission itself. The rest of the agents are 
denied, which means that either they have to try again next time, or they have to just 
continue what they're currently doing.

NOTE that for the sake of efficiency, ONLY open missions, i.e. missions that are still looking
for members, are kept on the blackboard. Once the mission is filled, it is removed. This means
that sometimes we have to put the mission back on when, say, one of the members dies.

@author naimad
*/

public class Blackboard {

    /*
    The hashtable itself. Note that the actual implementation used is a 
    hashtable of VECTORS (which are the basic Java LIST structure). 
    This is because for every type of skill that might be posted, there
    will often be multiple missions.
    */
    Hashtable openMissions;
    int team;

    public Blackboard(int team) {
        openMissions =  new Hashtable();
        this.team = team;
    }
    
    
    //----------------------------MISSION STATUS------------------------

    /*
    Used by producers of missions to put it on the blackboard.
    */
    public void addMission(Mission mission) {
        Vector v = (Vector)openMissions.get(mission.getSkillName());
        if (v==null) {
            v = new Vector();
            openMissions.put(mission.getSkillName(), v);
        }
        if (!v.contains(mission)) v.addElement(mission);
    }

    /*
    Used by producers of missions to remove the mission, either because of 
    completion, or because the producer of the mission died.
    */
    public void removeMission(Mission mission) {
        Vector v = (Vector)openMissions.get(mission.getSkillName());
        if (v!=null && v.contains(mission)) {
            v.remove(mission);
        }
    }

    /*
    Returns a list of missions that call for a specific kind of skill.
    It all depends on the skill NAME, by the way, not the specific contents
    or implementation of the skill object itself. For example, different 
    agents might go about performing the same skill in different ways.
    */
    public Vector getMissionsForSkill(Skill skill) {
        Vector v = (Vector)openMissions.get(skill.getName());
        if (v==null) {
            v = new Vector();
            openMissions.put(skill.getName(), v);
        }
        return v;
    }
    
    /*
    Check if the given mission is in the blackboard.
    */
    public boolean missionIsPosted(Mission mission) {
        Vector v = (Vector)openMissions.get(mission.getSkillName());
        if (v==null) return false;
        else return v.contains(mission);
    }

    /*
    An agent is requesting to be removed from a mission, for example, because
    it has found a more important mission to attend to, or because it has died.
    Here we have to make the decision about whether or not to add the mission
    back onto the blackboard. We DO if the mission is still incomplete and is
    not full.
    */
    public void removeFromMission(Agent agent, Mission mission) {
        if (mission==null) return;
        mission.removeMember(agent);
        if (!mission.isFull() && !mission.getMissionComplete()) {
            Vector v = (Vector)openMissions.get(mission.getSkillName());
            if (v==null) {
                v = new Vector();
                openMissions.put(mission.getSkillName(), v);
            }
            if (!v.contains(mission)) v.addElement(mission);
        }
    }

    /*
    Used by agents wishing to apply for a mission.
    The "Application" class is a convenience class defined in this file which
    keeps track of relevant application data.
    */
    public void applyForMission(Agent agent, Mission mission, double relevance) {
        addApplication(new Application(agent, relevance), mission);
    }

    /*
    The tricky bit: we want to keep applications ordered by relevance.
    Relevance is a squishy measure of how good they are for the mission,
    taking into account their skill proficiency and they nearness to the 
    mission location.
    	
    All THIS does is add the application to the linked-list mission.applications
    such that at the end all the applications are ordered by relevance.
    */
    private void addApplication(Application newApp, Mission mission) {
        if (mission.applications==null) mission.applications = newApp;
        else {
            if (newApp.relevance > mission.applications.relevance) {
                newApp.nextApplication = mission.applications;
                mission.applications = newApp;
            }
            else {
                Application lastApp = mission.applications;
                Application curApp = mission.applications.nextApplication;
                while(curApp!=null) {
                    if (newApp.relevance > curApp.relevance) {
                        lastApp.nextApplication = newApp;
                        newApp.nextApplication = curApp;
                        return;
                    }
                    lastApp = curApp;
                    curApp = curApp.nextApplication;
                }
                lastApp.nextApplication = newApp;                    
            }
        }
    }

    //-----------------------------UPDATES--------------------------------

    /*
    This method performs the ACTUAL arbitration. It goes through each mission
    on the blackboard. For each mission, it looks at the list of applications
    for that mission. It starts granting the applications until the mission is full.
    The rest of the applications on the list it denies.
    */
    void updateMissionAssignments(double time) {
        Iterator iterator = openMissions.values().iterator();
        while(iterator.hasNext()) {
            Vector missions = (Vector)iterator.next();
            for (int c=0; c < missions.size(); c++) {
                Mission mission = (Mission)missions.get(c);
                
                //Remove the mission from the blackboard if it is complete.
                if (mission.getMissionComplete()) {
                    missions.removeElement(mission);
                    c--;
                    continue;
                }
                
                System.out.println(time + " Blackboard " + team + ": Assigning " + mission + " - priority " + mission.getPriority());
                
                //Start granting applications
                Application curApp = mission.applications;
                while(curApp!=null && !mission.isFull()) {
                    curApp.agent.grantRequest();
                    System.out.println("Granting mission to " + curApp.agent);
                    mission.addMember(curApp.agent);
                    curApp = curApp.nextApplication;
                }
                //Deny the rest.
                while(curApp!=null) {
                    curApp.agent.denyRequest();
                    curApp = curApp.nextApplication;
                }
                //if the mission is now full, we want to remove it from the blackboard, since
                //the blackboard ONLY contains open missions.
                if (mission.isFull()) {
                	missions.removeElement(mission);
                	c--;
                }
                mission.applications = null;
            }
        }
    }


	/*
	A convenience class for mission-applications. We use it to store who
	wants what mission, and how relevant they are to the mission. These
	will be stored in a linked list ordered by relevance. Then, given N
	open spots in the mission, we will simply read through the N top spots
	in the list.
	*/
    public class Application {
        public Agent agent;
        public double relevance;
        
        public Application nextApplication = null;
        
        public Application(Agent agent, double rel) {
            this.agent = agent;
            this.relevance = rel;
        }
    }    
}
