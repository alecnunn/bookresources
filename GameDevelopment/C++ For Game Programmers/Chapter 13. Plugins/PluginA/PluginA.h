
#ifndef PLUGINA_H_
#define PLUGINA_H_


#include "..\Plugin.h"


class PluginA : public IPlugin
{
  public:
    // Constructors and destructor
    PluginA( PluginManager & mgr );

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


#endif  // PLUGINA_H_
