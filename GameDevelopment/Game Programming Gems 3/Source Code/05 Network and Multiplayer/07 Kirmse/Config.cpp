#include "main.h"

/***************************************************************/
Config::Config()
{
}
/***************************************************************/
Config::~Config()
{
}
/***************************************************************/
bool Config::Load(const char *filename)
{
   FILE *f = fopen(filename, "r");
   if (f == NULL)
   {
      return false;
   }

   char line[MAX_LINE_LENGTH], name[MAX_LINE_LENGTH], value[MAX_LINE_LENGTH];
   while (fgets(line, sizeof line, f) != NULL)
   {
      if (sscanf(line, "%s = %s", name, value) == 2)
         SetString(name, value);
   }

   fclose(f);
   
   return true;
}
/***************************************************************/
bool Config::Save(const char *filename)
{
   FILE *f = fopen(filename, "w");
   if (f == NULL)
   {
      return false;
   }

   char line[MAX_LINE_LENGTH];
   string_map_type::iterator it;
   for (it = mStore.begin(); it != mStore.end(); ++it)
   {
      sprintf(line, "%s = %s\n", (*it).first.c_str(), (*it).second.c_str());
      if (fwrite(line, 1, strlen(line), f) == 0)
         return false;
   }

   fclose(f);
   
   return true;
}
/***************************************************************/
void Config::SetString(const char *name, char *value)
{
   mStore[name] = value;
}
/***************************************************************/
void Config::SetInt32(const char *name, int32 value)
{
   char str[50];
   sprintf(str, "%d", value);
   SetString(name, str);
}
/***************************************************************/
void Config::SetBool(const char *name, bool value)
{
   SetInt32(name, value);
}
/***************************************************************/
bool Config::LookupString(const char *name, char **value)
{
   string_map_type::iterator it = mStore.find(name);
   if (it == mStore.end())
      return false;

   *value = (char *) (*it).second.c_str();
   return true;
}
/***************************************************************/
bool Config::LookupInt32(const char *name, int32 *value)
{
   string_map_type::iterator it = mStore.find(name);
   if (it == mStore.end())
      return false;

   if (sscanf((*it).second.c_str(), "%d", value) != 1)
      return false;
   return true;
}
/***************************************************************/
bool Config::LookupBool(const char *name, bool *value)
{
   int32 v;
   if (!LookupInt32(name, &v))
      return false;

   *value = (v != 0);
   return true;
}
