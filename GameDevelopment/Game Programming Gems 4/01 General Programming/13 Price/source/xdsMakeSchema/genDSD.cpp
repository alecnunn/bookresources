#include "xdsMakeSchema.h"
#include "XDSinternal.h"


void writeXdsDsd(FILE *fp, xdsHandle *hXds)
{
	fwrite(xdsIoBuffer.Buffer(), xdsIoBuffer.Size(), fp);
}
