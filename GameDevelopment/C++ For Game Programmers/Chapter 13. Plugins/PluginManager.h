
#ifndef PLUGINMANAGER_H_
#define PLUGINMANAGER_H_


class IPlugin;


class PluginManager
{
public:
    static PluginManager & GetInstance(void);

    void LoadPlugins ( const std::string & strDir );
    void UnloadAll ( void );

    int GetNumPlugins ( void ) const;
    IPlugin * GetPlugin ( int nIndex );

private:
    PluginManager();
    void GetFilenames ( const std::string & dir, 
                        std::vector<std::string> & filenames ) const;
    bool LoadPlugin ( const std::string & filename );


    struct PluginInfo
    {
        IPlugin * pPlugin;
        HMODULE   hDll;
    };

    std::vector<PluginInfo> m_plugins;

    static PluginManager * s_pInstance;
};


#endif