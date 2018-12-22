#ifndef JOURNALING__HEADER
#define JOURNALING__HEADER

#include <fstream>

///////////////////////////////////////////////////////////////////////////////
//
// Journaling Services
// by Eric Robert
//
///////////////////////////////////////////////////////////////////////////////

#ifndef DEFAULT_NAME
#define DEFAULT_NAME        "Unknown"
#endif

#ifndef DEFAULT_DESCRIPTION
#define DEFAULT_DESCRIPTION "There is no description for this item."
#endif

class Switch;
class SwitchBox;

///////////////////////////////////////////////////////////////////////////////
//
// Journal
//
// This base class implements all streaming operations. Instances are shared
// resources and must be referenced accordingly.
//
///////////////////////////////////////////////////////////////////////////////

class Journal : public std::ostream
{
    int myCount;

public:

    enum {
        CONTINUE,
        ABORT,
        BREAK
    };

    // Interface //////////////////////////////////////////////////////////////

    Journal() : std::ostream(0) {
        myCount = 0;
    }

    void addReference() {
        ++myCount;
    }

    void releaseReference() {
        --myCount;

        if(!myCount) {
            delete this;
        }
    }

    virtual void beginReport();
    virtual char endReport();

    static Journal * getDefault() {
        if(ourDefaultInstance) {
            return ourDefaultInstance;
        }

        return staticFactory();
    }

private:

    static Journal * ourDefaultInstance;
    static Journal * staticFactory();
};

///////////////////////////////////////////////////////////////////////////////
//
// SwitchBase
//
// This class implements operations that are shared by both the Switch and the
// SwitchBox classes.
//
///////////////////////////////////////////////////////////////////////////////

class SwitchBase
{
protected:

    const char * myName;        // Unallocated string
    const char * myDescription; // Unallocated string

    SwitchBox * myOwner;

public:

    // Interface //////////////////////////////////////////////////////////////

    SwitchBox * getOwner() const {
        return myOwner;
    }

    const char * getName() const {
        return myName;
    }

    const char * getDescription() const {
        return myDescription;
    }

protected:

    SwitchBase(SwitchBox * owner, const char * name, const char * description);

    ///////////////////////////////////////////////////////////////////////////
    //
    // IteratorBase
    //
    // This template class is used to implement iterators for both the Switch
    // and the SwitchBox classes.
    //
    ///////////////////////////////////////////////////////////////////////////

    template<class T>
        class IteratorBase
        {
            typedef IteratorBase<T> Self;

            // Note:
            // This forward iterator is used only for enumeration. Therefore,
            // there is no need to keep track of the previous node.

            T * myNode;

        public:

            // Interface //////////////////////////////////////////////////////

            IteratorBase(T * node = 0) : myNode(node) {}

            T& operator*() const {
                return *myNode;
            }

            bool operator==(const Self& other) const {
                return myNode == other.myNode;
            }

            bool operator!=(const Self& other) const {
                return myNode != other.myNode;
            }

            Self& operator++() {
                myNode = myNode->myBrothers;
                return *this;
            }

            Self operator++(int) {
                Self self = *this;
                myNode = myNode->myBrothers;
                return self;
            }
        };
};

///////////////////////////////////////////////////////////////////////////////
//
// Switch
//
// This class is the primary user interface for journaling services. It supply
// the current on/off state and the current reporting journal pointer used for
// a specific category of events.
//
///////////////////////////////////////////////////////////////////////////////

class Switch : public SwitchBase
{
    Switch * myBrothers; // Brother nodes at the same level
    Journal * myJournal; // Associated journal

    bool myState;        // Associated state

public:

    typedef IteratorBase<Switch> Iterator;

    // Interface //////////////////////////////////////////////////////////////

    Switch(SwitchBox * owner, const char * name, const char * description);
    ~Switch();

    void setState(bool state, bool recursive = true);

    bool getState() const {
        return myState;
    }

    void setJournal(Journal * journal, bool recursive = true);

    Journal * getJournal() const {
        return myJournal;
    }

