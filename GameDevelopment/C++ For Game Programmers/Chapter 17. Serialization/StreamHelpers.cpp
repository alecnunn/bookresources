
#include "stream.h"


int ReadInt (IStream & stream)
{
    int n = 0;
    stream.Read(sizeof(int), (void *)&n);
    return n;
}

float ReadFloat (IStream & stream)
{
    float f = 0;
    stream.Read(sizeof(float), (void *)&f);
    return f;
}

bool WriteInt (IStream & stream, int n)
{
    int numWritten = stream.Write(sizeof(int), &n);
    return (numWritten == sizeof(int));
}

bool WriteFloat (IStream & stream, float f)
{
    int numWritten = stream.Write(sizeof(float), &f);
    return (numWritten == sizeof(float));
}

std::string ReadString (IStream & stream)
{
    int nLength = ReadInt(stream);
    char txt[512];
    stream.Read(nLength, txt);
    return std::string(txt);
}


bool WriteString (IStream & stream, const std::string & str)
{
    int nLength = str.length()+1;
    if (!WriteInt (stream, nLength))
        return false;

    return (stream.Write(nLength, str.c_str()) == nLength);
}
