/*  Filename:     textindex.h
 *  Author(s):    Jim Skon, Bob Kasper
 *  Last Update:  2/12/2022
 *
 *  Definitions: build and access Inverted Index
 *               of TEXT_FILENAME
 */

#include <list>
using namespace std;

#define TEXT_FILENAME "/var/www/html/class/csc3004/Shakespeare.txt"
// LOG_FILENAME changed from /tmp to special class directory for improved web security
#define LOG_FILENAME "/home/class/csc3004/tmp/dailatham-sslookup.log"

// interface functions

string getNextWord(string&);
int buildTextIndex (string);
list<int> indexSearch(string);