    void setParameters(bool state, Journal * journal, bool recursive = true);

    void setEnable(bool recursive = true) {
        setState(true, recursive);
    }

    void setDisable(bool recursive = true) {
        setState(false, recursive);
    }

    void printOut(std::ostream& stream);

    friend Iterator;
    friend SwitchBox;

private:

    void changeState(bool state) {
        myState = state;
    }

    void changeJournal(Journal * journal);

    void changeParameters(bool state, Journal * journal) {
        changeState(state);
        changeJournal(journal);
    }

    void copyState(Switch * source) { changeState(source->myState); }
    void copyJournal(Switch * source) { changeJournal(source->myJournal); }

    void copyParameters(Switch * source) {
        changeParameters(source->myState, source->myJournal);
    }
};

///////////////////////////////////////////////////////////////////////////////
//
// SwitchBox
//
// This class is a node in the SwitchBox tree used to organize Switch classes
// and propagate settings.
//
///////////////////////////////////////////////////////////////////////////////

class SwitchBox : public SwitchBase
{
    SwitchBox * myBrothers; // Brother nodes at the same level
    SwitchBox * myChildren; // First child in the list of children

    Switch * mySwitches;

public:

    typedef IteratorBase<SwitchBox> Iterator;

    // Interface //////////////////////////////////////////////////////////////

    SwitchBox(SwitchBox * owner, const char * name, const char * description);

    Iterator getChildrenBegin() {
        return Iterator(myChildren);
    }

    Iterator getChildrenEnd() {
        return Iterator(0);
    }

    void addChild(SwitchBox * item) {
        if(myChildren) {
            item->myBrothers = myChildren;
        }

        myChildren = item;
    }

    Switch::Iterator getSwitchesBegin() {
        return Switch::Iterator(mySwitches);
    }

    Switch::Iterator getSwitchesEnd() {
        return Switch::Iterator(0);
    }

    void addSwitch(Switch * item) {
        if(mySwitches) {
            item->myBrothers = mySwitches;
        }

        mySwitches = item;
    }

    void printOut(std::ostream& stream);

    friend Iterator;
    friend Switch;

protected:

    template<class T>
        void setSwitch(Switch * source, T member) {
            for(Switch::Iterator i = getSwitchesBegin(); i != getSwitchesEnd(); ++i) {
                Switch * leaf = &(*i);

                if(!strcmp(leaf->getName(), source->getName())) {
                    (leaf->*member)(source);
                }
            }
        }

    void propagateState(Switch * source);

    void setState(Switch * source) {
        setSwitch(source, Switch::copyState);
        propagateState(source);
    }

    void propagateJournal(Switch * source);

    void setJournal(Switch * source) {
        setSwitch(source, Switch::copyJournal);
        propagateJournal(source);
    }

    void propagateParameters(Switch * source);

    void setParameters(Switch * source) {
        setSwitch(source, Switch::copyParameters);
        propagateParameters(source);
    }

    template<class T>
        void propagateSwitch(Switch * source, T member) {
            for(SwitchBox::Iterator i = getChildrenBegin(); i != getChildrenEnd(); ++i) {
                SwitchBox * node = &(*i);
                (node->*member)(source);
            }
        }
};

///////////////////////////////////////////////////////////////////////////////
//
// Macro helpers
//
// Those macros are used to declare and implement instances of SwitchBox and
// Switch for a specified portion of the system.
//
///////////////////////////////////////////////////////////////////////////////

#define DECLARE_SINGLETON_CODE(Type, Class, Owner) \
    class Class : public Type                      \
    {                                              \
        typedef Owner OwnerSwitchBox;              \
                                                   \
        static Type * ourInstance;                 \
        static Type * staticFactory();             \
                                                   \
    public:                                        \
                                                   \
        Class();                                   \
                                                   \
        static Type * getInstance() {              \
            if(ourInstance) {                      \
                return ourInstance;                \
            }                                      \
                                                   \
            return staticFactory();                \
        }                                          \
    }

