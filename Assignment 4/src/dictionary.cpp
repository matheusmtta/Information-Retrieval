#include "includes/dictionary.h"

//Check if the input file exists, if it doesn't end the program execution
void checkFile(ifstream &file, string path){
    if (file.fail()){
        cout << "Path to file '" << path << "' not found." << endl;
        exit(0);
    }
}

void buildInFileDictionary(){
	string pathInvertedList = "output/invertedList.txt";
    ifstream infileInvertedList(pathInvertedList, std::ifstream::binary);
    checkFile(infileInvertedList, pathInvertedList);

    string pathDictionary = "output/dictionary.txt";
    ofstream outfileDict(pathDictionary.c_str());

    //Get the first tuple from the inverted list and the first term id
    string currTuple;
    getline(infileInvertedList, currTuple);

    int idx = 0; string tmp;
    while (currTuple[idx] != ' ') 
        tmp += currTuple[idx++];
    long long currTupleId = stoi(tmp);

    long long lastTupleId = currTupleId;
    long long lastLine = 0, currLine = 0;

    //We'll iterate through the inverted list setting the information blocks
    //of each term
    cout << "Building dictionary on file..." << endl;
    while (true){
        //If there're no more tuples to be read, print the last term block and break the loop
        if (!getline(infileInvertedList, currTuple)){
            outfileDict << lastTupleId << ' ' << lastLine << ' ' << currLine << '\n';
            break;
        }

        idx = 0; tmp.clear();
        while (currTuple[idx] != ' ') 
            tmp += currTuple[idx++];
        currTupleId = stoi(tmp);

        //if the current tuple term id is different then the last we set a new access
        //block relative to the last term in the inverted list
        if (currTupleId != lastTupleId) {
            outfileDict << lastTupleId << ' ' << lastLine << ' ' << currLine << '\n';
            lastLine = currLine;
        }

        //Update the last term id found
        lastTupleId = currTupleId;
        currLine = infileInvertedList.tellg();
    }
    cout << "Dictionary saved at 'output/dictionary.txt'." << endl;
    outfileDict.close();
    infileInvertedList.close();
}

//Get the next substring of a string separated with spaces
string getNextSlice(int &idx, string str){
    int len = str.size();
    string tmp;

    while (idx < len && str[idx] != ' ')
        tmp += str[idx++];
    idx++;

    return tmp;
}

Dictionary::Dictionary(){
    //Load files in the main memory
    cout << "Loading files..." << endl;
    string pathDictionary = "output/dictionary.txt";
    string pathVocabulary = "output/vocabularyIdList.txt";
    string pathURLsIndex = "output/urlIdList.txt";

    ifstream infileDictionary(pathDictionary);
    checkFile(infileDictionary, pathDictionary);

    ifstream infileVocabulary(pathVocabulary);
    checkFile(infileVocabulary, pathVocabulary);
    
    ifstream infilePathUrlInd(pathURLsIndex);
    checkFile(infilePathUrlInd, pathURLsIndex);

    string currVocabLine;
    //Construct the term id and vocabulary information with hash maps
    while (getline(infileVocabulary, currVocabLine)){
        int idx = 0;

        string term = getNextSlice(idx, currVocabLine);
        int id = stoi(getNextSlice(idx, currVocabLine));
        int frequency = stoi(getNextSlice(idx, currVocabLine));

        termFrequency[id] = frequency;
        compressTerm[term] = id;
        decompressTerm[id] = term;
    }

    string currUrlIndLine;
    //Construct the documents id and urls with hash maps
    while (getline(infilePathUrlInd, currUrlIndLine)){
        int idx = 0;

        int id = stoi(getNextSlice(idx, currUrlIndLine));
        string url = getNextSlice(idx, currUrlIndLine);

        decompressUrl[id] = url;
    }

    string currInfileDictLine;
    //Construct the inverted list dictionary with the relative information
    //of each unique term of the collection 
    while (getline(infileDictionary, currInfileDictLine)){
        int idx = 0; 

        //Get term id and initial/end position in bytes of the term 
        //in the inverted list to efficient access
        int id = stoi(getNextSlice(idx, currInfileDictLine));
        long long init = stoll(getNextSlice(idx, currInfileDictLine));
        long long end = stoll(getNextSlice(idx, currInfileDictLine));
        
        //Set the term frequency of each term in the vocabulary
        //and its respective idf
        int n_i = termFrequency[id];
        double idf = log2((double)decompressUrl.size()/(double)n_i);

        dictContainer currDictLine(init, end, n_i, idf);

        termDictLine[id] = (int)dictionary.size();
        dictionary.push_back(currDictLine);
    }
    cout << "Files successfully loaded on main memory." << endl;
}

void Dictionary::query(string term){
    //Search for a give term in the inverted list
    cout << "Searching for '" << term << "' in the collection inverted list..." << endl << endl; 
    if (compressTerm.find(term) == compressTerm.end()){
        cout << "There are no matching documents for this term in the collection." << endl;
        return;
    }

    int id = compressTerm[term], dictionaryIdx = termDictLine[id], countDoc = 0, countTerm = 0;
    long long init = dictionary[dictionaryIdx].init;
    long long end = dictionary[dictionaryIdx].end;
    
    string pathInvertedList = "output/invertedList.txt";
    ifstream infileInvertedList(pathInvertedList, ifstream::binary);
    checkFile(infileInvertedList, pathInvertedList);

    //get the first line in the inverted list relative to the given term
    infileInvertedList.seekg(init, ios::beg);
    
    //Save all unique urls where the term occurs
    set <int> documents;

    cout << "OCCURRENCES (INVERTED LIST): " << endl;
    string currInvertedListLine;
    while (getline(infileInvertedList, currInvertedListLine) && infileInvertedList.tellg() <= end){
        int idx = 0;

        string garb = getNextSlice(idx, currInvertedListLine);
        int idDoc = stoi(getNextSlice(idx, currInvertedListLine));
        //To print the occurrences individual information on terminal erase the comment below
        //cout << "    " << currInvertedListLine << endl;

        documents.insert(idDoc);
        countTerm++;
    }

    cout << endl << endl << "URLS: " << endl;
    for (int docId : documents){
        //To print the urls erase the comment below
        //cout << "    " << docId << "     " << decompressUrl[docId] << endl;
        countDoc++;
    }

    cout << "    " << endl << endl << "RESULTS: " << endl;
    cout << "    " << "Term:  " << term << endl;
    cout << "    " << "Total Occurrences: " << countTerm << endl;
    cout << "    " << "Documents (n_i):   " << countDoc << endl;
    cout << "    " << "IDF: " <<  dictionary[termDictLine[id]].idf << endl << endl << endl; 
}   