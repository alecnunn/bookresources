/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                   +
+  interupt.c - this file handles interrupt requests                +
+                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ globals                                                           +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

U1 interruptOn;

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ functions                                                         +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void handleInt(U1 byte)
{
	DBG_RUN1("handleInt(): received vector (%u)\n",byte);

	if(interruptOn==FALSE)
	{
		DBG_RUN0("handleInt(); interrupts are disabled\n");
		return;
	}

	switch(byte)
	{
		case 0:
		{ 
			DBG_RUN0("handleInt(): handling vector 0\n"); 
			handlefileIO();
		}break;
		case 1: 
		{
			DBG_RUN0("handleInt(): handling vector 1\n");
			handleFileManagement();
		}break;
		case 2: 
		{
			DBG_RUN0("handleInt(): handling vector 2\n");
			handleProcessManagement();

		}break;
		case 3:  
		{
			DBG_RUN0("handleInt(): handling vector 3\n");
			debug = TRUE;
		}break;
		case 4:  
		{
			DBG_RUN0("handleInt(): handling vector 4\n");
			handleTimeDateCall();

		}break;
		case 5:  
		{
			DBG_RUN0("handleInt(): handling vector 5\n");
			handleCommandLine();

		}break;
		case 6:
		{
			DBG_RUN0("handleInt(): handling vector 6\n");
			handleMemoryStatus();

		}break;
		case 7:
		{
			DBG_RUN0("handleInt(): handling vector 7\n");
			handleAllocCall();

		}break;
		case 8:
		{
			DBG_RUN0("handleInt(): handling vector 8\n");
			handleMathCall();

		}break;
		case 9:
		{
			DBG_RUN0("handleInt(): handling vector 9\n");
			handleNativeCall();

		}break;
		case 10:
		{
			DBG_RUN0("handleInt(): handling vector 10\n");
			handleIPC();

		}break;
		default:
		{
			DBG_RUN1("handleInt(): vector not handled (%u)\n",byte);
		}
	}

	return;

}/*end handleInt*/


