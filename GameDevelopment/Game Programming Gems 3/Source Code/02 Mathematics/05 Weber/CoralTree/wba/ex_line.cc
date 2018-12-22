#include <wba/wba.h>
#include <wba/lineint.h>

static void wbaTestInterpret(WBA_LineInterpreter *i, const char *string)
{
	GPL_Array<GPL_String> arguments;
	if (i->Interpret(string, arguments))
	{
		for (register unsigned long j = 0; arguments.GetCount() > j; ++j)
		{
			fprintf(stderr, "%d: '%s'\n", j, arguments[j].GetData());
		}
	}
}

int main(const int argc, const char **argv, const char **envp)
{
	WBA_LineInterpreter li;

	WBA_GLOBAL_ENV->LoadCommandLine(argc, argv);
	WBA_GLOBAL_ENV->LoadEnvVariables(envp);
	WBA_Executor executor;
	WBA_BourneShellLineInterpreter bsli;
	bsli.SetSource(WBA_GLOBAL_ENV);
	bsli.SetExecutor(&executor);

	for (register int i = 1; argc > i; ++i)
	{
		fprintf(stderr, "\"%s\"\n", argv[i]);
		wbaTestInterpret(&li, argv[i]);
		wbaTestInterpret(&bsli, argv[i]);
		fprintf(stderr, "\n");
	}

	return 0;
}
