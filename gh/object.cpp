#include "main.h"

void object::addmodifier(scripti& e) const {
}

void object::clear() {
    memset(this, 0, sizeof(*this));
}

deck& object::getcombatdeck() const {
	return game.enemy_combat;
}

int object::getmaximumhits() const {
    switch(kind.type) {
    case Player: return bsdata<playeri>::get(kind.value).health[1];
    default: return 0;
    }
}

void object::heal(int v) {
    if(hits==-1)
        return; // This object have infinite hit points
    if(is(Poison)) {
        remove(Poison);
        act(getnm("ActRemovePoison"));
    } else if(v>0) {
        auto m = getmaximumhits();
        auto n = hits + v;
        if(n > m)
            n = m;
        v = n - hits;
        if(v>0) {
            hits = n;
            act(getnm("ActHeal"), v);
        }
    }
}

void object::move(int v) {
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
    addmodifier(current);
    enemy.damage(current.bonus);
}

void object::attack(int damage, int range, int pierce, statef additional) {
}

void object::pull(int range, int targets) {
}

void object::push(int range, int targets) {
}

int object::get(variant v) const {
    switch(v.type) {
    case Action:
        return 0;
    case State:
        return states.is(v.value) ? 1 : 0;
    default:
        return 0;
    }
}

void object::set(variant i, int v) {
}

void object::apply(variant v, int bonus) {
	switch(v.type) {
	case Action:
		switch(v.value) {
		case Attack:
			attack(bonus, get(Range), get(Pierce), {});
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

void object::create(variant v) {
    kind = v;
    hits = getmaximumhits();
    states.clear();
}