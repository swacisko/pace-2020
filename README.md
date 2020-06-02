# pace-2020

**ExTREEm** - heuristic solver for treedepth problem, written as an entry to the PACE 2020 challenge.

A set of heuristics is used to find elimination tree of given graph, with height as small as possible.<br>
The main algorithm works in iterations, in each iteration a new treedepth decomposition is found (iterations are independent from each other, the longer the solver runs, the more iterations are done and the greater chance of finding a good decomposition).<br>


<br>


**Requirements**:

CMake VERSION 3.10.2 or higher<br>
c++ 17 or higher

<br>

**Installation**:

Use cmake to obtain a binary file, e.g. in linux in the main directory (pace-2020) you can use the following commands:

mkdir build<br>
cd build<br>
cmake ..<br>
make

After this, the executable file named "ExTREEm" should be in the "build" directory

<br>

**Usage:**

Given a graph in a file graph.gr, you can run ExTREEm in the following way
 
./ExTREEm < example_input.gr > example_output.out 2>example_logs.err


ExTREEm will run until it receives SIGTERM signal (e.g. using "kill -s SIGTERM $pid" command in linux).<br>
For small graphs (e.g. 50 vertices) large logs may be created. In case you intend to run ExTREEm on small instances for a very long time, you may consider not to keep stderr in a file.


<br>

**Remarks to the exact track**:

This program was submitted also as an entry to the treedepth exact track. In order to obtain the version that was submitted for the exact track, please change in file Pace20Params.cpp value of variable useExactTrack to true, before building.<br>
Please bear in mind that it is not guaranteed that ExTREEm will find optimal treedepth decomposition, but setting "useExactTrack=true" will make it more probable. It may also happpen (if useExactTrack=true and height of found elimination tree is large enough) that ExTREEm will not terminate after receiving SIGTERM. In that case no decomposition will be returned. If you wish to obtain the result anyway, you can change in code value 20 that occurs in two places: in teminate() function in Pace20Params.cpp and at the end of Pace20.cpp, and replace it with any large enough number (e.g. 1e9).