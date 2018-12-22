#include "sample.h"

///////////////////////////////////////////////////////////////////////////////
//
// Globals
//
///////////////////////////////////////////////////////////////////////////////

HWND  wnd;
HDC   dc;
HGLRC rc;

bool keys[256];
bool hits[256];

bool active = true;

GLfloat	xrot;
GLfloat	yrot;
GLfloat	zrot;
GLfloat hrot = -95.0f;
GLfloat drot = 5.0f;

GLuint texture;
GLuint help;

int state; // 0=off, 1=to on, 2=on, 3=to off
int timer;

///////////////////////////////////////////////////////////////////////////////
//
// Error
//
///////////////////////////////////////////////////////////////////////////////

void FatalError(const char * message) {
    MessageBox(0, message, "Error", MB_OK | MB_ICONSTOP);
    throw message;
}

///////////////////////////////////////////////////////////////////////////////
//
// Graphic
//
///////////////////////////////////////////////////////////////////////////////

GLuint loadBitmap(const char * filename) {
    FAST_TRACE;

    AUX_RGBImageRec * image = auxDIBImageLoad(filename);
    if(!image) {
        FatalError("Failed to load texture");
    }

    GRAPHIC_REPORT("Successful loading of texture \"" << filename << "\"");
    GRAPHIC_REPORT("Width=" << image->sizeX << " Height=" << image->sizeY);

    GLuint handle;
    glGenTextures(1, &handle);

    glBindTexture(GL_TEXTURE_2D, handle);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image->sizeX, image->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if(image->data) {
        free(image->data);
    }

    free(image);
    return handle;
}

void loadTextures() {
    FAST_TRACE;

    GRAPHIC_REPORT("Loading cube texture...");
    texture = loadBitmap("texture.bmp");

    GRAPHIC_REPORT("Loading help screen texture...");
    help = loadBitmap("help.bmp");
}

void initializeWindow() {
    FAST_TRACE;

    GRAPHIC_REPORT("Loading textures");
    loadTextures();

    glShadeModel(GL_SMOOTH);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

///////////////////////////////////////////////////////////////////////////////
//
// Display
//
///////////////////////////////////////////////////////////////////////////////

void updateWindow() {
    SLOW_TRACE;

	xrot += 0.2f;
	yrot += 0.4f;
	zrot += 0.6f;

    FAST_ASSERT(xrot < 360.0f);
    FAST_ASSERT(yrot < 360.0f);
    FAST_ASSERT(zrot < 360.0f);

    if(xrot >= 360.0f) xrot -= 360.0f;
    if(yrot >= 360.0f) yrot -= 360.0f;
    if(zrot >= 360.0f) zrot -= 360.0f;

    ANIMATION_REPORT("Cube rotation: " << xrot << ", "
                                       << yrot << ", "
                                       << zrot);

    if(state == 1 || state == 3) {
        hrot += drot;

        ANIMATION_REPORT("Help rotation: " << hrot);
    }

    if((state == 1 && hrot >= 0.0f) || (state == 3 && hrot <= 0.0f)) {
        hrot = 0.0f;
        if(drot < 0.0f) drot = -drot;

        state = 2;
        ANIMATION_REPORT("Help is now ON");
    }

    if((state == 1 && hrot < -95.0f) || (state == 3 && hrot > 95.0f)) {
        hrot = -95.0f;
        if(drot < 0.0f) drot = -drot;

        state = 0;
        ANIMATION_REPORT("Help is now OFF");
    }
}

void renderWindow() {
    SLOW_TRACE;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -3.0f);

    glRotatef(hrot, 0.0f, 1.0f, 0.0f);

    glBindTexture(GL_TEXTURE_2D, help);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-2.0f, -1.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 2.0f, -1.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 2.0f,  1.0f, 0.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-2.0f,  1.0f, 0.0f);
    glEnd();

    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -5.0f);

    glRotatef(xrot, 1.0f, 0.0f, 0.0f);
    glRotatef(yrot, 0.0f, 1.0f, 0.0f);
    glRotatef(zrot, 0.0f, 0.0f, 1.0f);

    glBindTexture(GL_TEXTURE_2D, texture);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);

        glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);

        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);

        glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);

        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);

        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
    glEnd();

    SwapBuffers(dc);
}

