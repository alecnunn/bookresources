ARTICLE: Preprocessed Solution For Open Terrain Navigation
AUTHOR: Smith Surasmith
EMAIL: ssurasmith@angelstudios.com

DIRECTORY TREE:
-sample	
	-solution
		-solution.cpp
		-solution.dsp
		-solution.dsw
	-test
		-sample.csv
		-samplei.csv
		-solution.csv
		-solution.exe
	-readme.txt

SOLUTION DIRECTORY:
1. Contains the source code and build instructions originally written in VC++5.0.
2. The solution.cpp contains code for reading and writing sample files.
3. It contains an implementation of Dijkstra's Algorithm.
4. It also contains the algorithm for creating the solution table from connectivity data.
5. It should compile fine with VC++5.0 or later.
6. Any C++ compiler should be okay to compile the code.
 

TEST DIRECTORY:

1. CSV files are comma separated values type fields that is written out by Microsoft Excel.  It is a text file, but can also be loaded by the Excel application.

2. The examples here are the examples shown in the article.

3. sample.csv
3.1. The first field defines the number of nodes in the graph.
3.2. The second field defines the starting cound of the nodes. Forexample, 1 means that the first node is index 1, and 0 means that the first node is index 0.  This is so the examples matches those in the article.
3.3. Each line after is a list of outgoing edges.  The first value is the number of outgoing edges.  Each entry is the node number destination and the cost.  For example, 2[109] means that the edge goes to node 2, and the cost of the edge is 109.

4. samplei.csv
4.1. This is an intermediate file put out by the executable showing the connectivity table.
4.2. The first field is the number of nodes in the table.
4.3. The structure of the table is an n-squared table of connectivity and costs
4.4. -1 is the value for non existing connections.
4.5. This is to resemble the connectivity table in the article.

5. solution.csv
5.1. This is the solution table output by the executable.
5.2. The first field is the number of nodes in the table.
5.3. The rest is an n-squared table of path solutions as explained in the article.

6. solution.exe
6.1. Is the executable to read in sample.csv and generate the solution.csv.
6.2. It is a dos shell application.
6.3. help with usage, type "solution -h"
6.4. Usage is "solution -ih <source> <destination>"
6.5. -i means for the application to write out an intermediate file. The intermediate 
file takes the file name without the extension and appends i.csv after it.  For example,
sample.csv produces samplei.csv intermediate file.
6.6. -h prints out usage instructions.
6.7. <source> file should be in the format like sample.csv
6.8. <destination> file will be in format like solution.csv






