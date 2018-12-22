//
// Installation routines for Win32
// Copyright (c) Glenn Fiedler (ptc@gaffer.org)
// This source code is in the public domain
//

// include files
#include "Routines.h"




BOOL InstallDLL(WORD resource,const char name[],BOOL force)
{
    // get the system directory
    char system_directory[MAX_PATH];
    GetSystemDirectory(system_directory,MAX_PATH);
    //wsprintf(system_directory,"%s","D:\\Gaffer\\ptc\\distributions\\win32\\install\\system");

    // setup the source filename
    char source[MAX_PATH];
    wsprintf(source,"%s\\~%s",system_directory,name);

    // setup the destination filename
    char destination[MAX_PATH];
    wsprintf(destination,"%s\\%s",system_directory,name);

    // save the resource to the source file
    SaveResourceAsFile(resource,source);

    // check if the destination file exists
    if (FileExists(destination))
    {
        // check force
        if (force)
        {
            // forced dll install
            if (!ReplaceDLL(source,destination)) goto failure;
        }
        else
        {
            // compare file version resources
            if (CompareDLL(source,destination)!=OLDER)
            {
                // this version is newer or as new so install it
                if (!ReplaceDLL(source,destination)) goto failure;
            }
            else
            {
                // the version already installed is newer

                // delete source file
                FileDelete(source);
            }
        }

    }
    else
    {
        // just move the file from source to destination
        if (!FileMove(source,destination)) goto failure;
    }

    // success
    return TRUE;

failure:

    // delete source file
    FileDelete(source);

    // failure
    return FALSE;
}


DLL_COMPARE CompareDLL(const char source[],const char destination[])
{
    // todo: compare version numbers!

    // dll to install is newer than existing dll
    return NEWER;
}


BOOL ReplaceDLL(const char source[],const char destination[])
{
    // copy file to destination location
    if (!(FileCopy(source,destination)))
    {
        // copy failed, we must schedule a dll replace operation
        if (!ScheduleDLL(source,destination)) return FALSE;
    }
    else
    {
        // delete source file
        FileDelete(source);
    }

    // success
    return TRUE;
}


BOOL ScheduleDLL(const char source[],const char destination[])
{
    /*
    // get the windows directory
    char windows_directory[MAX_PATH];
    //GetWindowsDirectory(system_directory,MAX_PATH);
    wsprintf(windows_directory,"%s","D:\\Gaffer\\ptc\\distributions\\win32\\install\\system");

    // setup wininit.ini file name
    char wininit_file[MAX_PATH];
    wsprintf(wininit_file,"%s\\wininit.ini",windows_directory);
    
    // setup wininit.ini data to save
    char wininit_data[MAX_PATH*2+1024];
    wsprintf(wininit_data,"[rename]\n%s %s",source,destination);

    // open "wininit.ini" file
    HANDLE hFile = CreateFile(wininit_file,GENERIC_WRITE,0,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

    // append data to "wininit.ini"
    DWORD bytes_written = 0;
    BOOL result = WriteFile(hFile,wininit_data,strlen(wininit_data)+1,&bytes_written,NULL);
    if (!result || bytes_written!=strlen(wininit_data)+1)
    {
        // close file
        CloseHandle(hFile);

        // failure
        return FALSE;
    }

    // close file
    CloseHandle(hFile);

    // success
    return TRUE;
    */

    // failure
    return FALSE;
}




BOOL SaveResourceAsFile(WORD resource,const char filename[])
{
    // get hInstance of this process
    HINSTANCE hInstance = GetModuleHandle(0);

    // find file resource
    HRSRC hResource = FindResource(hInstance,MAKEINTRESOURCE(resource),"FILE");
    
    // check resource
    if (hResource==0)
    {
        // failure
        return FALSE;
    }

    // get resource data
    HGLOBAL hResourceData = LoadResource(hInstance,hResource);

    // create file
    HANDLE hFile = CreateFile(filename,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);

    // check file creation
    if (hFile==INVALID_HANDLE_VALUE)
    {
        // free resource data
        FreeResource(hResourceData);

        // failure
        return FALSE;
    }

    // get size of resource
    DWORD size = SizeofResource(hInstance,hResource);

    // check size
    if (!size)
    {
        // free resource data
        FreeResource(hResourceData);

        // close file
        CloseHandle(hFile);

        // failure
        return FALSE;
    }
    
    // write resource data to file
    DWORD bytes_written = 0;
    DWORD result = WriteFile(hFile,hResourceData,size,&bytes_written,0);
    if (!result || bytes_written!=size)
    {
        // free resource data
        FreeResource(hResourceData);

        // close file
        CloseHandle(hFile);

        // failure
        return FALSE;
    }

    // close file
    CloseHandle(hFile);

    // free resource data
    FreeResource(hResourceData);
    
    // finished
    return TRUE;
}




BOOL FileExists(const char filename[])
{
    // create file
    HANDLE hFile = CreateFile(filename,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);

    // check file handle
    if (hFile==INVALID_HANDLE_VALUE) return FALSE;

    // close file
    CloseHandle(hFile);

    // file exists
    return TRUE;
}


BOOL FileCopy(const char source[],const char destination[])
{
    // copy file from source to destination
    return CopyFile(source,destination,FALSE);
}


BOOL FileMove(const char source[],const char destination[])
{
    // move file from source to destination
    return MoveFile(source,destination);
}


BOOL FileDelete(const char filename[])
{
    // delete file
    return DeleteFile(filename);
}
