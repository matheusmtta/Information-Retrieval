#include "includes/display.h"

using namespace std;

//Function that defines the sorting of the terms in the given
//inverted index, it will sort from the element that appears 
//the most to the least
bool comp(const InvertedList &a, const InvertedList &b){
    if (a.totalOccurrences == b.totalOccurrences)
        return a.word < b.word;
    return a.totalOccurrences > b.totalOccurrences;
}

//Display inverted index on terminal (Not recommended to large inputs)
void Display::displayInvertedIndex(){
    for (InvertedList x : invertedIndex){
   		for (int i = 0; i < 22; i++){
   			if (i < (int)x.word.size())
   				cout << x.word[i];
   			else
   				cout << " ";
   		}
   		cout << x.numDocuments << "     ";
   		for (int i = 0; i < x.numDocuments; i++){
   			cout << "[" << x.wordOccurrences[i].first << ", ";
   			cout << x.wordOccurrences[i].second.first << " [";
   			for (int j = 0; j < x.wordOccurrences[i].second.first; j++){
   				cout << x.wordOccurrences[i].second.second[j];
   				if (j < x.wordOccurrences[i].second.first-1)
   					cout << ", ";
   				else cout << "]";
   			}
   			cout << "]";
   			if (i != x.numDocuments - 1)
   				cout << ", ";
   		}
   		cout << endl;
    }
}

//Write inverted index on disc without formatting, just maintain
//raw information from the inverted index that can be easily receveived as
//an input for other algorithms
void Display::saveRawInvertedIndex(){
	string path = "output/rawInvertedIndex.txt";
	
	ofstream outfile;
	outfile.open(path);

	for (InvertedList x : invertedIndex){
		outfile << x.word << " " << " " << x.numDocuments << " ";
   		for (int i = 0; i < x.numDocuments; i++){
   			outfile << x.wordOccurrences[i].first << " " << x.wordOccurrences[i].second.first << " ";
   			for (int j = 0; j < x.wordOccurrences[i].second.first; j++)
   				outfile << x.wordOccurrences[i].second.second[j] << " ";
   		}
   		outfile << endl;
    }

	outfile.close();
}

//Write inverted index on disc with proper formatation, as the example given
//on class and presented at the documentation file
void Display::saveFormattedInvertedIndex(){
    string path = "output/formattedInvertedIndex.txt";

    ofstream outfile;
    outfile.open(path);

    for (InvertedList x : invertedIndex){
        for (int i = 0; i < 22; i++){
            if (i < (int)x.word.size())
                outfile << x.word[i];
            else
                outfile << " ";
        }
        outfile << x.numDocuments << "     ";
        for (int i = 0; i < x.numDocuments; i++){
            outfile << "[" << x.wordOccurrences[i].first << ", ";
            outfile << x.wordOccurrences[i].second.first << " [";
            for (int j = 0; j < x.wordOccurrences[i].second.first; j++){
                outfile << x.wordOccurrences[i].second.second[j];
                if (j < x.wordOccurrences[i].second.first-1)
                    outfile << ", ";
                else outfile << "]";
            }
            outfile << "]";
            if (i != x.numDocuments - 1)
                outfile << ", ";
        }
        outfile << endl;
    }
}