This is an extremely simple example of the usage of the "syncdebug" module. Comments
in the code explain a lot of what is going on.

In this project, the debug build has the syncdebug module fully enabled, and the release
doesn't. In practice we left the syncdebug in the release build, because it had little
effect on the performance of the game. Then, as the vast majority of the out-of-sync
bugs were found, we created a new build type without syncdebug enabled.

In startopia_syncreport.txt is an excerpt from an out-of-sync report from StarTopia.
This may be interesting, in that it shows how the system ends up getting used in
practice. You may be able to see, from how much is "snip"ed out of the file, that we
used to get sync reports of 6Mb in size quite normally. It becomes a fair skill to
extract the useful sections, usually using WinDiff to help find the differences between
the reports on different machines.

Note also how we tracked usage of the random number generator quite closely, and the
states of all the different components of the game.

- Jan Svarovsky (www.svarovsky.com/jan)