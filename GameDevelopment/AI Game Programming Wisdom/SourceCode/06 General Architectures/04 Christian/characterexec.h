

#ifndef _CHARACTOREXEC_H_
#define _CHARACTEREXEC_H_

class CharacterExec : public IEExec
{

public:
    Character();

    virtual void init   ();
    virtual bool start  ();
    virtual bool update () = 0;
    virtual bool finish ();
    virtual void reset  ();

    IEOwner * getOwner  ();

    const char * getName() = 0;

protected:
    Character * m_owner;    // owner of this exec type
};

#endif