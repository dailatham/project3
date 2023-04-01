/****************************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sstream>
#include <map>
#include "fifo.h"
#include "Ref.h"
#include "Verse.h"
#include "Bible.h"
#include <stdio.h>
#include "logfile.h"
#include <string.h>

/****************************************************************************/

using namespace std;

string receive_pipe = "request";
string send_pipe = "reply";

//Server main line,create name MAP, wait for and serve requests
int main() {

	#ifdef logging
		logFile.open(logFilename.c_str(), ios::out);
	#endif

	// Build indices
	cout << "" << endl;
	Bible webBible("/home/class/csc3004/Bibles/web-complete");
	cout << "Index built" << endl;

	// Create the communication fifos
	Fifo recfifo(receive_pipe);
	Fifo sendfifo(send_pipe);

	// only open once
	recfifo.openread();
	log("Open request fifo");

	while (1) {
		// Process request
		string results = "";
		string tokenValue[4];
	        bool debug = false;

		cout << "===========================================" << endl;
		cout << "Waiting for request..." << endl;

		// Get a message from a client
		results = recfifo.recv();
		log(results);

		cout << "Info recieved by server: " << results << endl;

		// Ensure that the request string contains enough tokens
		int num_colons = 0;
		for (char c : results) {
			if (c == ':') {
        			num_colons++;
    			}
		}

		// Assign each data
		tokenValue[0] = GetNextToken(results, ":"); // Book
		tokenValue[1] = GetNextToken(results, ":"); // Chap
		tokenValue[2] = GetNextToken(results, ":"); // Verse
		tokenValue[3] = GetNextToken(results, ":"); // Number of verses

		// Parse string into usable variables for request
		int b = stoi(tokenValue[0]);
		int c = stoi(tokenValue[1]);
		int v = stoi(tokenValue[2]);
		int nv = 1;
		if (num_colons > 2) {	// for cases like 1:1:1:4
			nv = stoi(tokenValue[3]);
		}

		// DFEBUG
		if (debug) {
		        cout << "\nInput Report: " << endl;
			cout << "b = " << b << endl;
			cout << "c = " << c << endl;
			cout << "v = " << v << endl;
			cout << "nv = " << nv << endl; 
		}
	        // Value to help keep track of program.
        	int count = 0;
        	int curVNum = 0;

		// Create Ref
		Ref ref(b, c, v);
		cout << "...Ref created." << endl;
		Verse lVerse;
		LookupResult result = OTHER;
		bool verseExist;

		cout << "...Now processing..." << endl;

		sendfifo.openwrite(); // Open write to client 
		cout << "...sendfifo.openwrite()" << endl;

		// Return Verse(s)
		lVerse = webBible.lookup(ref, result);
		cout << "...Bible lookup is done." << endl;
	        count++;
	        curVNum = ref.getVerse();

        	// DEBUG
        	if ( debug ) {
                	cout << "\nRESULT STATUS: ";
                	if (result == SUCCESS) {cout << "SUCCESS" << endl;}
                	if (result == NO_VERSE) {cout << "NO_VERSE"<< endl;}
                	if (result == NO_CHAPTER) {cout << "NO_CHAPTER" << endl;}
                	if (result == NO_BOOK) {cout << "NO_BOOK" << endl;}
        	}

	        // Report Verse
	        if (!(lVerse.getRef() == ref)){
			verseExist = false;
        	        string error = webBible.error(result);

                        stringstream ss;
                        ss.flush();
                        ss.clear();

//			ss << error << ":" << result;;
			ss << lVerse.getRef().getBook() << ":"
                                << lVerse.getRef().getChap() << ":"
                                << lVerse.getRef().getVerse() << ":"
                                << error << ":" << result;

                        string out = ss.str();
                        sendfifo.send(out);
        	} else {
			verseExist = true;
			string out = "";
                        stringstream ss;

//			int bookNum = lVerse.getRef().getBook();
//			string bookName = to_string(bookNum);
			string bookName = lVerse.getRef().getBookName();
       	                ss << bookName << ":"
                        	<< lVerse.getRef().getChap() << ":"
                                << lVerse.getRef().getVerse() << ":"
                                << lVerse.getVerse() << ":" << result;

                        out = ss.str();
                        sendfifo.send(out);
        	}

        	// Multi-verse
        	while (count < nv && (verseExist == true)) {
        	        lVerse = webBible.nextVerse(result);
        	        count++;
        	        curVNum = lVerse.getVerseNum();

	                // Report Verse
//	                if ( curVNum == 1) {
                        	string out = "";
                        	stringstream ss;

//				int bookNum = lVerse.getRef().getBook();
//				string bookName = to_string(bookNum);
				string bookName = lVerse.getRef().getBookName();
                        	ss << bookName << ":"
                        	        << lVerse.getRef().getChap() << ":"
                        	        << lVerse.getRef().getVerse() << ":"
                        	        << lVerse.getVerse() << ":" << result;

                        	out = ss.str();
                        	sendfifo.send(out);
/*        	        } else {
                	        verse.onlyVerseDisplay();
                	        cout << endl;
                	}
*/      	}

		sendfifo.fifoclose();  // Close send pipe
		log("close reply fifo");
		cout << "\n...Process complete." << endl;
	}
//	recfifo.fifoclose();
//	log("close request fifo");
}
