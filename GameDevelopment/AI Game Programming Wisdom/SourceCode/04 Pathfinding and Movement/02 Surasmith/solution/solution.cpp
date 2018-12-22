/**
	Smith Surasmith: Aug. 2001
*/

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <time.h>

/**
	Sample code for creating solution table

  - the solution table is 0 based.
  - the example table in the book is 1 based.
  - the values shown in soln.csv is one off from the book due
	to the starting base difference.
*/

/**
	Dijkstra:

	n - number of nodes
	rbuf - the list for the result path, length n
	tbl - connectivity table, lenght n-squared
			2D array of weights,
			<0 for no connections,
			row major.
	wbuf - buffer for scratch pad, length n
			cost - SHRT_MAX for unchecked,
					>=0 for open,
					<0 for closed.
			prev - the previous node index leading to current index
	src - beginning index
	dst - ending index
*/
struct recDijkstra {
	short cost;
	short prev;
};

bool Dijkstra(short n,short *rbuf,short *tbl,recDijkstra *wbuf,short src,short dst)
{
	short open,cur,min,cost,curCost;
	int i,r;

	/* prep */
	for(i=0;i<n;i++) {
		wbuf[i].cost = SHRT_MAX;
		wbuf[i].prev = -1;
		rbuf[i] = -1;
	}

	/* search */
	cur=src;
	open=1;
	r=cur*n;
	wbuf[cur].cost = -1;
	curCost = tbl[r+cur];
	while((cur!=dst)&&(0<open))
	{
		open--;

		/* look at all out going edges */
		for(i=0;i<n;i++) 
		{
			if(i==cur) {
				continue;
			}

			cost = tbl[r+i] + curCost;

			/* cost test */
			if((curCost<=cost)&&(cost<wbuf[i].cost))
			{
				wbuf[i].cost = cost;
				wbuf[i].prev = cur;
				open++;
			}

		}

		if(0<open)
		{
			/* find lowest cost */
			min=-1;
			for(i=0;i<n;i++)
			{
				if((-1!=wbuf[i].cost)&&(SHRT_MAX>wbuf[i].cost)&&((-1==min)||(wbuf[i].cost<wbuf[min].cost)))
				{
					curCost = wbuf[i].cost;
					min = i;
				}
			}

			if(-1<min)
			{
				rbuf[min] = wbuf[min].prev;
				cur = min;
				wbuf[cur].cost = -1;
				r=cur*n;
			}
		}
	}

	/* end conditions */
	if(cur==dst) {
		return true;
	} else {
		return false;
	}
}

/**
	LoadData

	file format:
	- ascii file
	- comma separated file
	- each line contains: <number of entries>,<entry in format <number>[<cost>],...
	- for and unconnected node, fill the first column with 0
	- 
*/
short *LoadData(short& n, short& start, const char *szFileName)
{
	/**
		find how many lines, that's our n
	*/
	short *data=0;
	FILE *fp = fopen(szFileName,"r");
	if(fp) {

		int i,c,res,nEntries,size;
		short line,node,cost;

		/* header */
		res = fscanf(fp,"%hd,%hd",&n,&start);
		if((EOF == res)||(2!=res)) {
			goto loadError;
		}
		c = fgetc(fp);
		while((','==c) && (EOF!=c)) {
			c = fgetc(fp);
		}
		if(EOF == c) {
			goto loadError;
		}

		/* new data */
		n += start;
		size = n*n;
		data = new short [size];
		if(!data) {
			printf("unable to allocate memory for data table\n");
			return 0;
		}
		for(i=0;i<size;i++) {
			data[i] = -1;
		}
		for(i=0;i<n;i++) {
			data[(i*n)+i] = 0;
		}

		/* each line */
		line=start;
		for(line=start;line<n;line++) {
			res = fscanf(fp,"%d",&nEntries);
			if((EOF == res)||(1!=res)) {
				goto loadError;
			}
			for(i=0;i<nEntries;i++) {
				c = getc(fp); /* get rid of comma delimiter */
				if(','!=c) {
					goto loadError;
				}
				res = fscanf(fp,"%hd[%hd]",&node,&cost);
				if((EOF == res)||(2!=res)) {
					goto loadError;
				}
				data[(line*n)+node] = cost;
			}

			/* get rid of trailing commas */
			c = fgetc(fp);
			while((','==c) && (EOF!=c)) {
				c = fgetc(fp);
			}
			if(EOF == c) {
				goto loadError;
			}
		}

		fclose(fp);

	} else {
		printf("couldn't open file %s\n", szFileName);
	}
	return data;

loadError:
	fclose(fp);
	printf("reading error in file %s\n", szFileName);
	if(data) {
		delete [] data;
	}
	return 0;
}


