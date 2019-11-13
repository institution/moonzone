
g++ -c -std=c++17 -O0 -g a.cpp -o a.o
g++ -c -std=c++17 -O0 -g t.cpp -o t.o
g++ -std=c++17 -O0 -g t.o a.o
