#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "parser.h"
#include <bits/stdc++.h>
#include "../rapidjson/document.h"
#include "../rapidjson/stringbuffer.h"

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
		unordered_map <string, int> compressUrl;
		unordered_map <int, double> urlPageRank;
		unordered_map <int, int> termFrequency;
		map <int, int> termDictLine; 
		vector <dictContainer> dictionary;
	public:	
		Dictionary();
		int query(string queryStr);
		void queryTerm(string term);
		void generateResults();
};

void checkFileDictionary(ifstream &file, string path);
void buildInFileDictionary();

string getNextSlice(int &idx, string str);

#endif