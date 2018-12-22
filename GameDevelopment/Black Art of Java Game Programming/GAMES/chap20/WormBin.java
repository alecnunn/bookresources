//---------------------------------------------------------
//
// File: WormBin.java
//
// Purpose: Top-level applet user interface object
//
// Copyright (C) 1996 Roman Mach
//
// mach@halcyon.com
//
//---------------------------------------------------------

import java.awt.*;
import java.applet.AudioClip;
import java.net.URL;
import java.net.MalformedURLException;

// the applet for the worm game starts here
//
public class WormBin
        extends java.applet.Applet
        implements Runnable
{
    static int WORMSCALE = 6;
    static int BORDERPAD = WORMSCALE * 2;
    static int THINBORDER = 1;
    static int EXPANDWIDTH = 160;
    static int EXPANDHEIGHT = 60;
    static int width, height;

    SessionManager smgr = null;
    Image wormimage = null;
    Graphics wormgraphics = null;
    Thread kicker = null;
    boolean threadSuspended = false;
    Panel PUser = new Panel();
    Panel PScores = new Panel();
    Panel PControls = new Panel();
    String scorestr = new String();

    // use alignments so numbers increment right justified
    Label LNoteCS = new Label(" Curr", Label.LEFT);
    Label LCurrScore = new Label("", Label.RIGHT);
    Label LNoteHS = new Label(" High:", Label.LEFT);
    Label LHighScore = new Label("", Label.RIGHT);
    Label LNoteLS = new Label(" Last:", Label.LEFT);
    Label LLastScore = new Label("", Label.RIGHT);
    Button BPause = new Button("Start");
    Checkbox CSpeedup = new Checkbox("Build up Speed");
    Checkbox CGrow = new Checkbox("Grow");

    // add sound
    AudioClip hitsoundData = null;
    AudioClip atesoundData = null;
    URL hitsound = null;
    URL atesound = null;


    // called only once, create our session manager
    // and size our client area
    //
    public void init()
    {
        // need to do this to get the keyboard to work
        // from within a browser without having to click
        // on the applet's client area
        requestFocus();
        smgr = new SessionManager();

        width = Integer.valueOf(
                        getParameter("width")).intValue();
        height = Integer.valueOf(
                        getParameter("height")).intValue();

        if (width < smgr.GetPixelWidth(WORMSCALE) +
                                            EXPANDWIDTH)
            width = smgr.GetPixelWidth(WORMSCALE) +
                                            EXPANDWIDTH;

        if (height < smgr.GetPixelHeight(WORMSCALE) +
                                            EXPANDHEIGHT)
            height = smgr.GetPixelHeight(WORMSCALE) +
                                            EXPANDHEIGHT;

        resize(width, height);

        setLayout(new BorderLayout());

        WriteScore();

        PScores.setLayout(new GridBagLayout());
        PScores.add(LNoteCS);
        PScores.add(LCurrScore);
        PScores.add(LNoteLS);
        PScores.add(LLastScore);
        PScores.add(LNoteHS);
        PScores.add(LHighScore);

        // add space before to correctly format
        // for all platforms
        LNoteCS.setText(" Curr:");
        LNoteLS.setText(" Last:");
        LNoteHS.setText(" High:");

        // use dummy numbers to preallocate space on display
        LCurrScore.setText("0000");
        LLastScore.setText("0000");
        LHighScore.setText("0000");

        CGrow.setState(true);

        PControls.setLayout(new GridLayout(2,1));
        PControls.add(CSpeedup);
        PControls.add(CGrow);

        PUser.setLayout(new BorderLayout());
        PUser.add("North", BPause);
        PUser.add("South", PScores);

        add("South", PUser);
        add("East", PControls);

        smgr.SetDrop(CGrow.getState());
        smgr.SetSpeedup(CSpeedup.getState());

        try {
            hitsound = new URL(
                            getDocumentBase(), "collide.au");

            hitsoundData = getAudioClip(hitsound);
        }
        catch (MalformedURLException e) {
            hitsound = null;
        }
        try {
            atesound = new URL(
                            getDocumentBase(), "treat.au");

            atesoundData = getAudioClip(atesound);
        }
        catch (MalformedURLException e) {
            atesound = null;
        }
    }

    // called to start our thread(s)
    public void start()
    {
        if (kicker == null) {
            kicker = new Thread(this);
            kicker.start();
        }
    }

    // stop our thread(s) here
    public void stop()
    {
        kicker = null;
    }

    // not used for this version
    public void destroy()
    {
    }

    // system may call to update graphics
    public void update(Graphics g)
    {
        // if we have an object, copy it from our hidden buffer
        // otherwise clear the visible screen
        if (wormimage != null)
            g.drawImage(wormimage, 0, 0, this);
        else
            ClearScreen(g);
    }

    // called when we need to repaint the client
    // area of our applet
    public void paint(Graphics g)
    {
        update(g);
    }

    // get button and checkbox info
    public boolean action(Event evt, Object what)
    {
        if (evt.target == BPause) {
            ToggleActive();
            return true;
        }
        else if (evt.target == CSpeedup) {
            smgr.SetSpeedup(CSpeedup.getState());
            return true;
        }
        else if (evt.target == CGrow) {
            smgr.SetDrop(CGrow.getState());
            return true;
        }

        return false;
    }

    // reset for start of each game (create new worm(s) etc.)
    //
    private void Reset()
    {
        smgr.Restart();
    }

    // This is called when we need to redraw the hidden
    // buffer with the entire working information for the
    // current state of the game. This is only called at
    // the start and end of each game for high performance
    // graphic response
    //
    private void PaintWorm(Graphics g, boolean bDone)
    {
        // if this is the first time, clear screen
        // and draw a treat
        if (!bDone) {
            ClearScreen(g);
            DrawTreat(g);
        }

        // iterate over all worms
        if (smgr.IsWormSegmentFirst()) {
            // draw the desired segment in the correct location
            do {
                // set the worm color
                g.setColor(smgr.GetCurrentWormColor());
                // draw the segment
                DrawSegment(g, smgr.GetCurrentWormSegment());

            } while (smgr.IsWormSegmentNext());
            // if this is not the first time that means
            // that the user has crashed into something as
            // there is no other way to get here so draw the
            // segment that contributed to the crash
            if (bDone)
                DrawCollision(g);
        }
    }

    //
    private void ClearScreen(Graphics g)
    {
        g.setColor(Color.black);
        g.fillRect( 0, 0,
            smgr.GetPixelWidth(WORMSCALE) + BORDERPAD,
            smgr.GetPixelHeight(WORMSCALE) + BORDERPAD);

        g.setColor(Color.green);
        g.drawRect( 0, 0,
            smgr.GetPixelWidth(WORMSCALE) + THINBORDER,
            smgr.GetPixelHeight(WORMSCALE) + THINBORDER);
    }

    // convert a session space coordinate into screen
    // coordinates and draw the square
    //
    private void DrawSegment(Graphics g, Dimension Seg)
    {
        int xs, ys;

        xs = Seg.width * WORMSCALE + THINBORDER;
        ys = Seg.height * WORMSCALE + THINBORDER;

        g.fillRect(xs, ys, WORMSCALE, WORMSCALE);
    }

    // draw the treat in green
    //
    private void DrawTreat(Graphics g)
    {
        g.setColor(Color.green);
        DrawSegment(g, smgr.GetTreatPoint());
    }

    // draw the collision in red
    //
    private void DrawCollision(Graphics g)
    {
        g.setColor(Color.red);
        DrawSegment(g, smgr.GetCollisionPoint());
    }

    // write high and current scores to display
    // use trick to convert int to string
    //
    private void WriteScore()
    {
        scorestr = "" + smgr.GetCurrScore();
        LCurrScore.setText(scorestr);
        scorestr = "" + smgr.GetLastScore();
        LLastScore.setText(scorestr);
        scorestr = "" + smgr.GetHighScore();
        LHighScore.setText(scorestr);
    }

    // called to get the next worm position
    //
    private boolean Next(Graphics g)
    {
        // call the session manager to get the next
        // worm location. if true returned, we had a collision
        if (smgr.Next()) {
            if(hitsoundData != null)
                hitsoundData.play();
            return true;
        }

        // check if we just ate a treat and created a new
        // one, if yes, we need to draw the new treat
        if (smgr.IsNewTreat()) {
            if(atesoundData != null)
                atesoundData.play();
            try {
                Thread.sleep(100);
            } catch (InterruptedException e)
            {
            }
            DrawTreat(g);
        }

        // erase the tail segment(s)
        if (smgr.IsFirstDropped()) {
            do {
                g.setColor(Color.black);
                DrawSegment(g, smgr.CurrDroppedPoint());
            } while (smgr.IsNextDropped());
        }

        // draw the head segment(s)
        if (smgr.IsFirstAdded()) {
            do {
                g.setColor(smgr.CurrAddedColor());
                DrawSegment(g, smgr.CurrAddedPoint());
            } while (smgr.IsNextAdded());
        }

        // write out score only once in a while to avoid
        // too much flicker
        if (smgr.IsDisplayScore())
            WriteScore();

        return false;
    }

    // this is the applet's thread
    public void run()
    {
        int delay;

        if (kicker != null) {
            // create a hidden buffer and clear it
            wormimage = createImage(
                            smgr.GetPixelWidth(WORMSCALE) +
                                    BORDERPAD,
                            smgr.GetPixelHeight(WORMSCALE) +
                                    BORDERPAD);
            // get context
            wormgraphics = wormimage.getGraphics();
            wormgraphics.setFont(getFont());
            wormgraphics.setColor(Color.black);
            wormgraphics.fillRect(0, 0,
                            smgr.GetPixelWidth(WORMSCALE) +
                                    BORDERPAD,
                            smgr.GetPixelHeight(WORMSCALE) +
                                    BORDERPAD);

            // reset all variables to beginning,
            // create new worm
            Reset();

            // draw current game status and paint to applet
            // client area
            PaintWorm(wormgraphics,false);
            repaint();

            ThreadPause("Start");

            // loop until system terminates us
            while (kicker != null) {

                // loop while not minimized and have valid
                // worker thread
                while ( size().width > 0 &&
                        size().height > 0 &&
                        kicker != null) {
                    // get next worm position
                    if (Next(wormgraphics)) {
                        // worm collided,
                        // redraw all new entities
                        PaintWorm(wormgraphics,true);
                        repaint();

                        // stop game until user presses
                        // mouse button or hits a key
                        ThreadPause("Start");

                        // reset all variables to beginning,
                        // create new worm
                        Reset();

                        // draw current game status and paint
                        // to applet client area
                        PaintWorm(wormgraphics,false);
                        repaint();

                        break;
                    }
                    // repaint from hidden area to visible area
                    repaint();
                    // sleep for a very short time to make the
                    // game playable on fast computers
                    delay = smgr.GetDelay();
                    if (delay > 0)
                        try {
                            Thread.sleep(delay);
                        } catch (InterruptedException e)
                        {
                        }
                }
            }
        }
    }

    // user pressed the mouse button and either wants to
    // restart game or change direction
    //
    public boolean mouseDown(java.awt.Event evt, int x, int y)
    {
        smgr.SetNewDirectionFromMouse( WORMSCALE, x, y );

        if (threadSuspended)
            ThreadResume();

        return true;
    }

    // user pressed a key and either wants to restart game
    // or change direction
    //
    public boolean keyDown(Event evt, int key)
    {
        // esc key
        if (key == 27)
            ToggleActive();
        else {
            smgr.SetNewDirectionFromKeyboard(key);

            if (threadSuspended)
                ThreadResume();
        }
        return true;
    }

    // each call pauses and resumes the game
    //
    private void ToggleActive()
    {
        if (threadSuspended)
            ThreadResume();
        else
            ThreadPause("Resume");
    }

    private void ThreadResume()
    {
        kicker.resume();
        threadSuspended = false;
        BPause.setLabel("Pause");
    }

    private void ThreadPause(String label)
    {
        BPause.setLabel(label);

        threadSuspended = true;
        kicker.suspend();
    }
}
