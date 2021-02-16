#include "includes/container.h"

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