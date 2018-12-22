
#include "StreamFile.h"
#include <stdio.h>
#include <assert.h>


StreamFile::StreamFile()
{
    m_file = NULL;
    m_bReadMode = false;
    m_bWriteMode = false;
}


StreamFile::~StreamFile()
{
    Close();
}


bool StreamFile::OpenRead (const std::string & filename)
{
    if (m_file != NULL)
        return false;

    m_file = fopen(filename.c_str(), "r");
    m_bReadMode = (m_file != NULL);
    return m_bReadMode;
}


bool StreamFile::OpenWrite (const std::string & filename)
{
    if (m_file != NULL)
        return false;

    m_file = fopen(filename.c_str(), "w");
    m_bWriteMode = (m_file != NULL);
    return m_bWriteMode;
}

void StreamFile::Close ()
{
    if (m_file != NULL)
        fclose(m_file);

    m_bWriteMode = false;
    m_bReadMode  = false;
}


int StreamFile::Read (int bytes, void * pBuffer)
{
    assert (pBuffer != NULL);

    if (!m_bReadMode)
        return 0;

    return fread(pBuffer, 1, bytes, m_file);
}


int StreamFile::Write (int bytes, const void * pBuffer)
{
    assert (pBuffer != NULL);

    if (!m_bWriteMode)
        return 0;

    return fwrite(pBuffer, 1, bytes, m_file);
}


bool StreamFile::SetCurPos (int pos)
{
    if (!m_bReadMode && !m_bWriteMode)
        return false;

    return (fseek(m_file, pos, SEEK_SET) == 0);
}


int StreamFile::GetCurPos ()
{
    if (!m_bReadMode && !m_bWriteMode)
        return 0;

    return ftell(m_file);
}

