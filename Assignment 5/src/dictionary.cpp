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

void Dictionary::queryTerm(string term){
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

bool rankComp(const pair<int, double> &lhs, const pair<int, double> &rhs){
    if (lhs.second == rhs.second)
        return lhs.first < rhs.first;
    return lhs.second > rhs.second;
}   

void Dictionary::query(string queryStr){
    auto initialExeTime = chrono::high_resolution_clock::now();

    Parser parser;
    string cleanQueryText = parser.getCleanText(queryStr);
    
    vector <string> individualTerms;

    int idx = 0;
    unordered_map <int,int> qryTerms;
    while (idx < (int)queryStr.size()){
        string term = getNextSlice(idx, cleanQueryText);

        if (compressTerm.find(term) == compressTerm.end()){
            cout << endl << "There're no matching documents for your query." << endl;
            return;
        }

        int id = compressTerm[term];
        if ((int)term.length() > 0 && !qryTerms.count(id))
            individualTerms.push_back(term);
        if ((int)term.length() > 0)
            qryTerms[id]++;
    }

    cout << "Clean query: ";
    for (int i = 0; i < (int)individualTerms.size(); i++)
        cout << individualTerms[i] << ' ';
    cout << endl;

    string pathInvertedList = "output/invertedList.txt";
    ifstream infileInvertedList(pathInvertedList, ifstream::binary);
    checkFile(infileInvertedList, pathInvertedList);

    unordered_map <int, vector<int>> termsPerDocument;
    
    vector <double> termIdf;
    
    for (int i = 0; i < (int)individualTerms.size(); i++){
        string term = individualTerms[i];

        int id = compressTerm[term], dictionaryIdx = termDictLine[id];
        long long init = dictionary[dictionaryIdx].init;
        long long end = dictionary[dictionaryIdx].end;
        
        infileInvertedList.seekg(init, ios::beg);
    
        string currInvertedListLine;

        while (getline(infileInvertedList, currInvertedListLine) && infileInvertedList.tellg() <= end){
            int idx = 0;
            string garb = getNextSlice(idx, currInvertedListLine);
            int idDoc = stoi(getNextSlice(idx, currInvertedListLine));

            if (termsPerDocument.find(idDoc) == termsPerDocument.end())
                termsPerDocument[idDoc].resize((int)individualTerms.size(), 0);

            termsPerDocument[idDoc][i]++;
        }

        termIdf.push_back(dictionary[id].idf);
        qryTerms[id]++;
    }   

    vector <pair<int, vector<double>>> candidates;
    
    for (auto doc : termsPerDocument){
        int e = 1;
        for (int termCount : doc.second) 
            e |= termCount;

        vector <double> tmp;

        if (e){
            vector <double> tmp;

            for (int i = 0; i < (int)individualTerms.size(); i++){
                double w_i = (1.0 + log2((double)doc.second[i]))*termIdf[i];
                tmp.push_back(w_i);
            }
            candidates.push_back({doc.first, tmp});
        }
    }

    if (candidates.size() == 0){
        cout << endl << "There're no matching documents for your query." << endl;
        return;
    }

    vector <double> basis;

    for (int i = 0; i < (int)individualTerms.size(); i++){
        int id = compressTerm[individualTerms[i]];
        double w_i = (1.0 + log2((double)qryTerms[id]))*termIdf[i];
        basis.push_back(w_i);
    }

    vector <pair<int, double>> resultQuery;

    for (int i = 0; i < (int)candidates.size(); i++){
        double dotAB = 0, normA = 0, normB = 0;
        for (int j = 0; j < (int)individualTerms.size(); j++){
            dotAB += basis[j]*candidates[i].second[j];

            normA += candidates[i].second[j]*candidates[i].second[j];
            normB += basis[j]*basis[j];
        }
        double sym = dotAB/(sqrt(normA)*sqrt(normB));
        resultQuery.push_back({candidates[i].first, sym});
    }

    sort(resultQuery.begin(), resultQuery.end(), rankComp);

    auto finalExeTime = chrono::high_resolution_clock::now();
    double exeTime = chrono::duration_cast<chrono::microseconds>(finalExeTime - initialExeTime).count();

    cout << endl << endl << "Most relevant document pages:" << endl << endl;
    for (int i = 0; i < 5; i++){
        cout << "[" << i+1 << "]  Document: " << resultQuery[i].first << "     Similarity: " << resultQuery[i].second << endl;
        cout << "     " << decompressUrl[resultQuery[i].first] << endl << endl;
    }
    cout << endl << "Total number of documents " << candidates.size() << " (" << exeTime*1e-6 << " seconds)" << endl << endl;
}