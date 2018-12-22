//---------------------------------------------------------
//
// File: Worm.java
//
// Purpose: Defines the individual worm object
//
// Copyright (C) 1996 Roman Mach
//
// mach@halcyon.com
//
//---------------------------------------------------------

import java.awt.*;

// we define the behavior of the worm here
// there is no graphics information stored by the worm object
//

public class Worm {
    Dimension m_points[];
    int m_startidx, m_endidx;
    int m_curridx;
    int m_startx, m_starty;
    int m_wormsize = 4;
    int m_wormlen = 5;
    int m_startxdir, m_startydir;
    int m_regrow = 0;
    boolean m_nextstop = false;
    boolean m_lastdropped = false;
    boolean m_nextadded = false;
    Dimension dDir;
    Dimension dCurr;
    Dimension dNext;
    Dimension dNewpoint;
    Dimension dOldpoint;
    Color wormcolor;

    // first time initialization
    //
    public Worm(   int startx,
                   int starty,
                   int wormlen,
                   Color wclr,
                   int wormsize,
                   int initx,
                   int inity)
    {
        wormcolor = wclr;
        m_wormsize = wormsize;
        m_wormlen = wormlen;
        m_startxdir = initx;
        m_startydir = inity;

        // set up storage array for worm's description

        m_points = new Dimension[m_wormsize];
        for (int i = 0; i < m_points.length; i++)
            m_points[i] = new Dimension();

        // save start location for worm
        m_startx = startx;
        m_starty = starty;

        // helper dimensions
        dDir = new Dimension();
        dCurr = new Dimension();
        dNext = new Dimension();
        dNewpoint = new Dimension();
        dOldpoint = new Dimension();
    }

    // called each time you want to restart the worm
    //
    public void ReviveWorm()
    {
        int count, i;

        for (i = 0, count = 0; i < m_wormlen; i++, count++)  {
            m_points[count].width = i + m_startx;
            m_points[count].height = m_starty;
        }
        // set the buffer's start and end points
        // which are the head and tail of the worm

        m_startidx = count - 1;
        m_endidx = 0;
        m_regrow = 0;

        // set the current travelling direction

        dDir.width = m_startxdir;
        dDir.height = m_startydir;
    }

    // checks if worm has a segment at the given point
    //
    public boolean DoYouHaveSegmentAt(Dimension newpoint)
    {
        int curridx = m_endidx;

        while (true) {
            // if matches, return true
            if (newpoint.width == m_points[curridx].width &&
                newpoint.height == m_points[curridx].height)
                return true;

            if (curridx == m_startidx)
                break;
            curridx++;
            if (curridx >= m_wormsize)
                curridx = 0;
        }
        // no match return false
        return false;
    }

    //
    //
    private int CountSegments()
    {
        int curridx = m_endidx;
        int count = 1;

        while (curridx != m_startidx) {
            curridx++;
            count++;
            if (curridx >= m_wormsize)
                curridx = 0;
        }
        return count;
    }

    // pass in the coordinate of the current mouse click
    // and adjust worm direction appropriately
    //
    public void SetNewDirectionFromMouse(   int wormscale,
                                            int x,
                                            int y)
    {
        // width is 0 if we are moving up or down
        if (dDir.width == 0) {
            if (x > m_points[m_startidx].width * wormscale)
                dDir.width = 1;// move right
            else
                dDir.width = -1;// move left
            dDir.height = 0;
        }
        else {
            if (y > m_points[m_startidx].height * wormscale)
                dDir.height = 1;// move down
            else
                dDir.height = -1;// move up
            dDir.width = 0;
        }
    }

    // do same for keyboard, easier calculation
    //
    public void SetNewDirectionFromKeyboard(int key)
    {
        // 1004=up 1005=down 1006=left 1007=right
        // use arrow keys or normal keys (keeps everyone happy)

        if (key == 1004 || key =='u') {
            if (dDir.height == 0) {
                dDir.width = 0;
                dDir.height = -1;
            }
        }
        else if (key == 1005 || key =='j') {
            if (dDir.height == 0) {
                dDir.width = 0;
                dDir.height = 1;
            }
        }
        else if (key == 1006 || key =='h') {
            if (dDir.width == 0) {
                dDir.width = -1;
                dDir.height = 0;
            }
        }
        else if (key == 1007 || key =='k') {
            if (dDir.width == 0) {
                dDir.width = 1;
                dDir.height = 0;
            }
        }
    }

