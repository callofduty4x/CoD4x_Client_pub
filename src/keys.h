
#define playerKeys (*((PlayerKeyState_t*)(0x8F1CA0)))

#define MAX_KEYS 256

typedef struct 
{
  qboolean down;
  int repeats;
  const char *binding;
}qkey_t;

typedef enum 
{
  LOC_SEL_INPUT_NONE = 0x0,
  LOC_SEL_INPUT_CONFIRM = 0x1,
  LOC_SEL_INPUT_CANCEL = 0x2,
}LocSelInputState_t;


typedef struct 
{
  int cursor;
  int scroll;
  int drawWidth;
  int widthInPixels;
  float charHeight;
  int fixedSize;
  char buffer[256];
}field_t;

typedef struct PlayerKeyState_s
{
  field_t chatField;
  int chat_team;
  int overstrikeMode;
  int anyKeyDown;
  qkey_t keys[MAX_KEYS];
  LocSelInputState_t locSelInputState;
}PlayerKeyState_t;

typedef struct keyname_s
{
  const char *name;
  int keynum;
}keyname_t;