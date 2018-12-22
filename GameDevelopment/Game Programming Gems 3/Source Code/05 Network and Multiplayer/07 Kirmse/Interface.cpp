#include "main.h"

static const char *app_name = "NetTool";

static const char *CONFIG_DISPLAY_PACKETS = "DisplayPackets";
static const char *CONFIG_LOG_TO_FILE     = "LogToFile";
static const char *CONFIG_WINDOW_POS      = "WindowPos";

static const char *CONFIG_FILTER_WIDTHS   = "FilterWidths";
static const char *CONFIG_HOST_WIDTHS     = "HostWidths";
static const char *CONFIG_PACKET_WIDTHS   = "PacketWidths";

static const ListViewColumn filter_columns[] =
{
   { "T",           30 },
   { "From",       102 },
   { "To",         102 },
   { NULL, },
};

static const ListViewColumn packet_columns[] =
{
   { "Time",       60 },
   { "Size",       40 },
   { "T",          40 },
   { "From",       120 },
   { "To",         120 },
   { "Action",     100 },
   { NULL, },
};

static const ListViewColumn host_columns[] =
{
   { "Address", 115 },
   { NULL, },
};

struct BandwidthButton
{
   int32 id;               // 0 means end of list
   uint32 send_bandwidth;
   uint32 receive_bandwidth;
};

static const BandwidthButton bandwidth_buttons[] =
{
   { IDC_BW1, 300 / 10,     300 / 10,     },
   { IDC_BW2, 14400 / 10,   14400 / 10,   },
   { IDC_BW3, 28800 / 10,   28800 / 10,   },
   { IDC_BW4, 33600 / 10,   33600 / 10,   },
   { IDC_BW5, 33600 / 10,   48000 / 10,   },
   { IDC_BW6, 128000 / 8,   384000 / 8,   },  // DSL
   { IDC_BW7, 1200000 / 10, 1200000 / 10, },  // T1
   { IDC_BW8, 0xffffffff,   0xffffffff,   },
   { 0 },
};

//
// Resizing
//

static const int MIN_WINDOW_WIDTH  = 770;
static const int MIN_WINDOW_HEIGHT = 530;

static ResizeItem resize_items[] =
{
   {RA_ID,   IDC_PACKET_LIST,        RA_RIGHT | RA_LEFT | RA_TOP | RA_BOTTOM, NULL },
   {RA_ID,   IDC_PACKET_CLEAR,       RA_LEFT | RA_BOTTOM, NULL },
   {RA_ID,   IDC_PACKET_TOGGLE,      RA_LEFT | RA_BOTTOM, NULL },
   {RA_ID,   IDC_LOG,                RA_LEFT | RA_BOTTOM, NULL },
   {RA_ID,   IDC_HOST_LIST,          RA_LEFT | RA_TOP | RA_BOTTOM, NULL },
   {RA_ID,   IDC_BANDWIDTH,          RA_LEFT | RA_TOP | RA_BOTTOM, NULL },
   {RA_ID,   IDC_PPP,                RA_LEFT | RA_BOTTOM, NULL },
   {RA_ID,   IDC_COMPRESSION,        RA_LEFT | RA_BOTTOM, NULL },
   {RA_ID,   IDC_COMPRESSION_SPIN,   RA_LEFT | RA_BOTTOM, NULL },
   {RA_ID,   IDC_BW1,                RA_LEFT | RA_BOTTOM, NULL },
   {RA_ID,   IDC_BW2,                RA_LEFT | RA_BOTTOM, NULL },
   {RA_ID,   IDC_BW3,                RA_LEFT | RA_BOTTOM, NULL },
   {RA_ID,   IDC_BW4,                RA_LEFT | RA_BOTTOM, NULL },
   {RA_ID,   IDC_BW5,                RA_LEFT | RA_BOTTOM, NULL },
   {RA_ID,   IDC_BW6,                RA_LEFT | RA_BOTTOM, NULL },
   {RA_ID,   IDC_BW7,                RA_LEFT | RA_BOTTOM, NULL },
   {RA_ID,   IDC_BW8,                RA_LEFT | RA_BOTTOM, NULL },
   {RA_ID,   IDC_SEND_SLIDER,        RA_LEFT | RA_BOTTOM, NULL },
   {RA_ID,   IDC_RECEIVE_SLIDER,     RA_LEFT | RA_BOTTOM, NULL },
   {RA_ID,   IDC_SEND_EDIT,          RA_LEFT | RA_BOTTOM, NULL },
   {RA_ID,   IDC_RECEIVE_EDIT,       RA_LEFT | RA_BOTTOM, NULL },
   {RA_ID,   IDC_HOST_ADDRESS,       RA_LEFT | RA_BOTTOM, NULL },
   {RA_ID,   IDC_ADD_HOST,           RA_LEFT | RA_BOTTOM, NULL },
   {RA_ID,   IDC_STATIC1,            RA_LEFT | RA_BOTTOM, NULL },
   {RA_ID,   IDC_STATIC2,            RA_LEFT | RA_BOTTOM, NULL },
   {RA_ID,   IDC_STATIC3,            RA_LEFT | RA_BOTTOM, NULL },
   {RA_ID,   IDC_STATIC4,            RA_LEFT | RA_BOTTOM, NULL },
   {RA_ID,   IDC_STATIC5,            RA_LEFT | RA_BOTTOM, NULL },
   {RA_ID,   IDC_STATIC6,            RA_LEFT | RA_BOTTOM, NULL },
   {RA_ID,   IDC_STATIC7,            RA_LEFT | RA_BOTTOM, NULL },
   {RA_END},
};

