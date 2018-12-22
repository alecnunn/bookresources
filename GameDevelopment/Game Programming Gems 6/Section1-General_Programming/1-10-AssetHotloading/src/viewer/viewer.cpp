// TODO:
// - Fix lights


#include "TextureResource.h"
#include <GL/glut.h>
#include <cstdio>
#include <cassert>
#include <conio.h>
#include <WinSock2.h>

#include <string>
#include <vector>

int mainWindowId = -1;

float headingDeg = -35.0f;
float pitchDeg = 20.0f;

bool rotating = false;
int beginMouseXPos = 0;
int beginMouseYPos = 0;

const int kListenPort = 34910;
SOCKET serverSocket = INVALID_SOCKET;
SOCKET fileWatcherSocket = INVALID_SOCKET;

const char* textureNames[] = 
{
    "textures\\texture1.texture",
    "textures\\texture2.texture",
    "textures\\texture3.texture"
};

const int textureNamesSize = sizeof(textureNames) / sizeof(textureNames[0]);

struct Mesh
{
    int displayList;
    int textureIndex;
};

const int MAXMESHES = 8;
Mesh meshes[MAXMESHES];

const int MAXTEXTURES = 16;
TextureResource* textures[MAXTEXTURES];
int numTextures = 0;



bool MakeMesh(Mesh& mesh)
{         
    mesh.displayList = glGenLists(1);    
    glNewList(mesh.displayList, GL_COMPILE);   
        glFrontFace(GL_CW);
        glutSolidTeapot(1.0f);
        glFrontFace(GL_CCW);
    glEndList();

    mesh.textureIndex = numTextures;
    textures[numTextures] = CreateFromFile(textureNames[mesh.textureIndex]);
    if (textures[numTextures] == NULL)
    {
        printf ("Error loading texture %s\n", textureNames[mesh.textureIndex]);
        return false;
    }

    ++numTextures;

    return true;
}


void RenderGrid (float width, float height, float stepSize)
{
    glDisable(GL_TEXTURE_2D);    
    glDisable(GL_LIGHTING);

    glBegin(GL_LINES);
    glColor3f(0.6f, 0.6f, 0.7f);
    for (float i=-width; i <= width; i += stepSize )
    {
        glVertex3f(i, 0, -height);
        glVertex3f(i, 0, +height);
    }
    for (float i=-height; i <= height; i += stepSize)
    {
        glVertex3f(-width, 0, i);
        glVertex3f(+width, 0, i);
    }
    glEnd();    
}


void RenderMesh (const Mesh& mesh, float x, float y, float z)
{
    glEnable(GL_TEXTURE_2D);    
    glEnable(GL_LIGHTING);

    glPushMatrix();
    glTranslatef(x,y,z);
    //glBindTexture(GL_TEXTURE_2D, textures[mesh.textureIndex].texID);
    glBindTexture(GL_TEXTURE_2D, textures[mesh.textureIndex]->openGLID);
    glCallList(mesh.displayList);
    glPopMatrix();    
}

void Redraw()
{
    glPushMatrix();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gluLookAt(0.0, 0.0, 10.0,
        0.0, 0.0, 0.0,      
        0.0, 1.0, 0.);

    glRotatef(pitchDeg, 1.0, 0.0, 0.0);
    glRotatef(headingDeg, 0.0, 1.0, 0.0);        

    glPushMatrix();
    glTranslatef(0,-0.75f, 0);
    RenderGrid(10, 10, 2);
    glPopMatrix();

    RenderMesh(meshes[0], -3, 0, 0);
    RenderMesh(meshes[1],  0, 0, 0);
    RenderMesh(meshes[2], +3, 0, 0);

    glutSwapBuffers();    

    glPopMatrix();
}



void OnMouseButton (int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) 
	{
        rotating = true;
        beginMouseXPos = x;
        beginMouseYPos = y;
    }

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
        rotating = false;
}


void OnMouseMove(int x, int y)
{
    if (rotating) 
	{
        headingDeg += (x - beginMouseXPos)/2.0f;
        beginMouseXPos = x;

        pitchDeg += (y - beginMouseYPos)/2.0f;
        beginMouseYPos = y;

        glutPostRedisplay();
    }
}

void CheckForConnectingFileWatcher()
{
	SOCKET connectedSocket = accept(serverSocket, NULL, NULL);

	if (connectedSocket == INVALID_SOCKET)
		assert(WSAGetLastError() == WSAEWOULDBLOCK);
	else
	{
		fileWatcherSocket = connectedSocket;
		printf("Got a connection!\n");
	}
}

bool ListenForClient();

void HandleDisconnect()
{
	printf("Filewatcher disconnected!\n");
	closesocket(fileWatcherSocket);
	fileWatcherSocket = INVALID_SOCKET;

	ListenForClient();
}

std::string ReadChangedAssetNameFromNetwork()
{
	std::string ret;
	char c;

	do
	{
		switch(recv(fileWatcherSocket, &c, 1, 0)) 
		{
		case 0:
			if (WSAGetLastError() != WSAEWOULDBLOCK)
				HandleDisconnect();
			return "";
		case -1:
			if (errno == EAGAIN)
				return ret;
			else
				return "";
		}

		if (c != '\n')
			ret += c;
	} while (c != '\n');

	return ret;
}

