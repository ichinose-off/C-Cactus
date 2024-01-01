#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "game.h"

void gameManager(void)
{
  Player player;
  Environment environment;
  Window window;
  bool gameStatus = true;
  char moveCommand;

  loadSetting(&window, &player, &environment);
  srand(time(0));
  system(CMD_CLEAR);
  char **gameMap = initGameMap(window, player, environment);
  showMessage(window.gameTitle, window);

  while (gameStatus)
  {
    showGameMap(gameMap, window, player, environment);
    showMenu(&moveCommand, environment);
    system(CMD_CLEAR);
    updateGameMap(gameMap, &gameStatus, moveCommand, window, &player, &environment);
  }
  showGameMap(gameMap, window, player, environment);
  freeGameMap(gameMap, window);
}

char **initGameMap(Window window, Player player, Environment environment)
{
  int rows = window.rows;
  int columns = window.columns;
  int i, j;

  char **gameMap = malloc(sizeof(char *) * rows);
  if (gameMap == NULL)
    allocateMemoryError();

  for (i = 0; i < rows; i++)
  {
    gameMap[i] = malloc(sizeof(char) * columns);
    if (gameMap[i] == NULL)
      allocateMemoryError();
  }

  for (i = 0; i < rows; i++)
  {
    for (j = 0; j < columns; j++)
    {
      if (i == 0 || i == rows - 1 || j == 0 || j == columns - 1)
        gameMap[i][j] = environment.wall;
      else
        gameMap[i][j] = ' ';
    }
  }

  gameMap[player.x][player.y] = player.character;

  i = 0;
  while (i != environment.nbLoot)
  {
    int randX = rand() % rows;
    int randY = rand() % columns;
    if (gameMap[randX][randY] == ' ')
    {
      gameMap[randX][randY] = environment.loot;
      i++;
    }
  }

  generateCactus(gameMap, window, environment);
  return gameMap;
}

void generateCactus(char **gameMap, Window window, Environment environment)
{
  int i, j;
  int randX, randY;
  int rows = window.rows;
  int columns = window.columns;
  char cactus = environment.cactus;

  if (environment.hasCactus)
  {
    if (rand() % 6 == 0)
    {
      for (i = 0; i < rows; i++)
      {
        for (j = 0; j < columns; j++)
        {
          if (gameMap[i][j] == cactus)
            gameMap[i][j] = ' ';
        }
      }
    }
    for (i = 0; i < columns; i++)
    {
      randX = rand() % rows;
      randY = rand() % columns;
      if (gameMap[randX][randY] == ' ')
        gameMap[randX][randY] = cactus;
    }
  }
}

void freeGameMap(char **gameMap, Window window)
{
  int rows = window.rows;
  for (int i = 0; i < rows; i++)
  {
    free(gameMap[i]);
    gameMap[i] = NULL;
  }
  free(gameMap);
  gameMap = NULL;
}

void showGameMap(char **gameMap, Window window, Player player, Environment environment)
{
  int i, j;
  int rows = window.rows;
  int columns = window.columns;
  for (i = 0; i < rows; i++)
  {
    for (j = 0; j < columns; j++)
    {
      if (gameMap[i][j] == environment.wall)
        updateForegroundColor(wallColor);
      else if (gameMap[i][j] == environment.cactus)
        updateForegroundColor(cactusColor);
      else if (gameMap[i][j] == environment.loot)
        updateForegroundColor(lootColor);
      else if (gameMap[i][j] == player.character && !player.isDead)
        updateForegroundColor(playerColor);
      else if (gameMap[i][j] == player.character && player.isDead)
        updateForegroundColor(playerDeadColor);
      printf("%c", gameMap[i][j]);
      updateForegroundColor(resetFgColor);
    }
    putchar('\n');
  }
}

void showMenu(char *moveCommand, Environment environment)
{
  updateForegroundColor(textColor);
  printf("Number of looters left: %d\n", environment.nbLoot);
  printf("> up (z)\n");
  printf("> down (s)\n");
  printf("> right (d)\n");
  printf("> left (q)\n");
  printf("> leave (l)\n");
  printf("> ");
  scanf("%c", moveCommand);
  flushBuffer();
  updateForegroundColor(resetFgColor);
}

