#include "../gen/WORLD_ACTOR.h"

#include <iostream>
using namespace std;

void print_actor(const UniqsModel::WORLD_ACTOR& actor) {
    cout << "uid:" << actor.actor_common.uid << endl;
    cout << "name:" << actor.actor_common.name << endl;
    cout << "rmb_payed:" << actor.money.rmb_payed << endl;
    cout << "first_inited:" << actor.actor_common.first_inited << endl;
}

int main() {
    UniqsModel::WORLD_ACTOR actor;

    actor.money.rmb_payed = 1234;
    actor.actor_common.uid = 5678;
    actor.actor_common.name = "hello world";
    actor.actor_common.first_inited = true;

    print_actor(actor);

    actor.Clear();

    print_actor(actor);

    return 0;
}
