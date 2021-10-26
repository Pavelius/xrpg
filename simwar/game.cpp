#include "main.h"

static unsigned	current_uid;
gamei			game;

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

bool gamei::apply(const variants& source, bool allow_test, bool allow_apply) {
	prefixa prefixes;
	for(auto v : source) {
		if(v.type == Prefix) {
			prefixes.set(v.value);
			continue;
		}
		auto a = getaction(v);
		auto b = getbonus(v);
		auto need_test = false;
		if(v.type == Bonus)
			need_test = allow_test && bsdata<bonusi>::get(v.value).isrequired();
		if(prefixes.is(Minus))
			b = -b;
		switch(a.type) {
		case Cost:
			if(prefixes.is(Permanent)) {
				if(province) {
					if(need_test && (province->income.get(a.value) + b) < 0)
						return false;
					if(allow_apply)
						province->income.add(a.value, b);
				}
			} else if(player) {
				if(need_test && (player->total.get(a.value) + b) < 0)
					return false;
				if(allow_apply)
					player->total.add(a.value, b);
			}
			break;
		case Event:
			if(player) {
				if(allow_apply)
					player->events.addrandom(a);
			}
			break;
		}
		prefixes.clear();
	}
	return true;
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
		if(a == id && !prefixes.is(Permanent))
			result += b;
		prefixes.clear();
	}
	return result;
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
				if(!game.apply(p->effect, true, false))
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
		apply(pe->effect, false, true);
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

unsigned gamei::adduid() {
	return (uid_base << 24) | (++uid);
}

void gamei::addtroop(uniti* type, provincei* province) {
	auto p = bsdata<troop>::addz();
	p->clear();
	p->uid = adduid();
	p->type = type;
	p->province = province;
}

void gamei::passturn() {
	turn++;
}

void gamei::initialize() {
	start_year = 943;
	for(auto& e : bsdata<provincei>())
		e.initialize();
}

void gamei::getdate(stringbuilder& sb) const {
	auto month = getmonth();
	auto season = bsdata<calendari>::get(month).season;
	auto season_part = bsdata<calendari>::get(month).season_part;
	sb.add("%1 %-2 %3i %-YearOf",
		getnm(bsdata<monthparti>::get(season_part).id),
		getnmof(bsdata<seasoni>::get(season).id),
		getyear());
}

provincei* gamei::choose_province() {
	answers an;
	for(auto& e : bsdata<provincei>()) {
		if(e.owner != game.player)
			continue;
		an.add((long)&e, getnm(e.id));
	}
	return (provincei*)an.choose(0, getnm("Cancel"), true, 0, 1);
}

hero* gamei::choose_hero() {
	answers an;
	for(auto& e : bsdata<hero>()) {
		//if(e.owner != game.player)
		//	continue;
		an.add((long)&e, getnm(e.id));
	}
	return (hero*)an.choose(0, getnm("Cancel"), true, 0, 1);
}

void gamei::message(const char* string) {
	answers an;
	an.add(1, getnm("Continue"));
	an.choose(string, 0, true, 0);
}

void gamei::build() {
	answers an; game.province->canbuild(an);
	auto p = (buildingi*)an.choose(getnm("ChooseBuildingToBuild"), getnm("Cancel"), true, 0, 1);
	if(!p)
		return;
	game.province->build(p, true);
}

void gamei::apply(variant v, stata& stat, costa& cost) {
	auto a = getaction(v);
	auto b = getbonus(v);
	switch(a.type) {
	case Cost: cost.add(a.value, b); break;
	case Stat: stat.add(a.value, b); break;
	}
}

void gamei::playerturn() {
	//static auto test_string = "$image plains 0 \"art/images\" \"@Plains\"\n"\
	//	"Test string for accept.";
	//message(test_string);
	while(true) {
		//game.province = choose_hero();
		choose_hero();
		if(!game.province)
			break;
		choose_province_action();
	}
}