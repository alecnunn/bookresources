/*

  Game Programming Gems vol III
  Fast Heightfield Normal Calculation
  by Jason Shankel

*/

#include <GL\glut.h>
#include <cstdlib>
#include <cmath>
#include <cstdio>
//#include <minmax.h>

#define min(x, y) ((x < y) ? x : y);
#define max(x, y) ((x > y) ? x : y);


/*
  Constants
*/
//Vector field indices (x,y,z)
const int kX = 0;
const int kY = 1;
const int kZ = 2;

//Size o' the heightmap (width and height)
const int kFieldDim = 256;




/*
  Variables
*/
//Vertex data
GLint *terrainIndices= 0;
GLfloat *vertices = 0,*normals = 0,*heightfield = 0;

//Camera parameters
GLfloat cameraTheta=45,cameraPhi=30,cameraDistance=125;

//Render modes
bool paused = false,showNormals = true;



/*
  Function Prototypes
*/
//Terrain generation functions
void generateTerrain();
GLfloat rangedRandom(GLfloat v1,GLfloat v2);
int safeFieldIndex(int x,int y);

//Vertex functions
void morphHeightfield(float v);
void calcNormals(void);


//GLUT event handlers
void onSpecialKey(int key,int x,int y);
void onKey(unsigned char key,int x,int y);
void onIdle(void);
void onDisplay(void);

//Start me up, shut me down
void init(void);
void shutdown(void);


/*
  Vertex Functions
*/
void calcNormals(void)
{
	int i=0,x,y;
	float *norm,*north,*south,*west,*east,mag;
	
	//Skip normals on border
	for (x=1;x<kFieldDim-1;x++)
	for (y=1;y<kFieldDim-1;y++)
	{
		i = 3*(x+y*kFieldDim);

		/*
		Temporary variables used here for code clarity
		Real world implementations should optimize this inner loop
		*/

		//The normal
		norm = &normals[i];
		
		//Vertices to the north, south, east and west
		north = &vertices[i-3*kFieldDim];
		south = &vertices[i+3*kFieldDim];
		east = &vertices[i+3];
		west = &vertices[i-3];

		/*
		NormalX = westZ-eastZ;
		NormalY = northZ-southZ;
		NormalZ = 2
		*/
		norm[kX] = west[kZ]-east[kZ];
		norm[kY] = north[kZ]-south[kZ];
		norm[kZ] = 2;

		/*
		Normalize the normal (set magnitude to 1)
		You can skip this step if GL_NORMALIZE is enabled
		*/
		mag = sqrtf(norm[kX]*norm[kX] + norm[kY]*norm[kY] + norm[kZ]*norm[kZ]);
		norm[kX] /= mag;
		norm[kY] /= mag;
		norm[kZ] /= mag;
	}
}



void morphHeightfield(float v)
{
	//Animate the heightfield between flat and the generated terrain (0<=v<=1)
	int x,y,i;
	for (x=0;x<kFieldDim;x++)
	for (y=0;y<kFieldDim;y++)
	{
		i = x + kFieldDim*y;
		vertices[3*i+kZ] = heightfield[i]*v;
	}
	calcNormals();
}



