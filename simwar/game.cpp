#include "main.h"

static unsigned	current_uid;
gamei			game;

struct game_string : stringbuilder {
	void addtag(const char* id) {
		add("[");
		add(getnm(id));
		add("]");
	}
	void addword(const char* id) {
		add(getnm(id));
	}
	void addref(army* p) {
		if(!p)
			return;
		auto player = p->getplayer();
		auto hero = p->getownerhero();
		if(player)
			add(getnmof(player->id));
		else
			addv(getnmof("NeutralForces"), 0);
		auto count = p->getcount();
		if(count > 0) {
			char temp[1024]; stringbuilder sb(temp);
			sb.add("[");
			sb.add("({");
			p->getinfo(sb);
			sb.add("})");
			sb.add("%1i", count);
			sb.add("]");
			adds(getnm("ArmyCount"), temp, sb.getbycount("Squad", count));
		}
		if(hero)
			adds(getnm("%LeadBy %1"), getnmof(hero->id));
	}
	void addidentifier(const char* identifier) override {
		if(equal(identifier, "province")) {
			if(game.province)
				addword(game.province->id);
			else
				addv("UnknownProvince", 0);
		} else if(equal(identifier, "player"))
			add(game.getname(game.player));
		else if(equal(identifier, "province_owner"))
			add(game.getname(game.province ? game.province->player : (playeri*)0));
		else if(equal(identifier, "attacker_of"))
			addref(game.attacker);
		else if(equal(identifier, "defender_of"))
			addref(game.garnison);
		else
			stringbuilder::addidentifier(identifier);
	}
	game_string(stringbuilder& sb) : stringbuilder(sb) {}
};

void gamei::error(stringbuilder* psb, const char* id, ...) {
	if(!psb)
		return;
	game_string sb = *psb;
	sb.addn("[-");
	sb.addv(getnm(id), xva_start(id));
	sb.add("]");
	*psb = sb;
}

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
		auto a = bonusi::getaction(v);
		auto b = bonusi::getbonus(v);
		auto need_test = false;
		if(v.type == Bonus)
			need_test = allow_test && a.type == Cost && bsdata<costi>::get(a.value).visible;
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
		auto a = bonusi::getaction(v);
		auto b = bonusi::getbonus(v);
		if(prefixes.is(Minus))
			b = -b;
		if(a == id && !prefixes.is(Permanent))
			result += b;
		prefixes.clear();
	}
	return result;
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

