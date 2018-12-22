float rtmatrix[4][4]={1,0,0,0,
0,1,0,0,
0,0,1,0,
0,0,0,1};
float rotationm[4][4]={1,0,0,0,
0,1,0,0,
0,0,1,0,
0,0,0,1};
float translationm[4][4]={1,0,0,0,
0,1,0,0,
0,0,1,0,
0,0,0,1};

float finalm[4][4]={1,0,0,0,
0,1,0,0,
0,0,1,0,
0,0,0,1};

float forward=0;
float  Camera[]={1,0,0,0,
0,1,0,0,
0,0,1,0,
0,0,0,1};

float xa;
float ya;
float za;
void multiplymatrixes(float matrix1[4][4], float matrix2[4][4],float outputmatrix[4][4])
{
	for(int y=0;y<4;y++)
		for(int x=0;x<4;x++)
			outputmatrix[y][x]=0;
		
		for(int j=0;j<4;j++)
			for(int i=0;i<4;i++)
			{
				for(int x=0;x<4;x++)
					outputmatrix[j][i]+=matrix1[j][x]*matrix2[x][i];
			}
			
}
void invertmatrix(float mt[4][4],float mt2[4][4])
{
	for(int y=0;y<4;y++)
		for(int x=0;x<4;x++)
		{
			mt[x][y]=mt2[y][x];
		}
}

void convertmatrix(float mt[4][4],float *mt2)
{
	int i=0;
	for(int y=0;y<4;y++)
		for(int x=0;x<4;x++)
		{
			mt2[i]=mt[y][x];
			i++;
		}
}

void printmatrix(float mt[4][4])
{
	for(int y=0;y<4;y++)
	{
		
		for(int x=0;x<4;x++)
			cout<<mt[y][x]<<" ";
		cout<<endl;
	}
}
void rotationmatrix(float anglex,float angley,float anglez,float matrix[4][4])
{
	float A       = cos(anglex);
    float B       = sin(anglex);
    float C       = cos(angley);
    float D       = sin(angley);
    float E       = cos(anglez);
    float F       = sin(anglez);
	
    float AD      =   A * D;
    float BD      =   B * D;
	
    matrix[0][0]  =   C * E;
    matrix[0][1]  =  -C * F;
    matrix[0][2]  =   D;
    matrix[1][0]  =  BD * E + A * F;
    matrix[1][1]  = -BD * F + A * E;
    matrix[1][2]  =  -B * C;
    matrix[2][0]  = -AD * E + B * F;
    matrix[2][1]  =  AD * F + B * E;
    matrix[2][2] =   A * C;
	
	matrix[2][3]= matrix[1][3]  =matrix[0][3]= matrix[3][0]=matrix[3][1] = matrix[3][2] = 0;
    matrix[3][3] =  1;
	
}
///////////End of Matrix Functions
void rotatex(float x)
{
	xa+=x;
}
void rotatey(float x)
{
	ya+=x;
}
void rotatez(float x)
{
	za+=x;
}
void moveforward(float x)
{
	translationm[0][3]+=rotationm[2][0]*x;
	translationm[1][3]+=rotationm[2][1]*x;
	translationm[2][3]+=rotationm[2][2]*x;
}
void moveup(float x)
{
	translationm[0][3]+=rotationm[1][0]*x;
	translationm[1][3]+=rotationm[1][1]*x;
	translationm[2][3]+=rotationm[1][2]*x;
}
void updatecamera()
{
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	rotationmatrix(xa,ya,0,rotationm);
	multiplymatrixes(rotationm,translationm,rtmatrix);
	invertmatrix(finalm,rtmatrix );
	convertmatrix(finalm,Camera);
	glLoadMatrixf(Camera);
	
}