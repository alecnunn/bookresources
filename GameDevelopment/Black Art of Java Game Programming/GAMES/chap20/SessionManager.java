//---------------------------------------------------------
//
// File: SessionManager.java
//
// Purpose: Manages object on the playing surface
//
// Copyright (C) 1996 Roman Mach
//
// mach@halcyon.com
//
//---------------------------------------------------------

import java.awt.*;

// we manage the session from this object.
// It controls how one or more worms interact with the
// logical working area. No reference to a
// a graphical workspace is used in this object
//
public class SessionManager {
    static int SIZEX = 30;
    static int SIZEY = 30;
    static int WORMCOUNT = 3;

    boolean m_newtr = false;
    boolean m_showscore = false, m_dontdrop = false;
    boolean m_speedup = false;

    Dimension wMax = new Dimension(SIZEX,SIZEY);
    Dimension dTemp = new Dimension();
    Dimension treatpoint = new Dimension();
    Dimension collisionpoint = new Dimension();
    Dimension newpoint = new Dimension();
    Dimension autopoint = new Dimension();
    Dimension dpoint[] = new Dimension[4];

    Worm worm[];
    int currscore;
    int highscore;
    int lastscore;
    int treatcount;
    int forcedelay;
    int currautoworm;
    int adddropidx;
    int grow;
    long age;
    int m_getallidx;
    int dirchange;

    // construct a manager and worm(s)
    public SessionManager()
    {
        currscore = lastscore = highscore = 0;

        for (int i = 0; i < 4; i++)
            dpoint[i] = new Dimension();

        worm = new Worm[WORMCOUNT];

        worm[0] = new Worm(2, 5, 10, Color.blue,
                                SIZEX * SIZEY / 2, 1, 0);
        worm[1] = new Worm(2, 15, 20, Color.yellow,
                                22, 0, -1);
        worm[2] = new Worm(2, 1, 20, Color.orange,
                                22, 0, 1);

        Restart();
    }

    // we hit a wall, let's reinit everything and
    // create a new treat
    //
    public void Restart()
    {
        collisionpoint.width = -1;
        collisionpoint.height = -1;

        lastscore = currscore;
        currscore = 0;
        treatcount = 0;
        currautoworm = 1;
        grow = 0;
        age = 0;
        forcedelay = 0;
        dirchange = 0;

        for (int i = 0; i < WORMCOUNT; i++)
            worm[i].ReviveWorm();

        NewTreat(true);
    }

    // called to get the delay, more advanced versions
    // would not return a constant by a dynamically changing
    // delay where the worm speeds up the longer the game
    // is played
    //
    public int GetDelay()
    {
        //return 100;

        int delay = 100;

        if (forcedelay != 0) {
            delay = forcedelay;
            forcedelay = 0;
        }
        else {
            /* aggressive speed increase
            if(age < 110)
                delay = 100;
            else if(age > 1000)
                delay = 0;
            else
                delay = 110 - (int)(age / 10);
            */
            if (m_speedup) {
                if (age < 200)
                    delay = 100;
                else if (age > 1000)
                    delay = 50;
                else
                    delay = 100 - (int)(age / 20);
            }
        }
        return delay;

    }

    // returns current score
    public int GetCurrScore()
    {
        return currscore;
    }

    // returns last score
    public int GetLastScore()
    {
        return lastscore;
    }

    // returns high score
    public int GetHighScore()
    {
        return highscore;
    }

    //
    //
    public void SetDrop(boolean sd)
    {
        m_dontdrop = sd;
    }

    //
    //
    public void SetSpeedup(boolean su)
    {
        m_speedup = su;
    }

    // This is called after each 100ms pause in the action
    // to get the next location of the head segment or to
    // terminate this game due to collision
    //
    public boolean Next()
    {
        boolean forcedrop;

        m_newtr = false;
        m_showscore = false;

        // do main worm first
        newpoint = worm[0].GetNextPoint();

        if (!SearchForTreat(newpoint)) {
            if (SearchForCollision(newpoint))
                return true;
        }
        else {
            forcedelay = 300;
            m_newtr = true;
        }

        forcedrop = worm[0].SetNextPoint(newpoint);

        if ((age++ % 32) != 0) {
            if (!forcedrop) {
                if (grow==0)
                    forcedrop = true;
                else
                    grow--;
            }
        }
        else {
            // don't give them any points until they
            // get a treat
            if (treatcount!=0)
                currscore = treatcount * (int)(age/10);
            else
                currscore = 0;

            // increment high score if it is less than
            // the current score
            if (currscore > highscore)
                highscore = currscore;

            m_showscore = true;
        }

        if (m_dontdrop == false)
            forcedrop = true;

        if (forcedrop)
            worm[0].DropLastPoint();

        if (m_newtr)
            NewTreat(false);

        // let all other worms move now
        // do only one at a time

        if (currautoworm != 0)
            DoAutoWorms();

        return false;
    }

