Black Art of Java Game Programming CD-ROM notes
===============================================

CD-ROM Contents
---------------
The contents and structure of the CD-ROM are as follows:

+---ARCHIVES               All CD files in ZIP or TAR format
+---BOOK                   Chapters 1 through 12 and Appendix E
|   +---APPE               Source code for Appendix E
|   +---CHAP01             Source code for Chapter 1
|   +---CHAP02             Source code for Chapter 2
|   +---CHAP03             Source code for Chapter 3
|   +---CHAP04             Source code for Chapter 4
|   +---CHAP05             Source code for Chapter 5
|   +---CHAP06             Source code for Chapter 6
|   +---CHAP07             Source code for Chapter 7
|   +---CHAP08             Source code for Chapter 8
|   +---CHAP09             Source code for Chapter 9
|   +---CHAP10             Source code for Chapter 10
|   +---CHAP11             Source code for Chapter 11
|   \---CHAP12             Source code for Chapter 12
+---GAMES                  Source code and executables for the Game Gallery
|   +---CHAP13             JAVAroids
|   +---CHAP14             Daleks!
|   +---CHAP15             NetOthello
|   +---CHAP16             SpaceDeath and WordQuest
|   |   +---SPDEATH      
|   |   |   \---SPACEDEA   SpaceDeath
|   |   \---WORDQUES       WordQuest
|   +---CHAP17             Magic Squares
|   +---CHAP18             MahJong server
|   +---CHAP19             Slider
|   \---CHAP20             Worm
\---JDK                    Java Developer's Kit 1.0.2 for Macintosh, Solaris,
                           Windows 95, and Windows NT 

How to use the Applets in the Book
-----------------------------------
In many instances, the examples in Chapters 1 through 12 use an HTML file
to launch an applet. In general, there are two ways that an HTML file is
used to launch these applets. One is with a Java-enabled Web browser, and
the other is to use the appletviewer program included with Sun Microsystem's
Java Developer's Kit (JDK). We will cover using the appletviewer included
with JDK 1.0.2. Instructions for installation of the JDK can be found in
the section "Installation Instructions."

Macintosh
---------
We are going to illustrate how to use the Applet Viewer by explaining
how to open Mondrian.html, found in Chapter 1 of the book. Other HTML
files may be opened by following the same steps.

1) Double-click on your hard drive icon and double-click on the folder
   "JDK 1.0.2".

2) Double-click on the Applets folder.

3) Find the program called "Applet Viewer" and double-click on this file.

4) Select Open Local from the File menu.

5) Click on the Desktop button on the File Open dialog box. NOTE: The
   dialog box does not have a displayed name on it. This is the standard
   way to refer to it, though.

6) Double-click on the Black Art CD-ROM.

7) Double-click on the BOOK folder.

8) Double-click on the CHAP01 folder.

9) For your last step, double-click on the file Mondrian.html. If you are
   using the Applet Viewer for the first time, a License Agreement pops
   up for your perusal. Read this license carefully, and if you agree to
   the terms of the license click on the Accept button. After a moment
   you will see Mondrian.class running within the Applet Viewer environment.

Solaris
-------
NOTE: These UNIX examples assume you will be using the mount point /cdrom.
If that is not the mount point you will be using, please substitute your
mount point with the one in the example. In all cases, the mount point must
exist or an error will occur. If the mount point does not exist, create the
directory using the mkdir command and try remounting the CD-ROM.

We are going to illustrate how to use the appletviewer by explaining
how to open Mondrian.html, found in Chapter 1 of the book. Other HTML
files may be opened by following the same steps.

1) Change to the CHAP01 directory on the CD-ROM by typing

cd /cdrom/cdrom0/book/chap01     <ENTER>

2) Type the following command to launch Mondrian.html:

appletviewer mondrian.html     <ENTER>

Windows 95 and Windows NT
-------------------------
NOTE: The examples in this section assume your system's main hard drive is
C: and your CD drive is D:. If your system configuration does not match
our assumption, please substitute the drive letters in these examples with
ones that correspond to your system.