///////////////////////////////////////////////////////////////////////////////
//
// Window
//
///////////////////////////////////////////////////////////////////////////////

void deactiveWindow() {
    FAST_TRACE;

    if(active) {
        WINDOW_REPORT("Deactivating");
        active = false;
    }
}

void activeWindow() {
    FAST_TRACE;

    if(!active) {
        WINDOW_REPORT("Activating");
        active = true;
    }
}

void closeWindow() {
    FAST_TRACE;

    WINDOW_REPORT("Posting close request");
    PostQuitMessage(0);
}

void keyDown(int index) {
    FAST_TRACE;

    if(keys[index] == false) {
        hits[index] = true;

        WINDOW_REPORT("Key just pressed (" << index << ")");
    }
    else {
        WINDOW_REPORT("Key pressed (" << index << ")");
    }

    keys[index] = true;
}

void keyUp(int index) {
    FAST_TRACE;

    WINDOW_REPORT("Key released (" << index << ")");
    keys[index] = false;
}

void resizeWindow(int width, int height) {
    FAST_TRACE;

    if(height == 0) {
        height = 1;
    }

    WINDOW_REPORT("Resizing (width=" << width << " height=" << height << ")");

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat) width / (GLfloat) height, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM w, LPARAM l) {
    SLOW_TRACE;

    switch(msg) {
        case WM_ACTIVATE:
            if(HIWORD(w)) {
                deactiveWindow();
            }
            else {
                activeWindow();
            }
            return 0;

        case WM_CLOSE:
            closeWindow();
            return 0;

        case WM_KEYDOWN:
            keyDown(LOWORD(w));
            return 0;

        case WM_KEYUP:
            keyUp(LOWORD(w));
            return 0;

        case WM_SIZE:
            resizeWindow(LOWORD(l), HIWORD(l));
            return 0;

        case WM_TIMER:
            KillTimer(wnd, timer);

            MessageBox(0, "Event was received", "Event", MB_OK);
            timer = 0;

            return 0;
    }

    return DefWindowProc(wnd, msg, w, l);
}

void destroyWindow() {
    FAST_TRACE;

    if(rc) {
        wglMakeCurrent(0, 0);
        wglDeleteContext(rc);

        rc = 0;
    }

    if(dc) {
        ReleaseDC(wnd, dc);
        dc = 0;
    }

    if(wnd) {
        DestroyWindow(wnd);
        wnd = 0;
    }

    UnregisterClass("Gem3Sample", GetModuleHandle(0));
}

void createWindow() {
    FAST_TRACE;

    WNDCLASS wc = {
        CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
        WndProc,
        0,
        0,
        GetModuleHandle(0),
        LoadIcon(0, IDI_WINLOGO),
        LoadCursor(0, IDC_ARROW),
        0,
        0,
        "Gem3Sample"
    };

    if(!RegisterClass(&wc)) {
        FatalError("Failed to register the window class");
    }

    wnd = CreateWindowEx(WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,
                         "Gem3Sample",
                         "Journaling Services Sample",
                         WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                         0,
                         0,
                         720,
                         440,
                         0,
                         0,
                         GetModuleHandle(0),
                         0);

    if(!wnd) {
        FatalError("Failed to create the window");
    }

    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        16,
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        16,
        0,
        0,
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };

    dc = GetDC(wnd);
    if(!dc) {
        FatalError("Failed to get the window device context");
    }

    int pf = ChoosePixelFormat(dc, &pfd);
    if(!pf) {
        FatalError("Failed to choose a suitable pixel format");
    }

    if(!SetPixelFormat(dc, pf, &pfd)) {
        FatalError("Failed to set the pixel format");
    }

    rc = wglCreateContext(dc);
    if(!rc) {
        FatalError("Failed to create a GL rendering context");
    }

    if(!wglMakeCurrent(dc, rc)) {
        FatalError("Failed to activate the GL rendering context");
    }

    ShowWindow(wnd, SW_SHOW);
}

///////////////////////////////////////////////////////////////////////////////
//
// Input
//
///////////////////////////////////////////////////////////////////////////////

