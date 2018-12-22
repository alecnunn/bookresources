
#ifndef STREAMFILE_H_
#define STREAMFILE_H_

#include <string>
#include "Stream.h"


class StreamFile : public IStream
{
public:
    StreamFile();
    virtual ~StreamFile();

    virtual int  Read (int bytes, void * pBuffer);
    virtual int  Write (int bytes, const void * pBuffer);

    virtual bool SetCurPos (int pos);
    virtual int  GetCurPos ();

    bool OpenRead (const std::string & filename);
    bool OpenWrite (const std::string & filename);
    void Close ();

private:
    FILE * m_file;
    bool   m_bReadMode;
    bool   m_bWriteMode;
};


#endif // STREAMFILE_H_