// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Misc code for maintaining the dungeon, printing player info

#include "headers.h"
#include "externs.h"

// Special damage due to magical abilities of object -RAK-
int itemMagicAbilityDamage(const Inventory_t &item, int total_damage, int monster_id) {
    bool is_ego_weapon = (item.flags & TR_EGO_WEAPON) != 0;
    bool is_projectile = item.category_id >= TV_SLING_AMMO && item.category_id <= TV_ARROW;
    bool is_hafted_sword = item.category_id >= TV_HAFTED && item.category_id <= TV_SWORD;
    bool is_flask = item.category_id == TV_FLASK;

    if (is_ego_weapon && (is_projectile || is_hafted_sword || is_flask)) {
        const Creature_t &creature = creatures_list[monster_id];
        Recall_t &memory = creature_recall[monster_id];

        // Slay Dragon
        if (((creature.defenses & CD_DRAGON) != 0) && ((item.flags & TR_SLAY_DRAGON) != 0u)) {
            memory.defenses |= CD_DRAGON;
            return total_damage * 4;
        }

        // Slay Undead
        if (((creature.defenses & CD_UNDEAD) != 0) && ((item.flags & TR_SLAY_UNDEAD) != 0u)) {
            memory.defenses |= CD_UNDEAD;
            return total_damage * 3;
        }

        // Slay Animal
        if (((creature.defenses & CD_ANIMAL) != 0) && ((item.flags & TR_SLAY_ANIMAL) != 0u)) {
            memory.defenses |= CD_ANIMAL;
            return total_damage * 2;
        }

        // Slay Evil
        if (((creature.defenses & CD_EVIL) != 0) && ((item.flags & TR_SLAY_EVIL) != 0u)) {
            memory.defenses |= CD_EVIL;
            return total_damage * 2;
        }

        // Frost
        if (((creature.defenses & CD_FROST) != 0) && ((item.flags & TR_FROST_BRAND) != 0u)) {
            memory.defenses |= CD_FROST;
            return total_damage * 3 / 2;
        }

        // Fire
        if (((creature.defenses & CD_FIRE) != 0) && ((item.flags & TR_FLAME_TONGUE) != 0u)) {
            memory.defenses |= CD_FIRE;
            return total_damage * 3 / 2;
        }
    }

    return total_damage;
}
