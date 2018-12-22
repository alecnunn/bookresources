#include "core.h"

namespace {

class CWatchLocation {
public:
	tBYTE		*pWatchStart;
	tBYTE		*pWatchEnd;         // actually one byte beyond the end
	tUINT32 	Checksum;
    
    
	CWatchLocation(void *ptr, tMEMSIZE size);

    tUINT32 CalcCheckSum();
	};

sgxVector<CWatchLocation> g_WatchLocations;

CWatchLocation::CWatchLocation(void *ptr, tMEMSIZE size)
{
    pWatchStart = (tBYTE *)ptr; 
    pWatchEnd = pWatchStart + size;
    Checksum = CalcCheckSum(); 
}

tUINT32 CWatchLocation::CalcCheckSum()
{
    tBYTE *ptr = pWatchStart;
    tUINT32 check = 0;

    while(ptr != pWatchEnd) {
        check += *ptr++;
    }

    return check;
}

}	// namespace


void sgxDebugRegister(void *ptr, tMEMSIZE size)
{
	g_WatchLocations.push_back(CWatchLocation(ptr, size));
}

void sgxCheckCheckSums()
{
    sgxVector<CWatchLocation>::iterator it = g_WatchLocations.begin();

    for(;it != g_WatchLocations.end(); ++it) {
        if ((*it).CalcCheckSum() != (*it).Checksum) {
            sgxTrace("Checksum error!");
        }
    }
}

void sgxCheckForOverwrites(void *ptr, tMEMSIZE size)
{
    tBYTE *pStart = (tBYTE *)ptr;
    tBYTE *pEnd = pStart + size;// actually one beyond the end

    sgxVector<CWatchLocation>::iterator it = g_WatchLocations.begin();

    for(;it != g_WatchLocations.end(); ++it) {
        if ((pStart < (*it).pWatchStart && pEnd > (*it).pWatchStart) ||
            ((*it).pWatchStart < pStart && (*it).pWatchEnd > pStart)) {
            sgxTrace("Memory overlap! 0x%x (%d)", ptr, size);
        }
    }
}


