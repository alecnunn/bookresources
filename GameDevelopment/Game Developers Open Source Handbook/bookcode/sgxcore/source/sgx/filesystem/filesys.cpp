#include "sgx/core/core.hpp"
#include "sgx/filesystem/filesystem.hpp"

CSGXFileSystem *CSGXFileSystem::sm_pSingleton = NULL;

CSGXFileSystem::CSGXFileSystem()
{
    m_CurrentPath = "/";

    m_pRoot = new CSGXFileEntry("/");
    m_pRoot->m_Data.m_Name = "/";
}


CSGXFileSystem *CSGXFileSystem::Get()
{
    if (sm_pSingleton == NULL) {
        sm_pSingleton = new CSGXFileSystem;
    }

    return sm_pSingleton;
}

CSGXFileSystem::~CSGXFileSystem()
{
	delete m_pRoot;
    delete sm_pSingleton;
}

CSGXFileEntry *CSGXFileSystem::GetChildEntry(CSGXFileEntry *pCurr, const sgxString &filename) const
{
    sgxAssert(pCurr);
    if (!pCurr) {
        return NULL;
    }

    CSGXFileEntry *ptr = pCurr->m_pFirstChild;

    while(ptr) {
        if (sgxStricmp(ptr->m_Data.m_Name.c_str(), filename.c_str())==0) {
            return ptr;
        }

        ptr = ptr->m_pNextSibling;
    }

    return NULL;
}

CSGXFileEntry *CSGXFileSystem::CreateNewFile(const CSGXFileSysData &FileData)
{
    sgxString basename;
    sgxString parent;

    GetBaseName(basename, FileData.m_Name);
    GetParent(parent, FileData.m_Name);

    CSGXFileEntry *pNewEntry = CreateFileEntry(parent, basename);

    if (!pNewEntry) {
        return NULL;
    }
    // Return error at lowest level, report at the highest
    // true in call cases; but xp code may have more error levels of indirection at
    // the bottom end
    CSGXFileEntry *pParentEntry = GetFileEntry(parent);

    pParentEntry->m_pFSDevice->CreateNewFile(pNewEntry);//, pParentState);
    pNewEntry->m_Data = FileData;
    pNewEntry->m_Data.m_Name = basename;

    return pNewEntry;
}


CSGXFileEntry *CSGXFileSystem::CreateFileEntry(const sgxString &parent, const sgxString &name)
{
    CSGXFileEntry *pParent = GetFileEntry(parent);

    if (!pParent) {
        // Can't create file, since parent doesn't exist
        return NULL;
    }

    CSGXFileEntry *pThis = new CSGXFileEntry(name);

    // Append latest entry to start of list
    pThis->m_pNextSibling = pParent->m_pFirstChild;
    pParent->m_pFirstChild = pThis;

    return pThis;
}

CSGXFileEntry *CSGXFileSystem::GetFileEntry(const sgxString &path) const
{
    // Returns the pFileEntry for the file/directory specified.
    // or NULL if the file doesn't exist.
    sgxString fullpath;

    // Easy early out case
    if (path == "/") {
        return m_pRoot;
    }

    GetFullPath(fullpath, path);

    CSGXFileEntry *pCurr = m_pRoot;
    CSGXFileEntry *pNextEntry;

    do {
        // Retrieve next name in path
        // current path always begins with a /
        sgxString name;
        sgxString::size_type pos;

        pos = fullpath.find_first_of('/', 1);
        if (pos == sgxString::npos) {
            name = fullpath.substr(1);
        } else {
            name = fullpath.substr(1, pos-1);
        }

        // Look for this name in the current layer
        // of the hierarchy
        pNextEntry = GetChildEntry(pCurr, name);
        if (!pNextEntry) {
            return NULL;
        }

        // The entry exists, and (if there's no more path to
        // search) we've found our man!
        if (pos == sgxString::npos) {
            return pNextEntry;
        }

        // We have an entry but it may be a file, or 
		// a directory, but we can only recurse down
		// into directories. So check for that...
        if (pNextEntry->IsFile()) {
            return NULL;
        }

        // Change directory...
        pCurr = pNextEntry;
    
        // ... move onto the next part of the path (keep the / for code above)
        fullpath = fullpath.substr(pos); 

    } while(TRUE);

return NULL;
}

