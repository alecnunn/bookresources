
#ifndef RTTI_H_
#define RTTI_H_


class RTTI 
{
  public:
    RTTI(const std::string & className) : 
        m_className(className), m_pBaseRTTI(NULL) {}
    RTTI(const std::string & className, const RTTI & baseRTTI) :
        m_className(className), m_pBaseRTTI(&baseRTTI) {}

    const std::string & GetClassName() const { return m_className; }
    bool IsExactly(const RTTI & rtti) const { return (this == &rtti); }
    bool DerivesFrom (const RTTI & rtti) const;

  private:
    // Prevent copying
    RTTI( const RTTI & obj );
    RTTI & operator=( const RTTI & obj );

    const std::string m_className;
    const RTTI *      m_pBaseRTTI;
};


#define RTTI_DECL \
    public: \
        virtual const RTTI & GetRTTI() { return rtti; } \
        static const RTTI rtti;

#define RTTI_IMPL_NOPARENT(name) \
    const RTTI name::rtti(#name);

#define RTTI_IMPL(name,parent) \
    const RTTI name::rtti(#name, parent::rtti);


#endif  // _RTTI_H_