/***************************************************************/
Interface::Interface()
{
   mpCurrentFilter = NULL;
   mpCurrentHost   = NULL;
   mpLogFile       = NULL;
   
   INITCOMMONCONTROLSEX ex;
   ex.dwSize = sizeof ex;
   ex.dwICC  = ICC_WIN95_CLASSES | ICC_INTERNET_CLASSES;
   InitCommonControlsEx(&ex);

   WNDCLASS wndclass;
   memset(&wndclass, 0, sizeof wndclass);
   wndclass.style         = CS_HREDRAW | CS_VREDRAW;
   wndclass.lpfnWndProc   = StaticWindowProc;
   wndclass.cbWndExtra    = DLGWINDOWEXTRA;
   wndclass.hInstance     = GetModuleHandle(NULL);
   wndclass.hbrBackground = (HBRUSH) (COLOR_3DFACE + 1);
   wndclass.hIcon         = LoadIcon(wndclass.hInstance, MAKEINTRESOURCE(IDI_ICON1));
   wndclass.lpszClassName = app_name;

   if (RegisterClass(&wndclass) == 0)
      dprintf("registering window class failed\n");

   mhMain = CreateDialog(wndclass.hInstance, MAKEINTRESOURCE(IDD_MAIN), 0, NULL);

   mpPacketList = new CListView(GetDlgItem(mhMain, IDC_PACKET_LIST));
   mpFilterList = new CListView(GetDlgItem(mhMain, IDC_FILTER_LIST));
   mpHostList   = new CListView(GetDlgItem(mhMain, IDC_HOST_LIST));
   
   mpPacketList->SetColumns(packet_columns);
   mpFilterList->SetColumns(filter_columns);
   mpHostList->SetColumns(host_columns);

   mpResizer = new WindowResizer(mhMain, resize_items);

   // Set window size and position
   int x, y, width, height;
   char *str;
   if (true == app->config->LookupString(CONFIG_WINDOW_POS, &str))
   {
      sscanf(str, "%d,%d,%d,%d", &x, &y, &width, &height);
      SetWindowPos(mhMain, HWND_TOP, x, y, width, height,
                   SWP_SHOWWINDOW);
      mpResizer->OnSize(mhMain, SIZE_RESTORED, width, height);
   }

   // Set list view column widths
   SetColumnWidths(mpFilterList, CONFIG_FILTER_WIDTHS);
   SetColumnWidths(mpHostList,   CONFIG_HOST_WIDTHS);
   SetColumnWidths(mpPacketList, CONFIG_PACKET_WIDTHS);
}
/***************************************************************/
Interface::~Interface()
{
   app->config->SetBool(CONFIG_DISPLAY_PACKETS, mboolDisplayPackets);
   app->config->SetBool(CONFIG_LOG_TO_FILE, mboolLogToFile);
   delete mpPacketList;
   delete mpFilterList;
   delete mpHostList;

   SetLogging(false);  // close log file
}
/***************************************************************/
void Interface::Initialize()
{
   // Timer for updating stats
   SetTimer(mhMain, 0, 1000, NULL);
   
   Edit_LimitText(GetDlgItem(mhMain, IDC_PORT1), 5);
   Edit_LimitText(GetDlgItem(mhMain, IDC_PORT2), 5);
   SetDlgItemInt(mhMain, IDC_PORT1, 0, false);
   SetDlgItemInt(mhMain, IDC_PORT2, 0, false);

   CheckDlgButton(mhMain, IDC_ENABLED, true);
   CheckDlgButton(mhMain, IDC_LISTED, true);

   // UDP by default
   CheckDlgButton(mhMain, IDC_UDP, TRUE);

   Trackbar_SetRange(GetDlgItem(mhMain, IDC_LATENCY_MEAN), 0, 5000, false);
   Trackbar_SetRange(GetDlgItem(mhMain, IDC_LATENCY_VARIANCE), 0, 5000, false);
   Trackbar_SetPageSize(GetDlgItem(mhMain, IDC_LATENCY_MEAN), 100);
   Trackbar_SetPageSize(GetDlgItem(mhMain, IDC_LATENCY_VARIANCE), 100);

   Edit_LimitText(GetDlgItem(mhMain, IDC_MEAN_EDIT), 4);
   Edit_LimitText(GetDlgItem(mhMain, IDC_VARIANCE_EDIT), 4);

   UpDown_SetRange(GetDlgItem(mhMain, IDC_LOSS_SPIN), 0, 100);
   UpDown_SetRange(GetDlgItem(mhMain, IDC_REORDER_SPIN), 0, 100);
   UpDown_SetRange(GetDlgItem(mhMain, IDC_DUPLICATION_SPIN), 0, 100);
   
   mpFilterList->SetSelectedItem(0);  // Select first filter

   Edit_LimitText(GetDlgItem(mhMain, IDC_SEND_EDIT), 6);
   Edit_LimitText(GetDlgItem(mhMain, IDC_RECEIVE_EDIT), 6);
   
   Trackbar_SetRange(GetDlgItem(mhMain, IDC_SEND_SLIDER), 0, 120000, false);
   Trackbar_SetRange(GetDlgItem(mhMain, IDC_RECEIVE_SLIDER), 0, 120000, false);
   Trackbar_SetPageSize(GetDlgItem(mhMain, IDC_SEND_SLIDER), 1000);
   Trackbar_SetPageSize(GetDlgItem(mhMain, IDC_RECEIVE_SLIDER), 1000);
   Edit_LimitText(GetDlgItem(mhMain, IDC_COMPRESSION), 3);
   UpDown_SetRange(GetDlgItem(mhMain, IDC_COMPRESSION_SPIN), 0, 100);

   mpHostList->SetSelectedItem(0);  // Select first host

   // Set window title to show our IP address
   char str[200], title[300];
   app->network->GetHostAddresses(str);
   sprintf(title, "%s on %s", app_name, str);
   SetWindowText(mhMain, title);

   mboolDisplayPackets = true;
   app->config->LookupBool(CONFIG_DISPLAY_PACKETS, &mboolDisplayPackets);
   CheckDlgButton(mhMain, IDC_PACKET_TOGGLE, mboolDisplayPackets);

   mboolLogToFile = false;
   app->config->LookupBool(CONFIG_LOG_TO_FILE, &mboolLogToFile);   
   CheckDlgButton(mhMain, IDC_LOG, mboolLogToFile);
   SetLogging(mboolLogToFile);
}
/***************************************************************/
void Interface::Run()
{
   Initialize();
   
   ShowWindow(mhMain, SW_SHOW);

   bool done = false;
   while (!done)
   {	
      Sleep(1);
      
      MSG msg;

      // Handle all messages in queue
      while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
      {
         if (GetMessage(&msg, NULL, 0, 0) == 0)
         {
            done = true;
            break;
         }

         if (IsDialogMessage(mhMain, &msg))
            continue;
         
         TranslateMessage(&msg);
         DispatchMessage(&msg);
      }

      app->network->RunCycle(app->timer->GetDuration());
   }
}
/***************************************************************/
void Interface::AddFilter(Filter *filter)
{
   app->network->AddFilter(filter);
   mpFilterList->AddItem(filter);
}
/***************************************************************/
void Interface::RemoveFilter(Filter *filter)
{
   app->network->RemoveFilter(filter);
}
/***************************************************************/
void Interface::AddHost(Host *host)
{
   app->network->AddHost(host);
   mpHostList->AddItem(host);
}
/***************************************************************/
void Interface::RemoveHost(Host *host)
{
   app->network->RemoveHost(host);
}
/***************************************************************/
void Interface::DisplayPacket(Message *msg, Protocol protocol,
                              const Endpoint &from_endpoint,
                              const Endpoint &to_endpoint,
                              const Action   &action)
{
   char str[100];
   time_t now = time(NULL);

   if (mboolLogToFile && mpLogFile != NULL)
   {
      strftime(str, sizeof str, "%H:%M:%S", localtime(&now));
      fprintf(mpLogFile, "%s\t%d\t", str, msg->GetLength());
      if (protocol == UDP)
         fprintf(mpLogFile, "UDP\t");
      else
         fprintf(mpLogFile, "TCP\t");
      from_endpoint.ToString(str);
      fprintf(mpLogFile, "%s\t", str);
      to_endpoint.ToString(str);
      fprintf(mpLogFile, "%s\t", str);
      action.ToString(str);
      fprintf(mpLogFile, "%s\n", str);
   }
   
   if (!mboolDisplayPackets)
      return;
   
   strftime(str, sizeof str, "%H:%M:%S", localtime(&now));
   int32 row = mpPacketList->AddNewRow(str);

   sprintf(str, "%d", msg->GetLength());
   mpPacketList->AddString(str, row, 1);

   if (protocol == UDP)
      strcpy(str, "UDP");
   else
      strcpy(str, "TCP");
   mpPacketList->AddString(str, row, 2);
   
   from_endpoint.ToString(str);
   mpPacketList->AddString(str, row, 3);

   to_endpoint.ToString(str);
   mpPacketList->AddString(str, row, 4);

   action.ToString(str);
   mpPacketList->AddString(str, row, 5);

   mpPacketList->ScrollToBottom();
}
/***************************************************************/
void Interface::DisplayCurrentFilter()
{
   if (mpCurrentFilter == NULL)
      return; // XXX Clear controls?
   
   Filter *filter = mpCurrentFilter;
   
   IPControl_SetAddress(GetDlgItem(mhMain, IDC_IPADDRESS_FROM),
                        filter->GetFromEndpoint().GetAddress());
   SetDlgItemInt(mhMain, IDC_PORT1,
                 filter->GetFromEndpoint().GetPort(), false);
   
   IPControl_SetAddress(GetDlgItem(mhMain, IDC_IPADDRESS_TO),
                        filter->GetToEndpoint().GetAddress());
   SetDlgItemInt(mhMain, IDC_PORT2,
                 filter->GetToEndpoint().GetPort(), false);

   CheckDlgButton(mhMain, IDC_ENABLED, filter->IsEnabled());
   CheckDlgButton(mhMain, IDC_LISTED, filter->IsListed());

   Trackbar_SetPos(GetDlgItem(mhMain, IDC_LATENCY_MEAN),
                   filter->GetLatencyMean(), true);
   SetDlgItemInt(mhMain, IDC_MEAN_EDIT,
                 filter->GetLatencyMean(), false);
   
   Trackbar_SetPos(GetDlgItem(mhMain, IDC_LATENCY_VARIANCE),
                   filter->GetLatencyVariance(), true);
   SetDlgItemInt(mhMain, IDC_VARIANCE_EDIT,
                 filter->GetLatencyVariance(), false);

   SetDlgItemInt(mhMain, IDC_LOSS_EDIT,
                 filter->GetPacketLoss(), false);
   SetDlgItemInt(mhMain, IDC_REORDER_EDIT,
                 filter->GetReorderProbability(), false);
   SetDlgItemInt(mhMain, IDC_DUPLICATION_EDIT,
                 filter->GetDuplicationProbability(), false);

   CheckDlgButton(mhMain, IDC_UDP, filter->GetProtocol() == UDP);
   CheckDlgButton(mhMain, IDC_TCP, filter->GetProtocol() == TCP);

   // Packet loss for UDP only
   SetCurrentProtocol(filter->GetProtocol());
}
/***************************************************************/
void Interface::DisplayCurrentHost()
{
   if (mpCurrentHost == NULL)
      return; // XXX Clear controls?

   Host *host = mpCurrentHost;
   
   IPControl_SetAddress(GetDlgItem(mhMain, IDC_HOST_ADDRESS), 
                        host->GetAddress());
   SetDlgItemInt(mhMain, IDC_SEND_EDIT, host->GetSendBandwidth(), false);
   SetDlgItemInt(mhMain, IDC_RECEIVE_EDIT, host->GetReceiveBandwidth(), false);
   Trackbar_SetPos(mhMain, IDC_SEND_SLIDER, host->GetSendBandwidth());
   Trackbar_SetPos(mhMain, IDC_RECEIVE_SLIDER, host->GetReceiveBandwidth());
   CheckDlgButton(mhMain, IDC_PPP, host->IsUsingPPP());
   SetDlgItemInt(mhMain, IDC_COMPRESSION, host->GetCompression(), false);
}
/***************************************************************/
void Interface::BandwidthButtonPressed(int32 id)
{
   const BandwidthButton *b = bandwidth_buttons;
   while (b->id != 0)
   {
      if (b->id == id)
      {
         SetDlgItemInt(mhMain, IDC_SEND_EDIT, b->send_bandwidth, false);
         SetDlgItemInt(mhMain, IDC_RECEIVE_EDIT, b->receive_bandwidth, false);
         Trackbar_SetPos(mhMain, IDC_SEND_SLIDER, b->send_bandwidth);
         Trackbar_SetPos(mhMain, IDC_RECEIVE_SLIDER, b->receive_bandwidth);
         return;
      }
      b++;
   }
}
/***************************************************************/
void Interface::SetLogging(bool logging_enabled)
{
   if (mpLogFile != NULL && !logging_enabled)
   {
      fclose(mpLogFile);
      mpLogFile = NULL;
   }

   if (mpLogFile == NULL && logging_enabled)
   {
      mpLogFile = fopen("log.txt", "a+t");
   }
   
   mboolLogToFile = logging_enabled;
}
/***************************************************************/
void Interface::DisplayBandwidthUsage()
{
   if (mpCurrentHost == NULL)
   {
      // XXX clear fields
      return;
   }

   uint32 percent, bandwidth;
   bandwidth = GetDlgItemInt(mhMain, IDC_SEND_EDIT, NULL, false);
   if (bandwidth == 0)
      percent = 0;
   else
      percent = 100 * mpCurrentHost->GetSendAverage() / bandwidth;
   SetDlgItemInt(mhMain, IDC_SEND_USAGE, percent, false);
      
   
   bandwidth = GetDlgItemInt(mhMain, IDC_RECEIVE_EDIT, NULL, false);
   if (bandwidth == 0)
      percent = 0;
   else
      percent = 100 * mpCurrentHost->GetReceiveAverage() / bandwidth;
   SetDlgItemInt(mhMain, IDC_RECEIVE_USAGE, percent, false);
}
/***************************************************************/
void Interface::FilterListNotify(NMHDR *nmhdr)
{
   static char buf[100];
   Filter *filter;

   switch (nmhdr->code)
   {
   case LVN_GETDISPINFO:
   {
      LV_DISPINFO *lvdi = (LV_DISPINFO *) nmhdr;
      filter = (Filter *) lvdi->item.lParam;
      switch (lvdi->item.iSubItem)
      {
      case 0:
         if (filter->GetProtocol() == UDP)
            lvdi->item.pszText = "UDP";
         else
            lvdi->item.pszText = "TCP";
         break;
      case 1:
         filter->GetFromEndpoint().ToString(buf);
         lvdi->item.pszText = buf;
         break;
      case 2:
         filter->GetToEndpoint().ToString(buf);
         lvdi->item.pszText = buf;
         break;
      }
      break;
   }

   case LVN_ITEMCHANGED:
   {
      NMLISTVIEW *nmia = (NMLISTVIEW *) nmhdr;
      if (nmia->uNewState & LVIS_SELECTED)
      {
         mpCurrentFilter = (Filter *) nmia->lParam;
         DisplayCurrentFilter();
      }
      break;
   }
   }
}
/***************************************************************/
void Interface::HostListNotify(NMHDR *nmhdr)
{
   static char buf[100];
   Host *host;

   switch (nmhdr->code)
   {
   case LVN_GETDISPINFO:
   {
      LV_DISPINFO *lvdi = (LV_DISPINFO *) nmhdr;
      host = (Host *) lvdi->item.lParam;
      switch (lvdi->item.iSubItem)
      {
      case 0:
         host->AddressToString(buf);
         lvdi->item.pszText = buf;
         break;
      }
      break;
   }

   case LVN_ITEMCHANGED:
   {
      NMLISTVIEW *nmia = (NMLISTVIEW *) nmhdr;
      if (nmia->uNewState & LVIS_SELECTED)
      {
         mpCurrentHost = (Host *) nmia->lParam;
         DisplayCurrentHost();
      }
      break;
   }
   }
}
/***************************************************************/
void Interface::On_WM_COMMAND(HWND hwnd, int32 id, HWND hCtl, uint32 notify)
{
   Filter *filter, *new_filter;
   Host *host;
   Endpoint endpoint1, endpoint2;
   uint32 address;
   int32 value;

   switch (id)
   {
   case IDC_ADD_FILTER:
      if (notify != BN_CLICKED)
         break;
      
      IPControl_GetAddress(GetDlgItem(mhMain, IDC_IPADDRESS_FROM), &address);
      endpoint1.SetAddress(address);
      endpoint1.SetPort(GetDlgItemInt(mhMain, IDC_PORT1, NULL, false));

      IPControl_GetAddress(GetDlgItem(mhMain, IDC_IPADDRESS_TO), &address);
      endpoint2.SetAddress(address);
      endpoint2.SetPort(GetDlgItemInt(mhMain, IDC_PORT2, NULL, false));

      if (IsDlgButtonChecked(mhMain, IDC_UDP) == BST_CHECKED)
         filter = new UDPFilter(endpoint1, endpoint2);
      else
         filter = new TCPFilter(endpoint1, endpoint2);
      filter->Enable(IsDlgButtonChecked(mhMain, IDC_ENABLED) == BST_CHECKED);
      filter->List(IsDlgButtonChecked(mhMain, IDC_LISTED) == BST_CHECKED);
      filter->SetLatencyMean(GetDlgItemInt(mhMain, IDC_MEAN_EDIT, NULL, false));
      filter->SetLatencyVariance(GetDlgItemInt(mhMain, IDC_VARIANCE_EDIT, NULL, false));
      filter->SetPacketLoss(GetDlgItemInt(mhMain, IDC_LOSS_EDIT, NULL, false));
      filter->SetDuplicationProbability(GetDlgItemInt(mhMain, IDC_DUPLICATION_EDIT, NULL, false));
      filter->SetReorderProbability(GetDlgItemInt(mhMain, IDC_REORDER_EDIT, NULL, false));

      AddFilter(filter);
      break;

   case IDC_REMOVE_FILTER:
      if (notify != BN_CLICKED)
         break;
      
      filter = (Filter *) mpFilterList->RemoveItem(mpFilterList->GetSelectedIndex());
      if (filter != NULL)
         RemoveFilter(filter);
      break;

   case IDC_ADD_REVERSE:
      if (notify != BN_CLICKED)
         break;

      filter = (Filter *) mpFilterList->GetSelectedItem();
      if (filter != NULL)
      {
         new_filter = filter->Clone();
         new_filter->SwapEndpoints();
         AddFilter(new_filter);
      }
      
      break;
      
   case IDC_ENABLED:
      if (notify != BN_CLICKED)
         break;

      if (mpCurrentFilter != NULL)
         mpCurrentFilter->Enable(IsDlgButtonChecked(mhMain, IDC_ENABLED) == BST_CHECKED);
      break;

   case IDC_LISTED:
      if (notify != BN_CLICKED)
         break;

      if (mpCurrentFilter != NULL)
         mpCurrentFilter->List(IsDlgButtonChecked(mhMain, IDC_LISTED) == BST_CHECKED);
      break;

   case IDC_MEAN_EDIT:
      if (notify != EN_CHANGE)
         break;

      value = GetDlgItemInt(mhMain, id, NULL, false);
      Trackbar_SetPos(GetDlgItem(mhMain, IDC_LATENCY_MEAN), value, true);
      if (mpCurrentFilter != NULL)
         mpCurrentFilter->SetLatencyMean(value);
      break;

   case IDC_VARIANCE_EDIT:
      if (notify != EN_CHANGE)
         break;

      value = GetDlgItemInt(mhMain, id, NULL, false);
      Trackbar_SetPos(GetDlgItem(mhMain, IDC_LATENCY_VARIANCE), value, true);
      if (mpCurrentFilter != NULL)
         mpCurrentFilter->SetLatencyVariance(value);
      break;

   case IDC_UDP:
      if (notify == BN_CLICKED)
         SetCurrentProtocol(UDP);
      break;

   case IDC_TCP:
      if (notify == BN_CLICKED)
         SetCurrentProtocol(TCP);
      break;
      
   case IDC_LOSS_EDIT:
      if (notify != EN_CHANGE)
         break;

      if (mpCurrentFilter != NULL)
         mpCurrentFilter->SetPacketLoss(GetDlgItemInt(mhMain, IDC_LOSS_EDIT,
                                                     NULL, false));      
      break;

   case IDC_REORDER_EDIT:
      if (notify != EN_CHANGE)
         break;

      if (mpCurrentFilter != NULL)
         mpCurrentFilter->SetReorderProbability(GetDlgItemInt(mhMain, IDC_REORDER_EDIT,
                                                             NULL, false));      
      break;
      
   case IDC_DUPLICATION_EDIT:
      if (notify != EN_CHANGE)
         break;

      if (mpCurrentFilter != NULL)
         mpCurrentFilter->SetDuplicationProbability(GetDlgItemInt(mhMain, IDC_DUPLICATION_EDIT,
                                                                 NULL, false));      
      break;

   case IDC_ADD_HOST:
      if (notify != BN_CLICKED)
         break;

      IPControl_GetAddress(GetDlgItem(mhMain, IDC_HOST_ADDRESS), &address);
      host = new Host(address);
      host->SetSendBandwidth(GetDlgItemInt(mhMain, IDC_SEND_EDIT, NULL, false));
      host->SetReceiveBandwidth(GetDlgItemInt(mhMain, IDC_RECEIVE_EDIT, NULL, false));
      AddHost(host);
      break;
      
   case IDC_REMOVE_HOST:
      if (notify != BN_CLICKED)
         break;
      
      host = (Host *) mpHostList->RemoveItem(mpHostList->GetSelectedIndex());
      if (host != NULL)
         RemoveHost(host);
      break;
      
   case IDC_SEND_EDIT:
      if (notify != EN_CHANGE)
         break;

      value = GetDlgItemInt(mhMain, id, NULL, false);
      Trackbar_SetPos(GetDlgItem(mhMain, IDC_SEND_SLIDER), value, true);
      
      if (mpCurrentHost != NULL)
         mpCurrentHost->SetSendBandwidth(value);
      
      break;

   case IDC_RECEIVE_EDIT:
      if (notify != EN_CHANGE)
         break;

      value = GetDlgItemInt(mhMain, id, NULL, false);
      Trackbar_SetPos(GetDlgItem(mhMain, IDC_RECEIVE_SLIDER), value, true);
      
      if (mpCurrentHost != NULL)
         mpCurrentHost->SetReceiveBandwidth(value);
      
      break;

   case IDC_PPP:
      if (notify != BN_CLICKED)
         break;

      if (mpCurrentHost != NULL)
         mpCurrentHost->SetUsingPPP(IsDlgButtonChecked(mhMain, id) == BST_CHECKED);
      break;
      
   case IDC_COMPRESSION:
      if (notify != EN_CHANGE)
         break;

      value = GetDlgItemInt(mhMain, id, NULL, false);
      
      if (mpCurrentHost != NULL)
         mpCurrentHost->SetCompression(value);
      
      break;
      
   case IDC_PACKET_TOGGLE:
      if (notify != BN_CLICKED)
         break;
      mboolDisplayPackets = (IsDlgButtonChecked(mhMain, id) == BST_CHECKED);
      break;

   case IDC_LOG:
      if (notify != BN_CLICKED)
         break;
      SetLogging(IsDlgButtonChecked(mhMain, id) == BST_CHECKED);
      break;
      
   case IDC_PACKET_CLEAR:
      if (notify != BN_CLICKED)
         break;
      mpPacketList->Clear();
      break;

   case IDC_BW1:
   case IDC_BW2:
   case IDC_BW3:
   case IDC_BW4:
   case IDC_BW5:
   case IDC_BW6:      
   case IDC_BW7:
   case IDC_BW8:
      if (notify == BN_CLICKED)
         BandwidthButtonPressed(id);
      break;
   }      
}
/***************************************************************/
void Interface::On_WM_NOTIFY(HWND hwnd, int32 id, NMHDR *nmhdr)
{
   switch (nmhdr->idFrom)
   {
   case IDC_FILTER_LIST:
      FilterListNotify(nmhdr);
      break;

   case IDC_HOST_LIST:
      HostListNotify(nmhdr);
      break;
   }
}
/***************************************************************/
void Interface::On_WM_HSCROLL(HWND hwnd, HWND hwndCtl, UINT code, int32 pos)
{
   if (hwndCtl == GetDlgItem(hwnd, IDC_LATENCY_MEAN))
   {
      int32 mean = Trackbar_GetPos(hwndCtl);
      SetDlgItemInt(hwnd, IDC_MEAN_EDIT, mean, false);
      
      if (mpCurrentFilter != NULL)
         mpCurrentFilter->SetLatencyMean(mean);
   }

   if (hwndCtl == GetDlgItem(hwnd, IDC_LATENCY_VARIANCE))
   {
      int32 variance = Trackbar_GetPos(hwndCtl);
      SetDlgItemInt(hwnd, IDC_VARIANCE_EDIT, variance, false);
      
      if (mpCurrentFilter != NULL)
         mpCurrentFilter->SetLatencyVariance(variance);
   }

   if (hwndCtl == GetDlgItem(hwnd, IDC_SEND_SLIDER))
   {
      int32 bandwidth = Trackbar_GetPos(hwndCtl);
      SetDlgItemInt(hwnd, IDC_SEND_EDIT, bandwidth, false);
      if (mpCurrentHost != NULL)
         mpCurrentHost->SetSendBandwidth(bandwidth);
   }

   if (hwndCtl == GetDlgItem(hwnd, IDC_RECEIVE_SLIDER))
   {
      int32 bandwidth = Trackbar_GetPos(hwndCtl);
      SetDlgItemInt(hwnd, IDC_RECEIVE_EDIT, bandwidth, false);
      if (mpCurrentHost != NULL)
         mpCurrentHost->SetReceiveBandwidth(bandwidth);
   }   
}
/***************************************************************/
void Interface::On_WM_TIMER(HWND hwnd, UINT id)
{
   DisplayBandwidthUsage();
}
/***************************************************************/
long Interface::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch (msg)
   {
      HANDLE_MSG(hwnd, WM_COMMAND, On_WM_COMMAND);
      HANDLE_MSG(hwnd, WM_HSCROLL, On_WM_HSCROLL);
      HANDLE_MSG(hwnd, WM_TIMER, On_WM_TIMER);
      HANDLE_MSG(hwnd, WM_SIZE, mpResizer->OnSize);
      
   case WM_NOTIFY:
      HANDLE_WM_NOTIFY(hwnd, wParam, lParam, On_WM_NOTIFY);
      return 0;

   case WM_GETMINMAXINFO:
       MINMAXINFO *mminfo;
       // Don't let the window get too small
       mminfo = (MINMAXINFO *) lParam;
       mminfo->ptMinTrackSize.x = MIN_WINDOW_WIDTH;
       mminfo->ptMinTrackSize.y = MIN_WINDOW_HEIGHT;
       return false;

   case WM_SOCKET_NOTIFY:
      app->network->On_WM_SOCKET_NOTIFY((SOCKET) wParam,
                                        WSAGETSELECTEVENT(lParam),
                                        WSAGETSELECTERROR(lParam));
      break;
      
   case WM_DESTROY:
      if (mpResizer != NULL)
      {
         delete mpResizer;
         mpResizer = NULL;
      }

      // Save window size and position
      RECT rect;
      GetWindowRect(mhMain, &rect);
      char buf[100];
      sprintf(buf, "%d,%d,%d,%d",
              rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
      app->config->SetString(CONFIG_WINDOW_POS, buf);

      // Save list view column widths
      SaveColumnWidths(mpFilterList, CONFIG_FILTER_WIDTHS);
      SaveColumnWidths(mpHostList,   CONFIG_HOST_WIDTHS);
      SaveColumnWidths(mpPacketList, CONFIG_PACKET_WIDTHS);
      
      PostQuitMessage(0);
      break;
   }
   
   return DefWindowProc(hwnd, msg, wParam, lParam);
}
/***************************************************************/
long WINAPI Interface::StaticWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   if (app->inter != NULL)
      return app->inter->WindowProc(hwnd, msg, wParam, lParam);
   return DefWindowProc(hwnd, msg, wParam, lParam);
}
/***************************************************************/
void Interface::SetCurrentProtocol(Protocol protocol)
{
   EnableWindow(GetDlgItem(mhMain, IDC_LOSS_EDIT), protocol == UDP);
   EnableWindow(GetDlgItem(mhMain, IDC_REORDER_EDIT), protocol == UDP);
   EnableWindow(GetDlgItem(mhMain, IDC_DUPLICATION_EDIT), protocol == UDP);
}
/***************************************************************/
void Interface::SaveColumnWidths(CListView *pListView, const char *pName)
{
   int i, num_columns = pListView->GetNumColumns();
   char buf[500];
   buf[0] = 0;

   for (i=0; i < num_columns; ++i)
   {
      char temp[100];
      sprintf(temp, "%d", pListView->GetColumnWidth(i));
      if (i > 0)
         strcat(buf, ",");
      strcat(buf, temp);
   }

   app->config->SetString(pName, buf);
}
/***************************************************************/
void Interface::SetColumnWidths(CListView *pListView, const char *pName)
{
   char *pValues;
   if (true == app->config->LookupString(pName, &pValues))
   {
      int width, index = 0;;
      while (1 == sscanf(pValues, "%d", &width))
      {
         pListView->SetColumnWidth(index, width);
         ++index;

         // Move pointer to next number
         while (*pValues != ',' && *pValues != 0)
            ++pValues;

         if (*pValues == ',')
            ++pValues;
      }
   }
}
