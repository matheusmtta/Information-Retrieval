#include <bits/stdc++.h>

#include "includes/invertedlist.h"
#include "includes/dictionary.h"

using namespace std;

int main(int argc, char const* argv[]) {
    if (argc < 2){
        cout << "Invalid Input: Not enough arguments" << endl;
        cout << "Options available" << endl << endl;
        cout << "(1) To build the inverted index and dictionary\n       ./main -b" << endl << endl;
        cout << "(2) To query for a given word in the inverted list\n       ./main -q" << endl; 
        exit(0);
    }
    else{
        stringstream streamInput(argv[1]);
        string argcommand; streamInput >> argcommand;

        if (argcommand == "-b"){
            //Build 64 sorted inverted lists with the html collection clean text
            buildInvertedLists();
            //Merge the created inverted lists into a unique also sorted list,
            mergeInvertedLists();
            //Create a dictionary to store the information about the occurrences
            //of each term on the final inverted list created, to get more efficient
            //access to the occurrences information of each term
            buildInFileDictionary();
        }
        else if (argcommand == "-q"){
            Dictionary dict;
            while (true){
                cout << "Type command" << endl;
                int command; cin >> command;
                bool raw; cin >> raw;
                if (command == 1){
                    cout << "Type term to search:" << endl;
                    string term; cin >> term;
                    dict.query(term, raw);
                }
                else exit(0);
            }
        }
        else{
            cout << "Invalid Input: Unknown argcommand" << endl;
            cout << "Options available" << endl << endl ;
            cout << "(1) To build the inverted index and dictionary\n       ./main -b" << endl << endl;
            cout << "(2) To query for a given word in the inverted list\n       ./main -q" << endl << endl; 
            exit(0);    
        }
    }

    return 0;
}
