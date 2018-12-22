#ifndef _SERVER_H_
#define _SERVER_H_

typedef struct {
  long Type;
  long Size;
} sGenericPacket;

typedef struct {
  sGenericPacket Header;
  char Message[256];
} sMessagePacket;

typedef struct {
  sGenericPacket Header;
  float Direction;
} sTurnPacket;

typedef struct {
  sGenericPacket Header;
  float Direction;
} sMovePacket;

typedef struct {
  sGenericPacket Header;
} sAttackPacket;

typedef struct {
  sGenericPacket Header;
  long Spell;
} sSpellPacket;


#endif
