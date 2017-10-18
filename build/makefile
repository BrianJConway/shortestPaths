# set flags and compiler, as well as file var
CC=mpic++
CXXFLAGS=-std=c++11

O_FILES1=mpi_sequential.o graphFuncs.o 
O_FILES2=mpi_centralized.o graphFuncs.o 

INCLUDES=-I ../include
OUTPUT=mpi_sequential
OUTPUT2=mpi_centralized

all: mpi_sequential mpi_centralized

mpi_sequential: $(O_FILES1)
	$(CC) $(CXXFLAGS) -o $(OUTPUT) $(O_FILES1) 

mpi_centralized: $(O_FILES2)
	$(CC) $(CXXFLAGS) -o $(OUTPUT2) $(O_FILES2)  

mpi_sequential.o: ../src/sequential.cpp
	$(CC) $(CXXFLAGS) -c ../src/sequential.cpp -o mpi_sequential.o $(INCLUDES)

mpi_centralized.o: ../src/centralized.cpp
	$(CC) $(CXXFLAGS) -c ../src/centralized.cpp -o mpi_centralized.o $(INCLUDES)

graphFuncs.o: ../src/graphFuncs.cpp
	$(CC) $(CXXFLAGS) -c ../src/graphFuncs.cpp -o graphFuncs.o $(INCLUDES)

# use this to run
sequential: $(OUTPUT)
	mpirun -n 1 -host master ./$(OUTPUT)

centralized_2: $(OUTPUT)
	mpirun -n 2 -host master,node001 ./$(OUTPUT2)

centralized_4: $(OUTPUT)
	mpirun -n 4 -host master,node001,node002,node003 ./$(OUTPUT2)

clean:
	rm $(OUTPUT) 
	rm $(OUTPUT2)
	rm *.o 
	rm *.csv   