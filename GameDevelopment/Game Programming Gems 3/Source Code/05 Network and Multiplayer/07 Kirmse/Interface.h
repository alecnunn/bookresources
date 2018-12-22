/*
 * Interface
 *
 * Handles the graphical interface and contains the main window loop
 *
 */

#ifndef _INTERFACE_H
#define _INTERFACE_H

class WindowResizer;


class Interface
{
public:

   Interface();
   ~Interface();

   void Run();

   HWND GetWindow() const
   {
      return mhMain;
   }
   
   void AddFilter(Filter *pFilter);
   void RemoveFilter(Filter *pFilter);

   void AddHost(Host *pHost);
   void RemoveHost(Host *pHost);
   
   void DisplayPacket(Message *msg, Protocol protocol,
                      const Endpoint &from_endpoint,
                      const Endpoint &to_endpoint,
                      const Action   &action);
   
protected:
   
   HWND mhMain;   // Main window

   CListView *mpPacketList;
   CListView *mpFilterList;
   CListView *mpHostList;

   // Currently displayed filter; NULL if none
   Filter *mpCurrentFilter;

   // Currently displayed host; NULL if none
   Host *mpCurrentHost;

   // true when new packets should be displayed in the packet list
   bool mboolDisplayPackets;

   // true to log packets to file
   bool mboolLogToFile;
   FILE *mpLogFile;

   WindowResizer *mpResizer;

   
   void Initialize();
   
   void DisplayCurrentFilter();
   void DisplayCurrentHost();
   void DisplayBandwidthUsage();

   void BandwidthButtonPressed(int32 id);

   void SetLogging(bool logging_enabled);
   void SetCurrentProtocol(Protocol protocol);

   void SaveColumnWidths(CListView *pListView, const char *pName);
   void SetColumnWidths(CListView *pListView, const char *pName);

   void FilterListNotify(NMHDR *nmhdr);
   void HostListNotify(NMHDR *nmhdr);
   void On_WM_COMMAND(HWND hwnd, int32 id, HWND hCtl, uint32 notify);
   void On_WM_NOTIFY(HWND hwnd, int32 id, NMHDR *nmhdr);
   void On_WM_HSCROLL(HWND hwnd, HWND hwndCtl, UINT code, int32 pos);
   void On_WM_TIMER(HWND hwnd, UINT id);
   
   long WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
   static long WINAPI StaticWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
   
};

#endif
