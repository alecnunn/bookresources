#include "journaling.h"
#include "debugger_journal.h"

///////////////////////////////////////////////////////////////////////////////
//
// Root
//
///////////////////////////////////////////////////////////////////////////////

IMPLEMENT_SWITCHBOX(Root, "Root",
                          "Root of all evil");

///////////////////////////////////////////////////////////////////////////////
//
// Journal
//
///////////////////////////////////////////////////////////////////////////////

Journal * Journal::ourDefaultInstance;

void Journal::beginReport() {
}

char Journal::endReport() {
    return CONTINUE;
}

Journal * Journal::staticFactory() {
    static DebuggerJournal debugger;
    debugger << "Debugger is online.\n" << std::flush;

    // Prevent the default instance from being deleted.
    debugger.addReference();

    ourDefaultInstance = &debugger;
    return ourDefaultInstance;
}

///////////////////////////////////////////////////////////////////////////////
//
// SwitchBase
//
///////////////////////////////////////////////////////////////////////////////

SwitchBase::SwitchBase(SwitchBox * owner, const char * name, const char * description) : myOwner(owner) {
    if(name == 0) {
        name = DEFAULT_NAME;
    }

    myName = name;

    if(description == 0) {
        description = DEFAULT_DESCRIPTION;
    }

    myDescription = description;
}

///////////////////////////////////////////////////////////////////////////////
//
// Switch
//
///////////////////////////////////////////////////////////////////////////////

Switch::Switch(SwitchBox * owner, const char * name, const char * description) : SwitchBase(owner, name, description) {
    if(myOwner) {
        myOwner->addSwitch(this);
    }

    changeParameters(true, Journal::getDefault());
}

Switch::~Switch() {
    myJournal->releaseReference();
}

void Switch::setState(bool state, bool recursive) {
    changeState(state);

    if(recursive) {
        myOwner->propagateState(this);
    }
}

void Switch::setJournal(Journal * journal, bool recursive) {
    changeJournal(journal);

    if(recursive) {
        myOwner->propagateJournal(this);
    }
}

void Switch::setParameters(bool state, Journal * journal, bool recursive) {
    changeParameters(state, journal);

    if(recursive) {
        myOwner->propagateParameters(this);
    }
}

void Switch::printOut(std::ostream& stream) {
    stream << myName << " (";

    if(myState) {
        stream << "On";
    }
    else {
        stream << "Off";
    }

    stream << ") - " << myDescription << std::endl;
}

void Switch::changeJournal(Journal * journal) {
    Journal * backup = myJournal;

    if(journal == 0) {
        journal = Journal::getDefault();
    }

    myJournal = journal;

    // Add a reference to the journal.
    myJournal->addReference();

    if(backup) {
        backup->releaseReference();
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// SwitchBox
//
///////////////////////////////////////////////////////////////////////////////

SwitchBox::SwitchBox(SwitchBox * owner, const char * name, const char * description) : SwitchBase(owner, name, description) {
    if(myOwner) {
        myOwner->addChild(this);
    }
}

void SwitchBox::printOut(std::ostream& stream) {
    stream << myName << "\n\tOwner:       ";

    if(myOwner) {
        stream << myOwner->myName;
    }
    else {
        stream << "None";
    }

    stream << "\n\tDescription: " << myDescription << "\n\tChildren:\n";

    if(getSwitchesBegin() == getSwitchesEnd()) {
        stream << "\t\tNone\n";
    }
    else {
        for(Switch::Iterator i = getSwitchesBegin(); i != getSwitchesEnd(); ++i) {
            stream << "\t\t" << *i;
        }
    }

    stream << "\n";

    for(Iterator i = getChildrenBegin(); i != getChildrenEnd(); ++i) {
        stream << *i;
    }
}

void SwitchBox::propagateState(Switch * source) {
    propagateSwitch(source, setState);
}

void SwitchBox::propagateJournal(Switch * source) {
    propagateSwitch(source, setJournal);
}

void SwitchBox::propagateParameters(Switch * source) {
    propagateSwitch(source, setParameters);
}

///////////////////////////////////////////////////////////////////////////////
//
// Assert
//
///////////////////////////////////////////////////////////////////////////////

void displayAssert(Journal& journal, const char * condition, const char * file, int line) {
    journal.beginReport();

    journal << "The following assertion has failed and should be corrected! The behaviour of the program is undertermined from this point.\n";
    journal << "Condition: " << condition << "\n\nFile: " << file << " (" << line << ")\n\nStack:\n";

    StackTracer::printOut(journal);

    char result = journal.endReport();
    if(result == Journal::ABORT) {
        ExitProcess(-1);
    }

    if(result == Journal::BREAK) {
        DebugBreak();
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// Trace
//
///////////////////////////////////////////////////////////////////////////////

void StackTracer::printOut(std::ostream& stream) {
    if(ourParent) {
        StackTracer * i = ourParent;

        while(i) {
            stream << "\t" << i->myName << "\n";
            i = i->myParent;
        }
    }
    else {
        stream << "\tStack is empty.\n";
    }
}

StackTracer * StackTracer::ourParent;
