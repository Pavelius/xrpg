#include "main.h"

heroi* heroi::find(const provincei* v) {
	for(auto& e : bsdata<heroi>()) {
		if(e.province == v)
			return &e;
	}
	return 0;
}
