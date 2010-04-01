
/*
WEAPON(flags, base)
MAGIC_WEAPON(flags, base, mod)
ARMOR(flags, base, mod)
CARD(flags, base)
WAND(flags, base)
THING(flags, base)
*/

START_OBJECTS

WEAPON(OF_DONOTUSE, "first weapon", "first weapon", 0, 0)
WEAPON(0, "long sword", "long sword", 1, 8)
WEAPON(0, "short sword", "short sword", 1, 6)
WEAPON(0, "two-handed sword", "two-handed sword", 2, 6)
WEAPON(0, "small axe", "small axe", 1, 4)
WEAPON(0, "large axe", "large axe", 1, 10)
WEAPON(0, "two-handed axe", "two-handed axe", 2, 8)
WEAPON(0, "dagger", "dagger", 1, 4)
WEAPON(0, "broadsword", "broadsword", 2, 10)
WEAPON(0, "giant sword", "giant sword", 3, 8)
WEAPON(OF_DONOTUSE, "last weapon", "last weapon", 0, 0)

MAGIC_WEAPON_UNIQUE(0, "starhammer", "hammer", 577, 5, 20)
MAGIC_WEAPON_UNIQUE(OF_HOLYFUCK, "two-handed sword", "two-handed sword", 9, 3, 6)

ARMOR(OF_DONOTUSE, "first armor", "first armor", 0)
ARMOR(0, "chain mail", "chain mail", 10)
ARMOR(0, "helmet", "helmet", 1)
ARMOR(OF_DONOTUSE, "last armor", "last armor", 0)

WAND(OF_DONOTUSE, "first wand")
WAND(0, "wand of fire")
WAND(0, "wand of ice")
WAND(0, "wand of healing")
WAND(0, "wand of thunder")
WAND(0, "wand of lightning")
WAND(0, "wand of fear")
WAND(OF_DONOTUSE, "last wand")

THING(OF_EATABLE, "red herring", "fish")
THING(OF_EATABLE, "heavy metal fruit", "fruit")
THING(OF_DRINKABLE, "amber ale", "ale")

RING(OF_DONOTUSE, "first ring", 0)
RING(0, "ring of chameleonism", 0)
RING(0, "ring of wisdom", 1)
RING(0, "ring of great wisdom", 2)
RING(0, "ring of the philosopher", 4)
RING(0, "ring of invisibility", 0)
RING(0, "ring of regeneration", 0)
RING(0, "ring of fire resistance", 0)
RING(0, "ring of fire", 0)
RING(0, "ring of protection", 1)
RING(0, "ring of great protection", 2)
RING(0, "ring of divine protection", 5)
RING(0, "ring of hibernating digestion", 0)
RING(0, "ring of light", 0)
RING(OF_DONOTUSE, "last ring", 0)

END_OBJECTS
