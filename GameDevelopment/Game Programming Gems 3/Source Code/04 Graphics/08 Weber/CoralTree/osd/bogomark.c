#include <osd/osd.h>

#define OSD_BOGODRV 1
#define OSD_BOGOINT 2
#define OSD_BOGOFLT 3
#define OSD_BOGOSTRLEN 10000
#define OSD_BOGO_HIST 7 

int main(int argc, char *argv[], char *envp[])
{
	long t1,t2;
	double t3;
	long i,k,m;
	double a;
	double variance;
	double metric[OSD_BOGO_HIST], average, divisor;
	long mode = OSD_BOGOINT;
	long reps = 0;
	char *string;
	char *bogofile;
	OSD_File file;

	average = 0.0;
	m = 0;

	if(argc > 1)
	{
		if(!strcmp(argv[1], "drive"))
		{
			mode = OSD_BOGODRV;
		}
		else if(!strcmp(argv[1], "float"))
		{
			mode = OSD_BOGOFLT;
		}
		else if(!strcmp(argv[1], "int"))
		{
			mode = OSD_BOGOINT;
		}
		else
		{
			printf("USAGE: %s [int|float|drive [filename]]\n", argv[0]);
			printf("  The default mode is int.  The modes are:\n");
			printf("       int   - do a bogus int speed benchmark\n");
			printf("       float - do a bogus float speed benchmark\n");
			printf("       drive - do a bogus drive speed benchmark\n");
			printf("               filename - file to use for drive test\n");
			exit(1);
		}
	}

	switch(mode)
	{
		case OSD_BOGODRV:
			divisor = 1.0;
			if(argc > 2)
			{
				bogofile = argv[2];
			}
			else
			{
				bogofile = strdup("BOGOFILE");
			}
			break;
		case OSD_BOGOFLT:
			divisor = 100.0;
			break;
		case OSD_BOGOINT:
			divisor = 100.0;
			break;
	}

	string = (char *)osdMalloc(OSD_BOGOSTRLEN);
	osdMemset(string, 'A', OSD_BOGOSTRLEN);

	k = 123;
	a = 123.0;

	while(1)
	{
		t1 = osdTimer();
		switch(mode)
		{
			case OSD_BOGODRV:
				if(!reps){reps = 1000;}
				osdFileOpen(&file, bogofile, OSD_FILE_READ|OSD_FILE_WRITE);
				osdFileSetPointer(&file,0,OSD_FILE_BEGIN);
				for(i=0;i<reps;i++)
				{
					osdFileWrite(&file, string, OSD_BOGOSTRLEN);
				}
				osdFileSetPointer(&file,0,OSD_FILE_BEGIN);
				for(i=0;i<reps;i++)
				{
					osdFileRead(&file, string, OSD_BOGOSTRLEN);
				}
				osdFileClose(&file);
				osdFileDelete(bogofile);
				break;
			case OSD_BOGOINT:
				if(!reps){reps = 100000;}
				for(i=0;i<reps;i++)
				{
					k = k * 3;
					k = k / 3;
					k = k * 7;
					k = k / 7;
					k = k * 11;
					k = k / 11;
					k = k * 13;
					k = k / 13;
					k = k * 17;
					k = k / 17;
				}
				break;
			case OSD_BOGOFLT:
				if(!reps){reps = 100000;}
				for(i=0;i<reps;i++)
				{
					a = a * 3.0;
					a = a / 3.0;
					a = a * 7.0;
					a = a / 7.0;
					a = a * 11.0;
					a = a / 11.0;
					a = a * 13.0;
					a = a / 13.0;
					a = a * 17.0;
					a = a / 17.0;
				}
				break;
			default:
				exit(1);
				break;
		}
		t2 = osdTimer();
		if(t2 == t1) { t3 = 0.001; }
		else { t3 = (double)(t2 - t1); }
		
		metric[m] = ((double)reps)/t3/divisor;
		if(0.0 == average)
		{
			for(i=1;i<OSD_BOGO_HIST;i++)
			{
				metric[i] = metric[0];
			}
		}
		average = 0.0;
		for(i=0;i<OSD_BOGO_HIST;i++)
		{
			average += metric[i];
		}
		average = average/(double)OSD_BOGO_HIST;
		variance = 0.0;
		for(i=0;i<OSD_BOGO_HIST;i++)
		{
			variance += (pow(metric[i] - average, 2.0));
		}
		variance = variance/(double)OSD_BOGO_HIST;
		printf("%g  %g  %g\n", metric[m], average, sqrt(variance));
		fflush(stdout);
		m++;
		if(m >= OSD_BOGO_HIST){ m = 0; }
		reps *= 1000.0/t3;
	}
}

