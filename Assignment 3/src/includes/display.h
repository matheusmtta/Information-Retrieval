#ifndef DISPLAY_H
#define DISPLAY_H

#include <bits/stdc++.h>
#include "invertedlist.h"

using namespace std;

//Define the inverted index sorting function
bool comp(const InvertedList &a, const InvertedList &b);

class Display{
	public:
		vector <InvertedList> invertedIndex;
		
		//Define display functions
		void displayInvertedIndex();
		void saveRawInvertedIndex();
		void saveFormattedInvertedIndex();

		//Define the display constructor, that will receive an invertedindex, and a boolean
		//parameter which will decide if the invertedlist is going to be sorted by the comp
		//function or not.
		Display(vector <InvertedList> basisIndex, bool toSort) : invertedIndex(basisIndex) {
			if (toSort)
				sort(invertedIndex.begin(), invertedIndex.end(), comp);
		};
};


#endif