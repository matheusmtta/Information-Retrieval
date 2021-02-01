#ifndef INVERTED_LIST_H
#define INVERTED_LIST_H

#include <bits/stdc++.h>

using namespace std;

struct InvertedList {
	public:
		string word;

		int numDocuments;
		int totalOccurrences;
		
		//Hash compressors to get usuful the access key from
		unordered_map <int, int> compress; //document to idx
		unordered_map <int, int> decompress; //idx to document at the wordoccurrences vector 

		//Each vector postion will be defined by {doc, number of occurrences at doc, occurrence positions}
		vector <pair<int, pair<int, vector<int>>>> wordOccurrences; //idx to list of occurrences

		//Construct the list for a given word
		InvertedList(string word);

		//Add a new document and position for the word occurrence list 
		void addWord(int document, int position); 
};

#endif