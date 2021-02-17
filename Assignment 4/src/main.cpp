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

        if (argcommand == "-b"){
            time_t begin, end;
            time(&begin);
            //Build 64 sorted inverted lists with the html collection clean text
            buildInvertedLists();
            //Merge the created inverted lists into a unique also sorted list,
            mergeInvertedLists();
            //Create a dictionary to store the information about the occurrences
            //of each term on the final inverted list created, to get more efficient
            //access to the occurrences information of each term
            buildInFileDictionary();
            time(&end);
            time_t elapsed = end - begin;

            long hours = elapsed/3600;
            long minutes = (elapsed%3600)/60;
            long seconds = elapsed - (hours*3600) - (minutes*60);

            cout << endl << "Running approximate time " << hours << "h " << minutes << "m " << seconds << "s " << endl; 
        }
        else if (argcommand == "-q"){
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
