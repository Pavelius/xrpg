#include "main.h"

BSDATAD(object)

object* last_object;
cardi* last_card;
static statef last_object_states;

int object::getpriority() const {
	if(isfocused())
		return 50;
	switch(kind.type) {
	case Player: return 10;
	case SummonedCreature: return 10;
	case Monster: return 10;
	case Trap: return 5;
	case Tile: return bsdata<tilei>::get(kind.value).priority;
	default: return 0;
	}
}

bool object::isalive() const {
	switch(kind.type) {
	case Player:
	case Monster:
	case SummonedCreature:
		return true;
	default:
		return false;
	}
}

bool object::isusable() const {
	switch(kind.type) {
	case Trap:
		return true;
	default:
		return false;
	}
}

void object::addattack(scripti& e) const {
	char temp[512]; temp[0] = 0;
	stringbuilder sb(temp);
	auto& cards = getcombatdeck();
	auto v = cards.get();
	if(v.type == CombatCard) {
		auto& ei = bsdata<combatcardi>::get(v.value);
		e.bonus += ei.bonus;
		sb.addn("%Bonus%+1i", ei.bonus);
		if(ei.feats)
			e.apply(ei.feats);
	}
	if(temp[0])
		act(temp);
}

void object::clear() {
	memset(this, 0, sizeof(*this));
}

deck& object::getcombatdeck() const {
	return game.enemy_combat;
}

int object::getmaximumhits() const {
	switch(kind.type) {
	case Player:
        return bsdata<playeri>::get(kind.value).health[imax(0, level - 1)];
	case Monster:
        return bsdata<monsteri>::get(kind.value).normal[
            imin(level, (char)(sizeof(monsteri::normal)/sizeof(monsteri::normal[0])))].hits;
	default:
        return 1000;
	}
}

void object::heal(int v) {
	if(hits == -1)
		return; // This object have infinite hit points
	if(is(Poison)) {
		remove(Poison);
		act(getnm("ActRemovePoison"));
	} else if(v > 0) {
		auto m = getmaximumhits();
		auto n = hits + v;
		if(n > m)
			n = m;
		v = n - hits;
		if(v > 0) {
			hits = n;
			act(getnm("ActHeal"), v);
		}
	}
}

indext object::getindex() const {
	return h2i(p2h(getposition()));
}

static void calculate_movemap(indext start, int range, bool you_is_hostile, bool jump, bool fly) {
	game.clearpath();
	game.blockwalls();
	objects obstacles;
	if(jump || fly)
		game.makewave(start);
	else {
		obstacles.selectalive();
		obstacles.match(Hostile, !you_is_hostile);
		game.block(obstacles);
		game.makewave(start);
	}
	obstacles.clear();
	obstacles.selectalive();
	game.block(obstacles);
	game.blockrange(range);
}

static void calculate_shootmap(indext start) {
	game.clearpath();
	game.blockwalls();
	game.makewave(start);
}

void object::move(int v) {
	calculate_movemap(getindex(), v, is(Hostile), get(Jump) != 0, get(Fly) != 0);
	if(isinteractive()) {
		auto goal = draw::choosemovement();
		moving(goal, true);
	} else {
		auto enemy = getnearestenemy();
		if(enemy)
			moveto(enemy->getindex(), v, 1);
	}
}

void object::kill() {
	clear();
}

void object::damage(int value) {
	auto shield = get(Shield);
	value -= shield;
	if(value <= 0)
		return;
	if(value >= hits) {
		// TODO: player have chance to avoid death
		if(isplayer()) {
		}
		kill();
		act(getnm("ActKilled"), value);
	} else {
		hits -= value;
		act(getnm("ActDamage"), value);
	}
}

void object::attack(object& enemy, const scripti& modifiers) {
	auto current = modifiers;
	addattack(current);
	enemy.damage(current.bonus);
}

static short unsigned move_copy[hms * hms];

void object::movefrom(indext i, int range) {
	calculate_movemap(getindex(), range, is(Hostile), get(Jump) != 0, get(Fly) != 0);
	game.getmove(move_copy);
	calculate_shootmap(i);
	auto goal = game.getfarest(move_copy);
	if(goal != Blocked)
		moving(goal, true);
}

