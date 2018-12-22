// A first example using a rectangle object in java
// Chapter 23 ( online)


public class Rectangle //extends Object
{
private double width;
private double length;
public Rectangle ( ){
width = 0;
length = 0;
}
public Rectangle ( double wid,double len){
width = wid;
length = len;
}
public double Perimeter ( ) {
return 2 *width + 2 * length;
}

public double Area ( ) {
return width * length;
}

public void Expand (int factor){
width = factor * width;
length = factor * length;
}

public static void main (String args[ ])
{ int x = 14;
int y = 7;

Rectangle r = new Rectangle (x,y);
System.out.println(r.Perimeter());
System.out.println (r.Area());
r.Expand(2);
System.out.println ("The area has quadrupled to : " + r.Area());
}
}//ends class


