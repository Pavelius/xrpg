#include "main.h"

BSDATAD(object)

int object::getpriority() const {
	if(isfocused())
		return 50;
	switch(kind.type) {
	case Player: return 10;
	case SummonedCreature: return 10;
	case Monster: return 10;
	case Trap: return 1;
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
	case Player: return bsdata<playeri>::get(kind.value).health[imax(0, level-1)];
	case Monster: return bsdata<monsteri>::get(kind.value).normal[level].hits;
	default: return 1000;
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

void object::move(int v) {
	auto index = getindex();
	game.clearpath();
	game.blockwalls();
	game.setmove(index, Blocked);
	collection obstacles;
	if(get(Jump) || get(Fly))
		game.makewave(index);
	else {
		obstacles.selectalive();
		obstacles.matchhostile(!is(Hostile));
		game.block(obstacles);
		game.makewave(index);
	}
	obstacles.clear();
	obstacles.selectalive();
	game.block(obstacles);
	game.blockrange(v);
	auto goal = draw::choosemovement();
	if(goal != Blocked)
		moving(goal, true);
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

void object::attack(int damage, int range, int pierce, int targets, statef additional) {
	collection source;
	source.select(bsdata<object>::source);
	source.matchhostile(!is(Hostile));
	if(!targets)
		targets = 1;
	for(int i = 0; i < targets && source; i++) {
		variant target;
		if(true)
			target = source.choose();
		else
			target = source.data[0];
		source.remove(target);
		auto& enemy = bsdata<object>::get(target.value);
		scripti params = {};
		params.action = Attack;
		params.bonus = damage;
		params.range = range;
		params.pierce = pierce;
		params.targets = targets;
		params.states = additional;
		attack(enemy, params);
	}
}

void object::pull(int range, int targets) {
}

void object::push(int range, int targets) {
}

int object::get(variant v) const {
	switch(v.type) {
	case Action:
		switch(v.value) {
		case Level: return level;
		default: return 0;
		}
	case State:
		return states.is(v.value) ? 1 : 0;
	default:
		return 0;
	}
}

static void make_wave(indext start) {
	game.clearpath();
	game.blockwalls();
	game.block(start);
	game.makewave(start);
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

void object::apply(variant v, int bonus) {
	switch(v.type) {
	case Action:
		switch(v.value) {
		case Attack:
			attack(bonus, get(Range), get(Pierce), get(Target), {});
			break;
		case Move:
			move(bonus);
			break;
		case Heal:
			heal(bonus);
			break;
		case Push:
			push(get(Target), bonus);
			break;
		case Pull:
			pull(get(Target), bonus);
			break;
		default:
			if(!bonus)
				bonus = 1;
			set(v, get(v) + bonus);
			break;
		}
		break;
	default:
		break;
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