void object::moveto(indext i, int range) {
	calculate_movemap(getindex(), range, is(Hostile), get(Jump) != 0, get(Fly) != 0);
	game.getmove(move_copy);
	calculate_shootmap(i);
	auto goal = game.getnearest(move_copy);
	if(goal != Blocked)
        moving(goal, true);
}

void object::moveto(indext i, int range, int distance) {
	calculate_movemap(getindex(), range, is(Hostile), get(Jump) != 0, get(Fly) != 0);
	game.getmove(move_copy);
	calculate_shootmap(i);
	game.blockrange(distance);
	auto goal = game.getnearest(move_copy);
	if(goal != Blocked)
		moving(goal, true);
}

void object::loot(int bonus) {
}

void object::action(action_s a, bool interactive, bool hostile, int range, int targets, int bonus) {
	if(!targets)
		targets = 1;
	objects source;
	auto start = getindex();
	if(range == 0)
		source.add(this);
	else {
		game.clearpath();
		game.blockwalls();
		game.makewave(start);
		source.selectalive();
		source.match(Hostile, hostile);
		source.matchrange(range, true);
	}
	// Target count is same as available target count.
	// In this case choosing has no sence.
	if(source.getcount() <= targets)
		interactive = false;
	for(int i = 0; i < targets && source; i++) {
		if(interactive)
			last_object = source.choose();
		else
			last_object = source.data[0];
		source.remove(last_object);
		switch(a) {
		case Heal: last_object->heal(bonus); break;
		case Pull: last_object->moveto(start, bonus); break;
		case Push: last_object->movefrom(start, bonus); break;
		default: break;
		}
	}
}

object*	object::getnearestenemy() const {
	objects source;
	source.selectalive();
	source.match(Hostile, !is(Hostile));
	if(!source)
		return 0;
	indext start = getindex();
	game.clearpath();
	game.blockwalls();
	game.makewave(start);
	source.sortnearest();
	return source[0];
}

void object::attack(int damage, int range, int pierce, int targets, statef additional) {
	if(range == 0)
		range = 1;
	if(targets == 0)
		targets = 1;
	indext start = getindex();
	game.clearpath();
	game.blockwalls();
	game.makewave(start);
	objects source;
	source.selectalive();
	source.match(Hostile, !is(Hostile));
	source.matchrange(range, true);
	source.sortnearest();
	auto interactive = isinteractive();
	if(source.getcount() <= targets)
		interactive = false;
	for(int i = 0; i < targets && source; i++) {
		if(interactive)
			last_object = source.choose();
		else
			last_object = source.data[0];
		source.remove(last_object);
		scripti params = {};
		params.action = Attack;
		params.bonus = damage;
		params.range = range;
		params.pierce = pierce;
		params.targets = targets;
		params.states = additional;
		attack(*last_object, params);
	}
}

static int getmonster(monsteri* p, variant a) {
	if(!p)
		return 0;
}

int object::get(variant v) const {
	switch(v.type) {
	case Action:
		switch(v.value) {
		case Level: return level;
		default:
			if(kind.type == Monster)
				return bsdata<monsteri>::get(kind.value).get(level, is(Elite)).get((action_s)v.value);
			return 0;
		}
	case State:
		return states.is(v.value) ? 1 : 0;
	default:
		return 0;
	}
}

void object::set(variant i, int v) {
	switch(i.type) {
	case State:
		states.set(i.value, v > 0);
		break;
	case Action:
		switch(i.value) {
		case Level: level = v; break;
		default: break;
		}
		break;
	default:
		break;
	}
}

void object::apply(scripti& e) {
	switch(e.action.type) {
	case Action:
		switch(e.action.value) {
		case Attack:
			attack(e.bonus, e.range, e.pierce, e.targets, e.states);
			break;
		case Move:
			move(e.bonus);
			break;
		case Heal:
			action(Heal, true, false, e.range, e.targets, e.bonus);
			break;
		case Push:
		case Pull:
            if(!e.range)
                e.range = 1;
			action((action_s)e.action.value, true, true, e.range, e.targets, e.bonus);
		case Loot:
			loot(e.bonus);
			break;
		default:
			if(!e.bonus)
				e.bonus = 1;
			set(e.action, get(e.action) + e.bonus);
			break;
		}
		break;
	default:
		break;
	}
}

