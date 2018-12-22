// A Fast Parametric Line Algorithm
// Tony Barrera, Barrera Kristiansen AB; Anders Hast, 
// Creative Media Lab, University of Gävle; 
// and Ewert Bengtsson, Centre For Image Analysis, Uppsala University
// tony.barrera@spray.se,  aht@hig.se, and ewert@cb.uu.se

// Click left mouse button in order to choose center and click again
// to choose radius for circles to be drawn
// Click left mouse button in order to draw line between coordinates

#include <GL/glut.h>
#define X 400
#define Y 300

int x_from;
int y_from;


void Line(int x1,int y1,int x2,int y2)
{
  int i;
  int w,x,y,u,v,p,q,w2,a2,b2;
  
  int dx=1,dy=1;
  int a=x2-x1,b=y2-y1;
  
  if(a<0){a=-a;dx=-1;}
  if(b<0){b=-b;dy=-1;}
  
  w=a+b;
  w2=w<<1;
  a2=a<<1;
  b2=b<<1;
  x=x1;y=y1;
  p=0;q=0;
  u=w;v=w;
  
  for(i=0;i<=w;i++) {
    if(u<p){x+=dx;u+=w2;}
    if(v<q){y+=dy;v+=w2;}
    p+=a2;
    q+=b2;
    glVertex2i(x,y);
  }
}


void myDisplay(){
  glClear(GL_COLOR_BUFFER_BIT);
  glFlush();
}


void myMouseFunc(int button, int state, int x, int y) {
  if (button==GLUT_LEFT_BUTTON && state==GLUT_DOWN){
    if (x_from==0) {
      x_from=x;
      y_from=y;
    }
    else {
      glBegin(GL_POINTS);
      Line(x_from, y_from, x ,y);
      x_from=x;
      y_from=y;
      glEnd();
      glFlush();
    }
  }
}


void main(int argc,char ** argv){
  glutInit(&argc, argv);
  glutInitWindowSize(X,Y);
  glutInitWindowPosition(0,0);
  glutCreateWindow("Line"); 
  glClearColor(1.0, 1.0, 1.0, 1.0); 
  gluOrtho2D(0.0, X, Y,  0.0);
  glColor3f(0.0,0.5,0.1);
  glutDisplayFunc(myDisplay); 
  glutMouseFunc(myMouseFunc);
  glutMainLoop();
}
