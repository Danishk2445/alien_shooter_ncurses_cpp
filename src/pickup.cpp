#include "pickup.hpp"

void Pickup::update() {
    lifetime--;
    blink_timer++;
    if (lifetime <= 0) alive = false;
}

int Pickup::get_value() const {
    switch (type) {
        case PickupType::Ammo:   return 10;
        case PickupType::Health: return 15;
    }
    return 0;
}

bool Pickup::is_visible() const {
    // Blink faster as lifetime decreases
    if (lifetime < 60) return (blink_timer / 4) % 2 == 0;
    if (lifetime < 120) return (blink_timer / 6) % 2 == 0;
    return true; // Solid when plenty of time left
}
