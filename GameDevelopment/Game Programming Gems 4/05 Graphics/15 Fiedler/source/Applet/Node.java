//
// Node class
// Glenn "Gaffer" Fiedler
//

import java.awt.*;
import java.util.*;

public class Node
{
    Node left;                      // binary tree child nodes
    Node right;

    int a;                          // left sample boundary
    int b;                          // right sample boundary
    int size;                       // size of the sample block this node is concerned with (size = b-a+1 because its [a,b] inclusive)

    Point points[];                 // points array containing all samples (same array shared across all nodes) [0,samples)
    Point rotated_points[];         // local array of rotated points in least squares line space (owned by this node) [0,size) corresponds to [a,b] in points

    Line average_line;				// average line fitting sample points in the range [a,b]
    Line maximum_line;              // maximum line bounding sample points in the range [a,b]
    Line minimum_line;              // minimum line bounding sample points in the range [a,b]
    
    float total_error;              // total error = sum of differences between min/max lines for range [a,b]
    float average_error;            // average error = total error / size

    public Node(Point points[], int a, int b, boolean sloped)
    {
        // initialize
    
        this.a = a;
        this.b = b;
        this.points = points;

        // subdivide and recurse nodes

        size = b-a + 1;
        
        if (size>2)
        {
            int c = (a+b) / 2;
            left = new Node(points, a, c, sloped);
            right = new Node(points, c, b, sloped);
        }
        
        // calculate average line
       
		if (sloped)
			average_line = leastSquaresLine(points, a, b);
		else
			average_line = horizontalLine(points, a, b);

        // we choose min/max lines parallel to the average line
        
        minimum_line = average_line.copy();
        maximum_line = average_line.copy();

        // enforce minimum and maximum line constraints
        
        for (int i=a; i<=b; i++)
        {
            final float x = points[i].x;
            final float y = points[i].y;
            
            if (maximum_line.evaluate(x)<y) maximum_line.intersect(x,y);
            if (minimum_line.evaluate(x)>y) minimum_line.intersect(x,y);
        }
        
        // calculate total and average error for this node
        
        total_error = 0;
        
        for (int i=a; i<=b; i++)
        {
            float x = points[i].x;

            float ymax = maximum_line.evaluate(x);
            float ymin = minimum_line.evaluate(x);
            total_error += ymax - ymin;
        }
        
        average_error = total_error / size;
    }

    public Line leastSquaresLine(Point points[], int a, int b)
    {
        // calculate least squares line
        
        float a00 = 0;
        for (int i=a; i<=b; i++)
            a00 += points[i].x * points[i].x;
            
        float a01 = 0;
        for (int i=a; i<=b; i++)
            a01 += points[i].x;
            
        float a10 = a01;          // A is a symmetric 2 x 2 matrix
        
        float a11 = b - a + 1;
		
        float b0 = 0;
        for (int i=a; i<=b; i++)
            b0 += points[i].x * points[i].y;
        
        float b1 = 0;
        for (int i=a; i<=b; i++)
            b1 += points[i].y;
        
        float k = (b0*a11-a01*b1) / (a00*a11-a01*a10);          // cramers rule for Ax = b
        float c = (a00*b1-b0*a10) / (a00*a11-a01*a10);
		
		return new Line(k,c);
    }

	public Line horizontalLine(Point points[], int a, int b)
	{
		// calculate horizontal line

		float k = 0;
		
		float c = 0;
		for (int i=a; i<=b; i++)
			c += points[i].y;
		
		c *= 1.0f / ((b-a)+1);
		
		return new Line(k,c);
	}
    
    public void render(Graphics2D graphics, int width, int height, float error)
    {
        // recurse until we satisfy error threshold
        
        if (average_error>error && left!=null && right!=null)
        {
            left.render(graphics, width, height, error);
            right.render(graphics, width, height, error);
            return;
        }
    
        // calculate x range
        
        final float ax = points[a].x;
        final float bx = points[b].x;
    
        // draw error volume

        drawQuad(graphics, ax, maximum_line.evaluate(ax), bx, maximum_line.evaluate(bx), bx, minimum_line.evaluate(bx), ax, minimum_line.evaluate(ax), new Color(0.85f, 0.85f, 0.85f));
        
        // average line

		Color lineColor = new Color(0.35f, 0.35f, 0.35f);

        drawLine(graphics, ax, average_line.evaluate(ax), bx, average_line.evaluate(bx), lineColor);

        // minimum line

        drawLine(graphics, ax, minimum_line.evaluate(ax), bx, minimum_line.evaluate(bx), lineColor);

        // maximum line

        drawLine(graphics, ax, maximum_line.evaluate(ax), bx, maximum_line.evaluate(bx), lineColor);

        // draw x range boundary
        
        drawLine(graphics, ax, 0, ax, 10000, lineColor);
        drawLine(graphics, bx, 0, bx, 10000, lineColor);

        // sample points

        for (int i=0; i<size; i++)
        {
            final int j = i + a;
        
            drawPoint(graphics, points[j].x, points[j].y, Color.black);
        }
    }
    
    void drawPoint(Graphics2D graphics, float x, float y, Color color)
    {
        final int radius = 10;
        graphics.setColor(color);
        graphics.fillOval((int)(x-radius/2), (int)(y-radius/2), radius, radius);
    }
    
    void drawLine(Graphics2D graphics, float x1, float y1, float x2, float y2, Color color)
    {
        drawLine(graphics, x1, y1, x2, y2, color, 1);
    }

	void drawLine(Graphics2D graphics, float x1, float y1, float x2, float y2, Color color, int width)
	{
        graphics.setStroke(new BasicStroke(width));
	    graphics.setColor(color);
		graphics.drawLine((int)x1, (int)(y1), (int)x2, (int)(y2));
	}
	
	void drawQuad(Graphics2D graphics, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, Color color)
	{
	    Polygon polygon = new Polygon();
	    polygon.addPoint((int)x1, (int)(y1));
	    polygon.addPoint((int)x2, (int)(y2));
	    polygon.addPoint((int)x3, (int)(y3));
	    polygon.addPoint((int)x4, (int)(y4));
	    graphics.setColor(color);
	    graphics.fill(polygon);
	}
	
	Line getAverageLine()
	{
	    return average_line;
	}
	
	Line getMaximumLine()
	{
	    return maximum_line;
	}
	
	Line getMinimumLine()
	{
	    return minimum_line;
	}

	float x1()
	{
	    return points[a].x;
	}
	
	float x2()
	{
	    return points[b].x;
	}
}
