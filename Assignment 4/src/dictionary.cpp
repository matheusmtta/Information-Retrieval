#include "includes/dictionary.h"

void buildInFileDictionary(){
	string pathInvertedList = "output/invertedList.txt";
    ifstream infileInvertedList(pathInvertedList, std::ifstream::binary);

    string pathDictionary = "output/dictionary.txt";
    ofstream outfileDict(pathDictionary.c_str());

    string currTuple;
    getline(infileInvertedList, currTuple);

    int idx = 0; string tmp;
    while (currTuple[idx] != ' ') 
        tmp += currTuple[idx++];
    long long currTupleId = stoi(tmp);

    long long lastTupleId = currTupleId;
    long long lastLine = 0, currLine = 0;

    while (true){
        if (!getline(infileInvertedList, currTuple)){
            outfileDict << lastTupleId << ' ' << lastLine << ' ' << currLine << '\n';
            break;
        }

        idx = 0; tmp.clear();
        while (currTuple[idx] != ' ') 
            tmp += currTuple[idx++];
        currTupleId = stoi(tmp);

        if (currTupleId != lastTupleId) {
            outfileDict << lastTupleId << ' ' << lastLine << ' ' << currLine << '\n';
            lastLine = currLine;
        }

        lastTupleId = currTupleId;
        currLine = infileInvertedList.tellg();
    }

    outfileDict.close();
    infileInvertedList.close();
}

string getNextSlice(int &idx, string str){
    int len = str.size();
    string tmp;

    while (idx < len && str[idx] != ' ')
        tmp += str[idx++];
    idx++;

    return tmp;
}

Dictionary::Dictionary(){
    string pathDictionary = "output/dictionary.txt";
    string pathVocabulary = "output/vocabularyIdList.txt";
    string pathURLsIndex = "output/urlIdList.txt";

    ifstream infileDictionary(pathDictionary);
    ifstream infileVocabulary(pathVocabulary);
    ifstream infilePathUrlInd(pathURLsIndex);

    string currVocabLine;
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
    while (getline(infilePathUrlInd, currUrlIndLine)){
        int idx = 0;

        int id = stoi(getNextSlice(idx, currUrlIndLine));
        string url = getNextSlice(idx, currUrlIndLine);

        decompressUrl[id] = url;
    }

    string currInfileDictLine;
    while (getline(infileDictionary, currInfileDictLine)){
        int idx = 0; 

        int id = stoi(getNextSlice(idx, currInfileDictLine));
        long long init = stoll(getNextSlice(idx, currInfileDictLine));
        long long end = stoll(getNextSlice(idx, currInfileDictLine));
        int n_i = termFrequency[id];
        double idf = log2((int)decompressUrl.size()/n_i);

        dictContainer currDictLine(init, end, n_i, idf);

        termDictLine[id] = (int)dictionary.size();
        dictionary.push_back(currDictLine);
    }
}

void Dictionary::query(string term, bool showUrls){
    if (compressTerm.find(term) == compressTerm.end()){
        cout << "There are no matching documents for this term in the collection" << endl;
        return;
    }

    int id = compressTerm[term], dictionaryIdx = termDictLine[id];
    long long init = dictionary[dictionaryIdx].init;
    long long end = dictionary[dictionaryIdx].end;
    
    string pathInvertedList = "output/invertedList.txt";
    ifstream infileInvertedList(pathInvertedList, ifstream::binary);

    infileInvertedList.seekg(init, ios::beg);
    
    set <int> documents;

    string currInvertedListLine;
    while (getline(infileInvertedList, currInvertedListLine) && infileInvertedList.tellg() <= end){
        int idx = 0;

        string garb = getNextSlice(idx, currInvertedListLine);
        int idDoc = stoi(getNextSlice(idx, currInvertedListLine));

        documents.insert(idDoc);
    }

    for (int docId : documents)
        cout << decompressUrl[docId] << endl;
}   