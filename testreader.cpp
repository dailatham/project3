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
	int b, c, v, n;
	LookupResult result;

	if (argc == 1) {
		cout << "Error: everything is missing" << endl;
		return 34;
	}
	else if (argc == 2) {
		cout << "Error: chapter and verse number are missing" << endl;
		return 34;
	}
	else if (argc == 3) {
		cout << "Error: verse number is missing" << endl;
		return 34;
	}
	else if (argc > 5) {
		cerr << "Error: too many arguments" << endl;
		return 34;
	}
	b = stoi(argv[1]);
	c = stoi(argv[2]);
	v = stoi(argv[3]);
	n = 1;
	if (argc > 4) {
		n = stoi(argv[4]);
	}

	// Create a reference from the numbers
	Ref ref(b, c, v);
	int curV = v;		// current verse


	// Look up the first verse base on reference and display the result.
	cout << "\n--------------------------------------------------" << endl;
	cout << "Result from the original (Bible.lookup) : " << endl;
	verse = webBible.lookup(ref, result);
	curV++;
 	if (!(verse.getRef() == ref)){
		ref.displayNonExisted();
		cout << endl;
		exit(2);
	} else {
		verse.display();
		cout << "\n" << endl;
	}

	cout << "\n--------------------------------------------------" << endl;
	cout << "Result from the index (Bible.lookup) : IN PROCESS.\n" << endl;

/*
	Ref ref(b, c, v);
	verse = webBible.lookup(ref, result);

	if (webBible.error(result) != "") {}
	else {
		verse.display();
		if (argc > 4) {
			if (n > 0) {
				for (int i = 1; i < n; i++) {
					Verse verseNext = webBible.nextVerse(result);
					cout << endl;
					verseNext.display();
					if (result != SUCCESS) {
						break;
					}
				}
			}
		}
		cout << endl;
	}
*/
}
