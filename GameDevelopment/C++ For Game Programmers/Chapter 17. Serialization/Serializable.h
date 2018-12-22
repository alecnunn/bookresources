
#ifndef SERIALIZABLE_H_
#define SERIALIZABLE_H_


class IStream;


class ISerializable
{
public:
    virtual ~ISerializable() {};
    virtual bool Write(IStream & stream) const = 0;
    virtual bool Read(IStream & stream) = 0;
    virtual void Fixup() = 0;
};



#endif // SERIALIZABLE_H_