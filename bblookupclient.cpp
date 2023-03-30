/*
#include <sstream>

#include "bblookupclient.h"
#include "Ref.h"

bblookupclient::bblookupclient(std::string pipe_request_id, std::string pipe_reply_id, std::string bibleVersion) : pipe_request(pipe_request_id), pipe_reply(pipe_reply_id), bibleVersion(bibleVersion) {}

bblookupclient::ServerReply bblookupclient::request(std::string action, const Ref &ref) {
	ServerReply reply;

	//  Construct the request and send it. 
	pipe_request.openwrite();
	std::stringstream out;
	out << bibleVersion << " " << action << " " << ref.toString();
	pipe_request.send(out.str());
	pipe_request.fifoclose();

	// Receive the server's reply. 
	pipe_reply.openread();
	std::string replyText = pipe_reply.recv();
	pipe_reply.fifoclose();

	// Split the reply.
	std::string statusText = GetNextToken(replyText, " ");
	std::string verseText = replyText; // The rest of the reply is the verse line (including ref and text).

	// Convert the reply.
	reply.result = static_cast<LookupResult>(atoi(statusText.c_str()));
	reply.ref = Ref(verseText);
	reply.verseText = verseText;

	return reply;
}

Verse bblookupclient::lookup(const Ref &ref, LookupResult &result) {
	ServerReply reply = request("lookup", ref);

	result = reply.result;
	return Verse(reply.verseText);
}

Ref bblookupclient::next(const Ref &ref, LookupResult &result) {
	ServerReply reply = request("next", ref);

	result = reply.result;
	return reply.ref;
}

Ref bblookupclient::prev(const Ref &ref, LookupResult &result) {
	ServerReply reply = request("prev", ref);

	result = reply.result;
	return reply.ref;
}


*/





















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

// Required file from Project1
//#include "Bible.h"
//#include "Ref.h"
//#include "Verse.h"

#define logging // enable log file
#include "logfile.h"

// Pipes for communication
string receive_pipe = "SSreply";
string send_pipe = "SSrequest";

//========================================================================================

int main() {
  // prepare the response output,
  // send required header before any other output
  cout << "Content-Type: text/plain\n\n" << endl;

  Cgicc cgi;  // create object used to access CGI request data
//  form_iterator sstring = cgi.getElement("sstring");
//  string searchString = **sstring;
//  int length = searchString.length();

  Fifo recfifo(receive_pipe);
  Fifo sendfifo(send_pipe);

  #ifdef logging
	logFile.open(logFilename.c_str(), ios::out);
  #endif

  log("Request recieved" << '\n');

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
                 cout << "<p>The numer of verse must be at least.</p>" << endl;
                 validInput = false;
		 exit(2);
        }
         else {
                 validInput = true;
        }
  }

  // Create a reference from the numbers;
  Ref ref(bookNum, chapterNum, verseNum);

  // Create Bible object to process the raw text file
  Bible webBible("/home/class/csc3004/Bibles/web-complete");
  Verse verseResult;
  LookupResult result;
  int count = 0;	// help to keep track with multi-verse
  int prevV, curV; 	// keep track of pervious and current verse
  Ref curRef;		// keep track of current ref 

  /////////////////////// P3pt2 
  string receive_pipe = "reply";
  string send_pipe = "request";
  Fifo recfifo(receive_pipe);
  Fifo sendfifo(send_pipe);
  stringstream ss;
  stringstream ss1;
  string out = "";

  // Call server to get results  
  ss << ref.getBook() << "&" << ref.getChap() << "&" << ref.getVerse() << "&" << nv << "&";
  out = ss.str();
  sendfifo.openwrite();
  sendfifo.send(out); // Send info as one long string to be split in Server
  log("Request sent to server: " << ss.str() << '\n');
  ss.flush();
  ss.clear();
  cout << endl;
  sendfifo.fifoclose();
  recfifo.openread();
  string results = "";
  results = recfifo.recv(); // update to go outside the loop BUT it DO BE unoptimized. Consider server sending seperate message for each
  // recfifo.fifoclose();
  log("Results from server recieved: " << results << '\n');

  // Look up the first verse base on reference and display the result.
  verseResult = webBible.lookup(ref, result);
  count++;

  // if all of the input are valid
  if (validInput) {
	cout << "Search Type: <b>" << **st << "</b><br>" << endl;
  }
  // else print out an error
  else {
	cout << "<p>Invalid Input: <em>report the more specific problem.</em></p>" << endl;
	exit(2);
  }

  // if the verse doesn't exist print an error
  if (!(verseResult.getRef() == ref)) {
	cout << "<br><b>" << endl;
	ref.displayNonExisted();
	cout << "</b>" << endl;

	if (result != SUCCESS){
		cout << "<br><b>" << endl;
		cout << webBible.error(result) << endl;
		cout << "</b>" << endl;
	}
	exit(2);
  }
  // else display the result.
  else {
	cout << "<br><b>" << endl;
	ref.displayBookNameCh();
	cout << "</b><br>" << **verse << " : " << verseResult.getVerse() << endl;
	// update the current verse number
	curV = verseResult.getVerseNum();
  }

  // Only apply on multi-verse case, keep looking for the next verse while the count is less than or equal to the verse number. 
  if (Numv > 1){
        do {
		// look up from ref
		verseResult = webBible.nextVerse(result);
                count++;

		// update the previous and current verse number
		prevV = curV;
		curV = verseResult.getVerseNum();

		// update current ref
		curRef = verseResult.getRef();

		if (curV < prevV) {
			cout << "<br><br><b>" << endl;
			curRef.displayBookNameCh();
			cout << "</b><br>" << verseResult.getVerseNum() << " : " << verseResult.getVerse();
			cout << "\n" << endl;
		}
		else {
			cout << "<br>" << verseResult.getVerseNum() << " : " << verseResult.getVerse(); 
	              	cout << "\n" << endl;
		}
        } while (count != Numv);
   }


  //return 0;
  recfifo.fifoclose();

}

