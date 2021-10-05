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

int	object::getinitiative() const {
	switch(kind.type) {
	case Player: return bsdata<playeri>::get(kind.value).initiative;
	case Monster: 30;
	default: return 99;
	}
}

void object::move(int v) {
	auto index = getindex();
	game.clearpath();
	game.blockwalls();
	objects obstacles;
	if(get(Jump) || get(Fly))
		game.makewave(index);
	else {
		obstacles.selectalive();
		obstacles.match(Hostile, !is(Hostile));
		game.block(obstacles);
		game.makewave(index);
	}
	obstacles.clear();
	obstacles.selectalive();
	game.block(obstacles);
	game.blockrange(v);
	if(isinteractive()) {
		auto goal = draw::choosemovement();
		moving(goal, true);
	} else if(obstacles) {
		auto enemy = getnearestenemy();
		if(enemy)
			moveto(enemy->getindex(), v);
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

static short unsigned move_copy[hms*hms];

void object::movefrom(indext i, int range) {
	objects enemies;
	enemies.selectalive();
	enemies.match(Hostile, !is(Hostile));
    auto start = getindex();
    game.clearpath();
    game.blockwalls();
    game.block(start);
    game.block(enemies);
    game.makewave(start);
    game.blockrange(range);
    game.getmove(move_copy);
    game.clearpath();
    game.blockwalls();
    game.block(i);
    game.makewave(i);
    auto goal = game.getfarest(move_copy);
    moving(goal, true);
}

void object::moveto(indext i, int range){
	objects enemies;
	enemies.selectalive();
	enemies.match(Hostile, !is(Hostile));
    auto start = getindex();
    game.clearpath();
    game.blockwalls();
    game.block(start);
    game.block(enemies);
    game.makewave(start);
    game.blockrange(range);
    game.getmove(move_copy);
    game.clearpath();
    game.blockwalls();
    game.block(i);
    game.makewave(i);
    auto goal = game.getnearest(move_copy);
    moving(goal, true);
}

void object::loot(int bonus) {
}

void object::action(action_s a, bool interactive, bool hostile, int range, int targets, int bonus) {
	if(!targets)
		targets = 1;
	objects source;
    auto start = getindex();
    if(range==0)
        source.add(this);
    else {
        game.clearpath();
        game.blockwalls();
        game.makewave(start);
        source.selectalive();
        source.match(Hostile, hostile);
        source.matchrange(range, true);
    }
    // Target count is same as available target count. In this case choosing has no sence.
    if(source.getcount()<=targets)
        interactive = false;
	for(int i = 0; i < targets && source; i++) {
		object* target = 0;
		if(interactive)
			target = source.choose();
		else
			target = source.data[0];
		source.remove(target);
		switch(a) {
        case Heal: target->heal(bonus); break;
        case Pull: target->moveto(start, bonus); break;
        case Push: target->movefrom(start, bonus); break;
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
	source.sort();
	return source[0];
}

void object::attack(int damage, int range, int pierce, int targets, statef additional) {
    if(range==0)
        range = 1;
	if(targets==0)
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
		object* target = 0;
		if(interactive)
			target = source.choose();
		else
			target = source.data[0];
		source.remove(target);
		scripti params = {};
		params.action = Attack;
		params.bonus = damage;
		params.range = range;
		params.pierce = pierce;
		params.targets = targets;
		params.states = additional;
		attack(*target, params);
	}
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
            action(Push, true, true, e.range, e.targets, e.bonus);
			break;
		case Pull:
            action(Pull, true, true, e.range, e.targets, e.bonus);
			break;
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