    // for auto worm
    //
    public void SetDirection(int x, int y)
    {
        dDir.width = x;
        dDir.height = y;
    }

    // for auto worm
    //
    public Dimension GetDirection()
    {
        return dDir;
    }

    // for auto worm
    public boolean NotRegrowing()
    {
        if (m_regrow > 0) {
            m_regrow--;
            if ((m_regrow < 100) && ((m_regrow & 7) == 0))
                return false;
        }
        return true;
    }

    // what is the worm's next point based on it's
    // current direction this is used to determine if
    // we will have a collision/treat
    //
    public Dimension GetNextPoint()
    {
        // wrong: dNext = m_points[m_startidx];

        dNext.width = m_points[m_startidx].width;
        dNext.height = m_points[m_startidx].height;

        dNext.width += dDir.width;
        dNext.height += dDir.height;

        m_nextadded = false;

        return dNext;
    }

    // We have found a valid point, so set the worm's
    // head to the new point If we have grown to our
    // maximum, force a drop
    //
    public boolean SetNextPoint(Dimension newpoint)
    {
        int tempidx;

        m_startidx++;
        if (m_startidx >= m_wormsize)
            m_startidx = 0;

        // wrong: m_points[m_startidx] = newpoint;

        m_points[m_startidx].width = newpoint.width;
        dNewpoint.width = newpoint.width;
        m_points[m_startidx].height = newpoint.height;
        dNewpoint.height = newpoint.height;

        // don't grown beyond max,
        // force a drop by calling routine
        tempidx = m_startidx + 1;
        if (tempidx >= m_wormsize)
            tempidx = 0;

        m_nextadded = true;

        return tempidx == m_endidx;
    }

    // We are done with the tail of this worm,
    // drop last segment
    //
    // return false if we can't drop last point
    //
    public boolean DropLastPoint()
    {
        // watch variable reuse!
        // this is wrong: dNext = m_points[m_endidx];
        // this a also bad: dOldpoint = m_points[m_endidx];

        dOldpoint.width = m_points[m_endidx].width;
        dOldpoint.height = m_points[m_endidx].height;

        if (m_regrow == 0)
            if (CountSegments() <= m_wormlen / 2)
                m_regrow = 150;

        if (m_endidx == m_startidx) {
            m_lastdropped = false;
            return m_lastdropped;
        }

        m_endidx++;
        if (m_endidx >= m_wormsize)
            m_endidx = 0;

        m_lastdropped = true;
        return m_lastdropped;
    }

    //
    //
    public boolean LastPointDropped()
    {
        boolean temp = m_lastdropped;
        m_lastdropped = false;
        return temp;
    }

    //
    //
    public boolean NextPointAdded()
    {
        boolean temp = m_nextadded;
        m_nextadded = false;
        return temp;
    }

    //
    //
    public Dimension GetAddedPoint()
    {
        return dNewpoint;
    }

    //
    //
    public Dimension GetDroppedPoint()
    {
        return dOldpoint;
    }

    // This is called to start iterating through
    // the worm's points
    //
    public boolean IsWormFirst()
    {
        m_curridx = m_endidx;
        m_nextstop = false;
        dCurr = m_points[m_curridx];
        return GetWormElement();
    }

    // This is called to get the next points
    // If it returns false, we are done, otherwise call
    // GetCurrentWormSegment() to get the location
    // of this segment
    //
    public boolean IsWormNext()
    {
        return GetWormElement();
    }

    // returns the current segment in the iterative loop above
    //
    public Dimension GetCurrentWormSegment()
    {
        return dCurr;
    }

    // returns the current color of this worm for the
    // iterative loop above
    //
    public Color GetCurrentWormColor()
    {
        return wormcolor;
    }

    // private function to iterate over all worm segments
    //
    private boolean GetWormElement()
    {
        // since m_curridx is a valid index, we have to
        // return false one call after we reach the head index
        if (m_nextstop)
            return false;

        // if at head index, the next call says we are done
        if (m_curridx == m_startidx)
            m_nextstop = true;

        // get current index
        dCurr = m_points[m_curridx++];
        if (m_curridx >= m_wormsize)
            m_curridx = 0;

        return true;
    }

}
