compileP1: part1.cpp
	 g++ -o part1 part1.cpp

simulateP1: part1

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
