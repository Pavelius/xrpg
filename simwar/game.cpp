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

bool gamei::apply(const variants& source, bool test_required) {
	prefixa prefixes;
	for(auto v : source) {
		if(v.type == Prefix) {
			prefixes.set(v.value);
			continue;
		}
		auto a = getaction(v);
		auto b = getbonus(v);
		auto t = false;
		if(v.type == Bonus)
			t = bsdata<bonusi>::get(v.value).isrequired();
		if(prefixes.is(Minus))
			b = -b;
		switch(a.type) {
		case Cost:
			if(prefixes.is(Income)) {
				if(province) {
					if(test_required) {
						if(t) {
							if((province->income.get(a.value) + b) < 0)
								return false;
						}
					} else
						province->income.add(a.value, b);
				}
			} else if(player) {
				if(test_required) {
					if(t) {
						if((player->total.get(a.value) + b) < 0)
							return false;
					}
				} else
					player->total.add(a.value, b);
			}
			break;
		case Event:
			if(player) {
				if(!test_required)
					player->events.addrandom(a);
			}
			break;
		}
		prefixes.clear();
	}
	return true;
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

static const eventcasei* getnext(const eventcasei* p);

static const eventcasei* find_eventcase(short parent, short id, const eventcasei* first, bool need_prompt) {
	auto pe = (const eventcasei*)bsdata<eventcasei>::source.end();
	if(!first)
		first = (const eventcasei*)bsdata<eventcasei>::source.begin() - 1;
	for(auto p = first + 1; p < pe; p++) {
		if(p->parent != parent)
			continue;
		if(p->id != id)
			continue;
		if(p->isprompt() != need_prompt)
			continue;
		if(p->isprompt()) {
			if(p->effect) {
				if(!game.apply(p->effect, true))
					continue;
			}
		} else if(p->next > 0) {
			if(!getnext(p))
				continue;
		}
		return p;
	}
	return 0;
}

static const eventcasei* getnext(const eventcasei* p) {
	if(p->next <= 0)
		return 0;
	return find_eventcase(p->parent, p->next, 0, true);
}

int gamei::get(variant id, const variants& source) {
	prefixa prefixes;
	int result = 0;
	for(auto v : source) {
		if(v.type == Prefix) {
			prefixes.set(v.value);
			continue;
		}
		auto a = getaction(v);
		auto b = getbonus(v);
		if(prefixes.is(Minus))
			b = -b;
		if(a == id && !prefixes.is(Income))
			result += b;
		prefixes.clear();
	}
	return result;
}

static void add_possible_lost(const eventcasei* p, stringbuilder& sbo) {
	auto pe = getnext(p);
	if(!pe)
		return;
	char temp[500]; temp[0] = 0; stringbuilder sb(temp);
	for(auto& e : bsdata<costi>()) {
		if(!e.visible)
			continue;
		auto r = game.get(variant(&e), pe->effect);
		if(!r)
		continue;
		if(temp[0])
			sb.add(", ");
		sb.add(getnm(e.id));
		sb.add("%+1i", r);
	}
	if(temp[0])
		sbo.add(" (%1)", temp);
}

void gamei::play(const eventi* event) {
	auto quest_id = bsdata<eventi>::source.indexof(event);
	if(quest_id == -1)
		return;
	char temp[4095];
	answers an;
	auto current_id = 0;
	while(true) {
		auto pe = find_eventcase(quest_id, current_id, 0, true);
		if(!pe)
			break;
		apply(pe->effect);
		if(!pe->text)
			break;
		an.clear();
		stringbuilder sb(temp);
		for(auto p = find_eventcase(quest_id, current_id, 0, false); p; p = find_eventcase(quest_id, current_id, p, false)) {
			sb.clear(); game.format(sb, p->text);
			add_possible_lost(p, sb);
			an.add((long)p, temp);
		}
		sb.clear(); game.format(sb, pe->text);
		auto p = (eventcasei*)draw::dialog(an, getnm("RandomEvent"), temp);
		if(!p)
			break;
		current_id = p->next;
		if(!current_id)
			break;
	}
}