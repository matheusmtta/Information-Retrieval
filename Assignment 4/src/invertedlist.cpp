#include "includes/invertedlist.h"

using namespace std;

bool comp(const termContainer &lhs, const termContainer &rhs){
	if (lhs.id == rhs.id and lhs.doc == rhs.doc)
		return lhs.pos < rhs.pos;
	if (lhs.id == rhs.id)
		return lhs.doc < rhs.doc;
	return lhs.id < rhs.id;
}

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

void mergeInvertedLists(){
	for (int it = 6; it > 0; it--){
        for (int j = 0, doc = 0; j < (1 << it); j += 2, doc++){
        	string pathfileA, pathfileB, pathfileMerge;
            
            pathfileA = "output/invertedList_" + to_string(it) + "_" + to_string(j) + ".txt";
            pathfileB = "output/invertedList_" + to_string(it) + "_" + to_string(j+1) + ".txt";
            ifstream infileA(pathfileA.c_str());
            ifstream infileB(pathfileB.c_str());

            if (it-1 == 0)
            	pathfileMerge = "output/invertedList.txt";
            else
            	pathfileMerge = "output/invertedList_" + to_string(it-1) + "_" + to_string(doc) + ".txt";
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
}

void buildDictionary(){
	string pathInvertedIndex = "output/invertedList.txt";
    ifstream infileInvertedIndex(pathInvertedIndex);

    string pathDictionary = "output/dictionary.txt";
    ofstream outfileDict(pathDictionary.c_str());

    string currTuple;
    getline(infileInvertedIndex, currTuple);

    int idx = 0; string tmp;
    while (currTuple[idx] != ' ') 
        tmp += currTuple[idx++];
    int currTupleId = stoi(tmp);

    int lastTupleId = currTupleId;
    int left = 0, right = 0, flag = false;
    while (true){
        if (!getline(infileInvertedIndex, currTuple)){
            if (!flag)
                outfileDict << lastTupleId << ' ' << left << ' ' << right-1 << '\n';
            break;
        }

        idx = 0; tmp.clear();
        while (currTuple[idx] != ' ') 
            tmp += currTuple[idx++];
        currTupleId = stoi(tmp);

        if (currTupleId != lastTupleId) {
            outfileDict << lastTupleId << ' ' << left << ' ' << right << '\n';
            left = right+1;
            flag = true;
        }
        else flag = false;

        lastTupleId = currTupleId;
        right++;
    }

    outfileDict.close();
    infileInvertedIndex.close();
}