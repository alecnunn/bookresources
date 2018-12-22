Accompanying Source Code for
A linear programming model for Windows-based games
Game Programming Gems 2
by Javier F. Otaegui (javier@sabarasa.com)

What you should look at:

1) MainPtah7.cpp
2) Ingame.cpp
3) Asteroids.cpp

MainPtah7.cpp includes all the initialization code for the linear programming
model. It makes use of Support.cpp, who includes all the initialization code
for DirectX, and its management.

Ingame.cpp includes a RunGame() function that will show you how easily you
can create a game which includes several screens. This is a very simple demo
and does not include any menus or branching options, but this can be easily
done. RunGame will show a Splash() function that includes a text
scroller and a random line generator. You go through screens with the ESC key.

After this is shown, a small space-movement demo is included, and, after that,
the Splash screen is shown again.

Please focus on the simplicity in which this is done. Also notice the Lost
Surface problem solution. We include a global stack which will be holding all
functions (and its arguments) that need to be called when the surfaces are
lost.

All the other modules are for sprite management, font management and primitive
management. You can also notice two modules, fplog.cpp, which is a very useful
logging function (the program will generate a debug.log every time it is
called), and mymalloc.cpp, which provides logged memory management to avoid
leaks.