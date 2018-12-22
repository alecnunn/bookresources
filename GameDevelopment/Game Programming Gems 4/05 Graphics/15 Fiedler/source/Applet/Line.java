//
// Simple Line Class (k=gradient,c=y-intercept)
// Glenn "Gaffer" Fiedler
//

public class Line
{
    public Line(float k, float c)
    {
        this.k = k;
        this.c = c;
    }
    
    public float evaluate(float x)
    {
        return x*k + c;
    }

    public void intersect(float x, float y)
    {
        c = y - k*x;
    }                  
    
    public Line copy()
    {
        return new Line(k,c);
    }
    
    public float k;
    public float c;
}
