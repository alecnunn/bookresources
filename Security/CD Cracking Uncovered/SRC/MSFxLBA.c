#include <stdio.h>


main(int argc, char **argv)
{
	int M,S,F;
	#define argM	atol(argv[1])
	#define argS	atol(argv[2])
	#define argF	atol(argv[3])
	#define argLBA	atol(argv[1])
	#define _MSF	4
	#define _LBA	2
	
	if ((argc != _MSF) && (argc != _LBA))
	{
		fprintf(stderr,"USAGE:\n");
		fprintf(stderr,"\tMSFxLBA.exe M S F for M:S:F --> LBA\n");
		fprintf(stderr,"\tMSFxLBA.exe LBA   for LBA   --> M:S:F\n");
		return 0;
	}

	if (argc == _MSF)
	{
		printf("%02d:%02d:%02d --LBA --> %d\n", argM, argS, argF,
				(argM*60+argS)*75+argF - 150);
		return 1;

	}

	if (argc == _LBA)
	{
		printf("%d  ---M:S:F--> %02d %02d %02d\n",
			argLBA, (argLBA+150L)/(60L*75), ((argLBA+150)/75L)%60,(argLBA+150) % 75);
		return 2;
	}

	fprintf(stderr,"-ERR:internal error\x7\n");
	
}