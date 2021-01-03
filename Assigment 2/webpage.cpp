#include "webpage.h"

bool operator<(const webPage &lhs, const webPage &rhs){
	if (rhs.urlWgt != lhs.urlWgt) return rhs.urlWgt < lhs.urlWgt;
	return rhs.domainWgt < lhs.domainWgt;
}
