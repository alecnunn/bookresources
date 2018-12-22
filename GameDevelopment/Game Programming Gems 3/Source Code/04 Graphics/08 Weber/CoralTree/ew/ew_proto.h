/*
 *  This file has been generated automatically by an auto-prototyping utility.
 *  Do not attempt to edit.  Your changes will be overwritten.
 */

/* clip.cc */

/* cursor.cc */

/* estring.cc */

/* ew_error.cc */
void ewPrint(long source,long type,char *file,long line,long logger,char *format,va_list ap);

/* event.cc */

/* eventcxt.cc */

/* gfx.cc */

/* loop.cc */
char *ewGetEnvironmentVariable(char **envp,char *name);
long ewDebugShowEnvironment(char **envp);
void ewExit(long value);
void ewExitFunction();
void ewRegisterExitCallback( void (*function)(void) );
void ewMexQueMouse(long on);
void ewMexGetMouse(long *x,long *y);
void ewMexGetWinMouse(long *x,long *y);
long ewXGetMousePosition(EW_Window *target,long *mousex,long *mousey);

/* node.cc */

/* region.cc */

/* resize.cc */

/* text.cc */
long ewExtractArgument(long number,char separator,char *input,char *output);

/* util.cc */
const char *ewColorCode(long index);
int ewStringToArgv(char *string, char ***argv);

/* wba_ew.cc */

/* widget.cc */

/* window.cc */
void ewGLXSetEntry(GLXconfig* ptr, int b, int m, int a);
void ewDefaultKillCallback(EW_Window *window,long quit_app);

/* xdbuf.cc */

/* xpm.cc */
unsigned char *ewReadImageXPM(char **data,long number_colors,long flags,EW_ColorMapping *remap,long *width,long *height);
unsigned char *ewLoadImageXPM(char *filename,long number_colors,long flags,EW_ColorMapping *remap,long *width,long *height);
unsigned char *ewInterpretXPM(char **data,char *filename,long number_colors,long flags,EW_ColorMapping *remap,long *width,long *height);
void ewGetColorMapping(long index,long intensity[3]);
long ewFindNearestColor(long number_colors,float intensity[3]);

