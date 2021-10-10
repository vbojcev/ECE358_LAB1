The following files must be in a common Unix folder:

exp.cpp
part1.cpp
part2_v2.cpp
q3_graph1.txt
q3_graph2.txt
q6_graph1.txt
q6_graph2.txt
Makefile

To test the Exponential Random Variable (Question 1), type these commands:

make compileExp
make runExp
make cleanExp (not strictly necessary but it's easy cleanup, this goes for the other two "clean XXXX" commands as well. Also, only "clean" commands after viewing the relevant outputs!)

To test the infinite buffer (Questions 2, 3, 4):

make compileP1
make runp1
make plotP2
make cleanP1

To test the finite buffer (Questions 5, 6):

make compileP2
make runP2
make plotP2
make cleanP2

The "plotP1", "plotP2" commands will produce PNG files. Display these as your system allows. Using MobaXTerm, we can use "display XXXX.png"