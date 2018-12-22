package net.tmachine.gpg.hue;

import java.awt.*;
import java.awt.event.*;

/**
 * A simple HUD that, when active, draws a crosshair and responds to drag / click+move events
 * by drawing a new Rectangle, and when the mouse is released it creates a new rectangle
 * of that size + position and adds it to the base document
 */
public class ShapeCreator extends BaseHUD
{
	/** co-ords where the current create-shape gesture was started */
	protected int sx, sy;
	
	/** indicates that we are in the middle of a drag action that is creating a shape */
	protected boolean creatingShape = false;
	
	public ShapeCreator()
	{
		name = "Creator";
	}
	
	public void paint( Graphics g )
	{
		int[] cursor = manager.getContext().getCursorPosition();
		int w = cursor[0] - sx;
		int h = cursor[1] - sy;
		int [] dimensions = getDimensions( sx, sy, w, h );
		int topLeftX = dimensions[0];
		int topLeftY = dimensions[1];
		w = dimensions[2];
		h = dimensions[3];
		
		g.setColor( Color.white );
		if( creatingShape )
			g.drawRect( topLeftX, topLeftY, w, h );
		else if( isActive() ) // only draw the crosshair if this HUD is active
		{
			g.drawLine( cursor[0], 0, cursor[0], manager.display.getHeight() );
			g.drawLine( 0, cursor[1], manager.display.getWidth(), cursor[1] );
		}
	}
	
	private int[] getDimensions( int x, int y, int w, int h )
	{
		if( w < 1 )
		{
			x += w;
			w *= -1;
		}
		if( h < 1 )
		{
			y += h;
			h *= -1;
		}
		
		return new int[] { x, y, w, h };
	}
	
	public void mousePressed( MouseEvent e )
	{
		manager.setExclusiveActiveHUD( this );
		
		sx = e.getX();
		sy = e.getY();
		creatingShape = true;
	}
	
	public void mouseReleased( MouseEvent e )
	{
		creatingShape = false;
		
		int[] cursor = manager.getContext().getCursorPosition();
		int w = cursor[0] - sx;
		int h = cursor[1] - sy;
		int [] dimensions = getDimensions( sx, sy, w, h );
		int topLeftX = dimensions[0];
		int topLeftY = dimensions[1];
		w = dimensions[2];
		h = dimensions[3];
		
		manager.getContext().getDocument().addShape( new Rectangle( topLeftX, topLeftY, w, h ) );
		
		manager.exitExclusiveActiveHUD();
	}
}
