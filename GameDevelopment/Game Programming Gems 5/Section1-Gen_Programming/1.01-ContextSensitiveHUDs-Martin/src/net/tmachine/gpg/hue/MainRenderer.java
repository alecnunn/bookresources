package net.tmachine.gpg.hue;

import java.awt.*;
import java.util.*;

import javax.swing.*;

import sun.net.www.http.*;

/**
 * A very simple main renderer that simply grabs a paint surface on every repaint (managed
 * by Sun's windowing system, "Swing") and gets each <b>visible</b> HUD to paint to that
 * surface in turn.
 */
public class MainRenderer extends JComponent
{
	protected HUDManager HUDManager;
	
	/**
	 * Creates a renderer; you MUST call setManager() afterwards before using
	 * the renderer, otherwise it won't be able to get hold of any HUDs to render!
	 */
	public MainRenderer()
	{
	}
	
	public void setManager( HUDManager am )
	{
		HUDManager = am;
	}
	
	protected void paintComponent( Graphics g )
	{
		if( HUDManager == null )
		{
			System.out.println( "HUDManager is currently null; cannot start painting yet; paint ignored");
			return;
		}
		
		g.setColor( Color.black );
		g.fillRect( 0, 0, getWidth(), getHeight());
		
		ArrayList huds = HUDManager.getVisibleHUDs();
		for( Iterator iter = huds.iterator(); iter.hasNext(); )
		{
			HUD element = (HUD) iter.next();
			element.paint(g);
		}
	}
}