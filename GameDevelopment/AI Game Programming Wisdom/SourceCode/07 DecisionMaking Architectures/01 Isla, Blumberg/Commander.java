package bb;

import java.awt.*;
import java.util.*;
import javax.swing.*;
import java.awt.geom.*;

/*
The commander is a rank above the soldiers. It actually gathers soldiers into little
groups of six to fulfill various kinds of missions, including location defense and
attack.

@author naimad
*/
public class Commander extends Agent {

	/*
	The commanders skills.
	Note that many of these skills themselves create and post missions when
	they become active. These missions are usually intended for soldiers, who 
	will form small squads.
	*/
    
    /*
    This mission causes the commander to defend a piece of land. This is often 
    in response to a mission posted by a defenseTactician, who has identified
    a strategically important area.
    */
    class DefendLocationSkill extends Skill {
    
        public final static String DEFENDLOCATION = "defend location";
        
        public DefendLocationSkill(Agent agent) {
            super(DEFENDLOCATION, agent);
        }

        LocatableMission myMission = null;
        Vec2 vtemp = new Vec2();
        
		//When I activate, I create a mission that soldiers can satisfy.
        public void activate() {
			if (myMission!=null) System.out.println("WARNING! " + agent + "'s myMission != null");
            myMission = new LocatableMission(Soldier.AttackLocationSkill.ATTACKLOCATION, 6, 2, vtemp, 0.0);
            agent.bb.addMission(myMission);
        }
        
        public void apply(double time, Mission mission) {
            if (mission!=null) {
                LocatableMission dmission = (LocatableMission)mission;
                dmission.getTargetPosition(vtemp);
                myMission.setTargetPosition(vtemp);
			
		        //Go toward my target position
                Vec2.sub(vtemp, agent.pos, vtemp);
                double dist = vtemp.mag();
                vtemp.scale(dist-20.0/dist);
                agent.addForce(vtemp);
            }
        }
        
        public void deActivate() {
            if (myMission!=null) {
            	myMission.setMissionComplete();
            	myMission = null;
            }
        }
    }

	/*
	The purpose of this skill is self-explanatory.
	This skill can be directed (if the mission it is passed contains the
	location of a city to attack) or undirected, since this skill is also
	the default skill of the commander (i.e. if there are no appropriate 
	missions available, it performs its AttackCitySkill). That means that
	it makes its own decision about what city to attack.
	*/
    class AttackCitySkill extends Skill {
    
        public final static String ATTACKCITY = "attack city";
        public final static double DESIRED_DIST = 50.0;
        
        public AttackCitySkill(Agent agent) {
            super(ATTACKCITY, agent);
            targetLocation = new Vec2(agent.pos);
        }
        
        Vec2 vtemp = new Vec2();
        City pickCity(Mission mission) {
            //if no other indication, choose the nearest one...
            if (mission!=null) {
            	LocatableMission lmission = (LocatableMission)mission;
            	lmission.getTargetPosition(vtemp);
				return agent.world.getNearestEnemyCityWithinRange(agent, vtemp, agent.viewRange);
            }
            else return agent.world.getNearestEnemyCityWithinRange(agent, agent.pos, agent.viewRange);
        }
        
        LocatableMission myMission = null;
        Vec2 targetLocation = new Vec2();
        Vec2 direction = new Vec2();
        City tcity = null;
        boolean missionBegun = false;
        
        public void activate() {
			if (myMission!=null) System.out.println("WARNING! " + agent + "'s myMission != null");
        	myMission = new LocatableMission(Soldier.AttackLocationSkill.ATTACKLOCATION, 6, 2, agent.pos);
            tcity = null;
        }
        
        public void apply(double time, Mission mission) {
            if (tcity==null) {
                //create the mission and post it!
                tcity = pickCity(mission);
                if (tcity!=null) {
                    agent.bb.addMission(myMission);     //it hasn't been added yet!
					tcity.getPosition(agent, targetLocation);
                }
            }
            
            if (tcity!=null) {
                if (!tcity.isAlive()) {
                    //disband the mission!
                    System.out.println(agent + " --> Mission a success!");
                    if (mission!=null) mission.setMissionComplete();	//completed the mission I was executing
                    else {												//we're acting on our own ... 
                    	currentMission = null;
                    	this.deActivate();
                    	currentSkill = null;
                    }
                }
                else {
                    if (myMission.isFull() && !missionBegun) {
                    	boolean allNearMe = true;
						for (int c=0; c < myMission.members.size() && allNearMe; c++) {
							Agent member = (Agent)myMission.members.elementAt(c);
							allNearMe &= (member.pos.distance(agent.pos) < 40.0);
						}
                    	missionBegun = allNearMe;
                    }
                    
                    if (missionBegun) {
						//Go toward my target position
                    	Vec2.sub(targetLocation, agent.pos, direction);
                    	double dist = direction.mag();
                    	if (dist!=0) {
                        	direction.normalize();
                        	direction.scale((dist-DESIRED_DIST)/DESIRED_DIST);
                        	agent.addForce(direction);
                    	}
                    	if (dist < DESIRED_DIST*1.1) myMission.setTargetPosition(targetLocation);
                    	else {	//have my soldier march in front of me.
                    		direction.normalize();
                    		direction.scale(50.0);
                    		Vec2.add(direction, agent.pos, direction);
                    		myMission.setTargetPosition(direction);
                    	}
                    }
                }
            }
        }

        public void deActivate() {
            if (myMission!=null) {
				missionBegun= false;
                myMission.setMissionComplete();
                myMission = null;
            }
        }
    }

    public final static int COST = 200;
	AttackCitySkill attackCity;

	//The COMMANDER constructor
    public Commander(int team, Vec2 pos, Blackboard bb, World world) {
        super(team, 2, pos, 4.0, 20.0, 2000.0, bb, world);

        //subclasses should add skills to their skill hashTable...
        attackCity = new AttackCitySkill(this);
        skills.put(attackCity.getName(), attackCity);
        
        DefendLocationSkill defendLoc = new DefendLocationSkill(this);
        skills.put(defendLoc.getName(), defendLoc);

        defaultSkill = attackCity;
    }

    //--------------------------------GRAPHICS------------------------------------
    
    Shape outline = new Ellipse2D.Double(-6.0, -6.0, 12.0, 12.0);
    public Shape createShape() {
        return new Ellipse2D.Double(-4.0, -4.0, 8.0, 8.0);
    }

    public void drawShape(double time, Graphics2D g2) {
        g2.fill(drawShape);
        g2.draw(outline);
    }
}