void updateGameMap(char **gameMap, bool *gameStatus, char moveCommand, Window window, Player *player, Environment *environment)
{
  int nextX = player->x;
  int nextY = player->y;
  char nextCase;

  switch (tolower(moveCommand))
  {
  case 'z':
    nextX -= 1;
    break;
  case 's':
    nextX += 1;
    break;
  case 'd':
    nextY += 1;
    break;
  case 'q':
    nextY -= 1;
    break;
  case 'l':
    showMessage(window.leaveMessage, window);
    *gameStatus = false;
    break;
  default:
    showMessage("Unknown command", window);
    break;
  }
  if (*gameStatus && (nextX != player->x || nextY != player->y))
  {
    nextCase = gameMap[nextX][nextY];
    if (nextCase == environment->wall)
    {
      showMessage("You can't go in the wall!", window);
    }
    else if (nextCase == environment->cactus && !player->isDead)
    {
      showMessage(window.gameOverMessage, window);
      movePlayer(gameMap, player, nextX, nextY);
      player->isDead = true;
      *gameStatus = false;
    }
    else
    {
      movePlayer(gameMap, player, nextX, nextY);
      if (nextCase == environment->loot)
      {
        (environment->nbLoot)--;
        if (environment->nbLoot == 0)
        {
          showMessage(window.winMessage, window);
          *gameStatus = false;
        }
        else
        {
          showMessage(window.gameTitle, window);
        }
      }
      else
      {
        showMessage(window.gameTitle, window);
        generateCactus(gameMap, window, *environment);
      }
    }
  }
}

void movePlayer(char **gameMap, Player *player, int nextX, int nextY)
{
  gameMap[player->x][player->y] = ' ';
  gameMap[nextX][nextY] = player->character;
  player->x = nextX;
  player->y = nextY;
}

void loadSetting(Window *window, Player *player, Environment *environment)
{
  char playerCharacter, wall, cactus, loot;
  int rows = 0;
  int columns = 0;
  int nbLoot = 0;
  int hasCactus = 0;

  FILE *file = fopen("setting.ini", "r");
  if (file == NULL)
  {
    fprintf(stderr, "Cannot open setting file (check setting.ini)\n");
    exit(EXIT_FAILURE);
  }

  fscanf(file, "rows=%d\n", &rows);
  fscanf(file, "columns=%d\n", &columns);
  fscanf(file, "player=%c\n", &playerCharacter);
  fscanf(file, "wall=%c\n", &wall);
  fscanf(file, "cactus=%c\n", &cactus);
  fscanf(file, "loot=%c\n", &loot);
  fscanf(file, "nbLoot=%d\n", &nbLoot);
  fscanf(file, "hasCactus=%d", &hasCactus);
  fclose(file);

  if (checkSettingCharacter(playerCharacter))
    characterSettingError("player");
  else if (checkSettingCharacter(wall))
    characterSettingError("wall");
  else if (checkSettingCharacter(cactus))
    characterSettingError("cactus");
  else if (checkSettingCharacter(loot))
    characterSettingError("loot");
  else if (rows < 10 || columns < 10)
    generalSettingError("<rows> and <columns> values must be greater than or equal to 10");
  else if (nbLoot >= rows * columns)
    generalSettingError("<nbLoot> value must be less than <rows> * <columns>");

  player->character = playerCharacter;
  player->x = rows - 2;
  player->y = 1;
  player->isDead = false;
  window->rows = rows;
  window->columns = columns;
  strcpy(window->gameTitle, "Cactus Growth!");
  strcpy(window->winMessage, "Congratulations! You escaped all the cactus!");
  strcpy(window->gameOverMessage, "Game over! The cactus have stung you!");
  strcpy(window->leaveMessage, "See you soon!");
  environment->wall = wall;
  environment->cactus = cactus;
  environment->hasCactus = hasCactus;
  environment->loot = loot;
  environment->nbLoot = nbLoot;
}

bool checkSettingCharacter(char character)
{
  if (isprint(character) == 0 && character != ' ')
    return true;
  return false;
}

void generalSettingError(char *message)
{
  fprintf(stderr, "In setting file, %s\n", message);
  exit(EXIT_FAILURE);
}

void characterSettingError(char *parameter)
{
  char message[256];
  sprintf(message, "<%s> value must be a printable character", parameter);
  generalSettingError(message);
}

void updateForegroundColor(char *color)
{
  printf("%s", color);
}

void allocateMemoryError(void)
{
  fprintf(stderr, "Dynamically allocate memory failed!\n");
  exit(EXIT_FAILURE);
}

void flushBuffer(void)
{
  int c = 0;
  while (c != '\n' && c != EOF)
    c = getchar();
}

void showMessage(char *message, Window window)
{
  int i;
  int spaces = (window.columns - strlen(message)) / 2;
  int columns = window.columns;

  for (i = 0; i < columns; i++)
    putchar('-');
  putchar('\n');

  for (i = 0; i < spaces; i++)
    putchar(' ');

  printf("%s\n", message);

  for (i = 0; i < columns; i++)
    putchar('-');
  putchar('\n');
}
