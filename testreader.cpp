// testreader.cpp

#include "Ref.h"
#include "Verse.h"
#include "Bible.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

int main(int argc, char** argv) {
	// Create Bible object to process the raw text file
	Bible webBible("/home/class/csc3004/Bibles/web-complete");

	Verse verse;
	int b, c, v, NumV, n = 0;
	LookupResult result;
	bool debug = true;

	if (argc == 1) {
		cout << "--------------------------------------------------" << endl;
		cout << "Error: everything is missing" << endl;
		return 34;
	}
	else if (argc == 2) {
		cout << "--------------------------------------------------" << endl;
		cout << "Error: chapter and verse number are missing" << endl;
		return 34;
	}
	else if (argc == 3) {
		cout << "--------------------------------------------------" << endl;
		cout << "Error: verse number is missing" << endl;
		return 34;
	}
	else if (argc > 5) {
		cout << "--------------------------------------------------" << endl;
		cerr << "Error: too many arguments" << endl;
		return 34;
	}

	// Assign value
	b = stoi(argv[1]);
	c = stoi(argv[2]);
	v = stoi(argv[3]);
	n = 0;
	if (argc > 4) {
		n = stoi(argv[4]);
	}

	// Create a reference from the numbers
	Ref ref(b, c, v);

	// DEBUG
	if (debug){
		cout << "b = " << b << endl;
		cout << "c = " << c << endl;
		cout << "v = " << v << endl;
		cout << "n = " << n << endl;
	}

	// Value to help keep track of program.
	int count = 0;
	int curVNum = 0;


	// Look up the first verse base on reference and display the result.
	//cout << "--------------------------------------------------" << endl;
	verse = webBible.lookup(ref, result);
	count++;
	curVNum = ref.getVerse();

	// DEBUG
	if ( true ) {
		cout << "RESULT STATUS: " << result << endl;
		if (result == SUCCESS) {cout << "SUCCESS" << endl;}
		if (result == NO_VERSE) {cout << "NO_VERSE"<< endl;}
		if (result == NO_CHAPTER) {cout << "NO_CHAPTER" << endl;}
		if (result == NO_BOOK) {cout << "NO_BOOK" << endl;}
	}
	// Report Verse
/*	if (!(verse.getRef() == ref)){
		ref.displayNonExisted();
		cout << endl;

		if (result != SUCCESS){
			cout << webBible.error(result) << endl;
		}
		exit(2);
	} else {
		verse.display();
		cout << endl;
	}
*/
	if (!(verse.getRef() == ref)){
		ref.displayNonExisted();
		cout << "\n" << webBible.error(result) << endl;
		exit(2);
	} else {
		verse.display();
		cout << endl;
	}

	// Multi-verse
	while (count < n) {
		verse = webBible.nextVerse(result);
		count++;
		curVNum = verse.getVerseNum();

		// Report Verse
		if ( curVNum == 1) {
			verse.display();
			cout << endl;
		} else {
			verse.onlyVerseDisplay();
			cout << endl;
		}
	}
	return 0;
}

