/* Demo server program for Bible lookup using AJAX/CGI interface
 * By James Skon, Febrary 10, 2011
 * updated by Bob Kasper, January 2020
 * Mount Vernon Nazarene University
 *
 * This sample program works using the cgicc AJAX library to
 * allow live communication between a web page and a program running on the
 * same server that hosts the web server.
 *
 * This program is run by a request from the associated html web document.
 * A Javascript client function invokes an AJAX request,
 * passing the input form data as the standard input string.
 *
 * The cgi.getElement function parses the input string, searching for the matching 
 * field name, and returing a "form_iterator" oject, which contains the actual
 * string the user entered into the corresponding field. The actual values can be
 * accessed by dereferencing the form iterator twice, e.g. **verse
 *     refers to the actual string entered in the form's "verse" field.
 */

#include <iostream>
#include <stdio.h>
#include <string.h>
using namespace std;

/* Required libraries for AJAX to function */
#include "/home/class/csc3004/cgicc/Cgicc.h"
#include "/home/class/csc3004/cgicc/HTTPHTMLHeader.h"
#include "/home/class/csc3004/cgicc/HTMLClasses.h"
using namespace cgicc;

// Required file from Project1
#include "Bible.h"
#include "Ref.h"
#include "Verse.h"

int main() {
  /* A CGI program must send a response header with content type
   * back to the web client before any other output.
   * For an AJAX request, our response is not a complete HTML document,
   * so the response type is just plain text to insert into the web page.
   */
  cout << "Content-Type: text/plain\n\n";

  Cgicc cgi;  // create object used to access CGI request data

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


  return 0;
}