    //
    //
    private void DoAutoWorms()
    {
        int direction, dx, dy;

        currautoworm++;
        if (currautoworm >= WORMCOUNT)
            currautoworm = 1;

        // save current direction of motion
        dpoint[0] = worm[currautoworm].GetDirection();

        // check if we hit anything ahead of us
        //worm[currautoworm].SetDirection(dx, dy);

        autopoint = worm[currautoworm].GetNextPoint();

        if (dirchange > 0)
            dirchange--;

        // do different things depending on what we hit
        // if hit a wall, go randomly right or left
        if (HitWall(autopoint) == true || dirchange == 1) {

            dirchange = (int)((Math.random() + 1) * 5 + 10);

            // pick a direction at random
            if (Math.random() < 0)
                direction = -1;
            else
                direction = 1;

            dpoint[1] = GetNewDirection(dpoint[0], direction);
        }
        else if (HaveSeg(autopoint) == true ||
                 HitTreat(autopoint) == true) {

            // always try to go left if hit an object
            dpoint[1] = GetNewDirection(dpoint[0], -1);
        }
        else {
            // no collision, all done
            worm[currautoworm].SetNextPoint(autopoint);
            // extend worm if it shrunk
            if (worm[currautoworm].NotRegrowing())
                worm[currautoworm].DropLastPoint();
            return;
        }

        // create remaining directions
        if (dpoint[0].width == 1) {
            dpoint[2].width = -1;
            dpoint[2].height = 0;
        }
        else if (dpoint[0].width == -1) {
            dpoint[2].width = 1;
            dpoint[2].height = 0;
        }
        else if (dpoint[0].height == 1) {
            dpoint[2].width = 0;
            dpoint[2].height = -1;
        }
        else {
            dpoint[2].width = 0;
            dpoint[2].height = 1;
        }

        if (dpoint[1].width == 1) {
            dpoint[3].width = -1;
            dpoint[3].height = 0;
        }
        else if (dpoint[1].width == -1) {
            dpoint[3].width = 1;
            dpoint[3].height = 0;
        }
        else if (dpoint[1].height == 1) {
            dpoint[3].width = 0;
            dpoint[3].height = -1;
        }
        else {
            dpoint[3].width = 0;
            dpoint[3].height = 1;
        }


        // skip this first since it is a known collision
        for (int i = 1; i < 4; i++) {

            worm[currautoworm].SetDirection(dpoint[i].width,
                                            dpoint[i].height);

            autopoint = worm[currautoworm].GetNextPoint();

            if (HitWall(autopoint) == false &&
                HaveSeg(autopoint) == false &&
                HitTreat(autopoint) == false) {

                // no collision, all done
                worm[currautoworm].SetNextPoint(autopoint);
                // extend worm if it shrunk
                if (worm[currautoworm].NotRegrowing())
                    worm[currautoworm].DropLastPoint();
                return;
            }
        }
        // no places left to go!

        if (worm[currautoworm].NotRegrowing())
            worm[currautoworm].DropLastPoint();
    }

    //
    //
    private Dimension GetNewDirection(  Dimension olddir,
                                        int motion)
    {
        if (olddir.width == 1 || olddir.width == -1) {
            dTemp.width = 0;
            dTemp.height = motion;
        }
        else {
            dTemp.width = motion;
            dTemp.height = 0;
        }
        return dTemp;
    }

    // Check if we created a new treat in Next() above
    // this is needed so we can draw the new treat if
    // we just ate one
    //
    public boolean IsNewTreat()
    {
        return m_newtr;
    }

    // return this value to determine if we need to display
    // score
    //
    public boolean IsDisplayScore()
    {
        return m_showscore;
    }

    //
    //
    public boolean IsFirstAdded()
    {
        adddropidx = -1;
        for (int i = 0; i < WORMCOUNT; i++) {
            adddropidx++;
            if (worm[adddropidx].NextPointAdded())
                return true;
        }

        return false;
    }

    public boolean IsNextAdded()
    {
        for (int i = adddropidx + 1; i < WORMCOUNT; i++) {
            adddropidx++;
            if (worm[adddropidx].NextPointAdded())
                return true;
        }

        return false;
    }

    public Color CurrAddedColor()
    {
        return worm[adddropidx].GetCurrentWormColor();
    }

    public Dimension CurrAddedPoint()
    {
        return worm[adddropidx].GetAddedPoint();
    }

    public boolean IsFirstDropped()
    {
        adddropidx = -1;
        for (int i = 0; i < WORMCOUNT; i++) {
            adddropidx++;
            if (worm[adddropidx].LastPointDropped())
                return true;
        }

        return false;
    }

