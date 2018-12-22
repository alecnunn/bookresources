/*
 * Config
 *
 * Load/save config from a file.
 */

#ifndef _CONFIG_H
#define _CONFIG_H

class Config
{
public:

   Config();
   ~Config();

   enum
   {
      MAX_LINE_LENGTH = 500,
   };
   
   bool Load(const char *filename);
   bool Save(const char *filename);

   void SetString(const char *name, char *value);
   void SetInt32(const char *name, int32 value);
   void SetBool(const char *name, bool value);

   // If name is found in the config, set value to it and return true.
   bool LookupString(const char *name, char **value); 
   bool LookupInt32(const char *name, int32 *value); 
   bool LookupBool(const char *name, bool *value); 
   
protected:
   typedef map<string, string> string_map_type;
   string_map_type mStore;
};

#endif
