BBWar - Ver. 15-7-01

The files in this directory implement the java BBWar demo, a demo of blackboard coordination of multiple agents.




ABOUT JAVA
----------
Java is a SUN-developed object-oriented programming language similar in syntax to C++. Java source code is compiled to a cross-platform bytecode which is then interpreted at run-time. While few commercial applications are implemented in Java, its syntax is close enough to that of C++ that there is practically no learning curve when transferring from one to the other.
For more on Java, including specifications, documentation and tutorials, see the SUN java site

http://java.sun.com/products/




INSTALLING JAVA
---------------

BBWar requires the Java 2 VM and associated libraries. These packages are available off of the SUN site, 

http://www.javasoft.com/

To simply run the code given the provided class files, you will need the Java Runtime Environment, Standard Edition, v. 1.3. If you want to play around with the code as well you will need the Java 2 SDK, Standard Edition, v. 1.3.

After you download and install the appropriate package, make sure that the appropriate executables directory is added to your PATH variable. For example, if you downloaded the JDK 1.3.1 SDK on a Windows system, you would by default have the directory c:\jdk1.3.1\bin\ added to your path.

Java is of course cross-platform, and so will run on any machine with a Java 2-compliant virtual machine.




TO RUN
------

BBWar can be run directly from the CD. From a command line 

java -classpath [PATH of the bb directory]\.. bb.BBWar




TO COMPILE
----------

The process of compilation will generate the byte-code .class files from the .java files. Obviously you can't write the class files to the CD, so the best option would probably be to copy the 'bb' directory to a local writeable directory. Then, in that writeable directory, execute the following

javac *.java

This will let you play around with the code a little bit and maybe <gasp!> extend it.




ABOUT BBWar
-----------

BBWar is a simple little toy-RTS game, in which two computer-controlled armies battle it out on a simple landscape. The object for each army is to completely eradicate the other army! They do this by constructing and deploying military units -- the AI that controls all the behavior is built around a blackboard. See the "Blackboard Architectures" article in the Game AI Wisdom Book.

Of course, technically, BBWar is not a game at all, since there is no interactivity involved! And the game is still strategically fairly stupid. However, it is an example of an architecture that supports modular addition of smarter units and that handles much agent-coordination concerns automatically.




RECENT CHANGES
--------------
The BBwar has changed a little bit since the submission of the Blackboard article. The major differences are:

* A resource-system has been introduced, such that in order to construct a unit, a city must spend a determined number of resources. A specified number of resource-pools of random size are placed randomly about the game-world at the beginning of each execution of the game.

* A "Worker" unit has been introduced, whose sole function is to gather resources and deliver them to the city most in need of them.

* The Colonel Unit has been removed from the game.

* A "Defense Tactician" unit has been introduced. This unit, while not participating directly in the battle, does a simple clustering analysis of death locations and "suggests" locations that it is important to defend (a tragically reactive form of military strategy!). Thus if a great number of soldiers have been lost in one area of the game-world, then that area will be defended more vigorously, regardless of whethere there is a city or a resource pool in that location. This mechanism results in the emergent formation of battle "fronts" in strategically important locations.




CONTACT
-------

Questions? Comments? Suggestions?

Damian Isla (naimad@media.mit.edu)
The Synthetic Characters Group, MIT Media Lab
