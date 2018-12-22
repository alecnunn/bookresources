package bb;

import java.awt.*;
import java.util.*;
import javax.swing.*;
import java.awt.geom.*;

/*
This is the base class for all agents in the BBWar system. All decision-making 
mechanisms are here. Sub classes have mostly to fill in their "SKILLS" hashtable, 
which contains all the abilities that the agent possesses.
*/

public abstract class Agent extends DrawableObject {

    int team;                       //the team we are on.
    World world = null;             //ref to the world.
    Blackboard bb = null;           //ref to the blackboard
    
    Mission currentMission = null;  //the mission we are currently satisfying...
    Skill currentSkill = null;      //the currently-active skill

    int rank = 1;                   //military rank
    double maxSpeed = 1.0;          //1.0, let's say, is average...
    double hitPoints = 10.0;        //10.0, let's say, is average...
    double viewRange = 200.0;       //200.0, let's say, is average...
    
    //Note that each agent subclass should have a list of string constants (skills) which
    //indicate the type of mission it can satisfy...
    public Hashtable skills = new Hashtable();
    
    public Agent(int team, int rank, Vec2 pos, double maxSpeed, double hitPoints, Blackboard bb, World world) {
        this(team, rank, pos, maxSpeed, hitPoints, 50.0, bb, world);
    }
    
    public Agent(int team, int rank, Vec2 pos, double maxSpeed, double hitPoints, double viewRange, Blackboard bb, World world) {
    	super(pos);
        this.team = team;
        this.rank = rank;
        this.bb = bb;
        this.world = world;
        this.maxSpeed = maxSpeed;
        this.hitPoints = hitPoints;
        this.viewRange = viewRange;
    }

    //-----------------------------------ACCESSORS--------------------------------
    
    public int getRank() {
        return rank;   
    }
    
    public int getTeam() {
        return team;
    }
    
    /*----------------------------------ACTION SELECTION-------------------------------
    This is a three-part process - 
    1) In actionSelect() the agent decides which mission it would like to apply for, and
        passes its application along to the blackboard.
    2) The blackboard distributes the missions in its updateMissionAssignments method. In
        the course of this process, it will call back successful applicants with
        grantRequest().
    3) If the mission that was applied for is granted, the agent switches over to that 
        mission (perhaps by removing itself from the current mission, if it has one).
    */
    
    Mission missionPending = null;                  //the mission we've applied for
    public final static double BIAS = 1.1;          //a bias towards staying with the current mission

    public void actionSelect(double time) {
        
        Mission highestPriorityMission = null;
        double highestPriority = 0;

        //Find the highest-priority relevant mission on the blackboard.
        Iterator iterator = skills.values().iterator();        
        while(iterator.hasNext()) {
            Skill skill = (Skill)iterator.next();
            Vector relevantMissions = bb.getMissionsForSkill(skill);
            for (int d=0; d < relevantMissions.size(); d++) {
                Mission mission = (Mission)relevantMissions.elementAt(d);
                double temp = calcPriority(mission, skill);
                if (temp > highestPriority) {
                    highestPriority = temp;
                    highestPriorityMission = mission;
                }
            }
        }

        //Decide whether we should end our current mission.
        if (currentMission !=null && (currentMission.getMissionComplete() || currentMission.getPriority()==0.0)) {
            System.out.println(this + ": Mission null or complete");
            bb.removeFromMission(this, currentMission);
            currentMission = null;
            if (currentSkill!=null) {
                currentSkill.deActivate();
                currentSkill = null;
            }
        }

        //Decide whether to apply for the new mission.
        //We have a certain BIAS for staying with our old mission.
        double curPriority = calcPriority(currentMission, currentSkill);
        if ((currentMission == null) || (BIAS*curPriority < highestPriority)) {
            if (highestPriorityMission!=null) bb.applyForMission(this, highestPriorityMission, highestPriority);
            missionPending = highestPriorityMission;
        }
        else missionPending = null;
    }

    //This method calculates the priority of a mission.   
    Vec2 scratch = new Vec2();
    double calcPriority(Mission mission, Skill skill) {
        //need to factor in distance as well...
        if (mission==null) return 0;
        double p = mission.getPriority()*skill.getProficiency();
        if (mission instanceof LocatableMission) {
            ((LocatableMission)mission).getTargetPosition(scratch);
            p = p / (1.0 + pos.distance(scratch)/10.0);
        }
        return p;
    }
    
