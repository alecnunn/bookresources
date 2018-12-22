package bb;

import java.awt.*;
import java.util.*;
import javax.swing.*;
import java.awt.geom.*;

/*
The shield agent surrounds another agent, thereby absorbing most attacks.

@author naimad
*/

public class Shield extends Agent {

    class ShieldSkill extends Skill {
    
        public final static String SHIELD = "shield";
    
        public ShieldSkill(Agent a) {
            super(SHIELD, a, 1.0);
        }

        Vec2 shieldLocation = new Vec2();        
        Vec2 direction = new Vec2();
        public void apply(double time, Mission mission) {
            if (mission==null) return;
            ((LocatableMission)mission).getTargetPosition(shieldLocation);
            
            //move toward that location!
            Vec2.sub(shieldLocation, agent.pos, direction);
            agent.addForce(direction);
        }
    }

    public final static int COST = 100;
    double radius = 9.0;

    public Shield(int team, Vec2 pos, Blackboard bb, World world) {
        super(team, 0, pos, 3.0, 100.0, 50.0, bb, world);

        Skill protect = new ShieldSkill(this);
        skills.put(protect.getName(), protect);

        defaultSkill = protect;
        
        mass = 20.0;
    }

    public void getPosition(Agent askingAgent, Vec2 inplace) {
        Vec2.sub(askingAgent.pos, this.pos, inplace);
        double dist = inplace.mag();
        dist -=  radius;
        inplace.normalize();
        inplace.scale(dist);
        Vec2.add(askingAgent.pos, inplace, inplace);
    }
    
    public void updatePhysics(double time) {
        externalForce.scale(0.0);
        super.updatePhysics(time);
    }
    
    public Shape createShape() {
        radius = 16.0;
        return new Ellipse2D.Double(-radius, -radius, 2*radius, 2*radius);
    }
    
    public void drawShape(double time, Graphics2D g2) {
        g2.draw(drawShape);
    }    
}
