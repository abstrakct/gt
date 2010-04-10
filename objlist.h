
/*
WEAPON(flags, base)
MAGIC_WEAPON(flags, base, mod)
ARMOR(flags, base, mod)
CARD(flags, base)
WAND(flags, base)
THING(flags, base)
*/

START_OBJECTS

WEAPON(OF_DONOTUSE, "first weapon", "first weapon", 0, 0, 0)
WEAPON(0, "long sword", "long sword", 1, 8, SK_LONGSWORD)
WEAPON(0, "short sword", "short sword", 1, 6, SK_SHORTSWORD)
WEAPON(0, "small axe", "small axe", 1, 4, SK_BATTLEAXE)
WEAPON(0, "large axe", "large axe", 1, 10, SK_BATTLEAXE)
WEAPON(0, "dagger", "dagger", 1, 4, 0)
WEAPON(0, "stick", "stick", 1, 3, 0)
WEAPON(0, "two-handed sword", "two-handed sword", 2, 6, 0)
WEAPON(0, "two-handed axe", "two-handed axe", 2, 8, SK_BATTLEAXE)
WEAPON(0, "broadsword", "broadsword", 2, 10, 0)
WEAPON(0, "giant sword", "giant sword", 3, 8, 0)
WEAPON(0, "morning star", "morning star", 2, 4, 0)
WEAPON(0, "warhammer", "warhammer", 2, 6, 0)
WEAPON(OF_DONOTUSE, "last weapon", "last weapon", 0, 0, 0)

MAGIC_WEAPON_UNIQUE(0, "starhammer", "hammer", 577, 5, 20, 0)
MAGIC_WEAPON_UNIQUE(OF_HOLYFUCK, "two-handed sword", "two-handed sword", 9, 3, 6, 0)

ARMOR(OF_DONOTUSE, "first armor", "first armor", 0)
ARMOR(OF_BODYARMOR, "leather armor", "leather armor", 1)
ARMOR(OF_BODYARMOR, "studded leather armor", "studded leather armor", 2)
ARMOR(OF_BODYARMOR, "ring mail", "ring mail", 3)
ARMOR(OF_BODYARMOR, "scale mail", "scale mail", 4)
ARMOR(OF_BODYARMOR, "chain mail", "chain mail", 5)
ARMOR(OF_BODYARMOR, "splint mail", "splint mail", 6)
ARMOR(OF_BODYARMOR, "plate mail", "plate mail", 7)
ARMOR(OF_BODYARMOR, "field plate armor", "field plate armor", 8)
ARMOR(OF_BODYARMOR, "full plate armor", "full plate armor", 9)
HEADARMOR(0, "helmet", "helmet", 1)
SHIELD(0, "small shield", "small shield", 1)
SHIELD(0, "large shield", "large shield", 2)
GLOVEARMOR(0, "leather gloves", "leather gloves", 1)
FOOTARMOR(0, "leather shoes", "leather shoes", 1)
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
