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
        act(getnm("ActRemovePoison"));
        remove(Poison);
    } else if(v>0) {
        auto m = getmaximumhits();
        auto n = hits + v;
        if(n > m)
            n = m;
        v = n - hits;
        if(v>0) {
            act(getnm("ActHeal"), v);
            hits = n;
        }
    }
}

void creature::move(int v) {
    if(isinteractive()) {
    } else {
    }
}

void creature::attack(creature& enemy, int damage, int pierce, statef additional) {
    if(isinteractive()) {
    } else {
    }
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

/*void creature::apply(variant v, int bonus) {
	switch(v.type) {
	case Action:
		switch(v.value) {
		case Attack:
			attack(bonus, get(Range), get(Pierce), getstates());
			break;
		case Move:
			move(bonus);
			break;
		case Heal:
			heal(bonus);
			break;
		case Push:
			push(bonus);
			break;
		case Pull:
			pull(bonus);
			break;
		default:
			if(!bonus)
				bonus = 1;
			set(v, get(v) + bonus);
			break;
		}
		break;
	}
}*/