int TextureIndexByName(const char* textureName)
{
	for (int i = 0; i < textureNamesSize; ++i)
	{
		if (!strcmpi(textureName, textureNames[i]))
			return i;
	}

	return -1;
}

void ReloadTexture(const char* textureName)
{
	const int textureIndex = TextureIndexByName(textureName);
	if (textureIndex != -1)
	{
		TextureResource*& texture = textures[textureIndex];

		RemoveOpenGLTexture(*texture);
		delete [] texture;
		texture = CreateFromFile(textureNames[textureIndex]);

		printf("Reloading Texture '%s'\n", textureName);
	}
}

void OnIdle()
{
	if (fileWatcherSocket == INVALID_SOCKET)
		CheckForConnectingFileWatcher();
	else
	{
		typedef std::vector< std::string > StringVector;
		StringVector filesToReload;

		std::string filename = ReadChangedAssetNameFromNetwork();
		while (!filename.empty())
		{
			filesToReload.push_back(filename);
			filename = ReadChangedAssetNameFromNetwork();
		}

		for (StringVector::const_iterator cur = filesToReload.begin(); cur != filesToReload.end(); ++cur)
			ReloadTexture(cur->c_str());

		Redraw();
	}
}

void ReloadTextures(int)
{
	for (int i = 0; i < textureNamesSize; ++i)
		ReloadTexture(textureNames[i]);
}

void SetLights()
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    const float lightZeroPosition[] = {10.0, 4.0, -5.0, 1.0};
    const float lightZeroColor[] = {0.8, 1.0, 0.8, 1.0};
    const float lightOnePosition[] = {-1.0, -2.0, 1.0, 0.0};
    const float lightOneColor[] = {0.6, 0.3, 0.2, 1.0};

    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
    glLightfv(GL_LIGHT0, GL_POSITION, lightZeroPosition);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.1);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.05);
    glEnable(GL_LIGHT0);
    
    glLightfv(GL_LIGHT1, GL_POSITION, lightOnePosition);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightOneColor);
    glEnable(GL_LIGHT1);
    glPopMatrix();
}

bool ListenForClient()
{
	if (listen(serverSocket, 1) == SOCKET_ERROR)
	{
		closesocket(serverSocket);
		printf("Couldn't listen on server socket on local port %d!\n", kListenPort);
		return false;
	}

	printf("Listening on port %d...\n", kListenPort);
	return true;
}

bool StartupListenServer()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("Error in WSAStartup!\n");
		return false;
	}

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == INVALID_SOCKET)
	{
		printf("Couldn't create a listen socket!\n");
		return false;
	}

	{
		BOOL reuseAddr(TRUE);
		if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuseAddr, sizeof(BOOL)) != 0)
		{
			printf("Couldn't set socket to reuse address!\n");
			return false;
		}
	}

	u_long nonblocking = 1;
	if (ioctlsocket(serverSocket, FIONBIO, &nonblocking) == SOCKET_ERROR)
	{
		closesocket(serverSocket);
		printf("Couldn't set listen socket to nonblocking!\n");
		return false;
	}

	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockaddr_in));
	sockAddr.sin_family = PF_INET;
	sockAddr.sin_port = htons(kListenPort);
	if (bind(serverSocket, (sockaddr*)&sockAddr, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		closesocket(serverSocket);
		printf("Couldn't bind listen socket to local port %d!\n", kListenPort);
		return false;
	}

	return ListenForClient();
}

int CleanUp()
{
    for (int i=0; i<numTextures; ++i)
        delete [] textures[i];

	if (serverSocket != INVALID_SOCKET)
		closesocket(serverSocket);

	if (fileWatcherSocket != INVALID_SOCKET)
		closesocket(fileWatcherSocket);

	WSACleanup();
	return 0;
}

int main(int argc, char **argv)
{
    const int viewportWidth = 640;
    const int viewportHeight = 480;

	if (!StartupListenServer())
		return 1;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    mainWindowId = glutCreateWindow("GPG6 - Asset hotloading demo");
    glutReshapeWindow(viewportWidth, viewportHeight);
    glClearColor(0.3f, 0.3f, 0.5f, 0.0f);

    glutDisplayFunc(Redraw);
    glutMouseFunc(OnMouseButton);
    glutMotionFunc(OnMouseMove);
	glutIdleFunc(OnIdle);

    glutCreateMenu(ReloadTextures);
    glutAddMenuEntry("Force reload textures", 1);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    if (!MakeMesh(meshes[0]) || !MakeMesh(meshes[1]) || !MakeMesh(meshes[2]))
        return 1;

    glEnable(GL_TEXTURE_2D);    
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);

    const float fovDeg = 40.0f;
    const float aspectRatio = (float)viewportWidth/viewportHeight;
    const float zNear = 1.0f;
    const float zFar = 40.0f;
    gluPerspective (fovDeg, aspectRatio, zNear, zFar);

    SetLights();
    glutMainLoop();

    return 0;
}