We are going to illustrate how to use the appletviewer by explaining
how to open Mondrian.html, found in Chapter 1 of the book. Other HTML
files may be opened by following the same steps.

If you are in Windows, exit to the DOS Prompt. Insert the CD-ROM into your
PC's CD drive.

1) Change to the CHAP01 directory on the CD-ROM by typing

d:     <ENTER>
cd \book\chap01     <ENTER>

2) Type the following command to launch Mondrian.html:

appletviewer Mondrian.html     <ENTER>

Installation instructions
-------------------------
Even though Java is a cross-platform language, the operating systems that
Java have been ported to have different installation instructions and
requirements. The Java Developer's Kit -- included on this CD-ROM -- is
available on the Macintosh, Solaris, Windows 95, and Windows NT operating
systems. The installation instructions that follow have been tailored to
each specific operating system you have. Please follow the instructions
for your particular operating system.

SPECIAL NOTE FOR ALL PLATFORMS: This CD-ROM was burned using the Level 2
specification of ISO-9660 (the standard for CD-ROM file and directory
naming conventions). This level of ISO-9660 limits filenames to 30
characters. Some of the programming examples on this CD-ROM used filenames
that were longer than 30 characters, and so were truncated during the CD-ROM
duplication process. When you encounter a problem that is related to
truncated filenames, look at the Java source code and rename the .java
and .class files to match the filename found inside. If you are uncomfortable
with this procedure, Windows 95 and Windows NT users may use the .EXE files
found in the ARCHIVES directory (see note in the Windows section), Solaris
and Macintosh users may use the .tar file found in the ARCHIVES
directory (see note in the Macintosh and Solaris sections), to replace
the truncated files with the correct ones.

GAME GALLERY NOTE: The authors of the Game Gallery use certain platforms
for their game development and everyday use. It may be necessary for you
to tweak certain parameters for to get the games running correctly on your
computer. Please read any of the documentation in the directory corresponding
to each game for successful configuration.

Macintosh
---------
When you double-click on the icon representing your CD-ROM, you should see
four folders: ARCHIVES, BOOK, GAMES, and JDK. You can install all of the
files, or you can install only the components that you want. The instructions
that follow show you how to do both a full and partial install, and then show
you how to install the Java Developer's Kit.

1) If you want to install all of the files in the BOOK and GAMES folders,
   create a folder on your hard drive by double-clicking on the hard drive's
   icon and selecting New Folder from the File menu. A folder named "untitled
   folder" is placed on your hard drive, with the name already selected. Type
   in a name, such as Black Art, and press RETURN. Now, double-click on the
   folder you just created. Hold down the Shift key and click on both the
   BOOK and GAMES folders. Drag both folders to the Black Art folder you
   just created on your hard drive. Depending on the performance of your
   system, this operation may take from a few moments to a few minutes.

2) If you only want to do a partial install, create a folder on your hard
   drive by double-clicking on the hard drive's icon and select New Folder
   from the File menu. A folder named "untitled folder" is placed on your hard
   drive, with the name already selected. Type in a name, such as Black Art,
   and press RETURN. Double-click on the folder you just created. The folders
   on the hard drive are divided by programming examples (BOOK) and the Game
   Gallery (GAMES). If you want to copy files from both folders, then create
   two folders named BOOK and GAMES. Otherwise, just create the folder that
   corresponds to what you want to copy. Shift-click on all of the folders
   you want to copy and drag them to the BOOK or GAMES folder on your hard
   drive. Depending on your system's performance, this may take a few
   moments to a few minutes.

3) To install the Java Developer's Kit, double-click on your CD-ROM drive's
   icon and double-click on the JDK folder. In this folder is the installer
   for the JDK, called JDK-1_0_2-MacOS.sea. This file is a self-extracting
   archive. Double-click on this file. Click Continue when prompted to do so.
   The copyright and license information is displayed next. You have the
   option of printing or saving this information. To continue this install,
   click on Continue. Now you will be at the main installation section. You
   have the option of an Easy Install (recommended) or a Custom Install (for
   advanced users only). You also can choose which hard drive to install the
   JDK to. Once you have made your selections, click Install. After the
   installation is complete, you will be asked if you want to continue
   installing or to quit. Choose Quit and restart your computer. If you need
   help configuring and running the Java Developer's Kit, please refer to Sun
   Microsystem's Java page at http://www.javasoft.com.

