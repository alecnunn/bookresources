package net.tmachine.gpg.hue;

import java.awt.*;
import java.awt.event.*;

/**
 * Provides most of the common methods etc for HUD implementations
 * 
 * Subclasses should set the "name" variable in their constructor.
 */
public abstract class BaseHUD implements HUD
{
	protected String name = "UNNAMED HUD";
	protected boolean isVisible;
	protected boolean isActive;
	protected HUDManager manager;
	
	public void setManager( HUDManager am )
	{
		manager = am;
	}
	
	/**
	 * @return the value of the <code>name</code> variable
	 */
	public String getName()
	{
		return name;
	}

	public void isVisible( boolean b )
	{
		isVisible = b;
	}

	public boolean isVisible()
	{
		return isVisible;
	}
	
	public void isActive( boolean b )
	{
		isActive = b;
	}
	
	public boolean isActive()
	{
		return isActive;
	}
	
	public void mouseClicked( MouseEvent e )
	{
	}

	public void mouseEntered( MouseEvent e )
	{
	}

	public void mouseExited( MouseEvent e )
	{
	}

	public void mousePressed( MouseEvent e )
	{
	}

	public void mouseReleased( MouseEvent e )
	{
	}

	public void mouseDragged( MouseEvent e )
	{
	}

	public void mouseMoved( MouseEvent e )
	{
	}
	
	public String toString()
	{
		return getName();
	}
}