void gamei::passturn() {
	turn++;
	refresh();
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

void gamei::message(const char* header, const char* string) {
	answers an;
	an.add(1, getnm("Continue"));
	an.choose(string, 0, true, 0, 1, header);
}

void gamei::messagef(const char* header, const char* format, ...) {
	char temp[4096]; stringbuilder sx(temp); game_string sb(sx);
	sb.addv(format, xva_start(format));
	return message(header, temp);
}

void gamei::build() {
	auto p = game.province->choosebuilding(true, false);
	if(p)
		game.province->build(p, true);
	draw::setactive(playermove);
}

void gamei::demontage() {
	auto p = game.province->choosebuilding(false, true);
	if(p)
		game.province->demontage(p, true);
	draw::setactive(playermove);
}

void gamei::apply(variant v, stata& stat, costa& cost, int multiplier) {
	auto a = bonusi::getaction(v);
	auto b = bonusi::getbonus(v) * multiplier;
	switch(a.type) {
	case Cost: cost.add(a.value, b); break;
	case Stat: stat.add((stat_s)a.value, b); break;
	}
}

void gamei::addaction(answers& an, action_s v) {
	int n;
	if(!game.execute(v, false))
		return;
	auto id = bsdata<actioni>::get(v).id;
	auto proc = bsdata<actioni>::get(v).proc;
	switch(v) {
	case ShowBuildings:
		n = game.province->getbuildcount();
		if(n)
			an.add((long)proc, "%ShowBuildings (%Builded %1i)", n);
		else
			an.add((long)proc, getnm(id));
		break;
	default:
		an.add((long)proc, getnm(id));
		break;
	}
}

void gamei::addaction(answers& an, const char* id, fnevent proc) {
	an.add((long)proc, getnm(id));
}

provincei* gamei::choose_province() {
	answers an;
	for(auto& e : bsdata<provincei>()) {
		if(e.player != game.player)
			continue;
		an.add((long)&e, getnm(e.id));
	}
	return (provincei*)an.choose(0, getnm("Cancel"), true, 0, 1);
}

heroi* gamei::choose_hero() {
	answers an;
	for(auto& e : bsdata<heroi>()) {
		//if(e.owner != game.player)
		//	continue;
		an.add((long)&e, "#$left image %1 0 \"art/portraits\" \"@%1\"\n###%2", e.id, getnm(e.id));
	}
	return (heroi*)an.choose(0, getnm("Cancel"), true, 0, 1);
}

void gamei::heroes() {
	//answers an;
	//for(auto& e : bsdata<heroi>()) {
	//	an.add((long)&e, "#$left image %1 0 \"art/portraits\" \"%1\"\n###%2", e.id, getnm(e.id));
	//}
	//an.choose(0, 0, true, 0, 1, getnm("Heroes"));
	char temp[4096]; stringbuilder osb(temp);
	game_string sb(osb);
	army attacker;
	attacker.add(&bsdata<uniti>::get(2));
	attacker.add(&bsdata<uniti>::get(3));
	game.attacker = &attacker;
	game.garnison = &game.province->garnison;
	attacker.conquer(game.province->garnison, &sb);
	game.message(getnm(game.province->id), temp);
}

void gamei::buildings() {
	char temp[4096]; stringbuilder sb(temp);
	temp[0] = 0; answers an;
	game.province->getbuildings(0, &sb);
	addaction(an, BuildProvince);
	addaction(an, DestroyProvince);
	addaction(an, CancelAction);
	draw::choose(an, temp, getnm(game.province->id));
}

void gamei::playermove() {
	char temp[4096]; stringbuilder sb(temp); answers an;
	game.province->getpresent(sb);
	addaction(an, ShowBuildings);
	addaction(an, ShowSites);
	addaction(an, RecruitUnits);
	addaction(an, "Heroes", heroes);
	addaction(an, EndTurn);
	draw::choose(an, temp, getnm(game.province->id));
}

static void hire_unit() {
	game.player->hire(game.unit->type, game.garnison, true);
}

static void disband_unit() {
	char temp[260]; stringbuilder sb(temp);
	sb.add(getdescription("DisbandUnit"), getnm(game.unit->type->id));
	if(!draw::confirm(getnm("DisbandUnit"), temp))
		return;
	auto garnison = game.unit->getarmy();
	if(!garnison)
		return;
	game.unit->clear();
	garnison->normalize();
}

void gamei::recruit() {
	if(game.province->player != game.player) {
		messagef(getnm(game.province->id), getnm("ProvinceWrongOwner"));
		draw::setdefactive();
		return;
	}
	auto leadership = game.province->getleadership();
	if(!leadership) {
		messagef(getnm(game.province->id), getnm("HireAllowOnlyInFort"));
		draw::setdefactive();
		return;
	}
	char temp[260]; stringbuilder sx(temp); game_string sb(sx);
	sb.add("%RecruitUnits - %province");
	army source, dest;
	source.selectall();
	source.choose(temp,
		getnm("AllowedToHire"), source, hire_unit,
		getnm("CurrentArmy"), game.province->garnison, disband_unit);
}

bool gamei::execute(action_s id, bool run) {
	if(id <= actiona::maximal_element && id != CancelAction) {
		if(!player)
			return false;
		if(!player->actions.get(id))
			return false;
	}
	int n;
	switch(id) {
	case DestroyProvince:
		n = province->getbuildcount();
		if(!n)
			return false;
		break;
	}
	return true;
}

void gamei::refresh() {
	for(auto& e : bsdata<provincei>())
		e.refresh();
	for(auto& e : bsdata<playeri>())
		e.refresh();
}

void gamei::nextmove() {
	game.passturn();
	draw::setactive(playermove);
}

int gamei::getleadership(int value, int level) {
	static char source[10][4] = {
		{3},
		{4},
		{4, 1},
		{5, 2},
		{5, 2},
		{6, 3, 1},
		{6, 3, 2},
		{7, 4, 2},
		{7, 4, 3, 1},
		{8, 5, 3, 2},
	};
	const int maximum_level = sizeof(source) / sizeof(source[0]);
	value -= 1;
	level -= 1;
	if(value >= maximum_level)
		value = maximum_level - 1;
	if(value < 0)
		return 0;
	if(level >= 4)
		level = 3;
	if(level < 0)
		level = 0;
	return source[value][level];
}

const char*	gamei::getname(const playeri* p) {
	if(p)
		return getnm(p->id);
	else
		return getnm("NeutralForces");
}

const char*	gamei::getnameof(const playeri* p) {
	if(p)
		return getnmof(p->id);
	else
		return getnmof("NeutralForces");
}