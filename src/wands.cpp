// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Wand code

#include "headers.h"
#include "externs.h"

static bool wandDischarge(Inventory_t *item, int direction) {
    bool identified = false;

    uint32_t flags = item->flags;

    (item->misc_use)--; // decrement "use" variable

    while (flags != 0) {
        int kind = getAndClearFirstBit(flags) + 1;
        int y = char_row;
        int x = char_col;

        // Wand types
        switch (kind) {
            case 1:
                printMessage("A line of blue shimmering light appears.");
                spellLightLine(char_col, char_row, direction);
                identified = true;
                break;
            case 2:
                spellFireBolt(y, x, direction, diceDamageRoll(4, 8), GF_LIGHTNING, spell_names[8]);
                identified = true;
                break;
            case 3:
                spellFireBolt(y, x, direction, diceDamageRoll(6, 8), GF_FROST, spell_names[14]);
                identified = true;
                break;
            case 4:
                spellFireBolt(y, x, direction, diceDamageRoll(9, 8), GF_FIRE, spell_names[22]);
                identified = true;
                break;
            case 5:
                identified = spellWallToMud(y, x, direction);
                break;
            case 6:
                identified = spellPolymorphMonster(y, x, direction);
                break;
            case 7:
                identified = spellChangeMonsterHitPoints(y, x, direction, -diceDamageRoll(4, 6));
                break;
            case 8:
                identified = spellSpeedMonster(y, x, direction, 1);
                break;
            case 9:
                identified = spellSpeedMonster(y, x, direction, -1);
                break;
            case 10:
                identified = spellConfuseMonster(y, x, direction);
                break;
            case 11:
                identified = spellSleepMonster(y, x, direction);
                break;
            case 12:
                identified = spellDrainLifeFromMonster(y, x, direction);
                break;
            case 13:
                identified = spellDestroyDoorsTrapsInDirection(y, x, direction);
                break;
            case 14:
                spellFireBolt(y, x, direction, diceDamageRoll(2, 6), GF_MAGIC_MISSILE, spell_names[0]);
                identified = true;
                break;
            case 15:
                identified = spellBuildWall(y, x, direction);
                break;
            case 16:
                identified = spellCloneMonster(y, x, direction);
                break;
            case 17:
                identified = spellTeleportAwayMonsterInDirection(y, x, direction);
                break;
            case 18:
                identified = spellDisarmAllInDirection(y, x, direction);
                break;
            case 19:
                spellFireBall(y, x, direction, 32, GF_LIGHTNING, "Lightning Ball");
                identified = true;
                break;
            case 20:
                spellFireBall(y, x, direction, 48, GF_FROST, "Cold Ball");
                identified = true;
                break;
            case 21:
                spellFireBall(y, x, direction, 72, GF_FIRE, spell_names[28]);
                identified = true;
                break;
            case 22:
                spellFireBall(y, x, direction, 12, GF_POISON_GAS, spell_names[6]);
                identified = true;
                break;
            case 23:
                spellFireBall(y, x, direction, 60, GF_ACID, "Acid Ball");
                identified = true;
                break;
            case 24:
                flags = (uint32_t) (1L << (randomNumber(23) - 1));
                break;
            default:
                printMessage("Internal error in wands()");
                break;
        }
    }

    return identified;
}

// Wands for the aiming.
void wandAim() {
    player_free_turn = true;

    if (inventory_count == 0) {
        printMessage("But you are not carrying anything.");
        return;
    }

    int item_pos_start, item_pos_end;
    if (!inventoryFindRange(TV_WAND, TV_NEVER, item_pos_start, item_pos_end)) {
        printMessage("You are not carrying any wands.");
        return;
    }

    int item_id;
    if (!inventoryGetInputForItemId(item_id, "Aim which wand?", item_pos_start, item_pos_end, CNIL, CNIL)) {
        return;
    }

    player_free_turn = false;

    int direction;
    if (!getDirectionWithMemory(CNIL, &direction)) {
        return;
    }

    if (py.flags.confused > 0) {
        printMessage("You are confused.");
        direction = getRandomDirection();
    }

    Inventory_t *item = &inventory[item_id];

    int player_class_lev_adj = class_level_adj[py.misc.class_id][CLASS_DEVICE] * py.misc.level / 3;
    int chance = py.misc.saving_throw + playerStatAdjustmentWisdomIntelligence(A_INT) - (int) item->depth_first_found + player_class_lev_adj;

    if (py.flags.confused > 0) {
        chance = chance / 2;
    }

    if (chance < PLAYER_USE_DEVICE_DIFFICULTY && randomNumber(PLAYER_USE_DEVICE_DIFFICULTY - chance + 1) == 1) {
        chance = PLAYER_USE_DEVICE_DIFFICULTY; // Give everyone a slight chance
    }

    if (chance <= 0) {
        chance = 1;
    }

    if (randomNumber(chance) < PLAYER_USE_DEVICE_DIFFICULTY) {
        printMessage("You failed to use the wand properly.");
        return;
    }

    if (item->misc_use < 1) {
        printMessage("The wand has no charges left.");
        if (!spellItemIdentified(item)) {
            itemAppendToInscription(item, ID_EMPTY);
        }
        return;
    }

    bool identified = wandDischarge(item, direction);

    if (identified) {
        if (!itemSetColorlessAsIdentifed(item)) {
            // round half-way case up
            py.misc.exp += (item->depth_first_found + (py.misc.level >> 1)) / py.misc.level;
            displayCharacterExperience();

            itemIdentify(&item_id);
        }
    } else if (!itemSetColorlessAsIdentifed(item)) {
        itemSetAsTried(item);
    }

    itemChargesRemainingDescription(item_id);
}
