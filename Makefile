
CC=g++
CFLAG=-g #-Wall
OBJS=$(patsubst %.cpp,%.o,$(wildcard *.cpp))
EXE=scheduler

$(EXE): $(OBJS)
	$(CC) $(CFLAG) $(INC) $(OBJS) -o $@

%.o: %.cpp
	$(CC) $(CFLAG) $(INC) -c $< -o $@


	
clean:
	rm *.o $(EXE)
