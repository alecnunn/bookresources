package bb;

import java.util.*;

/*
A special subclass of missions that also has an associated location.
It also has a "range" argument which indicates how close the 
agents should get to the target position.

@author naimad
*/

public class LocatableMission extends Mission {

    Vec2 targetPos;
    double range = 15.0;	//optional range. How close should we get?

    public LocatableMission(String skillname, int number, double priority, Vec2 targetPos) {
        super(skillname, number, priority);
        this.targetPos = new Vec2(targetPos);
    }

    public LocatableMission(String skillname, int number, double priority, Vec2 targetPos, double range) {
		this(skillname, number, priority, targetPos);
		this.range = range;
    }

    public void setTargetPosition(Vec2 newpos) {
        targetPos.set(newpos);
    }

    public void getTargetPosition(Vec2 inplace) {
        inplace.set(targetPos);
    }

    public void setRange(double range) {
    	this.range = range;
    }
    
    public double getRange() {
    	return range;
    }
    
    public String toString() {
        return super.toString() + " at " + targetPos;   
    }
    
}
