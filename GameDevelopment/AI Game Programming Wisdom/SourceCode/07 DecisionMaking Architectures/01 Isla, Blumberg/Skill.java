package bb;

import java.awt.*;
import java.awt.geom.*;

/*
The base class for all skills.
As it turns out, all skills have a name and a proficiency.

It is assumed that the skill's "activate" method will be called when
the skill starts being used, and that its "deactivate" method will be
called when the skill (or mission) is finished. To make the skill actually
perform its action, the "apply" method is called.

@author naimad
*/

public abstract class Skill {

    Agent agent;
    String name;
    
    double proficiency = 1.0;
    
    public Skill(String name, Agent agent) {
        this.agent = agent;
        this.name = name;
    }

    public Skill(String name, Agent agent, double prof) {
        this.agent = agent;
        this.name = name;
        this.proficiency = prof;
    }
    
    String getName() {
        return name;
    }
    
    //apply this skill to this mission.
    //could involve moving the agent around.
    public abstract void apply(double time, Mission mission);
    
    public double getProficiency() {
        return proficiency;
    }
    
    public void drawSkill(double time, Graphics2D g2) {
        //do nothing
    }
    
    public void activate() {}
    
    public void deActivate(){}
}