SPECIAL NOTE: As the warning above outlines, there is a possibility that some
filenames may have been truncated. The .tar files in the ARCHIVES directory
on the CD-ROM contain all of the files in pre-truncated form. We recomend
the Macintosh utility StuffIt Deluxe to extract the .tar files. Please see
the documentation included with StuffIt deluxe should you have any questions
about this procedure.

Solaris
-------
Note: These UNIX examples assume you will be using the mount point /cdrom.
If that is not the mount point you will be using, please substitute your
mount point with the one in the example. In all cases, the mount point must
exist or an error will occur. If the mount point does not exist, create the
directory using the mkdir command and try remounting the CD-ROM.

The Solaris operating system has a volume manager which will handle the
details of mounting a CD-ROM for you. For it to work, though, it must be
running. To see if your volume manager is running, type the following at the
UNIX command prompt

ps -eaf | grep vol     <ENTER>

If you see the /usr/bin/vold process running, then you can immediately
skip to step 2. If the process is not running, continue with step 1.

1) You must be root to start the volume manager. Start the volume manager
   by typing

/etc/init.d/volmgt start     <ENTER>

2) Insert the CD-ROM into your CD drive. After a couple of seconds, you should
   be able to access the CD-ROM by typing

cd /cdrom/cdrom0     <ENTER>

   if this is the first CD-ROM drive on your system. Use cdrom1 if you
   inserted the CD-ROM into your second CD drive.

3) You are now ready to copy the contents of the CD-ROM to your local drive.
   Ensure you're at the root by typing

cd /     <ENTER>

4) Create a directory for the files, such as black_art, by typing

mkdir /black_art     <ENTER>

5) Move into the directory you just created by typing

cd /black_art     <ENTER>

6) Copy each program example by typing

cp -r /cdrom/cdrom0/book/chap?? .    <ENTER>

   where chap?? is the name of the chapter you want to copy.

7) Copy each Game Gallery example by typing

cp -r /cdrom/cdrom0/games/chap?? .     <ENTER>

   where chap?? is the name of the chapter you want to copy.

NOTE: If your Solaris machine is not reading the CD-ROM correctly, i.e., 
filenames are not being displayed as uppercase and lowercase, you will
find all of the source code in the file book.tar, and the Game Gallery in
games.tar. Both files are found in the ARCHIVES directory. If you want to
extract the source code to your local drive, type

tar -xf /cdrom/cdrom0/archives/book.tar     <ENTER>

If you want to extract the Game Gallery to your local drive, type

tar -xf /cdrom/cdrom0/archives/games.tar     <ENTER>

The JDK 1.0.2 requires Sun Solaris 2.3, 2.4, or 2.5 on a SPARC-based machine
or Solaris 2.5 for Intel x86-based machines. You will need to extract the
tarred and compressed file before you can use the JDK.

1) If you already have a Java directory named anything other than java,
   simply move into that directory and skip to Step 3, otherwise continue
   with Step 2.

2) If you don't have a current Java directory or if you have a Java directory
   named java, ensure you are at the root by typing

cd /     <ENTER>

3) Now you are ready to install the JDK onto your local drive.

      a) If you have a SPARC-based machine type:

zcat /cdrom/cdrom0/JDK/Solaris/JDK-1_0_2-solaris2-sparc.tar.Z | tar xf - <ENTER>

      b) If you have an Intel x86-based machine type:

zcat /cdrom/cdrom0/JDK/Solaris/JDK-1_0_2-solaris2-x86.tar.Z | tar xf - <ENTER>

4) To use the Applet Viewer, you will need to add java/bin to your path with
   any standard text editor, such as vi. If you are using the JDK for the
   first time, your account must be restarted for your changes to take effect.
   Now, go into the Java directory containing an html file. A good example to
   use is example1.html in the java/demo/WireFrame directory. Move there by
   typing

