# pace-2020

**ExTREEm** - solver used in PACE 2020 contest

This repository contains code of the ExTREEm solver, used in PACE 2020 contest.
<br>
A set of heuristics is used to find Elimination Tree of given graph with height as small as possible.

<br>


**Requirements**:

CMake VERSION 3.10.2 or higher<br>
c++ 17 or higher

<br>

**Installation**:

Just use cmake to obtain a binary file, e.g. in linux in the main directory (pace-2020) use the following commands

mkdir build<br>
cd build<br>
cmake ..<br>
make

After this, the executable file name "ExTREEm" should be in the "build" directory

<br>

**Usage:**

Given a graph in a file graph.gr, you can run ExTREEm in the following way
 
./ExTREEm < graph.gr > extreem.out 2>logs.err


Please note, that the programm will run until it receives SIGTERM signal (e.g. using "kill -s SIGTERM $pid" command in linux).<br>
Also please note, that for small graphs (e.g. 50 vertices) large logs may be created. In case you intend to run ExTREEm on small instances for a very long time, you may consider not to keep stderr in a file.