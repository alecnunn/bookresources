package bb;

/*
A basic 2-dimensional vector class.

@author naimad
*/

public class Vec2 {

    double[] v = new double[2];
    
    public Vec2() {
        v[0] = 0.0;
        v[1] = 0.0;
    }
    
    public Vec2(double x, double y) {
        v[0] = x;
        v[1] = y;
    }
    
    public Vec2(double[] xy) {
        v[0] = xy[0];
        v[1] = xy[1];
    }
    
    public Vec2(Vec2 vv) {
        v[0] = vv.x();
        v[1] = vv.y();
    }
    
    public double x() {
        return v[0];
    }
    
    public double y() {
        return v[1];
    }
    
    public void x(double xx) {
        v[0] = xx;
    }
    
    public void y(double yy) {
        v[1] = yy;
    }
    
    public void set(double x, double y) {
        v[0] = x;
        v[1] = y;
    }

    public void set(double[] vv) {
        v[0] = vv[0];
        v[1] = vv[1];
    }

    public void set(Vec2 vv) {
        v[0] = vv.x();
        v[1] = vv.y();
    }
    
    public static void add(Vec2 v1, Vec2 v2, Vec2 vout) {
        vout.x(v1.x()+v2.x());
        vout.y(v1.y()+v2.y());
    }
    
    public static void sub(Vec2 v1, Vec2 v2, Vec2 vout) {
        vout.x(v1.x()-v2.x());
        vout.y(v1.y()-v2.y());
    }
    
    public void scale(double s) {
        v[0]*=s;
        v[1]*=s;
    }
    
    public double mag() {
        return Math.sqrt(v[0]*v[0] + v[1]*v[1]);
    }
    
    public void normalize() {
        scale(1.0/mag());
    }
    
    public double dot(Vec2 vv) {
        return v[0]*vv.x() + v[1]*vv.y();
    }
    
    public double distance(Vec2 vec) {
        return Math.sqrt(distanceSquared(vec));
    }

    public double distanceSquared(Vec2 vec) {
        double v1 = vec.x()-v[0];
        double v2 = vec.y()-v[1];
        return v1*v1 + v2*v2;
    }
    
    public Object clone() {
        return new Vec2(this);
    }
    
    public String toString() {
        return "["+v[0]+", " + v[1] + "]";
    }

}
