/*____________________________________________________________________
|
| File: random.h
|
| (C) Copyright 2004-2005 Timothy E. Roden.
|___________________________________________________________________*/

/*___________________
|
| Function Prototypes
|__________________*/

typedef enum {
  random_TYPE_FAST_1,
} RandomType;

typedef void *Random;

       Random     random_Init       (RandomType type);
       void       random_Init       ();
       void       random_Free       (Random r);
       void       random_SetSeed    (Random r, unsigned seed);
       unsigned   random_GetSeed    (Random r);
       RandomType random_GetType    (Random r);
       int        random_GetInt     (Random r, int low, int high);
inline int        random_GetInt     (int low, int high); 
       float      random_GetFloat   (Random r);
       float      random_GetFloat   ();
       double     random_GetDouble  (Random r);
       double     random_GetDouble  ();
       float      random_GetPercent (Random r);
inline float      random_GetPercent ();
