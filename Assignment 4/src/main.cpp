#include <bits/stdc++.h>

#include "includes/invertedlist.h"
#include "includes/dictionary.h"

using namespace std;

int main(int argc, char const* argv[]) {
    if (argc < 2){
        cout << "Invalid Input: Not enough arguments" << endl;
        cout << "Options available" << endl << endl;
        cout << "To build the inverted index and dictionary\n       ./main -b" << endl << endl;
        cout << "Otherwise, to query for a given word in the inverted list\n       ./main -q" << endl; 
        exit(0);
    }
    else{
        stringstream streamInput(argv[1]);
        string argcommand; streamInput >> argcommand;
        //To build the files (inverted list, vocabulary, url list, dictionary)
        if (argcommand == "-b"){
            time_t begin, end;
            time(&begin);
            //Build 64 sorted inverted lists with the html collection clean text
            buildInvertedLists();
            //Merge the created inverted lists into a unique also sorted list,
            mergeInvertedLists();
            //Create a dictionary to store the information about the occurrences
            //of each term on the final inverted list created, to get more efficient
            //access to the information of each term occurrence in the collection
            buildInFileDictionary();
            time(&end);
            time_t elapsed = end - begin;

            int hours = elapsed/3600;
            int minutes = (elapsed%3600)/60;
            int seconds = elapsed - (hours*3600) - (minutes*60);

            cout << endl << "Running approximate time " << hours << "h " << minutes << "m " << seconds << "s " << endl; 
        }
        //To query for a given term in the collection and ''efficiently'' get
        //information about its occurrences 
        else if (argcommand == "-q"){
            //Load the dictionary, vocabulary and url list files on main memory
            Dictionary dict;
            while (true){
                cout << endl << "Type a term:  ";
                string term; cin >> term;

                dict.query(term);
                cout << endl;
            
                cout << "To search for another term, type:    1" << endl;
                cout << "Otherwise, to end program execution type any other string." << endl << endl;
                string command; cin >> command;

                if (command != "1") exit(0);
            }
        }
        else{
            cout << "Invalid Input: Unknown argcommand" << endl;
            cout << "Options available" << endl << endl ;
            cout << "To build the inverted index and dictionary\n       ./main -b" << endl << endl;
            cout << "Otherwise, to query for a given word in the inverted list\n       ./main -q" << endl << endl; 
            exit(0);    
        }
    }

    return 0;
}
