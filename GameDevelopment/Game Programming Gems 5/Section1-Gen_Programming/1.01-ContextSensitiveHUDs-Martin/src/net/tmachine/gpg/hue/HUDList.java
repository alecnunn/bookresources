package net.tmachine.gpg.hue;

import java.awt.*;
import java.awt.event.*;

import javax.naming.*;
import javax.swing.*;

import com.sun.rsasign.*;

/**
 * Useful control that allows the user to hide/show any layer and also to activate / de-activate
 * any layer; also displays name and status of all layers, and shows their paint order
 * <P>
 * Note: HUD at top of the list is painted first, HUD at bottom is painted last, i.e this class
 * displays a list in "paint order" rather than logical order (which would put the last-painted
 * layer "highest" i.e at the top of the list)
 */
public class HUDList extends JComponent implements MouseListener//extends JList
{
	protected HUDManager hudManager;
	protected ListModel list;
	
	protected int visibilityColumnWidth = 15;
	protected int activeColumnWidth = 15;
	protected int rowHeight = 20;
	protected int titleHeight = 20;
	
	/**
	 * Requires a HUD manager so that it can retrieve the list of HUDs!
	 * 
	 * @param am
	 */
	public HUDList( HUDManager am )
	{
		hudManager = am;
		ListModel lm = new AbstractListModel()
		{
			public int getSize()
			{
				return hudManager.getAllHUDs().size();
			}
			
			public Object getElementAt( int index )
			{
				return hudManager.getAllHUDs().get( index );
			}
		};
		
		setModel(lm);
		
		addMouseListener(this );
	}
	
	/**
	 * Internal use; uses the ListModel for easy compatibilty with Swing
	 * 
	 * @param lm
	 */
	public void setModel( ListModel lm )
	{
		list = lm;
	}
	
	/**
	 * All painting takes place here; this component uses no widgets, and does everything
	 * by hand
	 */
	protected void paintComponent( Graphics g )
	{
		int fontHeight = g.getFontMetrics().getHeight();
		FontMetrics metrics = g.getFontMetrics();
		
		// Draw the title
		g.setColor( Color.blue );
		g.drawString( "V", (visibilityColumnWidth - metrics.stringWidth("V") )/2, fontHeight );
		g.drawString( "A", visibilityColumnWidth + (activeColumnWidth - metrics.stringWidth("A") )/2, fontHeight );
		g.drawString( "HUD Name", visibilityColumnWidth+activeColumnWidth, fontHeight );
		 
		// Draw the indivudual HUDs
		for( int i = 0; i < list.getSize(); i++ )
		{
			HUD hud = (HUD) list.getElementAt(i);
			g.setColor( Color.black );
			g.drawString( hud.getName(), visibilityColumnWidth+activeColumnWidth, i * rowHeight + fontHeight + titleHeight );
			
			int x = 1;
			int y = 1 + i*rowHeight + titleHeight;
			int w = visibilityColumnWidth-2;
			int h = visibilityColumnWidth-2;
			
			if( hud.isActive() )
				g.setColor( new Color( 0, 128, 0 ) );
			
			//			 NB: "features" in Sun's 2D API mean that a drawed circle is 2 pixels larger/wider
			//    than a filled one, so we have to shrink it if it's drawn rather than filled
			if( hud.isVisible() )
				g.fillOval( x, y, w, h );
			else
				g.drawOval( x+1, y+1, w-2, h-2 );
			
			x = visibilityColumnWidth + 1;
			w = activeColumnWidth - 2;
			h = w;
			
			// NB: "features" in Sun's 2D API mean that a drawed circle is 2 pixels larger/wider
			//    than a filled one, so we have to shrink it if it's drawn rather than filled
			if( hud.isActive() )
				g.fillOval( x, y, w, h );
			else
				g.drawOval( x+1, y+1, w-2, h-2 );
		}
		
		g.setColor( Color.gray );
		g.drawLine(visibilityColumnWidth, 0, visibilityColumnWidth, getHeight() );
		g.drawLine(visibilityColumnWidth+activeColumnWidth, 0, visibilityColumnWidth+activeColumnWidth, getHeight() );
		g.drawLine(0, titleHeight, getWidth(), titleHeight);
	}
	
	/**
	 * Sun's Swing API does automatic dynamic sized layout - it needs this method to tell it
	 * how much space to allocate to this particular GUI element (the HUDList)
	 */
	public Dimension getMinimumSize()
	{
		Dimension result = new Dimension();
		int width = 0;
		Font font = getGraphics().getFont();
		FontMetrics metrics = getGraphics().getFontMetrics();
		for( int i = 0; i < list.getSize(); i++ )
		{
			HUD hud = (HUD) list.getElementAt(i);
			if( width < metrics.stringWidth( hud.getName()))
				width = metrics.stringWidth( hud.getName() );
		}
		result.width = width + visibilityColumnWidth+activeColumnWidth;
		result.height = list.getSize() * metrics.getHeight() + titleHeight;
		
		return result;
	}
	
	/**
	 * @return getMinimumSize()
	 */
	public Dimension getPreferredSize()
	{
		return getMinimumSize();
	}
	
	/**
	 * @return getMinimumSize()
	 */
	public Dimension getMaximumSize()
	{
		return getMinimumSize();
	}
	
	/**
	 * Logic for either showing/hiding a HUD or activating/de-activating, depending upon which
	 * circle/icon was clicked
	 */
	public void mouseClicked( MouseEvent e )
	{
		Font font = getGraphics().getFont();
		FontMetrics metrics = getGraphics().getFontMetrics();
		
		System.out.println( "(x,y) = ("+e.getX()+", "+e.getY()+"), metrics.getHeight = "+metrics.getHeight()+", visibv = "+visibilityColumnWidth );
		int elementRow = (e.getY() - titleHeight) / rowHeight;
		
		System.out.println ("row of selected mosue click = "+elementRow);
		
		if( e.getX() < visibilityColumnWidth )
		{
			HUD hud = (HUD) list.getElementAt(elementRow);
			
			if( hud.isVisible() )
				hudManager.hideHUD( hud );
			else
				hudManager.showHUD( hud );
			
			hudManager.repaint();
		}
		else if( e.getX() < (visibilityColumnWidth + activeColumnWidth ))
		{
			HUD hud = (HUD) list.getElementAt(elementRow);
			
			if( hud.isActive() )
				hudManager.deActivateHUD( hud );
			else
				hudManager.activateHUD( hud );
			
			hudManager.repaint();
		}
		
		repaint();
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
}