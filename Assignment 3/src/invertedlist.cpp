#include "includes/invertedlist.h"

using namespace std;

//Create a list for a given a word
InvertedList::InvertedList(string word){
	this->word = word;

	numDocuments = 0;
	totalOccurrences = 0;
}

//It will receive the document and position where
//the list word occurs and properly add it to the occurrence list
void InvertedList::addWord(int document, int pos){
	//If it is not the first word occurrence at the document
	if (compress.count(document)){
		//get the wordoccurrences document id
		int id = compress[document];

		//Properly increment the count occurrences variable
		//and add the new position to the list of occurrences
		//at the given document
		wordOccurrences[id].second.first++;
		wordOccurrences[id].second.second.push_back(pos);

		//Increment the number of total occurrences of the word
		totalOccurrences++;
	}
	//If there's no such document at the occurrence list yet
	//we create a new id and position for the given document
	//and its respective first occurrence position
	else {
		//Set the document list ID as the current number of documents
		compress[document] = numDocuments;
		decompress[numDocuments] = document;

		int id = compress[document];

		//Add a new element to the wordOccurrence vector which can
		//be accessed by the id = compress[document]
		wordOccurrences.push_back({0, {0, vector<int>()}});

		//Set the wordOccurrences[id] position with the given document
		//increment the number of the word occurrences at the docuemnt and
		//save the postion in which it occurs 
		wordOccurrences[id].first = document;
		wordOccurrences[id].second.first++;
		wordOccurrences[id].second.second.push_back(pos);

		//Increment the number of documents n_i where word appears
		//and increment the number of total occurrences
		numDocuments++;
		totalOccurrences++;
	}
}