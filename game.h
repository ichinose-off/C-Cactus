#if !defined(__GAME__H__)
#define __GAME__H__

#define playerDeadColor "\e[31m"
#define wallColor "\e[30;1m"
#define cactusColor "\e[32m"
#define lootColor "\e[33m"
#define playerColor "\e[37m"
#define textColor "\e[37m"
#define resetFgColor "\e[0m"

typedef enum bool
{
  false,
  true
} bool;

typedef struct Player
{
  char character;
  int x;
  int y;
  bool isDead;
} Player;

typedef struct Environment
{
  char wall;
  char cactus;
  bool hasCactus;
  char loot;
  int nbLoot;
} Environment;

typedef struct Window
{
  int rows;
  int columns;
  char gameTitle[256];
  char winMessage[256];
  char gameOverMessage[256];
  char leaveMessage[256];
} Window;

void gameManager(void);

void loadSetting(Window *window, Player *player, Environment *environment);
bool checkSettingCharacter(char character);
void generalSettingError(char *message);
void characterSettingError(char *parameter);

char **initGameMap(Window window, Player player, Environment environment);
void allocateMemoryError(void);
void freeGameMap(char **gameMap, Window window);
void generateCactus(char **gameMap, Window window, Environment environment);

void showGameMap(char **gameMap, Window window, Player player, Environment environment);
void showMenu(char *moveCommand, Environment environment);
void updateForegroundColor(char *color);
void flushBuffer(void);
void showMessage(char *message, Window window);

void updateGameMap(char **gameMap, bool *gameStatus, char moveCommand, Window window, Player *player, Environment *environment);
void movePlayer(char **gameMap, Player *player, int nextX, int nextY);

#endif
#ifdef _WIN32
#define CMD_CLEAR "cls"
#else
#define CMD_CLEAR "clear"
#endif