package bb;

import java.awt.*;
import java.util.*;
import javax.swing.*;
import java.awt.geom.*;

/*
A worked is just good for gathering resources. It always picks up its load 
from the nearest non-empty resource pool. Then it dumps if off at the city
specified by its mission. If it has no mission, it dumps it off at the nearest
city.

@author naimad
*/

public class Worker extends Agent {

    class GatherSkill extends Skill {
    
    	public final static String GATHER = "gather";
    	public final static double DIST_THRESHOLD = 10.0;

    	public GatherSkill(Agent agent, double prof) {
    		this(GATHER, agent, prof);
    	}
    	
    	public GatherSkill(String name, Agent agent, double prof) {
    		super(name, agent, prof);
    	}

		Vec2 vtemp = new Vec2();
		private City pickCity(Mission mission) {
			City city = null;
			if (mission==null) {
				city = agent.world.getNearestAlliedCityWithinRange(agent, agent.pos, 2000.0);
			}
			else if (mission instanceof LocatableMission) {
				LocatableMission lmission = (LocatableMission)mission;
				lmission.getTargetPosition(vtemp);
				city = agent.world.getNearestAlliedCityWithinRange(agent, vtemp, 2000.0);
			}
			return city;
		}
		
		private Resource pickResource() {
    		return agent.world.getNearestResourceInRange(agent.pos, 2000.0);
		}
		
		double resources = 0.0;
		City targetCity = null;
		Resource targetResource = null;
		
    	public void apply(double time, Mission mission) {
    		//find the nearest res source and go there and get res!
    		if (resources > Worker.CAPACITY) {
    			//go back to city
    			if (targetCity==null) targetCity = pickCity(mission);
    			if (targetCity!=null) {
    				targetCity.getPosition(agent, vtemp);
    				Vec2.sub(vtemp, agent.pos, vtemp);
    				
    				double dist = vtemp.mag();
    				
    				if (dist < DIST_THRESHOLD) {
    					targetCity.addResources(resources);
    					resources = 0.0;
    					bb.removeFromMission(agent, mission);
    					agent.currentMission = null;
    					agent.currentSkill = null;
    					targetResource=null;
    				}
    				else {
    					vtemp.normalize();
    					agent.addForce(vtemp);
    				}
    			}
    		}
    		else {
    			//go to the appropriate resource
    			if (targetResource==null || targetResource.resources <= 0.0) targetResource = pickResource();
    			if (targetResource!=null) {
    				vtemp.set(targetResource.pos);
    				Vec2.sub(vtemp, agent.pos, vtemp);
    				
    				double dist = vtemp.mag();
    				
    				if (dist < DIST_THRESHOLD) {
    					resources += targetResource.extractResources();
    					targetCity=null;
    				}
    				else {
    					vtemp.normalize();
    					agent.addForce(vtemp);
    				}
    			}
    		}
    	}
    }

    public final static int COST = 30;
    public final static double CAPACITY = 50.0;

    public Worker(int team, Vec2 pos, Blackboard bb, World world) {
        super(team, 0, pos, 5.0, 10.0, 250.0, bb, world);

        //subclasses should add skills to their skill hashTable...
        Skill gather = new GatherSkill(this, 1.0);
        skills.put(gather.getName(), gather);

        defaultSkill = gather;
    }

    //Give workers the ability to avoid enemies (since we know they can't defend themselves)
	Vector elist = new Vector();
	Vec2 vtemp = new Vec2();
	public void updatePhysics(double time) {
		
		elist.removeAllElements();
		world.getEnemiesWithinRegion(this, pos, 110.0, elist);
		
		for (int c=0; c < elist.size(); c++) {
			Agent a = (Agent)elist.elementAt(c);
			if (!(a instanceof Worker)) {	//we're not scared of workers!
				a.getPosition(this, vtemp);
				Vec2.sub(pos, vtemp, vtemp);
				double d = vtemp.mag();
				if (d == 0.0) {
					vtemp.set(1.0, 0.0);
					d = 1.0;
				}
				else vtemp.normalize();
				vtemp.scale((110.0-d)/40.0);
				this.addForce(vtemp);
			}
		}
		
		super.updatePhysics(time);
	}
    
    public Shape createShape() {
    	return new Rectangle2D.Double(-4.0, 4.0, 8.0, 8.0) ;
    }    
}
