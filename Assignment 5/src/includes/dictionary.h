#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "parser.h"
#include <bits/stdc++.h>

using namespace std;

struct dictContainer{
	long long init;
	long long end;
	int frequency;
	double idf;

	dictContainer(long long init, long long end, int frequency, double idf) : init(init), end(end), frequency(frequency), idf(idf) {}
};

class Dictionary{
	private:
		unordered_map <string, int> compressTerm;
		unordered_map <int, string> decompressTerm;
		unordered_map <int, string> decompressUrl;
		unordered_map <int, int> termFrequency;
		map <int, int> termDictLine; 
		vector <dictContainer> dictionary;
	public:	
		Dictionary();
		void query(string queryStr);
		void queryTerm(string term);
};

void checkFileDictionary(ifstream &file, string path);
void buildInFileDictionary();

string getNextSlice(int &idx, string str);

#endif