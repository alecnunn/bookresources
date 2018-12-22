package bb;

import java.awt.*;
import java.util.*;
import javax.swing.*;
import java.awt.geom.*;

/*
The city agent is the factory for the army. It has lots of skills - most of them
when executed don't ACTUALLY perform the requested operation (e.g. a city can't
ATTACK_CITY directly) but rather build a unit that can.

The city also issues some other basic missions. For example, its "defendMeMission"
requests a shield to protect the city at all times. Its "defendMeSoldierMission" 
requests direct defense from soldier units whenever the city is under attack. 
Finally its "workerMission" enlists Worker units to gather resources for it so that
it can build things.

@author naimad
*/

public class City extends Agent {

	/*
	The city's skills. Most of these are self-explanatory.
	*/

    class CreateWorkerSkill extends Skill {
        public final static String CREATE_WORKER = "create shield";

        public CreateWorkerSkill(String name, Agent agent, double prof) {
            super(name, agent, prof);
        }
        
        public CreateWorkerSkill(Agent agent, double prof) {
            this(CREATE_WORKER, agent, prof);
        }
        
        int count =0;
        public void apply(double time, Mission mission) {
            //System.out.println("Creating a shield for mission " + mission);
            count++;
            if (((City)agent).requestResources(Worker.COST)) {
                Vec2 position = new Vec2();
                getPosition(agent, position);
                System.out.println("City " + agent + " creating worker at location " + position);
                Worker worker = new Worker(getTeam(), position, bb, world);
                world.add(worker);
                if (mission!=null) {
                    bb.removeFromMission(agent, mission);
                    currentMission = null;
                }
            }
        }
    }


    class CreateShieldSkill extends Skill {
        public final static String CREATE_SHIELD = "create shield";

        public CreateShieldSkill(String name, Agent agent, double prof) {
            super(name, agent, prof);
        }
        
        public CreateShieldSkill(Agent agent, double prof) {
            this(CREATE_SHIELD, agent, prof);
        }
        
        int count =0;
        public void apply(double time, Mission mission) {
            //System.out.println("Creating a shield for mission " + mission);
            count++;
            if (((City)agent).requestResources(Shield.COST)) {
                Vec2 position = new Vec2();
                getPosition(agent, position);
                System.out.println("City " + agent + " creating shield at location " + position);
                Shield shield = new Shield(getTeam(), position, bb, world);
                world.add(shield);
                if (mission!=null) {
                    bb.removeFromMission(agent, mission);
                    currentMission = null;
                }
            }
        }
    }


    class CreateSoldierSkill extends Skill {

        public final static String CREATE_SOLDIER = "create soldier";

        public CreateSoldierSkill(String name, Agent agent, double prof) {
            super(name, agent, prof);
        }
        
        public CreateSoldierSkill(Agent agent, double prof) {
            this(CREATE_SOLDIER, agent, prof);
        }

        public void apply(double time, Mission mission) {
            //System.out.println("Creating a soldier for mission " + mission);
            if (((City)agent).requestResources(Soldier.COST)) {
                Vec2 position = new Vec2();
                getPosition(agent, position);
                System.out.println("City " + agent + " creating soldier at location " + position);
                Soldier soldier = new Soldier(getTeam(), position, bb, world);
                world.add(soldier);
                if (mission!=null) {
                    bb.removeFromMission(agent, mission);
                    currentMission = null;
                }
            }
        }
    }

    class CreateCommanderSkill extends Skill {

        public final static String CREATE_COMMANDER = "create commander";

        public CreateCommanderSkill(String name, Agent agent, double prof) {
            super(name, agent, prof);
        }
        
        public CreateCommanderSkill(Agent agent, double prof) {
            this(CREATE_COMMANDER, agent, prof);
        }
        
        int count =0;
        public void apply(double time, Mission mission) {
            if (((City)agent).requestResources(Commander.COST)) {
                Vec2 position = new Vec2();
                getPosition(agent, position);
                System.out.println("City " + agent + " creating commander at location " + position);
                Commander commander = new Commander(getTeam(), position, bb, world);
                world.add(commander);
                if (mission!=null) {
                    bb.removeFromMission(agent, mission);
                    currentMission = null;
                }
            }
        }
    }
    
