package bb;

import java.awt.*;
import java.util.*;
import javax.swing.*;
import java.awt.geom.*;

/*
The DefenseTactician is a unit that analyses the battle and posts missions 
which prompt commanders and their squads to defend strategically
important areas.

It does this by receiving notifications of its team members' deaths. The more
deaths that occur in a single area them more important it is to defend that
area. This is a simple little home-brewed, half-baked clustering algorithm.
Whenever a team-mate dies, a cluster is sought to "explain" that death (a 
cluster simply being a circular area of land defined by a center and a 
radius). If the death location lies inside a pre-existing cluster, then that
cluster's radius is shrunk slightly. If the death location does NOT lie
within an existing cluster, a new cluster is created.

This clustering system is entirely internal to the defense tactician, so 
no one else knows about it. For each cluster a corresponding DEFEND_LOCATION
mission is posted. The priority of the mission is inversely proportional to
the cluster's radius. i.e. a small tight cluster will be very high priority.
The cluster's radius also grows gradually over time, and the cluster will
eventually be removed if no deaths have occured there for a long time
(indicating that the cluster no longer represents a strategically
important area).

Interestingly, the defense tactician is an important unit, but it also has a 
physical body. So if one team manages to kill the other's defense tactician, 
then all this cluster-based tactics is lost and the army begins to act
stupidly.

@author naimad
*/

public class DefenseTactician extends Agent implements EventNotifier {

    public final static int COST = 500;

    public DefenseTactician(int team, Vec2 pos, Blackboard bb, World world) {
        super(team, 5, pos, 5.0, 20.0, 10000.0, bb, world);
        
        world.addEventNotifier(team, this);

        defaultSkill = null;
    }

	/*
	Update the clusters.
	*/
    public void actionExecute(double time) {
        for (int c=0; c < clusters.size();c++) {
            Cluster cluster = (Cluster)clusters.elementAt(c);
            cluster.update(time);
            if (cluster.weight < 0.1) {
                cluster.remove();
                clusters.removeElement(cluster);
                c--;
            }
        }
    }
    
    Shape clusterShape;
    public Shape createShape() {
        clusterShape = new Ellipse2D.Double(-50.0, -50.0, 100.0, 100.0);
        return new Ellipse2D.Double(-5.0, -5.0, 10.0, 10.0);
    }

    public void updateGraphics(double time, Graphics2D g2) {
        g2.translate(pos.x(), pos.y());
        drawShape(time, g2);
        g2.translate(-pos.x(), -pos.y());
        
        //Optionally, draw each of my clusters...
        /*
        for (int c=0; c < clusters.size(); c++) {
            Cluster cluster = (Cluster)clusters.elementAt(c);
            g2.translate(cluster.center.x(), cluster.center.y());
        	((Ellipse2D.Double)clusterShape).setFrame(-cluster.radius, -cluster.radius, 2*cluster.radius, 2*cluster.radius);
            g2.draw(clusterShape);
            g2.translate(-cluster.center.x(), -cluster.center.y());
        }
        */
    }
    
    //-------------------EventNotification Interface-------------------
    
    Vector clusters = new Vector();
    
	//The call-back we get when a fellow team-mate is killed.
    public void deathNotification(Agent a, Vec2 location) {
        boolean clusterFound = false;
        for (int c=0; c < clusters.size(); c++) {
            Cluster cluster = (Cluster)clusters.elementAt(c);
            if (cluster.inside(location)) {
                clusterFound = true;
                cluster.addExample(location);   
            }
        }
        if (!clusterFound) {
            Cluster newc = new Cluster(location);
            newc.addExample(location);   
            clusters.addElement(newc);
        }
    }

    //The convenience class representing a cluster.
    
    class Cluster {
        double radius = 50.0;
        Vec2 center;
        double weight = -0.5;
        double fconst = 0.9;
        
        LocatableMission mission;
        
        public Cluster(Vec2 center) {
            this.center = new Vec2(center);
            mission = new LocatableMission(Commander.DefendLocationSkill.DEFENDLOCATION, 1, 0, center);
            bb.addMission(mission);
        }
        
        public boolean inside(Vec2 loc) {
            return (loc.distance(center) < radius);
        }
        
        public void update(double time) {
            weight *= 0.98;
            radius = 200.0/(2.5+weight);
        }
        
        Vec2 temp = new Vec2();
        public void addExample(Vec2 location) {
            weight+=1.0;
            center.scale(fconst);
            temp.set(location);
            temp.scale(1-fconst);
            Vec2.add(center, temp, center);
            
            mission.setPriority(weight);
            mission.setTargetPosition(center);
        }
        
        public void remove() {
            mission.setMissionComplete();
            bb.removeMission(mission);
        }
    }
}
