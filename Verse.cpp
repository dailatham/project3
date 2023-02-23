

// Verse class function definitions
// Computer Science, MVNU

#include "Verse.h"
#include <iostream>
using namespace std;

Verse::Verse() {  	// Default constructor
    verseText = "Uninitialized Verse!";
    verseRef = Ref();
}

// REQUIRED: Parse constructor - pass verse string from file
Verse::Verse(const string s) {
	// use Ref constructor to create verseRef
	//DAISY
	string buffer = s;
	int book = atoi(GetNextToken(buffer, DELIM).c_str());
	int chap = atoi(GetNextToken(buffer, DELIM).c_str());
	int verse = atoi(GetNextToken(buffer, DELIM).c_str());
	// store the remainder of the string in verseText
	//DAISY
	verseText = buffer;
	verseRef = Ref(book, chap, verse);
	verseNum = verse;
}

// REQUIRED: Accessors
string Verse::getVerse() {
	//DAISY
	return verseText;
}

Ref Verse::getRef() {
	//DAISY
	return verseRef;
}
int Verse::getVerseNum() {
	return verseNum;
}
// display reference and verse
void Verse::display() {
    verseRef.display();
    cout << " " << verseText;
 }
