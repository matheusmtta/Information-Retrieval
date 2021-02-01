#include <bits/stdc++.h>

#include "includes/parser.h"
#include "includes/invertedlist.h"
#include "includes/display.h"

using namespace std;

int main(int argc, char* argv[]) {
    //Get the number of documents that will be
    //indexed by the algorithm
    if (argc < 1)
        exit(0);
    
    int numDocuments = atoi(argv[1]);

    Parser parser;

    unordered_map <string, int> wordHashKey;
    vector <InvertedList> invertedIndex;

    int countTotal = 0, uniqueWords = 0, error = 0;

    for (int doc = 0; doc < numDocuments + error; doc++){
        //for each doc defines the path where the document is
        //saved and parse the document text
        string path = "files/" + to_string(doc) + ".html";
        string text = parser.getCleanText(path) + "  ";

        //If the text is not valid we increment the error
        //variable and go to the next document
        if (text == "fail"){
            error++;
            continue;
        }

        //Iterate through the text caracters and initialize the
        //first word position as pos = 0
        int textSize = text.size(), pos = 0;
        for (int i = 0; i < textSize; i++){
            //Set the current term string as empty, and iterate
            //through the text till we find a separator caracter
            string currWord;
            for (int j = i; j < textSize; j++){
                if (text[j] == ' '){
                    if (!currWord.empty()){
                        //if the word is not empty we check if it does
                        //already have a hash id, if yes, we just add
                        //properly add the document and position to the
                        //vocabulary word id at the inverted index
                        int id;
                        if (wordHashKey.count(currWord)){
                            id = wordHashKey[currWord];

                            invertedIndex[id].addWord(doc, pos);
                        }
                        //Otherwise, we create a new id and invertedlist
                        //to the new vocabulary word
                        else {
                            wordHashKey[currWord] = (int)invertedIndex.size();
                            id = wordHashKey[currWord];

                            InvertedList tmpInvertedList(currWord);
                            invertedIndex.push_back(tmpInvertedList);

                            uniqueWords++;

                            invertedIndex[id].addWord(doc, pos);
                        }
                        //Increment the number of words already found
                        //and increment the position to the next potential
                        //word
                        countTotal++;
                        pos++;
                    }
                    //Set the main index 'i' to 'j', in order to get a linear
                    //complexity to the algorithm
                    i = j;
                    break;
                }
                //Append text[j] caracter to the current word
                currWord += text[j];
            }
        }
    }

    //Create an display object and determine
    //if the index will be sorted (true) or not (false) 
    Display output(invertedIndex, false);

    //Display the inverted index on terminal
    output.displayInvertedIndex();

    cout << endl << "VOCABULARY SIZE: " << uniqueWords << endl;
    cout << "TOTAL NUMBER OF TERMS: " << countTotal << endl;

    //Writes on disk the inverted index
    //output.saveFormattedInvertedIndex(); //formatted, as the examples in given in class
    //output.saveRawInvertedIndex();       //raw just with meaningful information

    return 0;
}
