Component based object management
---------------------------------

The main part of the system (as described in the gem) is in the ComponentSystem folder. The main files to look at are "CObjectManager.h", "CObjectManager.cpp" and "SObjectManagerDB.h".

To show how the system might be put to use, we have made a little game that uses it. This game is merely an example of how to use the system. The most interesting files to look at are all in the components folder. The files in this folder are merely there to make the game tick along. A few improvements could be made to the game (and the system) if we intented to take it further.

Here are some suggestions (exercises for the reader :-)

- The ability to delete components would be handy. All the structures allow this, so that should be a simple exercise. It could be useful to post a delete message to the objects/components we want to delete, and then remove the components after the update.

- The ability to spawn objects is something most games will want. That is simply a matter of having the parameter nodes of the object to be spawned to hand when needed.

- We could do with a mechanism for finding out whether messages have been handled (Checking return values from the HandleMessage() function). We could make post and broadcast iterators that would let us do this.

- Storing interface pointers. In the example code, we query for an interface whenever we need it. In the long run this isn't too efficient, so some kind of smart pointer could be useful.

We hope you found it all of some use.