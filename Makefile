# Bibleajax program
# Bob Kasper, MVNU Computer Science
# updated January 2020 to use
# c++11 compiler option, current paths on cs.mvnu.edu

# This is a Makefile for the Bible web app demo program.
# Copy this directory to a location within your home directory. 
# Change the USER name value below to your own user name.
# Then use "make" to build the server program,
# and deploy it to the live web server directory.
# To test the program, go to http://cs.mvnu.edu/class/csc3004/USER/
# and open the bibleajax.html link.

# TO DO: Must replace "username" by your username
USER= dailatham

# Use GNU C++ compiler with C++11 standard
CC= g++
CFLAGS= -g -std=c++11

all: $(PutCGI) $(PutHTML) bblookupserver #PutCGI PutHTML


bblookupserver.o: 	bblookupserver.cpp fifo.h Ref.h Verse.h Bible.h
			$(CC) $(CFLAGS) -c bblookupserver.cpp

bblookupserver: 	bblookupserver.o fifo.o Ref.o Verse.o Bible.o
			$(CC) $(CFLAGS) -o bblookupserver bblookupserver.o fifo.o Ref.o Verse.o Bible.o

#bblookupclient.o: 	bblookupclient.cpp fifo.h
#			$(CC) $(CFLAGS) -c bblookupclient.cpp

#bblookupclient: 	bblookupclient.o fifo.o
#			$(CC) $(CFLAGS) -o bblookupclient bblookupclient.o fifo.o -L/usr/local/lib –lcgicc

fifo.o: 		fifo.cpp fifo.h
			$(CC) $(CFLAGS) -c fifo.cpp

# Ref Object
Ref.o : Ref.h Ref.cpp
	$(CC) $(CFLAGS) -c Ref.cpp

# Verse Object
Verse.o : Ref.h Verse.h Verse.cpp
	$(CC) $(CFLAGS) -c Verse.cpp

# Bible Object
Bible.o : Ref.h Verse.h Bible.h Bible.cpp
	$(CC) $(CFLAGS) -c Bible.cpp

# Main Program source
testreader.o: Ref.o Verse.o Bible.o testreader.cpp
	$(CC) $(CFLAGS) -c testreader.cpp

testreader: testreader.o
	$(CC) $(CFLAGS) -o testreader testreader.o Ref.o Verse.o Bible.o

#PutCGI: bblookupclient
#		chmod 757 bblookupclient
#		cp bblookupclient /var/www/html/class/csc3004/$(USER)/cgi-bin
#		echo "Current contents of your cgi-bin directory: "
#		ls -l /var/www/html/class/csc3004/$(USER)/cgi-bin/

#PutHTML: bibleindex.html
#		cp bibleindex.html  /var/www/html/class/csc3004/$(USER)
#		echo "Current contents of your HTML directory: "
#		ls -l  /var/www/html/class/csc3004/$(USER)

clean:
		rm *.o corebblookupserver