void object::apply(const variants& source) {
	const variant* pb = source.begin();
	const variant* pe = source.end();
	scripti script;
	while(pb < pe) {
		script.clear();
		pb = script.apply(pb, pe);
		apply(script);
	}
}

void object::refresh() {
	this->hits = getmaximumhits();
	states.clear();
}

void object::create(variant v) {
	this->kind = v;
	this->level = 1;
	refresh();
}

static activity* find_activity(variant owner, variant source) {
	for(auto& e : bsdata<activity>()) {
		if(e.owner == owner && e.source == source)
			return &e;
	}
	return 0;
}

void object::activate(duration_s duration, int count, variant source, variants effect) {
	variant owner = this;
	auto p = find_activity(owner, source);
	if(!p)
		p = bsdata<activity>::addz();
	p->owner = owner;
	p->source = source;
	p->effect = effect;
	p->duration = duration;
	p->count = count;
}

void object::moving(indext i, bool interactive) {
	auto goal = h2p(i2h(i));
	draw::moving(getreference(), goal, 8);
}

variant object::chooseaction() const {
	playeri* player = kind;
	if(player) {
		cardi* card1 = player->cards[0];
		cardi* card2 = player->cards[1];
		cardi* std_attack = variant("StandartAttackAction");
		cardi* std_move = variant("StandartMoveAction");
		answers an;
		if(card1)
			an.add((long)card1, getnm(card1->id));
		if(card2)
			an.add((long)card2, getnm(card2->id));
		if(!is(UseUpper))
			an.add((long)std_attack, getnm(std_attack->id));
		if(!is(UseLower))
			an.add((long)std_move, getnm(std_move->id));
		variant result = (void*)an.choose(0, 0, true, 0, 1);
		if(result.type == Card) {
			auto p = (cardi*)bsdata<cardi>::source.ptr(result.value);
			if(p == card1 || p == std_attack)
				player->cards[0].clear();
			else if(p == card2 || p == std_move)
				player->cards[1].clear();
			last_card = p;
		}
		return result;
	}
	monsteri* monster = kind;
	if(monster)
		return monster->abilities_deck.look(0);
	return variant();
}

const variants& object::choosepart(const cardi& e) {
    varianta source;
    if(e.upper && !is(UseUpper))
        source.add("UpperCardPart");
    if(e.lower && !is(UseLower))
        source.add("LowerCardPart");
    auto result = source.choosedef(0);
    if(result.type==ActionBonus) {
		if(bsdata<actionbonusi>::get(result.value).bonus) {
			states.set(UseLower);
			return e.lower;
		}
    }
	states.set(UseUpper);
    return e.upper;
}

static void remove_state(object* p, state_s v) {
	if(!last_object_states.is(v) || !p->is(v))
		return;
	p->remove(v);
}

void object::beginround() {
	initiative = game.getinitiative(kind);
}

void object::maketurn() {
	last_object_states = states;
	switch(kind.type) {
	case Player:
		while(true) {
			auto result = chooseaction();
			if(!result)
				break;
			applyaction(result);
		}
		break;
	case Monster:
		applyaction(chooseaction());
		break;
	}
	remove_state(this, Muddle);
	remove_state(this, Immobilize);
	remove_state(this, Invisibility);
	remove_state(this, Stun);
	remove_state(this, Strenght);
}

void object::applyaction(variant v) {
    switch(v.type) {
    case Card:
        apply(choosepart(bsdata<cardi>::get(v.value)));
        break;
	case MonsterCard:
		apply(bsdata<monstercardi>::get(v.value).abilities);
		break;
	default:
        break;
    }
}

void object::modify(scripti& e) const {
	if(e.action)
		e.bonus += get(e.action);
}

int object::getinitiative() const {
	return initiative;
}