all : LibParser

LibParser : LibParser.cpp
	$(CC) $(CLIBDEFS) LibParser.cpp -o LibParser.o
	$(CC) $(CLIBDEFS) LibPostgresql.cpp -o LibPostgresql.o

clean:
	-$(RM) *~ *.o
