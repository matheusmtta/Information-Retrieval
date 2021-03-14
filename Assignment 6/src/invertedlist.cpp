#include "includes/invertedlist.h"

using namespace std;

//Function used to sort the inverted list by the termContainer term id,
//document id and occurrence position respectively
bool comp(const termContainer &lhs, const termContainer &rhs){
    if (lhs.id == rhs.id and lhs.doc == rhs.doc)
        return lhs.pos < rhs.pos;
    if (lhs.id == rhs.id)
        return lhs.doc < rhs.doc;
    return lhs.id < rhs.id;
}

//From a readen tuple string split the tuple into its respective
//term container data, term id, document id and occurrence position  
termContainer getContainer(string strTuple){
    int id;
    int doc, pos;

    int len = strTuple.size();
    string tmp;

    int i = 0;
    while (i < len && strTuple[i] != ' ') 
        tmp += strTuple[i++];
    id = stoi(tmp);
    tmp.clear();

    i++;
    while (i < len && strTuple[i] != ' ') 
        tmp += strTuple[i++];
    doc = stoi(tmp);
    tmp.clear();

    i++;
    while (i < len) 
        tmp += strTuple[i++];
    pos = stoi(tmp);
    
    termContainer currContainer(id, doc, pos);

    return currContainer;
}

void buildInvertedLists(){
    //Initialize HTML parser and define the access path
    //to the HTML collection
    Parser parser;
    string pathCollection = "files/collection.jl";

    //Get access to the HTML collection
    ifstream infile(pathCollection);
    if(infile.fail()){
        cout << "Path to file '" << pathCollection << "' not found" << endl;
        exit(0);
    }
    string currLine;

    //Initialize the file hash map that will maintain information
    //about the id of each occurring term and its respective frequency
    //in the collection (n_i)
    vector <string> vocabulary;
    unordered_map <string, int> termHashKey;
    unordered_map <string, pair<int, int>> termFrequency;
    
    //Get the output path files to the vocabulary ids and url ids,
    string pathTermsID = "output/vocabularyIdList.txt";
    ofstream outvocabularyfile(pathTermsID.c_str());

    string pathUrlsID = "output/urlIdList.txt";
    ofstream outurllistfile(pathUrlsID.c_str());

    //Set the number of HTML documents that will be indexed
    //per inverted list file and total number of HTML documents
    int maxDocumentsPerFile = 15625;
    int currDocument = 0;

    //Create 64 initial sorted files whre each of them will
    //have an inverted list constructed wih maxDocumentsPerFile HTML
    //documents
    cout << "Building Inverted Lists on file... " << endl;
    bool finish = false;
    for (int doc = 0; doc < 64; doc++){
        cout << doc << ' ';
        //Create a empty inverted list where each position is
        //a term container, i.e, a tuple with the term and its
        //respective document and position
        vector <termContainer> invertedlist;

        //Just to fit the last 68 html documents
        if (doc == 63) maxDocumentsPerFile += 68;
        for (int i = 0; i < maxDocumentsPerFile; i++){
            //For each document in each main file we will
            //parse the json information from the collection
            //and parse the html content and get the clean text
            if (!getline(infile, currLine)){
                finish = true;
                break;
            }
            rapidjson::Document lineDocument;
            lineDocument.Parse(currLine.c_str());

            string html = lineDocument["html_content"].GetString();
            string url = lineDocument["url"].GetString();
            string cleanText = parser.getCleanHTML(html) + " ";

            outurllistfile << currDocument << ' ' << url << '\n';

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
                            if (!termHashKey.count(currTerm)){
                                termHashKey[currTerm] = (int)termHashKey.size();
                                vocabulary.push_back(currTerm);

                                termFrequency[currTerm].first = 1;
                                termFrequency[currTerm].second = currDocument;
                            }
                            if (termFrequency[currTerm].second != currDocument){
                                termFrequency[currTerm].first += 1;
                                termFrequency[currTerm].second = currDocument;
                            }

                            int id = termHashKey[currTerm]; 


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
        cout << "DONE" << endl;        
    }

    for (string term : vocabulary)
        outvocabularyfile << term << ' ' << termHashKey[term] << ' ' << termFrequency[term].first << '\n';

    infile.close();
    outvocabularyfile.close();
    outurllistfile.close();
}

//Iterate through the inverted list created and mege them into one file in log_2(64) steps
//similarly to merge sort
void mergeInvertedLists(){
    cout << endl << "Merging Inverted Lists on file..." << endl;
	for (int it = 6; it > 0; it--){
        for (int j = 0, doc = 0; j < (1 << it); j += 2, doc++){
        	string pathfileA, pathfileB, pathfileMerge;
            
            //Open file A and B which will merged into one new file 
            pathfileA = "output/invertedList_" + to_string(it) + "_" + to_string(j) + ".txt";
            pathfileB = "output/invertedList_" + to_string(it) + "_" + to_string(j+1) + ".txt";
            ifstream infileA(pathfileA.c_str());
            ifstream infileB(pathfileB.c_str());

            //Create new file
            if (it-1 == 0)
            	pathfileMerge = "output/invertedList.txt";
            else
            	pathfileMerge = "output/invertedList_" + to_string(it-1) + "_" + to_string(doc) + ".txt";
            ofstream outfileMerge(pathfileMerge.c_str());

            string lineContentA, lineContentB;

            //Get the first line from each document to initialize the comparisons
            getline(infileA, lineContentA);
            getline(infileB, lineContentB);

            termContainer lhsTerm = getContainer(lineContentA);
            termContainer rhsTerm = getContainer(lineContentB);

            //Iterate through both files till both of them get all of its lines read
            bool goNextA = true, goNextB = true;
            while (goNextA || goNextB){
                //Compare the next line of file A and B
                //if the next line of A is smaller the B
                //write the line content of A on the new file
                if (comp(lhsTerm, rhsTerm)){
                    outfileMerge << lineContentA << "\n";
                    goNextB =  false, goNextA = true;
                }
                else{ //Otherwise, writhe the line content of B
                    outfileMerge << lineContentB << "\n";
                    goNextB = true, goNextA = false;
                }
                //As A was smaller, we need to take the next line from A
                if (goNextA == true){
                    if (getline(infileA, lineContentA)){
                        lhsTerm = getContainer(lineContentA);
                    } //If there're no more lines to be read on A, write the
                    //remaining lines of B in the new file
                    else {
                        outfileMerge << lineContentB << "\n";
                        while (getline(infileB, lineContentB))
                            outfileMerge << lineContentB << "\n";
                        goNextA = goNextB = false;
                    }
                }
                //As B was smaller, we need to take the next line from B
                if (goNextB == true){ 
                    if (getline(infileB, lineContentB)){
                        rhsTerm = getContainer(lineContentB);
                    }
                    //If there're no more lines to be read on A, write the
                    //remaining lines of B in the new file
                    else {
                        outfileMerge << lineContentA << "\n";
                        while (getline(infileA, lineContentA))
                            outfileMerge << lineContentA << "\n";
                        goNextA = goNextB = false;
                    }
                }
            }
            infileA.close(); infileB.close(); outfileMerge.close();
            //Delete the merged files
            remove(pathfileA.c_str());
            remove(pathfileB.c_str());
        }
    }
    cout << "Final Inverted List saved at 'output/invertedList.txt.'" << endl << endl;
}