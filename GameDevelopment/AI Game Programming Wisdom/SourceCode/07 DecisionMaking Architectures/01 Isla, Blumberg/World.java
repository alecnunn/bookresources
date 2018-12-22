package bb;

import java.util.*;
import java.awt.*;
import java.awt.geom.*;

/*
This class maintains the physical world model, handles world rendering and 
also manages the updates of the agents in both teams and manages the teams 
themselves.

@author naimad
*/

public class World {

    int numTeams;
    Vector[] teams;
    Color[] colors;
    
    Vector blackboards = new Vector();
    Vector agents = new Vector();
    Vector resources = new Vector();

    //Events:
    Vector[] notifiers;

    public World(int numTeams) {
        this.numTeams = numTeams;
        teams = new Vector[numTeams];
        colors = new Color[numTeams];
        notifiers = new Vector[numTeams];
        for (int c=0; c < numTeams; c++) {
            teams[c] = new Vector();
            notifiers[c] = new Vector();
        }
        
        if (numTeams>0) colors[0] = Color.red;
        if (numTeams>1) colors[1] = Color.blue;
        if (numTeams>2) colors[2] = Color.green;
        //etc...
    }

    public void add(Agent a) {
        agents.addElement(a);
        int t = a.getTeam();
        if (t < numTeams) {
            teams[t].addElement(a);
        }
        else System.out.println("ERROR: TOO MANY TEAMS!");
    }
    
    public void add(Blackboard bb) {
        blackboards.addElement(bb);
    }
    
    public void add(Resource r) {
    	resources.addElement(r);
    }

    public void remove(Agent a) {
        agents.remove(a);
        int t = a.getTeam();
        if (t < numTeams) {
            teams[t].remove(a);
        }
        else System.out.println("ERROR: TOO MANY TEAMS!");
    }
    
    public Vector getAgents() {
        return agents;
    }
    
    //----------------------------UPDATES-------------------------------

    public void update(double time) {

    	//Update the resource pools
    	for (int c=0; c < resources.size(); c++) {
    		Resource r = (Resource)resources.elementAt(c);
    		if (r.resources <= 0.0) {
    			c--;
    			resources.removeElement(r);
    		}
    	}

    	//Update the agents (note that we are NOT updating one team at a time -
    	//this avoids giving one team an overall execution order advantage.
    	//Instead all agents are dumped into one big list in order of creation
    	//time, which, after the initial few, is more or less random.
        for (int c=0; c < agents.size(); c++) {
            Agent a = (Agent)agents.elementAt(c);
            a.actionSelect(time);
        }

		//Have the blackboards perform their arbitration
        for (int c=0; c < blackboards.size(); c++) {
            Blackboard bb = (Blackboard)blackboards.elementAt(c);
            bb.updateMissionAssignments(time);
        }
        
		//Have the agents execute their current skills.
        for (int c=0; c < agents.size(); c++) {
            Agent a = (Agent)agents.elementAt(c);
            a.actionExecute(time);
        }

        //Finally update their physics, or, if they are dead, 
        //remove them from the list.
        for (int c=0; c < agents.size(); c++) {
            Agent a = (Agent)agents.elementAt(c);
            if (a.isAlive()) a.updatePhysics(time);
            else {
                for (int d=0; d < notifiers[a.getTeam()].size(); d++) {
                    EventNotifier en = (EventNotifier)(notifiers[a.getTeam()].elementAt(d));
                    en.deathNotification(a, a.pos);
                }
                remove(a);
            }
        }
    }
    
    void updateGraphics(double time, Graphics2D g2) {
    	
		g2.setPaint(java.awt.Color.gray);
		
    	for (int c=0; c < resources.size(); c++) {
    		Resource r = (Resource)resources.elementAt(c);
    		r.updateGraphics(time, g2);
    	}
    	
        for (int d=0; d < numTeams; d++) {
        
            //change the color!
            g2.setPaint(colors[d]);
        
            for (int c=0; c < teams[d].size(); c++) {
                Agent a = (Agent)teams[d].elementAt(c);
                a.updateGraphics(time, g2);
            }
        }
    }
    
    //---------------------------------SERVICES---------------------------------

	/*
	These are a series of self-explanatory services that the world provides to help
	the agents make their decisions about what to do.
	*/

    Vec2 vtemp = new Vec2();
        
    public void getEnemiesWithinRegion(Agent sensingAgent, Vec2 center, double range, Vector addToList) {
        double rangeSquared = range*range;
        for (int c=0; c < agents.size(); c++) {
            Agent atemp = (Agent)agents.elementAt(c);
            if (atemp.getTeam()!=sensingAgent.getTeam()) {
                atemp.getPosition(sensingAgent, vtemp);
                double dtemp = center.distanceSquared(vtemp);
                if (dtemp <= rangeSquared) addToList.addElement(atemp);
            }
        }
    }

