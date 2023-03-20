// Bible class function definitions
// Computer Science, MVNU

#include "Ref.h"
#include "Verse.h"
#include "Bible.h" 
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

Bible::Bible() { // Default constructor
	infile = "/home/class/csc3004/Bibles/web-complete";
	buildIndex(infile);
	bibleIter = index.begin();
}

// Constructor – pass bible filename
Bible::Bible(const string s) {
	infile = s;
	buildIndex(infile);
}


int Bible::buildIndex(string filename)
{
	bool debug = true;
	ifstream infile;     // input file descriptor
	int position;        // location of line in the file
	infile.open(filename.c_str(), ios::in);
	if (!infile) {
		cerr << "Error - can't open input file: " << filename << endl;
		return 1;  // false, indicates error
	}
	while (!infile.fail()) {
		// Get the file position at beginning of line
		position = infile.tellg();
		// get the next line
		string line;
		getline(infile, line);
		// make reference from the line, add to index
		Ref ref(line);
		index[ref] = position; // remember offset for current line
		if (ref.getBook() == 66 && ref.getChap() == 22 && ref.getVerse() == 21) {
			break;
		}
	}

	if (debug) {
		cout << "Refs made: " << index.size() << endl;
		auto iter = index.end();
		iter--;
		cout << "Last byte offset: " << iter->second << endl;
		iter = index.begin();
		cout << "Byte offset 1st ref: " << iter->second << endl;
		iter++;
		cout << "Byte offset 2nd ref: " << iter->second << endl;
		iter++;
		cout << "Byte offset 3rd ref: " << iter->second << endl;
		return 0;
	}
	return 0;
}

map<Ref, int>::iterator Bible::indexSearch(Ref ref)
{
	auto value = index.find(ref);
	auto iter = index.end();
	iter--;
	if (value->first < iter->first) {
		return value;
	}
	map<Ref, int>::iterator failSearch = index.begin();
	return failSearch;
}


// REQUIRED: lookup finds a given verse in this Bible
Verse Bible::lookup(Ref ref, LookupResult& status) { 
    // update the status variable
    status = OTHER; // placeholder until retrieval is attempted
    // create and return the verse object
    Verse aVerse;	// default verse, to be replaced by a Verse object
	                // that is constructed from a line in the file.

    // TODO: scan the file to retrieve the line that holds ref ...
    // DAISY
    instream.open(infile.c_str(), ios::in);
    string s;
//    Ref cur = Ref();
    bool bookFound = false, chapFound = false, verseFound = false;
    bool doIndex = true;
    bool debug = true;
    int returnValue;
    string line;

    // creating Ref for error checking
    int bookNum = ref.getBook();
    int chapNum = ref.getChap();
    int verseNum = ref.getVerse();
    Ref refBook = Ref(bookNum, 1, 1);
    Ref refBookChap = Ref(bookNum, chapNum, 1);

   if (debug){
	cout << "\nBible.cpp error checking REF: " << endl;
   	cout << "bookNum = " << bookNum << endl;
   	cout << "chapNum = " << chapNum << endl;
   	cout << "verseNum = " << verseNum << endl;
   }

/*
    // Regular Search
    do {
	// get a next verse
	getline(instream, s);
		if (s != ""){
		verse = Verse(s);
		cur = verse.getRef();

		// check if the current book, chapter, verse exist.
		if (cur.getBook() == ref.getBook())
			bookFound = true;
		if (cur.getBook() == ref.getBook() && cur.getChap() == ref.getChap())
			chapFound = true;
		if (cur.getBook() == ref.getBook() && cur.getChap() == ref.getChap() && cur.getVerse() == ref.getVerse())
			verseFound == true;

		// check if the retreive verse match the ref
		if (verse.getRef() == ref){
			aVerse = verse;
			status = SUCCESS;
		}
		}
    } while (!(verse.getRef() == ref) && !instream.fail());

    // update th status
    if (status != SUCCESS) {
	if (bookFound && chapFound) { status = NO_VERSE; }
	else if (bookFound){ status = NO_CHAPTER; }
	else { status = NO_BOOK; }
    }
*/
cout << "\nhere before check if book exist" << endl;
    // Checking if book exist
    bibleIter = index.find(refBook);
    if (bibleIter != index.end()) {
	bookFound = true;
    }

cout << "\nhere before check if chapter exist" << endl;
    // Check if chapter exist
    bibleIter = index.find(refBookChap);
    if (bibleIter != index.end()) {
	chapFound = true;
    }

cout << "\nhere before do index search" << endl;
    // Index Search
    bibleIter = index.find(ref); 
cout << "\nbefore do SUCCESS" << endl;
//    if (bookFound == true && chapFound == true){
    if (bibleIter != index.end()) {
cout << "\nin SUCCESS loop do SUCCESS" << endl;
 	returnValue = bibleIter->second;
   	instream.seekg(returnValue);
    	getline(instream, line);
    	aVerse = Verse(line);
	status = SUCCESS;
	verseFound = true;

    } else {
cout << "\nthe ref doesnt exist" << endl;
	aVerse = Verse();
    }

cout << "\nupdating status" << endl;
    // Update status
    if (status != SUCCESS) {
	if (bookFound && chapFound) { status = NO_VERSE; }
	else if (bookFound){ status = NO_CHAPTER; }
	else { status = NO_BOOK; }
    }

    return aVerse;
}

// REQUIRED: Return the next verse from the Bible file stream if the file is open.
// If the file is not open, open the file and return the first verse.
Verse Bible::nextVerse(LookupResult& status) {
	Verse verse;
	Ref ref1 = Ref(1,1,1);
	string s;
	int returnValue;

	if (!instream){
		instream.open(infile.c_str(), ios::in);
		verse = lookup(ref1, status);
	}
	else {
		bibleIter++;
		auto iter = index.end();
		iter--;
		if (bibleIter->second == iter->second) {
			returnValue = bibleIter->second;
			instream.seekg(returnValue, ios::beg); 
			getline(instream, s);
			verse = Verse(s);
			status = OTHER; 
		}
		else if (bibleIter->first < iter->first) { 
			returnValue = bibleIter->second;
			instream.seekg(returnValue, ios::beg);
			getline(instream, s);
			verse = Verse(s);
			status = SUCCESS;
		}
		else {
			status = OTHER;
			verse = Verse();
		}
	}
	return verse;
}

// REQUIRED: Return an error message string to describe status
string Bible::error(LookupResult status) {
	//DAISY
	if (status == NO_BOOK) {
		return "Error : No book found.";
	}
	else if (status == NO_CHAPTER) {
		return "Error : No chapter found.";
	}
	else if (status == NO_VERSE) {
		return "Error : No verse found.";
	}
	else if (status == OTHER) {
		return "Error : Unspecified";
	}
	else {
		return "!!Error: something is VERY WRONG!!";
	}
}

void Bible::display() {
	cout << "Bible file: " << infile << endl;
}

// OPTIONAL access functions
// OPTIONAL: Return the reference after the given ref
Ref Bible::next(const Ref ref, LookupResult& status) {
	auto value = indexSearch(ref); // Assume ref exists already
	value++;

	auto iter = index.end();
	iter--;
	if (value->first < iter->first) {
		status = SUCCESS;
		return value->first;
	}
	map<Ref, int>::iterator failSearch = index.begin();
	status = OTHER;
	return failSearch->first;

//	return ref;
}

// Return the reference before the given ref
//const Ref Bible::prev(Ref ref, LookupResult& status) {}