/*

Diamond-Square Terrain Generator

generateTerrain() generates a random fractal terrain using the diamond-square (plasma fractal) algorithm
safeFieldIndex() returns an index into the heightfield given x and y in the range -kFieldDim to +kFieldDim
rangedRandom() returns a random number in the range v1-v2

*/
void generateTerrain()
{
	int x,y,w = kFieldDim;

	float dH = kFieldDim/2,minH = 0,maxH = 0;


	heightfield[0] = 0;
	while (w > 0)
	{
		//Diamond Step
		for (x=0;x<kFieldDim;x+=w)
		for (y=0;y<kFieldDim;y+=w)
		{
			int p1 = safeFieldIndex(x,y);
			int p2 = safeFieldIndex(x+w,y);
			int p3 = safeFieldIndex(x+w,y+w);
			int p4 = safeFieldIndex(x,y+w);
			int i = safeFieldIndex(x+w/2,y+w/2);

			heightfield[i] = rangedRandom(-dH,dH)+(heightfield[p1]+heightfield[p2]+heightfield[p3]+heightfield[p4])*0.25f;
			minH = min(minH,heightfield[i]);
			maxH = max(maxH,heightfield[i]);
		}

		//Square step
		for (x=0;x<kFieldDim;x+=w)
		for (y=0;y<kFieldDim;y+=w)
		{
			int p1 = safeFieldIndex(x,y);
			int p2 = safeFieldIndex(x+w,y);
			int p3 = safeFieldIndex(x+w/2,y-w/2);
			int p4 = safeFieldIndex(x+w/2,y+w/2);
			int i = safeFieldIndex(x+w/2,y);
			heightfield[i] = rangedRandom(-dH,dH)+(heightfield[p1]+heightfield[p2]+heightfield[p3]+heightfield[p4])*0.25f;
			minH = min(minH,heightfield[i]);
			maxH = max(maxH,heightfield[i]);
			
			p1 = safeFieldIndex(x,y);
			p2 = safeFieldIndex(x,y+w);
			p3 = safeFieldIndex(x+w/2,y+w/2);
			p3 = safeFieldIndex(x-w/2,y+w/2);
			i = safeFieldIndex(x,y+w/2);
			heightfield[i] = rangedRandom(-dH,dH)+(heightfield[p1]+heightfield[p2]+heightfield[p3]+heightfield[p4])*0.25f;
			minH = min(minH,heightfield[i]);
			maxH = max(maxH,heightfield[i]);
		}


		//Iterate
		dH *= 0.5f;
		w /= 2;
	}

	
	/*
	Normalize heightfield so altitudes fall in the 0-20 range
	*/
	for (x=0;x<kFieldDim;x++)
	for (y=0;y<kFieldDim;y++)
	{
		int i = (x+y*kFieldDim);
		heightfield[i] = 20.0f*(heightfield[i]-minH)/(maxH-minH);
	}
}

int safeFieldIndex(int x,int y)
{
	return ( ((x+kFieldDim)%kFieldDim) + ((y+kFieldDim)%kFieldDim)*kFieldDim );
}

GLfloat rangedRandom(GLfloat v1,GLfloat v2)
{
	return v1 + (v2-v1)*static_cast<GLfloat>(rand())/static_cast<GLfloat>(RAND_MAX);
}


/*
  GLUT Event Handlers
*/

void onDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//Set up viewport
	GLfloat w = glutGet(GLUT_WINDOW_WIDTH),h = glutGet(GLUT_WINDOW_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(100.0,w/h,1,1000);

	//Set up camera
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0,0,-cameraDistance);
	glRotatef(cameraPhi+270,1,0,0);
	glRotatef(cameraTheta,0,0,1);

	//Set up light
	GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};  
	GLfloat light_position[] = {1, 1, 1, 0};  
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHT0);
	
	
	//Render heightfield
	glColor3f(1,1,1);
	int i=0,x,y;
	for (y=0;y<kFieldDim-1;y++)
	{
		glDrawElements(GL_TRIANGLE_STRIP,kFieldDim*2,GL_UNSIGNED_INT,&terrainIndices[y*2*kFieldDim]);
	}

	//Render normals
	if(showNormals)
	{
		glDisable(GL_LIGHTING);
		glColor3f(0,1,0);
		glBegin(GL_LINES);

		for (x=1;x<kFieldDim-1;x++)
		for (y=1;y<kFieldDim-1;y++)
		{
			i = 3*(x+y*kFieldDim);
			glColor3f(0,1,0);
			glVertex3f(vertices[i+kX],vertices[i+kY],vertices[i+kZ]);
			glColor3f(1,0,0);
			glVertex3f(vertices[i+kX]+normals[i+kX],vertices[i+kY]+normals[i+kY],vertices[i+kZ]+normals[i+kZ]);
		}
		glEnd();
		glEnable(GL_LIGHTING);
	}

	glutSwapBuffers();
	glutPostRedisplay();
}




