//
// Least Squares Line Applet
// Glenn "Gaffer" Fiedler
//

import java.awt.*;
import java.applet.*;
import java.util.*;
import java.awt.event.*;


public class LeastSquaresLine extends Applet implements AdjustmentListener
{
    LeastSquaresLineCanvas canvas;    
    
    Scrollbar scrollbar;

    public void init()
    {  
        setLayout(new BorderLayout());
    
        int visible = 50;
        int maximum = 350;
        scrollbar = new Scrollbar(Scrollbar.HORIZONTAL, maximum, visible, 0, maximum+visible);
        scrollbar.addAdjustmentListener(this);
        add("South", scrollbar);

        canvas = new LeastSquaresLineCanvas();
        add("Center", canvas);

        canvas.setErrorThreshold(scrollbar.getValue());
    }

    public void adjustmentValueChanged(AdjustmentEvent e)
    {        
        canvas.setErrorThreshold(scrollbar.getValue());
    }
}
