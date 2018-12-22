
#ifndef PLUGINB_H_
#define PLUGINB_H_


#include "..\Plugin.h"


class PluginB : public IPlugin
{
  public:
    // Constructors and destructor
    PluginB( PluginManager & mgr );

    virtual bool Initialize();
    virtual bool Shutdown();
    virtual void About(HWND hParent);
    
    virtual const std::string & GetName();
    virtual const std::string & GetExportName();

    virtual bool Export( const char * pData );

  private:
      PluginManager &  m_mgr;
      std::string      m_Name;
      std::string      m_ExportName;
};


#endif  // PLUGINB_H_
