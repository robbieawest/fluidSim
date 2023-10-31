# fluidSim
 Fluid Simulation based on Jos Stam's Real-Time Fluid Dynamics for Games
 Finished 23/02/2022

Small amount of commits as was created without GitHub then imported to it

## How to run
### Windows
On Windows you can just run Fluid.exe inside of "Items for Running"

###Linux
I have provided nothing except code/linuxrun
linuxrun is just all code in one file since I had errors with compiling SFML
```
$ g++ -c main.cpp
$ g++ main.o -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system
$ ./sfml-app
```

^Easy enough with g++
Although you have to have SFML installed. on debian it looks like 
```
$ sudo apt install libsfml-dev
```
If you do not specify a non-default directory for SFML then you do not have to fuck around with the linker when compiling g++.