cd /java/demo/WireFrame     <ENTER>

5) Run the Applet Viewer on the html file by typing

appletviewer example1.html     <ENTER>

Windows 95 or Windows NT
------------------------
NOTE: The examples in this section assume your system's main hard drive is
C: and your CD drive is D:. If your system configuration does not match
our assumption, please substitute the drive letters in these examples with
ones that correspond to your system.

If you are in Windows, exit to the DOS Prompt. Insert the CD-ROM into your
PC's CD drive.

1) Ensure you are at the root directory of your hard drive by typing

c:      <ENTER>
cd\     <ENTER>

2) Create a destination directory, such as black_art, by typing

md black_art     <ENTER>

   You may substitute another directory name.

3) Move to the directory you just created by typing

cd black_art     <ENTER>

4) To copy the individual programming example chapter directories, you will
   need to manually create the directories on your hard drive and copy the
   contents into each chapter subdirectory. For instance, if you wanted to
   copy the contents of Chapter 3 you would type

md chap03     <ENTER>

5) Move into the Chapter 3 directory by typing

cd \black_art\chap03     <ENTER>

6) Now type

xcopy d:\book\chap03\. /v /s     <ENTER>

   This will copy the contents of the Chapter 3 subdirectory to your hard
   drive. This example works for Chapters 1 through 12 and for Appendix E.
   For each additional chapter you would like to copy, substitute the chap03
   directory with the directory name that corresponds to the chapter you
   want to copy.

7) To copy the individual Game Gallery chapter directories, you will need to
   manually create the directories on your hard drive and copy the contents
   into each chapter subdirectory. For instance, if you wanted to copy the
   contents of Chapter 15 you would type

md chap15     <ENTER>

8) Move into the Chapter 15 directory by typing

cd \black_art\chap15     <ENTER>

9) Now type

xcopy d:\games\chap15\. /v /s     <ENTER>

   This will copy the contents of the Chapter 15 subdirectory to your hard
   drive. This example works for Chapters 13 through 20. For each additional
   chapter you would like to copy, substitute the chap03 directory with the
   directory name that corresponds to the chapter you want to copy.

SPECIAL NOTE FOR WINDOWS NT USERS: Because of Windows NT's file system, you
may have a problem reading some of the source code files from the CD-ROM.
We have provided two self-extracting archives, called BOOK.EXE and GAMES.EXE,
which contain all of the source code files for the book and the Game
Gallery. Both files are found in the ARCHIVES directory.

1) To extract the source code for the book, type

d:\archives\book     <ENTER>

   This will create a directory in your root called book and extract all
   of the source code into it. If you do not want to extract the source code
   to your root directory, create and move into a directory of your choice
   [somedirectory] by typing

md [somedirectory]     <ENTER>
cd [somedirectory]     <ENTER>

2) Now type

d:\archives\book     <ENTER>

   The files will be extracted into a directory called book inside of
   [somedirectory].

3) To extract the contents of GAMES.EXE, substitute BOOK.EXE with GAMES.EXE
   in the example above.

The Windows version of the JDK will only run on Windows 95 or the Intel
version of Windows NT. You will need to extract the file from the CD-ROM
to use the JDK.

1) On both platforms, exit to the DOS prompt. Ensure you are at the root
   directory of your C: drive by typing

c:      <ENTER>
cd\     <ENTER>

2) Extract the JDK by typing

d:\JDK\JDK-1_0_2-win32-x86.exe

   and press ENTER. This will extract the JDK into a directory called java
   on your root directory.

3) To use the Applet Viewer, you first need to update your path to include
   the java\bin directory. The easiest way to do this is by editing your
   AUTOEXEC.BAT file in the root directory of your C: drive. When you are
   at the root, type

edit autoexec.bat     <ENTER>

   Locate your path statement, and add at the end the following information:

c:\java\bin;

4) Save your updated file and exit the editor. Reboot your machine.

5) Exit to the DOS prompt and go top your root directory. Type

cd \java\demo\WireFrame     <ENTER>

6) Run the Applet Viewer by typing

appletviewer example1.html     <ENTER>

