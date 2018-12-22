//
// Canvas Class for Applet
// Glenn "Gaffer" Fiedler
//

import java.awt.*;
import java.util.*;
import java.awt.event.*;

public class LeastSquaresLineCanvas extends Canvas implements MouseListener
{
    Image offscreenImage;           // double buffered rendering
    Graphics offscreenGraphics;
    Graphics2D offscreenGraphics2D;

	long seed = 1067;

    Random random = new Random();

    final int border = 20;          // screen border when rendering
    
    final int samples = 50;         // number of samples to generate
    
    final float variance = 0.25f;   // for generating sample points with midpoint displacement

    Point[] points;                 // points in x,y coordinates

    float error;                    // the error threshold for node rendering
    
    Node root;                      // root of node tree

	boolean sloped = true;			// sloped or horizontal lines

    public LeastSquaresLineCanvas()
    {
        addMouseListener(this);

        build();
    }
    
    void build()
    {
		// update seed

		random.setSeed(seed++);

		// generate random sample points

	    final int width = getWidth();
	    final int height = getHeight();

		for (int i=0; i<1000; i++)
		{
			float f = randomFloat(height/4.0f,height/4.0f*3);
		}

		float x = border;
        float dx = (width-border*2) / (float)(samples-1);
        
        points = new Point[samples];

        for (int i=0; i<samples; i++)
        {
            points[i] = new Point(x, randomFloat(height/4.0f,height/4.0f*3));
            x += dx;
        }

		// generate basic fractal terrain
        
        recurseSample(0, samples-1);
        
        // recursively build tree of nodes
        
        root = new Node(points, 0, samples-1, sloped);
        
        // update vis
        
        repaint();
    }

	void toggleSloped()
	{
		sloped = !sloped;

		// recursively build tree of nodes
        
		root = new Node(points, 0, samples-1, sloped);
        
		// update vis
        
		repaint();
	}

    void recurseSample(int a, int b)
    {
	    final int width = getWidth();
	    final int height = getHeight();
	    
        int c = (b + a) / 2;
        
        if (a==c || b==c) return;
        
        float dx = points[b].x - points[a].x;
        float cy = (points[b].y + points[a].y) / 2;
            
        points[c].y = cy + randomFloat(-variance*dx, variance*dx);
        if (points[c].y<border) points[c].y = border;
        else if (points[c].y>height-border) points[c].y = height - border;
        
        recurseSample(a,c);
        recurseSample(c,b);
    }
    
    public void setErrorThreshold(float error)
    {
        this.error = error;
        repaint();
    }

    public void update(Graphics graphics)
    {
        // overriding update stops it clearing the background to white before painting
        
        paint(graphics);
    }

    public void paint(Graphics graphics) 
    {
        // create offscreen buffer on first paint
    
        if (offscreenImage==null) 
        {
            offscreenImage = createImage(getWidth(), getHeight());
            offscreenGraphics = offscreenImage.getGraphics();
            offscreenGraphics2D = (Graphics2D) offscreenGraphics;
            offscreenGraphics2D.translate(0,getHeight());
            offscreenGraphics2D.scale(1,-1);
        }

        // render to offscreen buffer
        
        render(offscreenGraphics2D);

        // copy offscreen buffer to screen
        
        graphics.drawImage(offscreenImage,0,0,this);
	}

    public void render(Graphics2D graphics)
    {
        // enable anti-aliasing
    
	    graphics.setRenderingHint(RenderingHints.KEY_ANTIALIASING,RenderingHints.VALUE_ANTIALIAS_ON);
	    
        // white background

	    final int width = getWidth();
	    final int height = getHeight();
    
        graphics.setColor(Color.white);
        graphics.fillRect(0, 0, width, height);
        
		// render nodes recursively from root
        
        root.render(graphics, width, height, error);
	}

	int randomInteger(int min, int max)
	{
	    return (int) (min + random.nextDouble() * (max-min));
	}

	float randomFloat(float min, float max)
	{
	    return (float) (min + random.nextDouble() * (max-min));
	}
	
	public int getWidth()
	{
	    return 1000;
	}
	
	public int getHeight()
	{
	    return 590;
	}

    public void mouseClicked(MouseEvent event)
    {
        // rebuild sample points and node tree on left mouse click, toggle sloped lines on right click
        
		if (event.getButton()==1)
			build(); 
		else
			toggleSloped();
    }

    public void mouseEntered(MouseEvent event)
    {
    }
    
    public void mouseExited(MouseEvent event) 
    {
    }

    public void mousePressed(MouseEvent event)
    {
    }

    public void mouseReleased(MouseEvent event)
    {
    }
}
