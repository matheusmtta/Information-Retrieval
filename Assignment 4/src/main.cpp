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
    string pathTermsID = "files/terms_id.txt";
    ofstream outhashfile(pathTermsID.c_str());

    //Set the number of HTML documents that will be indexed
    //per inverted list file and total number of HTML documents
    int maxDocumentsPerFile = 10;
    int currDocument = 0;

    //For each inverted list document
    for (int doc = 0; doc < 64; doc++){
        vector <termContainer> tupleTerm;

        for (int i = 0; i < maxDocumentsPerFile; i++){
            getline(infile, currLine);
            rapidjson::Document lineDocument;
            lineDocument.Parse<0>(currLine.c_str());

            string html = lineDocument["html_content"].GetString();
            string cleanText = parser.getCleanText(html) + " ";

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

                            termContainer tmpTermContainer(currTerm, currDocument, pos);
                            tupleTerm.push_back(tmpTermContainer);

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

        cout << tupleTerm.size() << endl;
        sort(tupleTerm.begin(), tupleTerm.end(), comp);

        string outputPath = "output/invertedList_6_" + to_string(doc) + ".txt";
        ofstream outlistfile(outputPath.c_str());

        for (termContainer tuple : tupleTerm)
            outlistfile << tuple.word << ' ' << tuple.doc << ' ' << tuple.pos << '\n';        
    }
    infile.close();
    outhashfile.close();

    for (int it = 6; it > 0; it--){
        for (int j = 0, doc = 0; j < (1 << it); j += 2, doc++){
            string pathfileA = "output/invertedList_" + to_string(it) + "_" + to_string(j) + ".txt";
            string pathfileB = "output/invertedList_" + to_string(it) + "_" + to_string(j+1) + ".txt";
            ifstream infileA(pathfileA.c_str());
            ifstream infileB(pathfileB.c_str());

            string pathfileMerge = "output/invertedList_" + to_string(it-1) + "_" + to_string(doc) + ".txt";
            ofstream outfileMerge(pathfileMerge.c_str());

            string lineContentA, lineContentB;

            getline(infileA, lineContentA);
            getline(infileB, lineContentB);

            termContainer lhsTerm = getContainer(lineContentA);
            termContainer rhsTerm = getContainer(lineContentB);

            bool goNextA = true, goNextB = true;
            while (goNextA || goNextB){
                if (comp(lhsTerm, rhsTerm)){
                    outfileMerge << lineContentA << "\n";
                    goNextB =  false, goNextA = true;
                }
                else{
                    outfileMerge << lineContentB << "\n";
                    goNextB = true, goNextA = false;
                }
                if (goNextA == true){
                    if (getline(infileA, lineContentA)){
                        lhsTerm = getContainer(lineContentA);
                    }
                    else {
                        outfileMerge << lineContentB << "\n";
                        while (getline(infileB, lineContentB))
                            outfileMerge << lineContentB << "\n";
                        goNextA = goNextB = false;
                    }
                }
                if (goNextB == true){
                    if (getline(infileB, lineContentB)){
                        rhsTerm = getContainer(lineContentB);
                    }
                    else {
                        outfileMerge << lineContentA << "\n";
                        while (getline(infileA, lineContentA))
                            outfileMerge << lineContentA << "\n";
                        goNextA = goNextB = false;
                    }
                }
            }
            infileA.close(); infileB.close(); outfileMerge.close();
            remove(pathfileA.c_str());
            remove(pathfileB.c_str());
        }
    }

    return 0;
}
