# Simplex
Simplex algorithm developped as a school project

#Args
The program takes two args, the input file, and the output file. No output is made through the terminal.
simplex data.dat results.txt

#Input files
There is a collection of input files we used to test the program inside of testfiles/, they work like this :
[Number of constraints][Number of vars][Optimisation type]
...
then as many lines as there are constraints
[Type of constraint][value] //Constraint are >, = or <, and the value is what is after (= 30, 30 being it)
...
Then again as many lines as there are vars, ordered by var number. (First equation, then second, etc)

