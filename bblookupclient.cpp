
#include <iostream>
#include <stdio.h>
#include <string.h>

// Required libraries for AJAX to function
#include "/home/class/csc3004/cgicc/Cgicc.h"
#include "/home/class/csc3004/cgicc/HTTPHTMLHeader.h"
#include "/home/class/csc3004/cgicc/HTMLClasses.h"
using namespace cgicc;

//Stuff for pipes
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include "fifo.h"
using namespace std;

//#include <Bible.h>

#define logging // enable log file
#define LOG_FILENAME "/home/class/csc3004/tmp/dailatham-bblookup.log"
#include "logfile.h"

// Pipes for communication
string receive_pipe = "reply";
string send_pipe = "request";

int main() {
  // prepare the response output,
  // send required header before any other output
  cout << "Content-Type: text/plain\n\n" << endl;

  Cgicc cgi;  // create object used to access CGI request data

  Fifo recfifo(receive_pipe);
  Fifo sendfifo(send_pipe);

  #ifdef logging
	logFile.open(logFilename.c_str(), ios::out);
  #endif

  // GET THE INPUT DATA
  // browser sends us a string of field name/value pairs from HTML form
  // retrieve the value for each appropriate field name
  form_iterator st = cgi.getElement("search_type");
  form_iterator book = cgi.getElement("book");
  form_iterator chapter = cgi.getElement("chapter");
  form_iterator verse = cgi.getElement("verse");
  form_iterator nv = cgi.getElement("num_verse");

  int chapterNum, bookNum, verseNum, Numv;

  // Convert and check input data
  bool validInput = false;
  if (chapter != cgi.getElements().end()) {
	 chapterNum = chapter->getIntegerValue();
	 if (chapterNum > 150) {
		 cout << "<p>The chapter number (" << chapterNum << ") is too high.</p>" << endl;
		 exit(2);
	 }
	 else if (chapterNum <= 0) {
		 cout << "<p>The chapter must be a positive number.</p>" << endl;
		 exit(2);
	 }
	 else
		 validInput = true;
  }
  // Most book is 66
  if (book != cgi.getElements().end()) {
	bookNum = book->getIntegerValue();
	if (bookNum > 66) {
		cout << "<p>The book number (" << chapterNum << ") is too high.</p>" << endl;
		exit(2);
	}
	 else if (bookNum <= 0) {
		 cout << "<p>The book must be a positive number.</p>" << endl;
		 validInput = false;
		 exit(2);
	}
	 else {
		 validInput = true;
	 }
 }
  // Most verse is 176
  if (verse != cgi.getElements().end()) {
	verseNum = verse->getIntegerValue();
	 if (verseNum > 176) {
		 cout << "<p>The verse number (" << verseNum << ") is too high.</p>" << endl;
		 exit(2);
	 }
	 else if (verseNum <= 0) {
		 cout << "<p>The verse must be a positive number.</p>" << endl;
	       	 validInput = false;
		 exit(2);
	}
	 else {
		 validInput = true;
	}
  }
  // Check the number of verse
  if (nv != cgi.getElements().end()) {
        Numv = nv->getIntegerValue();
         if (Numv <= 0) {
                 cout << "<p>The numer of verse must be 1 or above.</p>" << endl;
                 validInput = false;
		 exit(2);
        }
         else {
                 validInput = true;
        }
  }

  /////////////////////// P3pt2 
  stringstream ss;
  stringstream ss1;
  string out = "";

  ss << bookNum << "&" << chapterNum << "&" << verseNum << "&" << Numv;
  out = ss.str();

  sendfifo.openwrite();
  log("Open request pipe.");

  // call server to get results
  sendfifo.send(out);
  log("Request sent to server: " << ss.str());

  recfifo.openread();
  log("Open reply fifo.");

 // output the response to the web page
  string results = "";

  int count = 1;
  int curV, prevV = 0;

  for (int i = 0; i < Numv; i++) {
	log("...IN THE FOR-LOOP...i : " << i);

  	// receive results from te server.
 	results = recfifo.recv();
	log("Receive result: " << results);

  	// sperating the string
  	string recvB, str, recvVtext;
  	int recvC, recvV, recvS = 0;
  	int po = 1;
  	bool debug = false;

  	stringstream recvSS(results);

  	while (getline(recvSS, str, '&')) {
		// DEBUG
		if (debug) {
			cout << "str = " << str << endl;
			cout << "po = " << po << "<br>" << endl;
  		}

   		switch (po) {
  			case 1:
				recvB = str;
				po++;
				break;
			case 2:
				recvC = stoi(str);
				po++;
				break;
			case 3: recvV = stoi(str);
				po++;
				break;
			case 4:
				recvVtext = str;
				po++;
				break;
			case 5:
				recvS = stoi(str);
				po++;
				break;
	  	}
	  }
    	  log("SUCCESS seperating the result");
	  curV = recvV;

	  // DEBUG
	  if (debug) {
		cout << "<br>recvB = " << recvB << endl;
		cout << "<br>recvC = " << recvC << endl;
		cout << "<br>recvV = " << recvV << endl;
		cout << "<br>recvVtext = " << recvVtext << endl;
		cout << "<br>recvS = " << recvS << "<br><br>" << endl;
		cout << "<br>curV = " << curV;
		cout << "<br>prevV = " << prevV;
	  }

	  // Report Output
  	  if (recvS != 0) {
		cout << recvVtext << endl;
		log("SUCCESS result report");
		break;
  	  } else {
		if  (count == 1 || curV < prevV) {
			if (count > 1) { cout << "<br><br>" << endl;}
			cout << "<b>" << recvB << " "<< recvC << "</b><br>" <<
			recvV << " : " <<
			recvVtext << endl;
		} else {
			cout << "<br>" << recvV << " : " <<
			recvVtext << endl;
		}
	  }
	  count++;
  	  prevV = curV;
	  log("SUCCESS result report");

	  //cout << "<br>prevV = " << prevV;
  }
  log("...END OF FOR-LOOP.");
  cout << endl; // flush output when done
  recfifo.fifoclose();
  log("Close reply fifo.");
  sendfifo.fifoclose();
  log("Close request fifo.");

 return 0;

}

