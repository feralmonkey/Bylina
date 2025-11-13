#pragma once

struct NPCTag {}; // todo rigolo - replace this with an NPC Component at some point
struct SpriteTag {};
struct TextCharacterTag { int layer; explicit TextCharacterTag(const int layer = 0) : layer(layer) {} };
struct PushableComponent {};  // todo rigolo temporary! This will need to be it's own component at some time
 