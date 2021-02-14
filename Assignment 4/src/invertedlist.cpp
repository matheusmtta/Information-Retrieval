#include "includes/invertedlist.h"

using namespace std;

bool comp(const termContainer &lhs, const termContainer &rhs){
	if (lhs.word == rhs.word and lhs.doc == rhs.doc)
		return lhs.pos < rhs.pos;
	if (lhs.word == rhs.word)
		return lhs.doc < rhs.doc;
	return lhs.word < rhs.word;
}

termContainer getContainer(string strTuple){
	string word;
	int doc, pos;

	int len = strTuple.size(), slice1 = 0, slice2 = 0;

	for (int i = 0; i < len; i++){
		if (!slice1 && strTuple[i] == ' ')
			slice1 = i; 
		if (slice1 && strTuple[i] == ' ')
			slice2 = i;
	}

	word = strTuple.substr(0, slice1-1);
	doc = stoi(strTuple.substr(slice1+1, slice2-1));
	pos = stoi(strTuple.substr(slice2+1, len));

	termContainer currContainer(word, doc, pos);

	return currContainer;
}