    boolean requestGranted = false;
    public void grantRequest() {
        requestGranted = true;
    }
    
    public void denyRequest() {
        requestGranted = false;
    }
    
    public void actionExecute(double time) {
        //we know the action we are executing.
        //we just need a pointer to the world to do it.
        
        if (!isAlive()) return;
        

        //First of all, check if we were expecting a response for a request
        //we made.
        if (missionPending!=null) {
            if (requestGranted) {
                System.out.println("Agent " + this + " granted mission " + missionPending);
                if (currentMission != null) bb.removeFromMission(this, currentMission);
                if (currentSkill!=null) currentSkill.deActivate();  //deactivate old skill
                currentMission = missionPending;
                currentSkill = (Skill)skills.get(currentMission.getSkillName());
                currentSkill.activate();                            //activate new skill
            }
            missionPending = null;
        }
        
        //In some cases, we resort to our default skill...
        if (currentSkill==null || currentMission==null) {
            Skill def = defaultSkill();
            if (currentSkill != def) {
            	if (currentSkill!=null) currentSkill.deActivate();
            	currentSkill = def;
            	currentSkill.activate();
            }
        }

        //Finally, apply the current skill to the current mission.
        if (currentSkill!=null) currentSkill.apply(time, currentMission);
    }

    //Subclasses might have specific default skills.
    Skill defaultSkill = null;
    public Skill defaultSkill() {
        return defaultSkill;
    }

    //---------------------------------INTERACTIONS------------------------------

    public void getPosition(Agent askingAgent, Vec2 inplace) {
        inplace.set(pos);
    }

    public void damage(double damage) {
        hitPoints -= damage;
        if (hitPoints<=0) die();
    }
    
    public void die() {
        //agents can die!

        if (currentSkill!=null) currentSkill.deActivate();
        
        //let the blackboard know that we cannot complete the request...
        bb.removeFromMission(this, currentMission);

        System.out.println("Agent " + this + " dying!");
        
        //just DIE!
        alive = false;
    }

    boolean alive = true;
    public boolean isAlive() {
        return alive;
    }

    //-----------------------------------PHYSICS-----------------------------------

    /*
    There are not really full physics, just point-mass physics. Each agent is a mass, 
    on which various forces act, for locomotion, for example, or for attack response 
    (agents get thrown backwards if a strong force hits them).
    */

    /*
    These are self-generated forces. This force vector will eventually be normalized
    (since the agent can only go so fast).
    */
    public void addForce(Vec2 f) {
        Vec2.add(force, f, force);
    }

    /*
    An external force is, for example, a force from an enemy attack, and does not
    get normalized.
    */
    public void addExternalForce(Vec2 f) {
        Vec2.add(externalForce, f, externalForce);
    }
    
    double mass = 1.0;
    double lasttime = -1.0;
    Vec2 vel = new Vec2();
    Vec2 force = new Vec2();
    Vec2 externalForce = new Vec2();
    double drag;
    public double MAX_ACCEL = 1.0;
    public void updatePhysics(double time) {
        if (lasttime == -1.0) {
            lasttime = time;
            return;
        }
        
        double dt = time - lasttime;
        
        //add in forces from avoiding other agents...eventually
        
        force.scale(1.0/mass);
        double fm = force.mag();
        if (fm > MAX_ACCEL) force.scale(MAX_ACCEL/fm);   //accel can be 0 to MAX_ACCEL.
        force.scale(dt);
        
        Vec2.add(force, externalForce, force);
        externalForce.scale(0.0);

        drag = (maxSpeed-dt*MAX_ACCEL)/maxSpeed;
        vel.scale(drag);
        
        Vec2.add(force, vel, vel);
        
        force.set(vel);
        force.scale(dt);
        Vec2.add(force, pos, pos);
        
        force.scale(0.0);
        
        lasttime = time; 
    }

	//---------------------------------DRAWGRAPHICS---------------------------------------

    //Give the skill a chance to draw itself if it wants to.
    public void updateGraphics(double time, Graphics2D g2) {
    	super.updateGraphics(time, g2);
    	if (currentSkill!=null) currentSkill.drawSkill(time, g2);
    }

}