void onSpecialKey(int key,int x,int y)
{
	switch(key)
	{
	case GLUT_KEY_RIGHT:
		cameraTheta = cameraTheta+5;
		while (cameraTheta > 360) cameraTheta -= 360;
		break;
	case GLUT_KEY_LEFT:
		cameraTheta = cameraTheta-5;
		while (cameraTheta < 0) cameraTheta += 360;
		break;
    case GLUT_KEY_UP:
		cameraPhi = cameraPhi+5;
		if (cameraPhi > 90) cameraPhi = 90;
		break;
	case GLUT_KEY_DOWN:
		cameraPhi = cameraPhi-5;
		if (cameraPhi < 0) cameraPhi = 0;
		break;
	};
}


void onKey(unsigned char key,int x,int y)
{
	switch(key)
	{
	case '+':
	case '=':
		cameraDistance++;
		if (cameraDistance > 1000) cameraDistance=1000;
		break;

	case '-':
		cameraDistance--;
		if (cameraDistance <20) cameraDistance = 20;
		break;

	case ' ':
		generateTerrain();
		break;

	case 'p':
	case 'P':
		paused = !paused;
		break;

	case 'n':
	case 'N':
		showNormals = !showNormals;
		break;
	};
}


void onIdle(void)
{
	static int cycle = 0;
	if(!paused)
	{
		cycle = (cycle+1)%100;
	}
	float v = 1+sinf(static_cast<float>(cycle)*3.14159f/50.f);
	morphHeightfield(v);
}

void init(void)
{
	//Initialize OpenGL settings
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);


	//Allocate vertex data
	heightfield = new float[kFieldDim*kFieldDim];
	vertices = new float[kFieldDim*kFieldDim*3];
	normals = new float[kFieldDim*kFieldDim*3];
	terrainIndices = new GLint[kFieldDim*kFieldDim*2];
	glVertexPointer(3,GL_FLOAT,0,vertices);
	glNormalPointer(GL_FLOAT,0,normals);

	
	//Initialize vertex data
	int i=0,x,y;
	for (x=0;x<kFieldDim;x++)
	for (y=0;y<kFieldDim;y++)
	{
		i = 3*(x + y*kFieldDim);
	
		vertices[i+kX] = x-kFieldDim/2;
		vertices[i+kY] = y-kFieldDim/2;
		vertices[i+kZ] = 0;
	}

	//Terrain indices used for drawing heightfield
	i=0;
	for (y=0;y<kFieldDim-1;y++)
	for (x=0;x<kFieldDim;x++)
	{
		terrainIndices[i] = x+y*kFieldDim;
		terrainIndices[i+1] = x+(y+1)*kFieldDim;
		i+=2;
	}


	generateTerrain();
	calcNormals();

}

void shutdown(void)
{
	delete[] heightfield;
	delete[] normals;
	delete[] vertices;
	delete[] terrainIndices;
}

int main(int argc, char **argv)
{
	printf("Game Gems III - Fast Heightfield Normal Calculation\n");
	printf("by Jason Shankel\n\n");
	printf("n = toggle show normals\n");
	printf("space = generate new terrain\n");
	printf("p = toggle pause\n");
	printf("arrow keys and +/- to move camera\n");

	//Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Fast Heightfield Normal Calculation");
	
	//Set event handlers
	glutDisplayFunc(onDisplay);
	glutSpecialFunc(onSpecialKey);
	glutKeyboardFunc(onKey);
	glutIdleFunc(onIdle);

	//Run, Lola, Run
	init();
	glutMainLoop();
	shutdown();

	return 0;             
}