/**
	SaveIntermediateFile
*/
void SaveIntermediateFile(short n, short start, short *data,const char *szFileName)
{
	int i,j,r,c,res;
	static char sz[128];
	char *p;
	strcpy(sz,szFileName);
	p = strchr(sz,'.');
	sprintf(p,"i.csv");
	FILE *fp = fopen(sz,"w");
	if(fp) {
		fprintf(fp,"%hd\n",n-start);
		for(i=start;i<n;i++) {
			r = i*n;
			c = n-1;
			for(j=start;j<c;j++) {
				res = fprintf(fp,"%hd,",data[r+j]);
				if(0 > res) {
					goto saveError;
				}
			}
			res = fprintf(fp,"%hd\n",data[r+c]);
			if(0 > res) {
				goto saveError;
			}
		}
		fclose(fp);
	} else {
		printf("unable to open intermediate file %s\n", sz);
	}

	return;

saveError:
	fclose(fp);
	printf("error in writing to intermediate file.\n");
	return;
}

/**
	GetFirstNode
*/
short GetFirstNode(short *rbuf,short src,short dst) {
	while(rbuf[dst]!=src) {
		dst = rbuf[dst];
	}
	return dst;
}

/**
	SaveSolution
*/
bool SaveSolution(short n,short start,short *soln,const char *szFileName)
{
	FILE *fp = fopen(szFileName,"w");
	if(fp) {
		int i,j,r;
		fprintf(fp,"%hd,\n",n-start);
		for(i=start;i<n;i++) {
			r = i*n;
			for(j=start;j<n;j++) {
				fprintf(fp,"%hd,",soln[r+j]);
			}
			fprintf(fp,"\n");
		}
		fclose(fp);
		return true;
	}
	return false;
}

/**
	PrintUsage
*/
void PrintUsage()
{
	printf("usage: -ih <source filename> <solution filename>\n");
	printf("i : write out intermediate connectivity file.\n");
	printf("h : help\n");
	printf("<source filename> is the data source in csv format.\n");
	printf("<solution filename> is the filename for data output.\n");
	printf("default source filename is in.csv.\n");
	printf("default solution filename is out.csv.\n"); 
}

/**
	MAIN
*/
int main(int argc, char **argv)
{
	/* read params */
	static char szIn[128] = "in.csv";
	static char szOut[128] = "out.csv";
	bool bIFile = false;
	int param=1;
	if(param < argc) {
		if('-' == *argv[param]) {
			if(0 != strchr(argv[param],'h')) {
				PrintUsage();
				return 0;
			}
			if(0 != strchr(argv[param],'i')) {
				bIFile = true;
			}
			param++;
		}
	}
	if(param < argc) {
		strcpy(szIn,argv[param]);
		param++;
	}
	if(param < argc) {
		strcpy(szOut,argv[param]);
	}


	/* load data */
	short len,start;
	short *tbl = LoadData(len,start,szIn);

	if(0==tbl) {
		return 1;
	}

	if(true == bIFile) {
		SaveIntermediateFile(len,start,tbl,szIn);
	}

	short *rbuf = new short [len];
	recDijkstra *wbuf = new recDijkstra [len];
	short *soln = new short [len*len];
	int i,j,r;

	/* create solution table */
	time_t t0,t1;
	t0 = time(0);

	for(i=0;i<len;i++) {
		r = len * i;
		for(j=0;j<len;j++) {
			if(i==j) {
				soln[r+j] = j;
			} else {
				if(Dijkstra(len,rbuf,tbl,wbuf,i,j)) {
					soln[r+j] = GetFirstNode(rbuf,i,j);
				} else {
					soln[r+j] = -1;
				}
			}
		}
	}

	t1 = time(0);

	printf("profile: soln table creation took %d seconds.\n", difftime(t1,t0));

	/* write out the soln */
	if(false == SaveSolution(len,start,soln,szOut)) {
		printf("error saving to file %s.\n",szOut);
	}

	delete [] rbuf;
	delete [] wbuf;
	delete [] tbl;
	delete [] soln;
	return 0;
}
