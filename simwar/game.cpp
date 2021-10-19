#include "main.h"

gamei game;

struct game_string : stringbuilder {
	void addtag(const char* id) {
		add("[");
		add(getnm(id));
		add("]");
	}
	void addidentifier(const char* identifier) override {
		if(equal(identifier, "province")) {
			if(game.province)
				addtag(game.province->id);
			else
				addv("UnknownProvince", 0);
		} else if(equal(identifier, "player")) {
			if(game.player)
				addtag(game.player->id);
			else
				addv(getnm("NeutralForces"), 0);
		} else
			stringbuilder::addidentifier(identifier);
	}
	game_string(stringbuilder& sb) : stringbuilder(sb) {}
};

void gamei::format(stringbuilder& sbr, const char* string, ...) {
	game_string sb = sbr;
	sb.addv(string, xva_start(string));
	sbr = sb;
}

void gamei::apply(const variants& source) {
	prefixa prefixes;
	for(auto v : source) {
		if(v.type == Prefix) {
			prefixes.set(v.value);
			continue;
		}
		auto a = getaction(v);
		auto b = getbonus(v);
		if(prefixes.is(Minus))
			b = -b;
		switch(a.type) {
		case Cost:
			if(prefixes.is(Income)) {
				if(province)
					province->income.add(a.value, b);
			} else if(player)
				player->total.add(a.value, b);
			break;
		}
		prefixes.clear();
	}
}

variant gamei::getaction(variant v) {
	if(v.type == Bonus)
		return bsdata<bonusi>::elements[v.value].type;
	return v;
}

int gamei::getbonus(variant v) {
	if(v.type == Bonus)
		return bsdata<bonusi>::elements[v.value].bonus;
	return 1;
}