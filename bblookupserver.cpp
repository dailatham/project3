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
//	string receive_pipe = "request";
//	string send_pipe = "reply";

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

        bool debug = true;

	while (1) { 
		// Process request
		string results = "";
		string tokenValue[4];

		cout << "" << endl;
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
		cout << "Ref is create" << endl;
		Verse lVerse;
		LookupResult result = OTHER;
		bool verseExist;

		cout << "Now processing..." << endl;

		sendfifo.openwrite(); // Open write to client 
		cout << "sendfifo.openwrite()" << endl;

		// Return Verse(s)
		cout << "now doing look up..." << endl;
		lVerse = webBible.lookup(ref, result);
		cout << "bible lookup is done." << endl;
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

                        Verse noVerse;
                        stringstream ss;
                        ss.flush();
                        ss.clear();

/*
                        ss << noVerse.getRef().getBook() << ":"
                                << noVerse.getRef().getChap() << ":"
                                << noVerse.getRef().getVerse() << ":"
                                << noVerse.getVerse() << ":" << result << ":";
*/
			ss << error;
                        string out = ss.str();
                        sendfifo.send(out);
        	} else {
			verseExist = true;
			string out = "";
                        stringstream ss;

       	                ss << lVerse.getRef().getBook() << ":"
                        	<< lVerse.getRef().getChap() << ":"
                                << lVerse.getRef().getVerse() << ":"
                                << lVerse.getVerse() << ":" << result << ":";

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

                        	ss << lVerse.getRef().getBook() << ":"
                        	        << lVerse.getRef().getChap() << ":"
                        	        << lVerse.getRef().getVerse() << ":"
                        	        << lVerse.getVerse() << ":" << result << ":";

                        	out = ss.str();
                        	sendfifo.send(out);
/*        	        } else {
                	        verse.onlyVerseDisplay();
                	        cout << endl;
                	}
*/      	}

        	// Bible.cpp next function
/*        	bool displayNextRef = false;

        	if (displayNextRef) {
        	Ref nextRef = webBible.next(ref, result);
        	nextRef.displayBookNameCh();
        	cout << endl;
        	nextRef.displayVerse();
        	cout << endl;
        	}
*/
/*		if (result != SUCCESS) {
			cout << "result != SUCCESS" << endl;

			Verse noVerse;
			stringstream ss;

			ss << noVerse.getRef().getBook() << ":"
				<< noVerse.getRef().getChap() << ":"
				<< noVerse.getRef().getVerse() << ":"
				<< noVerse.getVerse() << ":" << result << ":";

			string out = ss.str();
			sendfifo.send(out);
		}
		else { // successful search
			cout << "result == SUCCESS" << endl;

			int numVerses = nv;
			if (numVerses <= 0) {
				Verse noVerse;
				stringstream ss;

				ss << noVerse.getRef().getBook() << ":"
					<< noVerse.getRef().getChap() << ":"
					<< noVerse.getRef().getVerse() << ":"
					<< noVerse.getVerse() << ":" << result << ":";

				string out = ss.str();
				sendfifo.send(out);
			}
			else {
				if (result != SUCCESS) {
					Verse noVerse;
					stringstream ss;

					ss << noVerse.getRef().getBook() << ":"
						<< noVerse.getRef().getChap() << ":"
						<< noVerse.getRef().getVerse() << ":"
						<< noVerse.getVerse() << ":" << result << ":";

					string out = ss.str();
					sendfifo.send(out);
				}
				else {
					string out = "";
					stringstream ss;

					ss << lVerse.getRef().getBook() << ":"
						<< lVerse.getRef().getChap() << ":"
						<< lVerse.getRef().getVerse() << ":"
						<< lVerse.getVerse() << ":" << result << ":";

					out = ss.str();
					sendfifo.send(out);
					ss.flush();
					ss.clear();

					if (numVerses > 0) {
						for (int i = 1; i < numVerses; i++) { 
							string out2 = "";
							stringstream ss2;
							Verse verseNext = webBible.nextVerse(result);

							ss2 << verseNext.getRef().getBook() << ":"
								<< verseNext.getRef().getChap() << ":"
								<< verseNext.getRef().getVerse() << ":"
								<< verseNext.getVerse() << ":" << result << ":";

							out2 = ss2.str();
							sendfifo.send(out2);
							ss2.flush();
							ss2.clear();
							if (result != SUCCESS) {
								break;
							}
						}
					}
				}
			}
		}
*/
		sendfifo.fifoclose();  // Close send pipe
		log("close reply fifo");
		cout << "Process complete" << endl;
	}
//	recfifo.fifoclose();
//	log("close request fifo");
}
