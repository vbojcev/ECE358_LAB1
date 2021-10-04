compileExp: exp.cpp
	g++ -o exp exp.cpp

runExp: exp
	./exp 75

cleanExp:
	rm exp

compileP1: part1.cpp
	 g++ -o part1 part1.cpp

runP1: part1
	./part1 2000 0.25 0.95 2000 1000000

plotP1: q3_graph1.txt q3_graph2.txt
	gnuplot q3_graph1.txt
	gnuplot q3_graph2.txt

cleanP1:
	rm q3_graph1.png
	rm q3_graph2.png
	rm q3_data1.txt
	rm q3_data2.txt
	rm part1

compileP2: part2_v2.cpp
	g++ -o part2 part2.cpp

runP2: part2
	./part2 2000 0.5 1.5 2000 1000000

plotP2: q4_graph1.txt q4_graph2.txt
	gnuplot q4_graph1.txt
	gnuplot q4_graph2.txt

cleanP2:
	rm q4_graph1.png
	rm q4_graph2.png
	rm q4_data1.txt
	rm q4_data2.txt
	rm part2
