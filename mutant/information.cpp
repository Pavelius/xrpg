#include "main.h"

void variant::getinfo(stringbuilder& sb) const {
	sb.add("##%1\n", getname());
	switch(getkind()) {
	case Location:
		sb.addn("����� ������ �� �����. ��� ��� � ��� ����� ������.");
		break;
	}
}