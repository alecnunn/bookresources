This is the sample implementation of the FreezeMgr.

SOURCE FILES:

freezelib.h - You'll find the FreezeMgr and Freezable classes defined here.  Everything of interest is wrapped in the namespace Freezelib.

freezelib.cpp - The implementation code.

main.cpp - Puts the FreezeMgr through its paces.  I do the minimum here; it's no substitute for a more serious set of regression tests which I'd recommend you implement for your own project.

OUTPUT FILES:

fruitList.ice - A binary file containing a linked list of classes.  Used to test the basic functionality of the FreezeMgr and Freezable classes (preservation of pointers & vftables).  This file is written once, then opened in 'edit' mode and augmented.

fruitBowl.ice - Tests the case of multiple Freezable classes nested inside a non-Freezable class.