	//The three missions the city itself issues. Note that sometimes it might
	//respond to its own mission -- i.e. by building a shield to satisfy the 
	//"defendMeMission"
    Mission defendmeMission;
    Mission defendmeSoldierMission;
    Mission workerMission;
    
    public final static double CITY_LIFE = 100.0;

	/*
	The CITY constructor.
	*/
    public City(int team, Vec2 pos, Blackboard bb, World world) {
        super(team, 0, pos, 0.0, CITY_LIFE, 200.0, bb, world);

		//A city's skills:
        
        Skill createSoldier = new CreateSoldierSkill(Soldier.AttackLocationSkill.ATTACKLOCATION, this, 0.5);
        skills.put(createSoldier.getName(), createSoldier);

        Skill createShield = new CreateShieldSkill(Shield.ShieldSkill.SHIELD, this, 0.5);
        skills.put(createShield.getName(), createShield);

        Skill createCommander = new CreateCommanderSkill(Commander.AttackCitySkill.ATTACKCITY, this, 0.5);
        skills.put(createCommander.getName(), createCommander);

        Skill createWorker = new CreateWorkerSkill(Worker.GatherSkill.GATHER, this, 0.001);
        skills.put(createWorker.getName(), createWorker);

        defaultSkill = createCommander;


        
        //Create and post some missions of my own
        
        defendmeMission = new LocatableMission(Shield.ShieldSkill.SHIELD, 1, 1.0, pos);
        bb.addMission(defendmeMission);
        
        workerMission = new LocatableMission(Worker.GatherSkill.GATHER, 1, 1.0, pos);
        bb.addMission(workerMission);
    }

	
	Vector enemyList = new Vector();
	Vector allyList = new Vector();
	int highestNum = -1;

	/*
	In addition to performing the usual action arbitration that all
	agents perform, the city also has to maintain the missions it has 
	posted.
	*/
    public void actionExecute(double time) {

        if (hitPoints < CITY_LIFE/2) {
            double d = (CITY_LIFE/2 - hitPoints)/(CITY_LIFE/2);
            defendmeMission.setPriority(1.0+10.0*d);
        }

		enemyList.removeAllElements();
		allyList.removeAllElements();
        world.getEnemiesWithinRegion(this, pos, 200.0, enemyList);
        world.getAlliesWithinRegion(this, pos, 200.0, allyList);
       	int num = enemyList.size();
       	if (num>highestNum) {
       		if (defendmeSoldierMission!=null) defendmeSoldierMission.setMissionComplete();
       		defendmeSoldierMission = new LocatableMission(Soldier.AttackLocationSkill.ATTACKLOCATION, num, 1.0, pos);
       		bb.addMission(defendmeSoldierMission);
       		highestNum = num;
       	}
       	double d = (enemyList.size()-allyList.size());
       	if (d < 0) d=0;
       	defendmeSoldierMission.setPriority(0.1+d);
        
        super.actionExecute(time);
    }
    
    public Shape createShape() {
        return new Rectangle2D.Double(-10.0, -10.0, 20.0, 20.0);
    }

	//The city doesn't move around, so it has no need to update its physics.
    public void updatePhysics(double time) {}
    
    public void die() {
    	bb.removeMission(defendmeMission);
    	defendmeMission.setMissionComplete();
    	bb.removeMission(defendmeSoldierMission);
    	defendmeSoldierMission.setMissionComplete();
		workerMission.setMissionComplete();
    	super.die();
    }
    
    //---------------------------RESOURCE MANAGEMENT-------------------------------------
    
    double resources = 200.0;		//initial state
    
    public void addResources(double res) {
    	resources += res;
    }
    
    double lastP = 1.0;
    double filter = 0.99;

    /*
    Requesting resources indirectly effects the priority and capacity of the
    workerMission to gather resources.
    */
    public boolean requestResources(double cost) {
		if (resources >= cost) {
			resources -= cost;
			workerMission.setPriority(1.0);
			return true;
		}
		else {
			int numw = 1+(int)(cost/Worker.CAPACITY);
			if (workerMission.getNumber() != numw) {
				workerMission.setNumber(numw);
				if (!workerMission.isFull()) bb.addMission(workerMission);	//if it's already full, it will just be removed...
			}
			
			if (!workerMission.isFull()) {
				lastP = lastP*filter + (1-filter)*100.0;
				workerMission.setPriority(lastP);
			}
			else workerMission.setPriority(1.0);
			
			return false;
		}
    }
}