#include <bits/stdc++.h>

#include "includes/parser.h"
#include "includes/invertedlist.h"

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"

using namespace std;

int main() {
    //Initialize HTML parser and define the access path
    //to the HTML collection
    Parser parser;
    string pathCollection = "files/collection.jl";

    //Get access to the HTML collection
    ifstream infile(pathCollection);
    string currLine;

    //Initialize the hash map that will maintain information
    //about the id of each occurring term
    unordered_map <string, int> wordHashKey;
    string pathTermsID = "output/vocabulary_id.txt";
    ofstream outhashfile(pathTermsID.c_str());

    //Set the number of HTML documents that will be indexed
    //per inverted list file and total number of HTML documents
    int maxDocumentsPerFile = 2;
    int currDocument = 0;

    //Create 64 initial sorted files whre each of them will
    //have an inverted list constructed wih maxDocumentsPerFile HTML
    //documents
    bool finish = false;
    for (int doc = 0; doc < 64; doc++){
        //Create a empty inverted list where each position is
        //a term container, i.e, a tuple with the term and its
        //respective document and position
        vector <termContainer> invertedlist;

        for (int i = 0; i < maxDocumentsPerFile; i++){
            //For each document in each main file we will
            //parse the json information from the collection
            //and parse the html content and get the clean text
            if (!getline(infile, currLine)){
                finish = true;
                break;
            }
            rapidjson::Document lineDocument;
            lineDocument.Parse<0>(currLine.c_str());

            string html = lineDocument["html_content"].GetString();
            string cleanText = parser.getCleanText(html) + " ";

            //Iterate through the clean text searching for separator caracters
            //and extracting each individual term and adding those which hasn't
            //had appeared so far to the word hash, which will associate an id to
            //it. Furthermore, adding each individual term and its respective document
            //and postion to the inverted list
            int textSize = cleanText.size(), pos = 0;
            for (int j = 0; j < textSize; j++){
                string currTerm;
                for (int k = j; k < textSize; k++){
                    if (cleanText[k] == ' '){
                        if (!currTerm.empty()){
                            if (!wordHashKey.count(currTerm)){
                                wordHashKey[currTerm] = (int)wordHashKey.size();
                                outhashfile << currTerm << ' ' << wordHashKey[currTerm] << '\n';
                            }

                            int id = wordHashKey[currTerm]; 

                            termContainer tmpTermContainer(id, currDocument, pos);
                            invertedlist.push_back(tmpTermContainer);

                            pos++;
                        }
                        j = k;
                        break;
                    }
                    currTerm += cleanText[k];
                }
            }
            currDocument++;
        }
        if (finish)
            break;

        //Sort the inverted list on the main memory with relation to the id
        //document and position respectively 
        sort(invertedlist.begin(), invertedlist.end(), comp);

        //Write on file the sorted inverted list
        string outputPath = "output/invertedList_6_" + to_string(doc) + ".txt";
        ofstream outlistfile(outputPath.c_str());

        for (termContainer tuple : invertedlist)
            outlistfile << tuple.id << ' ' << tuple.doc << ' ' << tuple.pos << '\n';        
    }
    infile.close();
    outhashfile.close();

    //Merge the 64 sorted inverted lists into a unique also sorted list,
    //and create a dictionary to store the information about the occurrences
    //of each term on the final inverted list created, to get more efficient
    //access to the information of each term
    mergeInvertedLists();
    buildDictionary();

    return 0;
}
