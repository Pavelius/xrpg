#include "main.h"

struct creature_stringbuilder : public stringbuilder {
    const creature* ps;
    creature_stringbuilder(const stringbuilder& sb, const creature* ps)
        : stringbuilder(sb), ps(ps) {}
	void addidentifier(const char* identifier) override {
        if(equal(identifier, "герой")
            || equal(identifier, "name")) {
            add(ps->getname());
        } else
            stringbuilder::addidentifier(identifier);
	}
};

void creature::clear() {
    memset(this, 0, sizeof(*this));
}

// Fix object properties
void creature::act(const char* format, ...) const {
    char temp[4096]; creature_stringbuilder sb(temp, this);
    sb.addv(format, xva_start(format));
    draw::status(temp);
}

deck& creature::getcombatdeck() const {
	return game.enemy_combat;
}

void creature::heal(int v) {
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

void creature::move(int v) {
    if(isinteractive()) {
    } else {
    }
}

void creature::kill() {
    clear();
}

void creature::damage(int value) {
    auto shield = get(Shield);
    value -= shield;
    if(value <= 0)
        return;
    if(value >= hits) {
        // TODO: player have chance to avoid death
        if(isplayer()) {
        }
        kill();
        act(getnm("ActDamage"), value);
    } else {
        hits -= value;
        act(getnm("ActDamage"), value);
    }
}

void creature::attack(creature& enemy, int damage, int pierce, statef additional) {
    // TODO: attack modifier
    // TODO: fix attack
    enemy.damage(damage);
}

void creature::attack(int damage, int range, int pierce, statef additional) {
    if(isinteractive()) {
    } else {
    }
}

void creature::pull(int range, int targets) {
    if(isinteractive()) {
    } else {
    }
}

void creature::push(int range, int targets) {
    if(isinteractive()) {
    } else {
    }
}

int creature::get(variant v) const {
    switch(v.type) {
    case Action:
        return 0;
    case State:
        return states.is(v.value) ? 1 : 0;
    default:
        return 0;
    }
}

void creature::set(variant i, int v) {
}

void creature::apply(variant v, int bonus) {
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