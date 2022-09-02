# bracetopia

This was a project for a CS course. It's a Conway's Game of Life simulation that aims to solve the battle between coders who prefer inline bracing or newline bracing. 
This project is implemented in C, with some implementation in Ncurses. To run this game, you must be on a linux device or run it on a linux server. Some challenges I 
encountered with the project was using pointers to update the 2-d grid for the simulation and calculating the new spots for each person in the simulation.

How to play:
- type -h on the Command Line to see all the settings and format of the game
- enter any settings you want or just use the default settings for the game
- If you pick the finite mode, it will print out the simulation for each cycle of the simulation
- If you pick the infinite mode. it will bring you to a blank screen and show you each cycle with Ncurses and will continue updating the screen until you stop the program

