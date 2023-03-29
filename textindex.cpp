/*****************************************************************************/
/*  Filename:     textindex.cpp                                              */
/*  Author(s):    Jim Skon                                                   */
/*  Last Update:  2/16/2018 by Bob Kasper (comments and formatting)          */
/*                                                                           */
/*  This code creates an inverted index of all the words in some text        */
/*  as a STL map structure that maps from words to positions in the file.    */
/*  The resulting structure can than be used to retrieve a complete list of  */
/*  all places in the file that contain a certain word.                      */
/*                                                                           */
/*  The function buildTextIndex(filename) creates the inverted index.        */
/*                                                                           */
/*  The function indexSearch(string) returns a STL list structure of         */
/*  all positions where that word is found in the file.                      */
/*****************************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <stdlib.h>
#include <map>
#include <utility>
#include <list>
#include <set>
// Include stemming stuff
#include "english_stem.h"
using namespace std;
#include "textindex.h"

map<string, list<int> > refs; // represents the inverted index
                        // to be accessed and updated only by functions below

/*****************************************************************************/
/*      string getNextWord(string&)                                          */
/*          given a string, return the next word,                            */
/*          and remove this word from the string (a destructive function)    */
/*****************************************************************************/
string getNextWord(string& line) {
  string word="";
  int pos;

  if (line.length() > 1)
        {
          pos = line.find_first_of(".,; :?!""''()");
          /* get next word */
          if (pos != 0) {
                word = line.substr(0,pos);
        }
          /* trim the line */
          if (pos == string::npos) {
                pos = line.length()-1;
          }
          line = line.substr(pos+1, 2000);
        }
  return word;
}







/*****************************************************************************/
/*      buildTextIndex - reads text file and constructs inverted index       */
/*****************************************************************************/
int buildTextIndex (string filename) {
  ifstream  infile;     /* input file descriptor */
  int position;         /* location of line in the file */
  string text, line, word;
  int wordcount = 0, rootcount = 0, pos;
  stemming::english_stem<char, std::char_traits<char> > StemEnglish;

  /* open text file */
  infile.open(filename.c_str(), ios::in);
  if (!infile) {
        cerr << "Error - can't open input file: " << filename << endl;
        return 0;  /* false, indicates error */
  }

  /* process text file */
  while (!infile.fail()) {
    /* Get the file position at beginning of line */
    position = infile.tellg();
    /* get the next line */
    getline(infile,line);
    /* Convert line to upper case*/
    transform(line.begin(), line.end(), line.begin(), ::toupper);

        /* for each word in the line, stem it and put stem in map */
        while (line.length() > 1) {
          word = getNextWord(line);
          if (word.length() > 0 ) {
                /* Reduce to Stem */
                StemEnglish(word);
                /* Convert to upper case*/
                transform(word.begin(), word.end(),word.begin(), ::toupper);
                wordcount++;
                refs[word].push_back(position); // remember offset for current line
          }
        } // end while loop for one line

  } // end while loop for lines of file
  cout << "Words: " << wordcount << " Unique: " << refs.size() << endl;
  return 1;  /* true, indicates success */
}

/*****************************************************************************/
/*  indexSearch(string)                                                      */
/*      returns a list of offsets for all lines                              */
/*      containing a word that matches the input string                      */
/*****************************************************************************/
list<int> indexSearch(string word) {
  map<string, list<int> >::iterator it;  // iterator for find
  list<int> blank; // return for no matches

  /* Convert search to all caps */
  transform(word.begin(), word.end(),word.begin(), ::toupper);
  /* return the refs */
  /* First use find, so as to NOT create a new entry in refs */
  it = refs.find(word);
  if (it == refs.end()) {
      return(blank);
  } else {
      return (refs[word]);
  }
}
