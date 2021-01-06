#include "taskurl.h"

bool operator<(const TaskUrl &lhs, const TaskUrl &rhs){
	if (rhs.urlWgt != lhs.urlWgt) return rhs.urlWgt < lhs.urlWgt;
	return rhs.domainWgt < lhs.domainWgt;
}