    public boolean IsNextDropped()
    {
        for (int i = adddropidx + 1; i < WORMCOUNT; i++) {
            adddropidx++;
            if (worm[adddropidx].LastPointDropped())
                return true;
        }

        return false;
    }

    public Dimension CurrDroppedPoint()
    {
        return worm[adddropidx].GetDroppedPoint();
    }

    // Check if the coordinates of our new point match
    // those of a treat
    //
    private boolean SearchForTreat(Dimension newpoint)
    {
        if (newpoint.width != treatpoint.width ||
            newpoint.height != treatpoint.height)
            return false;

        // increase their score faster the longer they
        // have been playing
        treatcount++;
        grow = (int)(age / 100) + 2;

        return true ;
    }

    // Check if we ran into something like a wall
    // or a worm body
    //
    private boolean SearchForCollision(Dimension newpoint)
    {
        if (HaveSeg(newpoint)) {
            collisionpoint.width = newpoint.width;
            collisionpoint.height = newpoint.height;
            return true;
        }
        if (HitWall(newpoint)) {
            collisionpoint.width = -1;
            collisionpoint.height = -1;
            return true;
        }
        return false;
    }

    // Get the size in pixels of the width of our working area
    //
    public int GetPixelWidth(int wormscale)
    {
        return SIZEX * wormscale;
    }

    // Get the size in pixels of the height of our working area
    //
    public int GetPixelHeight(int wormscale)
    {
        return SIZEY * wormscale;
    }

    // Check if we have something to draw
    // (the first worm tail segment)
    //
    public boolean IsWormSegmentFirst()
    {
        m_getallidx = 0;
        return worm[m_getallidx].IsWormFirst();
    }

    // Get the next worm segment
    //
    public boolean IsWormSegmentNext()
    {
        boolean next;

        next = worm[m_getallidx].IsWormNext();
        if (next == false) {
            m_getallidx++;
            if (m_getallidx >= WORMCOUNT)
                return false;
            next = worm[m_getallidx].IsWormFirst();
        }
        return next;
    }

    // Get the actual postion of the current worm
    // segment after calling the above functions
    //
    public Dimension GetCurrentWormSegment()
    {
        return worm[m_getallidx].GetCurrentWormSegment();
    }

    // Get the current color of this worm after calling the
    // above functions
    //
    public Color GetCurrentWormColor()
    {
        return worm[m_getallidx].GetCurrentWormColor();
    }

    // Set the new direction the worm will take
    // based on user input
    //
    public void SetNewDirectionFromMouse(   int wormscale,
                                            int x,
                                            int y)
    {
        worm[0].SetNewDirectionFromMouse(wormscale, x, y);
    }

    // Set the new direction the worm will take based
    // on user input do the same as mouse but for keyboard
    //
    public void SetNewDirectionFromKeyboard(int key)
    {
        worm[0].SetNewDirectionFromKeyboard(key);
    }

    // return the current treat location
    //
    public Dimension GetTreatPoint()
    {
        return treatpoint;
    }

    // create a new location for a treat
    // this location must not intersect a worm body
    // return false if each random try resulted in placing
    // the treat on the worm(s)'s body
    //
    public boolean NewTreat(boolean bFirst)
    {
        int tries = 100, curridx;
        boolean haveseg;

        while ((tries--)!=0) {
            if (bFirst) {
                treatpoint.width =
                    (int)(Math.random() * SIZEX/2 + SIZEX/2);
                treatpoint.height =
                    (int)(Math.random() * SIZEY/2 + SIZEY/2);
            }
            else {
                treatpoint.width =
                    (int)(Math.random() * (SIZEX-0.5f));
                treatpoint.height =
                    (int)(Math.random() * (SIZEY-0.5f));
            }
            if (HaveSeg(treatpoint) == false)
                return true;
        }
        return false;
    }

    // check if this point has a segment across all worms
    //
    private boolean HaveSeg(Dimension segpoint)
    {
        for (int i = 0; i < WORMCOUNT; i++)
            if (worm[i].DoYouHaveSegmentAt(segpoint))
                return true;

        return false;
    }

    //
    //
    private boolean HitWall(Dimension segpoint)
    {
        if (segpoint.width < 0 ||
            segpoint.height < 0 ||
            segpoint.width >= wMax.width ||
            segpoint.height >= wMax.height)
            return true;
        return false;
    }

    //
    //
    private boolean HitTreat(Dimension segpoint)
    {
        if (segpoint.width != treatpoint.width ||
            segpoint.height != treatpoint.height)
            return false;
        return true;
    }

    // get collision location
    //
    public Dimension GetCollisionPoint()
    {
        if (collisionpoint.width < 0) {
            // wall collision
            return worm[0].GetCurrentWormSegment();
        }

        return collisionpoint;
    }
}