tBOOL CSGXFileSystem::IsFile(const sgxString &path) const
{
    CSGXFileEntry *ptr = GetFileEntry(path);
    if (ptr && ptr->IsFile()) {
        return TRUE;
    } else {
        return FALSE;
    }
}


void CSGXFileSystem::ShowHeirarchy(CSGXFileEntry *pCurr, tMEMSIZE iDepth)
{
    sgxString str;
    sgxString file_stub="|", dir_stub="+";

    for(tMEMSIZE i=0; i<iDepth; i++) {
        file_stub += "---";
        dir_stub  += "   ";
    }

    CSGXFileEntry *ptr = pCurr;

    // List files first
    while(ptr) {
        if (ptr->IsFile()) {
            str = file_stub + "> " + ptr->m_Data.m_Name;
            sgxTrace(str.c_str());
        }
        ptr = ptr->m_pNextSibling;
    }

    // Then directories
    ptr = pCurr;
    while(ptr) {
        if (!ptr->IsFile()) {
            str = dir_stub + "+ " + ptr->m_Data.m_Name;
            sgxTrace(str.c_str());
        }
        if (ptr->m_pFirstChild) {
            ShowHeirarchy(ptr->m_pFirstChild, iDepth+1);
        }
        ptr = ptr->m_pNextSibling;
    }
}

void CSGXFileSystem::ShowHeirarchy()
{
    ShowHeirarchy(m_pRoot, 0);
}


tBOOL CSGXFileSystem::GetParent(sgxString &parent, const sgxString &location) const
{
    sgxString fullpath;
    
    GetFullPath(fullpath, location);

// Because we create a full path there must always be
    // at least one / present (even if it's the root)
    int pos = fullpath.find_last_of('/');

    if (pos == 0) { // parent is the root
        parent = "/"; 
    } else {
        parent = fullpath.substr(0, pos);
    }

    return true;
}

void CSGXFileSystem::GetBaseName(sgxString &basename, const sgxString &path) const
{
int pos = path.find_last_of('/');

    basename = path.substr(pos+1, sgxString::npos);
}

void CSGXFileSystem::GetFullPath(sgxString &fullpath, const sgxString &path) const
{
    // Build a full path. This guarantees we always
    // start searching from the root.
    if (path[0] == '/') {
        fullpath = path;
    } else {
        fullpath = m_CurrentPath + path;
    }
    // We could also add further validation here
}

CSGXFileEntry *CSGXFileSystem::MakeNewFileEntry(const sgxString &path)
{
    sgxString fullpath;

    GetFullPath(fullpath, path);

    if (GetFileEntry(fullpath)) {
        return NULL;
    }

    // create dir with base name
    sgxString basename;
    sgxString parent;

    GetBaseName(basename, path);
    GetParent(parent, path);

    return CreateFileEntry(parent, basename);
}

CSGXFileEntry *CSGXFileSystem::CreateNewDirectory(const sgxString &PathName)
{
    return MakeNewFileEntry(PathName);
}

tBOOL CSGXFileSystem::Mount(CSGXDevice &Device, const sgxString &location)
{
    CSGXFileEntry *pMount = MakeNewFileEntry(location);

    if (!pMount) {
        sgxTrace("Count create mount point at '%s'", location);
        return false;
    }

    // attach to device
    sgxString fullpath;
    GetFullPath(fullpath, location);

    pMount->m_pFSDevice = &Device;

    return Device.Mount(pMount, fullpath);
}

tDISC_SIZE  CSGXFileSystem::GetFileSize(const sgxString &path) const
{
CSGXFileEntry *ptr = GetFileEntry(path);

    if (ptr) {
        return ptr->GetFileSize();
    } else {
        return (tDISC_SIZE)-1;
    }

}

