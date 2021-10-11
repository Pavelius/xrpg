#include "main.h"

gamei game;

void gamei::add(variant i, int v) {
	set(i, get(i) + v);
}

int gamei::get(variant i) const {
	switch(i.type) {
	case GameProperty:
		if(i.value < sizeof(ability) / sizeof(ability[0]))
			return ability[i.value];
		return 0;
	case Element:
		if(i.value < sizeof(elements) / sizeof(elements[0]))
			return elements[i.value];
		return 0;
	default:
		return 0;
	}
}

bool gamei::isallow(variant v) const {
	switch(v.type) {
	case Player:
		return allowed_players.is(v.value);
	default:
		return false;
	}
}

void gamei::set(variant i, int v) {
	switch(i.type) {
	case GameProperty:
		if(i.value < sizeof(ability) / sizeof(ability[0]))
			ability[i.value] = v;
		break;
	case Element:
		if(i.value < sizeof(elements) / sizeof(elements[0])) {
			elements[i.value] = v;
			if(elements[i.value] > 2)
				elements[i.value] = 2;
			else if(elements[i.value] < 0)
				elements[i.value] = 0;
		}
		break;
	default:
		break;
	}
}

void gamei::buildcombatdeck() {
	enemy_combat.clear();
	enemy_combat.addcombat({});
	enemy_combat.shuffle();
}

static object* find_emphty() {
	for(auto& e : bsdata<object>()) {
		if(!e)
			return &e;
	}
	return 0;
}

object* gamei::create(variant v, bool inverse) {
	auto p = find_emphty();
	if(!p)
		p = bsdata<object>::addz();
	p->create(v);
	if(inverse)
		p->set(Mirrored, 1);
	return p;
}

object* gamei::create(point position, variant v, bool inverse) {
	auto p = create(v, inverse);
	auto pt = h2p(position);
	if(v.type == Tile)
		pt = pt - bsdata<tilei>::get(v.value).offset;
	p->setposition(pt);
	switch(v.type) {
	case Tile:
		bsdata<tilei>::get(v.value).creating(position, inverse);
		break;
	case Monster:
		p->set(Level, getlevel() / 2);
		break;
	default:
		game.setpass(h2i(position));
		break;
	}
	return p;
}

int gamei::getlevel() const {
	auto c = players.getcount();
	if(!c)
		return 0;
	auto r = c - 1;
	for(auto& e : players) {
		if(!e)
			continue;
		r += bsdata<playeri>::get(e.value).get(Level);
	}
	return r / c;
}

void gamei::beginround() {
	for(auto& e : bsdata<object>()) {
		if(e.isalive())
			e.beginround();
	}
}

void gamei::maketurn() {
	objects creatures;
	creatures.selectalive();
	creatures.sort();
	for(auto p : creatures) {
		if(!p->operator bool())
			continue;
		p->focusing();
		p->maketurn();
	}
}

void gamei::endround() {
	objects creatures;
	creatures.selectalive();
	creatures.sort();
	// All creatures make end round action
	for(auto p : creatures)
		p->beginround();
	// Reshuffle mosters card
	varianta source;
	selectkind(source);
	sort(source);
	for(auto v : source) {
		monsteri* pm = v;
		if(!pm)
			continue;
		pm->abilities_deck.changeone();
		// TODO: Make reshufle
	}
}

void gamei::makeround() {
	beginround();
	maketurn();
	endround();
	rounds++;
}

static bool allow_next_round() {
	int side = -1;
	for(auto& e : bsdata<object>()) {
		if(!e)
			continue;
		if(e.isalive()) {
			auto ns = e.is(Hostile) ? 1 : 0;
			if(side == -1)
				side = ns;
			else if(side != ns)
				return true;
		}
	}
	return false;
}

void gamei::playtactic() {
	game.rounds = 0;
	while(allow_next_round())
		game.makeround();
}

void gamei::selectkind(varianta& source) {
	for(auto& e : bsdata<object>()) {
		if(!e)
			continue;
		if(e.isalive())
			source.add(e.kind);
	}
	source.distinct();
}

int gamei::getinitiative(variant v) {
	switch(v.type) {
	case Card: return bsdata<cardi>::get(v.value).initiative;
	case Monster: return bsdata<monsteri>::get(v.value).getinitiative();
	case MonsterCard: return bsdata<monstercardi>::get(v.value).initiative;
	case Player: return bsdata<playeri>::get(v.value).getinitiative();
	default: return 99;
	}
}

static int compare_initiative(const void* p1, const void* p2) {
	auto v1 = *((variant*)p1);
	auto v2 = *((variant*)p2);
	return game.getinitiative(v1) - game.getinitiative(v2);
}

void gamei::sort(varianta& source) {
	qsort(source.data, source.count, sizeof(source.data[0]), compare_initiative);
}