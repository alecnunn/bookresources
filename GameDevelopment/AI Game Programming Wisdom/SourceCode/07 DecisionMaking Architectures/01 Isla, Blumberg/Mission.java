package bb;

import java.util.*;

/*
The base class for all missions.

@author naimad
*/

public class Mission {

    int number;
    String skillName;
    double priority;
    
    public Mission(String skillname, int number, double priority) {
        this.skillName = skillname;
        this.number = number;
        this.priority = priority;
    }
    
    String getSkillName() {
        return skillName;
    }
    
    int getNumber() {
        return number;
    }
    
    void setNumber(int num) {
    	number = num;
    }
    
    boolean isFull() {
        return (members.size() >= number);
    }

    public void setPriority(double p) {
        priority = p;
    }

    public double getPriority() {
        return priority;
    }
    
    boolean complete = false;
    
    public boolean getMissionComplete() {
        return complete;
    }
    
    public void setMissionComplete() {
        complete = true;
    }
    
    public String toString() {
        if (complete) return skillName + "(C)";
        else return skillName + "(I)";
    }

    //Though not strictly necessary, for convenience we also maintain a list
    //of the agents that are officially working on this mission.
    
    Vector members = new Vector();
    public void addMember(Agent a) {
        if (!members.contains(a)) members.addElement(a);
    }
    
    public void removeMember(Agent a) {
        if (members.contains(a)) members.remove(a);
    }

    //----------------------------APPLICATION LIST--------------------------
    //A convenience structure to help the blackboard to arbitrate the mission
    //assignments.
    
    public Blackboard.Application applications = null;


}
