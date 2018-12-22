#include "main.h"

app_type *app;

const char *config_file = "config.txt";

/********************************************************************/
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
   app_type a;
   app = &a;
   memset(app, 0, sizeof app_type);

   srand(time(NULL));
   
   app->debug   = new Debug;
   app->config  = new Config;

   if (!app->config->Load(config_file))
      dprintf("Error in config file\n");

   app->timer   = new CTimer;
   app->inter   = new Interface;
   app->network = new Network;

   app->network->ReadFromConfig();
   
   app->inter->Run();

   delete app->network;
   delete app->inter;
   delete app->timer;

   if (!app->config->Save(config_file))
      dprintf("Error writing config file\n");
   
   delete app->config;
   delete app->debug;
   
   return 0;
}
