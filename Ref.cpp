// Ref class function definitions
// Computer Science, MVNU

#include "Ref.h"
#include <iostream>
#include <string>
#include <sstream>
using namespace std;

// GetNextToken - general routine for getting next token from a string
// Parameters
//    str - string to search. search is destructive, i.e., token is removed.
//    delimiters - string, a list of characters to use a delimiters between tokens
// Returns: string token (removed from str)

string GetNextToken(string& str, const string& delimiters = " ") {
  // Skip delimiters at beginning                                          
  string::size_type startPos = str.find_first_not_of(delimiters, 0);
  // Find position of delimiter at end of token                                           
  string::size_type endPos = str.find_first_of(delimiters, startPos);

  // Found a token, remove it from string, and return it                       
  string next = str.substr(startPos, endPos - startPos);
  string rest = str.substr(endPos - startPos + 1, string::npos);
  str = rest;
  return(next);
}

// Ref member functions

Ref::Ref() {book = 0; chap = 0; verse = 0;}  	// Default constructor

Ref::Ref(const int b, const int c, const int v) { 	// Construct Ref from three integers
	book = b;
	chap = c;
	verse = v;
}

// Book name
static string book_names[67] = {"","Genesis",
	"Exodus",
	"Leviticus",
	"Numbers",
	"Deuteronomy",
	"Joshua",
	"Judges",
	"Ruth",
	"1 Samuel",
	"2 Samuel",
	"1 Kings",
	"2 Kings",
	"1 Chronicles",
	"2 Chronicles",
	"Ezra",
	"Nehemiah",
	"Esther",
	"Job",
	"Psalms",
	"Proverbs",
	"Ecclesiastes",
	"Song of Solomon",
	"Isaiah",
	"Jeremiah",
	"Lamentations",
	"Ezekiel",
	"Daniel",
	"Hosea",
	"Joel",
	"Amos",
	"Obadiah",
	"Jonah",
	"Micah",
	"Nahum",
	"Habakkuk",
	"Zephaniah",
	"Haggai",
	"Zechariah",
	"Malachi",
	"Matthew",
	"Mark",
	"Luke",
	"John",
	"Acts",
	"Romans",
	"1 Corinthians",
	"2 Corinthians",
	"Galatians",
	"Ephesians",
	"Philippians",
	"Colossians",
	"1 Thessalonians",
	"2 Thessalonians",
	"1 Timothy",
	"2 Timothy",
	"Titus",
	"Philemon",
	"Hebrews",
	"James",
	"1 Peter",
	"2 Peter",
	"1 John",
	"2 John",
	"3 John",
	"Jude",
	"Revelation"
};

// Accessors
int Ref::getBook() {return book;}	 // Access book number
int Ref::getChap() {return chap;}	 // Access chapter number
int Ref::getVerse() {return verse;}; 	// Access verse number
//string Ref::getBookName() {return book_name[book];}

// TO DO: implement comparison functions
// REQUIRED: == comparison
bool Ref::operator==(const Ref r) {
	return (book == r.book && chap == r.chap && verse == r.verse);
}

bool Ref::operator<(const Ref r) const {
    if (book < r.book) {
        return true;
    }
    if (book > r.book) {
        return false;
    }
    if (chap < r.chap) {
        return true;
    }
    if (chap > r.chap) {
        return false;
    }
    if (verse >= r.verse) {
        return false;
    }
    return true;
}

/*
bool Ref::operator<(const Ref r) const {
  const bool isPrevVerse;
  const bool isPrevBook = book < r.book;

  if (isPrevBook == true) {
  	 const bool isPrevChapter = ( book == r.book && chap < r.chap );
  }
  if (isPrevChapter == true) {
	isPrevVerse = (( book == r.book) && chap == r.chap && verse < r.verse );
  }

  //const bool isPrevChapter = isPrevBook ? true : book == r.book && chap < r.chap;
  //const bool isPrevVerse = isPrevChapter ? true : book == r.book) && chap == r.chap && verse < r.verse;

  return isPrevVerse;
}

bool Ref::operator<=(const Ref r) const {
  return *this < r || *this == r;
}
*/
// TO DO: modify display function to show book name instead of book number
Ref::Ref(const string s) { // Parse constructor - receives a line "34:5:7 text"
    string rtext = s; // make local copy of string to avoid modifying parameter
    // parse the reference - notice, currently there is no error checking!
    // Get book number
    string strbook = GetNextToken(rtext,":");
    book = atoi(strbook.c_str());
    // Get the chapter number
    string strchap = GetNextToken(rtext,":");
    chap = atoi(strchap.c_str());
    // Get the verse number
    string strverse = GetNextToken(rtext," ");
    verse = atoi(strverse.c_str());
}
void Ref::display() { 	// Display Reference
     cout << "\n" << book_names[book] << " " << chap << "\n" << verse;
}
void Ref::displayNonExisted() {
     cout << "Error : " << book_names[book] << ", chapter " << chap << ", verse " << verse << " DOES NOT EXSIT.";
}
void Ref::displayBookNameCh() {
     cout << "\n" << book_names[book] << " " << chap;
}
void Ref::displayVerse() { 	
     cout << verse;
}
std::string Ref::toString() const {
	std::stringstream ss;
	ss << book << ":" << chap << ":" << verse;
	return ss.str();
}