    public void getEnemyCitiesWithinRegion(Agent sensingAgent, Vec2 center, double range, Vector addToList) {
        double rangeSquared = range*range;
        for (int c=0; c < agents.size(); c++) {
            Agent atemp = (Agent)agents.elementAt(c);
            if (atemp.getTeam()!=sensingAgent.getTeam() && (atemp instanceof City)) {
                atemp.getPosition(sensingAgent, vtemp);
                double dtemp = center.distanceSquared(vtemp);
                if (dtemp <= rangeSquared) addToList.addElement(atemp);
            }
        }
    }
    
    
    public Agent getNearestWithinRegion(Agent sensingAgent, Vec2 center, double range) {
        Agent nearest = null;
        
        double nearVal = Double.MAX_VALUE;
        for (int c=0; c < agents.size(); c++) {
            Agent atemp = (Agent)agents.elementAt(c);
            if (atemp.getTeam()!=sensingAgent.getTeam()) {
                atemp.getPosition(sensingAgent, vtemp);
                double dtemp = center.distanceSquared(vtemp);
                if (dtemp < nearVal) {
                    nearVal = dtemp;
                    nearest = atemp;
                }
            }
        }
        if (nearVal <= range*range) return nearest;
        else return null;
    }

    public City getNearestEnemyCityWithinRange(Agent sensingAgent, Vec2 center, double range) {
        Agent nearest = null;
        
        double nearVal = Double.MAX_VALUE;
        for (int c=0; c < agents.size(); c++) {
            Agent atemp = (Agent)agents.elementAt(c);
            if (atemp.getTeam()!=sensingAgent.getTeam() && (atemp instanceof City)) {
                atemp.getPosition(sensingAgent, vtemp);
                double dtemp = center.distanceSquared(vtemp);
                if (dtemp < nearVal) {
                    nearVal = dtemp;
                    nearest = atemp;
                }
            }
        }
        if (nearVal <= range*range) return (City)nearest;
        else return null;
    }
    
    
    public void getAlliesWithinRegion(Agent sensingAgent, Vec2 center, double range, Vector addToList) {
        double rangeSquared = range*range;
        for (int c=0; c < agents.size(); c++) {
            Agent atemp = (Agent)agents.elementAt(c);
            if (atemp.getTeam()==sensingAgent.getTeam()) {
                atemp.getPosition(sensingAgent, vtemp);
                double dtemp = center.distanceSquared(vtemp);
                if (dtemp <= rangeSquared) addToList.addElement(atemp);
            }
        }
    }
    
    public void getAlliedCitiesWithinRegion(Agent sensingAgent, Vec2 center, double range, Vector addToList) {
        double rangeSquared = range*range;
        for (int c=0; c < agents.size(); c++) {
            Agent atemp = (Agent)agents.elementAt(c);
            if (atemp.getTeam()==sensingAgent.getTeam() && (atemp instanceof City)) {
                atemp.getPosition(sensingAgent, vtemp);
                double dtemp = center.distanceSquared(vtemp);
                if (dtemp <= rangeSquared) addToList.addElement(atemp);
            }
        }
    }
    
    public City getNearestAlliedCityWithinRange(Agent sensingAgent, Vec2 center, double range) {
        Agent nearest = null;
        
        double nearVal = Double.MAX_VALUE;
        for (int c=0; c < agents.size(); c++) {
            Agent atemp = (Agent)agents.elementAt(c);
            if (atemp.getTeam()==sensingAgent.getTeam() && (atemp instanceof City)) {
                atemp.getPosition(sensingAgent, vtemp);
                double dtemp = center.distanceSquared(vtemp);
                if (dtemp < nearVal) {
                    nearVal = dtemp;
                    nearest = atemp;
                }
            }
        }
        if (nearVal <= range*range) return (City)nearest;
        else return null;
    }
    
    
    //-----------------------------RESOURCES-----------------------------------
    
    public Resource getNearestResourceInRange(Vec2 center, double range) {
        Resource nearest = null;
        double nearVal = Double.MAX_VALUE;

        for (int c=0; c < resources.size(); c++) {
            Resource atemp = (Resource)resources.elementAt(c);
            double dtemp = center.distanceSquared(atemp.pos);
            if (dtemp < nearVal) {
                nearVal = dtemp;
                nearest = atemp;
            }
        }
        if (nearVal <= range*range) return nearest;
        else return null;
    }

    //----------------------------EVENTS-----------------------------
    
    public void addEventNotifier(int team, EventNotifier en) {
        notifiers[team].addElement(en);
    }
}
