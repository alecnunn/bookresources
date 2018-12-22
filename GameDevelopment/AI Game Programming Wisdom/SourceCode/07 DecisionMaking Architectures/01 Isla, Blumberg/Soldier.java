package bb;

import java.awt.*;
import java.util.*;
import javax.swing.*;
import java.awt.geom.*;

/*
The soldier agent. This is the agent that actually does most of the work, both for attack
and for defense.

@author naimad
*/

public class Soldier extends Agent {

	//This skill just randomly approaches and attacks the nearest enemy.
    class ShootSkill extends Skill {
    
        double hitProb;
        public final static double SHOOT_DAMAGE = 10.0;
        public final static String SHOOT = "shoot";
        public final static int SHOOT_FREQ = 10;
        public final static double SHOOT_RANGE = 100.0;
        
        public double DESIRED_DIST = 15.0;
    
        public ShootSkill(Agent a, double prof) {
            this(SHOOT, a, prof);
        }
        
        public ShootSkill(String name, Agent a, double prof) {
            super(name, a, prof);
            hitProb = 1.0 - 1.0/(1.0+getProficiency());
            count = (int)((double)SHOOT_FREQ*Math.random());
        }
        
        Vec2 targetPosition(Mission mission) {
        	if (enemy==null) return null;
            else return enemy.pos;
        }
        
        int count = 0;
        double lastShootTime = 0.0;
        Agent enemy = null;
        Vec2 direction = new Vec2();
        Vec2 scratch = new Vec2();
        public void apply(double time, Mission mission) {
            enemy = agent.world.getNearestWithinRegion(agent, agent.pos, agent.viewRange);
            Vec2 target = targetPosition(mission);
            if (target!=null) {
            	Vec2.sub(target, agent.pos, direction);
            	double dist = direction.mag();
            	if (dist==0.0) {
            		direction.x(Math.random()-0.5);
            		direction.y(Math.random()-0.5);
            		direction.normalize();
            	}
            	direction.normalize();
            	scratch.set(direction);
            	
            	double desiredDist = DESIRED_DIST;
            	if (mission instanceof LocatableMission) {
            		desiredDist = ((LocatableMission)mission).getRange();
            	}
            	
            	scratch.scale((dist-desiredDist)/DESIRED_DIST);
            	agent.addForce(scratch);
            }
            
            
            if (enemy!=null) {
                //shoot him!
                
            	Vec2.sub(enemy.pos, agent.pos, direction);
            	double dist = direction.mag();
            	direction.normalize();
                
                if (dist <= SHOOT_RANGE) {
                    if (count%SHOOT_FREQ==0) {   //just so that we can't shoot all the time...
                        //System.out.println("Agent " + agent + " shooting " + enemy);
                        if (Math.random() <= hitProb) {
                            double damage = Math.random()*SHOOT_DAMAGE;
                            enemy.damage(damage);
                            direction.scale(damage*1.0);
                            enemy.addExternalForce(direction);
                            lastShootTime = time;
                        }
                    }
                    count++;
                }
            }
        }
        
        public void drawSkill(double time, Graphics2D g2) {
            if (time==lastShootTime && enemy!=null) {
                //draw me!
                Shape line = new Line2D.Double(agent.pos.x(), agent.pos.y(), enemy.pos.x(), enemy.pos.y());
                g2.draw(line);
            }
        }
    }

	//This skill ALSO attacks the nearest enemy but also moves in the direction of the
	//target location specified by the mission.
    class AttackLocationSkill extends ShootSkill {
        
        public static final String ATTACKLOCATION = "attack location";
        Vec2 location = new Vec2();
        
        public AttackLocationSkill(Agent a, double prof) {
            super(ATTACKLOCATION, a, prof);
            DESIRED_DIST = 20.0;
        }
        
        Vec2 targetPosition(Mission mission) {
            if (mission!=null && (mission instanceof LocatableMission)) {
                ((LocatableMission)mission).getTargetPosition(location);
                return location;
            }
            else return super.targetPosition(mission);
        }
    }


    public final static int COST = 50;

    public Soldier(int team, Vec2 pos, Blackboard bb, World world) {
        super(team, 0, pos, 7.5, 10.0, 250.0, bb, world);

        //subclasses should add skills to their skill hashTable...
        Skill shoot = new ShootSkill(this, 1.0);
        skills.put(shoot.getName(), shoot);

        Skill attackLocation = new AttackLocationSkill(this, 1.0);
        skills.put(attackLocation.getName(), attackLocation);

        defaultSkill = shoot;
    }
    
    public Shape createShape() {
        return new Ellipse2D.Double(-5.0, -5.0, 10.0, 10.0);
    }    
}