#define IMPLEMENT_SINGLETON_CODE(Type, Class, Name, Description)              \
    Class::Class() : Type(OwnerSwitchBox::getInstance(), Name, Description) { \
    }                                                                         \
                                                                              \
    Type * Class::staticFactory() {                                           \
        static Class instance;                                                \
                                                                              \
        ourInstance = &instance;                                              \
        return ourInstance;                                                   \
    }                                                                         \
                                                                              \
    Type * Class::ourInstance = Class::getInstance()

#define DECLARE_SWITCHBOX(Class, Owner) DECLARE_SINGLETON_CODE(SwitchBox, Class, Owner)
#define DECLARE_SWITCH(Class, Owner)    DECLARE_SINGLETON_CODE(Switch, Class, Owner)

#define IMPLEMENT_SWITCHBOX(Class, Name, Description) IMPLEMENT_SINGLETON_CODE(SwitchBox, Class, Name, Description)
#define IMPLEMENT_SWITCH(Class, Name, Description)    IMPLEMENT_SINGLETON_CODE(Switch, Class, Name, Description)

///////////////////////////////////////////////////////////////////////////////
//
// Root
//
///////////////////////////////////////////////////////////////////////////////

struct NoParent
{
    static SwitchBox * getInstance() {
        return 0;
    }
};

DECLARE_SWITCHBOX(Root, NoParent);

///////////////////////////////////////////////////////////////////////////////
//
// Report
//
///////////////////////////////////////////////////////////////////////////////

#define REPORT(Leaf, Expression)                    \
    do {                                            \
        Switch * leaf = Leaf::getInstance();        \
                                                    \
        if(leaf->getState()) {                      \
            Journal& journal = *leaf->getJournal(); \
                                                    \
            journal.beginReport();                  \
            journal << Expression;                  \
            journal.endReport();                    \
        }                                           \
    } while(0)

///////////////////////////////////////////////////////////////////////////////
//
// Assert
//
///////////////////////////////////////////////////////////////////////////////

#define ASSERT(Leaf, Condition)                                                 \
    do {                                                                        \
        Switch * leaf = Leaf::getInstance();                                    \
                                                                                \
        if(leaf->getState() && !(Condition)) {                                  \
            displayAssert(*leaf->getJournal(), #Condition, __FILE__, __LINE__); \
        }                                                                       \
    } while(0)

void displayAssert(Journal& journal, const char * condition, const char * file, int line);

///////////////////////////////////////////////////////////////////////////////
//
// Trace
//
///////////////////////////////////////////////////////////////////////////////

class StackTracer
{
    const char * myName;

    StackTracer * myParent;
    Switch * myLeaf;

public:

    StackTracer(Switch * leaf, const char * name) : myLeaf(leaf), myName(name) {
        myParent = ourParent;

        reportEvent("Enter -- ");
        ourParent = this;
    }

    ~StackTracer() {
        reportEvent("Leave -- ");
        ourParent = myParent;
    }

    static void printOut(std::ostream& stream);

private:

    void reportEvent(const char * prefix) {
        if(myLeaf->getState()) {
            Journal& journal = *myLeaf->getJournal();

            journal.beginReport();
            journal << "[Trace] - " << prefix << myName;
            journal.endReport();
        }
    }

    static StackTracer * ourParent;
};

#define TRACE(Leaf)                                           \
    StackTracer stackTracer(Leaf::getInstance(), __FUNCSIG__) \

///////////////////////////////////////////////////////////////////////////////
//
// Output
//
///////////////////////////////////////////////////////////////////////////////

inline std::ostream& operator<<(std::ostream& stream, Switch& object) {
    object.printOut(stream);
    return stream;
}

inline std::ostream& operator<<(std::ostream& stream, SwitchBox& object) {
    object.printOut(stream);
    return stream;
}

///////////////////////////////////////////////////////////////////////////////
//
// Journaling
//
///////////////////////////////////////////////////////////////////////////////

struct Journaling
{
    static void printOut(std::ostream& stream) {
        Root::getInstance()->printOut(stream);
    }
};

#endif