template<class F>
    void toggleSwitch(char index, Switch * node, F functor)
    {
        SLOW_TRACE;

        if(hits[index]) {
            functor(node, !keys[VK_SHIFT]);
            hits[index] = false;
        }
    }

void changeState(Switch * node, bool recursive) {
    FAST_TRACE;

    bool state = !node->getState();

    DEBUG_REPORT("Inverting state of \"" << node->getName() << "\" of \"" << node->getOwner()->getName() << "\"");
    DEBUG_REPORT("State is now " << (state ? "On" : "Off") << " (recursive=" << (recursive ? "True" : "False") << ")");

    node->setState(state, recursive);
}

void toggleState(char index, Switch * node) {
    SLOW_TRACE;

    toggleSwitch(index, node, changeState);
}

void changeJournal(Switch * node, bool recursive) {
    FAST_TRACE;

    Journal * journal;

    DEBUG_REPORT("Swapping journal of \"" << node->getName() << "\"");
    if(node->getJournal() == Journal::getDefault()) {
        DEBUG_REPORT("Journal will be changed to the message box (recursive=" << (recursive ? "True" : "False") << ")");

        journal = messageBox;
    }
    else {
        DEBUG_REPORT("Journal will be changed to the debugger (recursive=" << (recursive ? "True" : "False") << ")");

        journal = Journal::getDefault();
    }

    node->setJournal(journal, recursive);
}

bool processInput() {
    SLOW_TRACE;

    if(hits['H']) {
        switch(state) {
            case 0:
                GRAPHIC_REPORT("Opening help panel");
                state = 1;
                break;

            case 1:
            case 3:
                GRAPHIC_REPORT("Reversing help animation");
                drot = -drot;
                break;

            case 2:
                GRAPHIC_REPORT("Closing help panel");
                state = 3;
        }

        hits['H'] = false;
    }

    toggleState('1', FastTraceOfRoot::getInstance());
    toggleState('2', SlowTraceOfRoot::getInstance());

    toggleState('3', FastAssertOfRoot::getInstance());
    toggleState('4', SlowAssertOfRoot::getInstance());

    if(hits['A']) {
        DUMMY_ASSERT(2 * 2 == 5);
        hits['A'] = false;
    }

    if(hits['J']) {
        changeJournal(SlowAssertOfRoot::getInstance(), !keys[VK_SHIFT]);
        changeJournal(FastAssertOfRoot::getInstance(), !keys[VK_SHIFT]);
        changeJournal(DummyUsageAssert::getInstance(), !keys[VK_SHIFT]);

        hits['J'] = false;
    }

    toggleState('5', ReportOfRoot::getInstance());
    toggleState('6', ReportOfDebug::getInstance());
    toggleState('7', ReportOfGraphic::getInstance());
    toggleState('8', ReportOfAnimation::getInstance());
    toggleState('9', ReportOfWindow::getInstance());
    toggleState('0', ReportOfEvents::getInstance());

    if(hits['E']) {
        if(timer) {
            KillTimer(wnd, timer);
            EVENTS_REPORT("Previous event was canceled");
        }

        int duration = rand() % 5000;
        EVENTS_REPORT("Event is generated and will be reported in " << duration << " milliseconds");

        timer = (int) SetTimer(wnd, 1, duration, 0);
        hits['E'] = 0;
    }

    return keys[VK_ESCAPE] == false;
}

///////////////////////////////////////////////////////////////////////////////
//
// Main
//
///////////////////////////////////////////////////////////////////////////////

void mainLoop() {
    FAST_TRACE;

    for(;;) {
        MSG msg;

        if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            if(msg.message == WM_QUIT) {
                WINDOW_REPORT("Quit message received");
                break;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            if(active) {
                updateWindow();
                renderWindow();

                if(!processInput()) {
                    WINDOW_REPORT("ESC was pressed");
                    break;
                }
            }
        }
    }
}

void executeSample() {
    FAST_TRACE;

    try {
        GENERAL_REPORT("Creating window...");
        createWindow();

        GENERAL_REPORT("Initializing...");
        initializeWindow();

        GENERAL_REPORT("Entering main loop...");
        mainLoop();
    }
    catch(...) {
    }

    GENERAL_REPORT("Closing window...");
    destroyWindow();
}
