#include <osd/osd.h>

int main(const int argc, const char **argv)
{
	int status;
	char *output;
	int i;

	for (i = 1; argc > i; ++i)
	{
		status = 0;
		output = (char *) 0;
		if (!osdExecute((char *) argv[i], &status, &output, FALSE))
		{
			fprintf(stderr, "%s failed\n", argv[i]);
		}
		else
		{
			if(output) fprintf(stderr, "%s\n", output);
			fprintf(stderr, "%s exited with %d\n", argv[i], status);
		}
		if ((char *) 0 != output)
		{
			osdFree(output);
		}
	}
	return 0;
}
