all: a.out b.out
a.out: CFD2dGrid_1.cc
	mpicxx -std=c++11 -o a.out CFD2dGrid_1.cc
b.out: CFD2dGrid.cc
	g++ -std=c++11 -o b.out CFD2dGrid.cc
