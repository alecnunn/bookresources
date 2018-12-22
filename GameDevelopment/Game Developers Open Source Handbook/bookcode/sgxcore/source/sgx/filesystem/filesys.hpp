#ifndef SGX_FILESYSTEM_FILESYS_HPP
#define SGX_FILESYSTEM_FILESYS_HPP	1


//class CSGXDevice;
//class CSGXFileEntry;


class CSGXFileSystem
{
    public:
        CSGXFileEntry * MakeNewFileEntry(const sgxString &path);

        tBOOL           IsFile(const sgxString &path) const;
        CSGXFileEntry * GetFileEntry(const sgxString &path) const;
        CSGXFileEntry * GetChildEntry(CSGXFileEntry *pCurr, const sgxString &filename) const;
        CSGXFileEntry * CreateFileEntry(const sgxString &parent, const sgxString &name);
        tBOOL           GetParent(sgxString &parent, const sgxString &location) const;
        void            GetFullPath(sgxString &fullpath, const sgxString &path) const;
        void            GetBaseName(sgxString &basename, const sgxString &path) const;
        tBOOL           Mount(CSGXDevice &Device, const sgxString &location);
        tDISC_SIZE      GetFileSize(const sgxString &path) const;

        CSGXFileEntry * CreateNewFile(const CSGXFileSysData &FileData);
        CSGXFileEntry * CreateNewDirectory(const sgxString &PathName);

        void            ShowHeirarchy(CSGXFileEntry *pCurr, tMEMSIZE iDepth);
        void            ShowHeirarchy();

        static	CSGXFileSystem * Get();

    protected:	// Can not use private, as it prevents derivation
        CSGXFileSystem();
        virtual ~CSGXFileSystem();

        static CSGXFileSystem      *sm_pSingleton;

        CSGXFileEntry              *m_pRoot;
        sgxString                  m_CurrentPath;

};

#endif /* SGX_FILESYSTEM_FILESYS_HPP */
