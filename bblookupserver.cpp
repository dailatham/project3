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
// #include "textindex.h"
#include "Ref.h"
#include "Verse.h"
#include "Bible.h" 
#include <stdio.h>
#include "logfile.h"
//#define logging // enable log file
//#define LOG_FILENAME "/tmp/samriffle-LookupServer.log" 
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

	while (1) { 
		// Process request
		string results = "";
		string tokenValue[5];

		cout << "" << endl;
		cout << "Waiting for request..." << endl;

		// Get a message from a client
		results = recfifo.recv();
		log(results);

		cout << "Info recieved by server: " << results;
		cout << endl;

		// Assign each data
		tokenValue[0] = GetNextToken(results, "&"); // Book
		tokenValue[1] = GetNextToken(results, "&"); // Chap
		tokenValue[2] = GetNextToken(results, "&"); // Verse
		tokenValue[3] = GetNextToken(results, "&"); // Number of verses
//		tokenValue[4] = GetNextToken(results, "&"); // Bible Version

		// Parse string into usable variables for request
		int b = stoi(tokenValue[0]);
		int c = stoi(tokenValue[1]);
		int v = stoi(tokenValue[2]);
		int nv = stoi(tokenValue[3]);
//		int bibleVers = stoi(tokenValue[4]);
//		cout << "Bible version: " << bibleVers << endl;

		Ref ref(b, c, v);
		Verse lVerse;
		LookupResult result = OTHER;

		cout << "Now processing..." << endl;

		sendfifo.openwrite(); // Open write to client 

		// Return Verse(s)
		lVerse = webBible.lookup(ref, result);
		if (result != SUCCESS) {
			Verse noVerse;
			stringstream ss;

			ss << noVerse.getRef().getBook() << "&"
				<< noVerse.getRef().getChap() << "&"
				<< noVerse.getRef().getVerse() << "&"
				<< noVerse.getVerse() << "&" << result << "&";

			string out = ss.str();
			sendfifo.send(out);
		}
		else { // successful search
			int numVerses = nv;
			if (numVerses <= 0) {
				Verse noVerse;
				stringstream ss;

				ss << noVerse.getRef().getBook() << "&"
					<< noVerse.getRef().getChap() << "&"
					<< noVerse.getRef().getVerse() << "&"
					<< noVerse.getVerse() << "&" << result << "&";

				string out = ss.str();
				sendfifo.send(out);
			}
			else {
				if (result != SUCCESS) {
					Verse noVerse;
					stringstream ss;

					ss << noVerse.getRef().getBook() << "&"
						<< noVerse.getRef().getChap() << "&"
						<< noVerse.getRef().getVerse() << "&"
						<< noVerse.getVerse() << "&" << result << "&";

					string out = ss.str();
					sendfifo.send(out);
				}
				else {
					string out = "";
					stringstream ss;

					ss << lVerse.getRef().getBook() << "&"
						<< lVerse.getRef().getChap() << "&"
						<< lVerse.getRef().getVerse() << "&"
						<< lVerse.getVerse() << "&" << result << "&";

					out = ss.str();
					sendfifo.send(out);
					ss.flush();
					ss.clear();

					if (numVerses > 0 && numVerses != NULL) {
						for (int i = 1; i < numVerses; i++) { 
							string out2 = "";
							stringstream ss2;
							Verse verseNext = webBible.nextVerse(result);

							ss2 << verseNext.getRef().getBook() << "&"
								<< verseNext.getRef().getChap() << "&"
								<< verseNext.getRef().getVerse() << "&"
								<< verseNext.getVerse() << "&" << result << "&";

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
		sendfifo.fifoclose();  // Close send pipe
		log("close reply fifo");
		cout << "Process complete" << endl;
	}
	//recfifo.fifoclose();
	//log("close request fifo");
}
