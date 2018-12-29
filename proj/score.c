#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>
#include <string.h>
#include <minix/sysutil.h>
#include <stdio.h>

#include "i8042.h"
#include "cromoparty.h"
#include "interface.h"
#include "PS2mouse.h"
#include "score.h"

//VARIABLE INITIALIZATION
int score_to_print = 0;
int score_counter = 0;
char player_name[25];
bool click = false;

//FUNCTIONS
//////////////////////////////////////////////////////////////////

void score(int distance, int *cdance)
{
  if (abs(462 - distance) < 10)
  {
    score_counter += 30;
    score_to_print = 1;
    return;
  }
  else if (abs(462 - distance) < 35)
  {
    score_counter += 10;
    score_to_print = 2;
    return;
  }
  else if (abs(462 - distance) < 70)
  {
    score_counter += 5;
    score_to_print = 3;
    return;
  }
  else
  {
    score_to_print = 4;

    if (!(score_counter - 5 < 0))
    {
      score_counter -= 5;
    }
    else
    {
      score_counter = 0;
    }

    *cdance = 4;
  }
}

//////////////////////////////////////////////////////////////////

void printScore()
{ 
  switch (score_to_print)
  {
    case 0:
      //prints nothing
      break;
    case 1:
    {
      drawBitmap(images.perfect, 362, 330, ALIGN_LEFT);
      break;
    }
    case 2:
    {
      drawBitmap(images.great, 362, 330, ALIGN_LEFT);
      break;
    }
    case 3:
    {
      drawBitmap(images.okay, 362, 330, ALIGN_LEFT);
      break;
    }
    case 4:
    {
      drawBitmap(images.miss, 362, 330, ALIGN_LEFT);
      break;
    }
     case 5:
    {
      drawBitmap(images.extra, 362, 330, ALIGN_LEFT);
      break;
    }
  }
}

//////////////////////////////////////////////////////////////////

void show_score(int score, int x, int y)
{
  if (score == 1)
    score = score_counter;

  int digits = number_of_digits(score);
  int gap = 75; //distance between numbers

  for (int i = 0; i < digits; i++)
  {
    print_digit(score % 10, x - gap, y);
    score /= 10;
    gap += 60; // gap increases with each extra digit
  }
}

//////////////////////////////////////////////////////////////////

void print_digit(int score, int x, int y)
{
  switch (score)
  {
    case 0:
    {
      drawBitmap(images.zero, x, y, ALIGN_LEFT);
      break;
    }
    case 1:
    {
      drawBitmap(images.one, x, y, ALIGN_LEFT);
      break;
    }
    case 2:
    {
      drawBitmap(images.two, x, y, ALIGN_LEFT);
      break;
    }
    case 3:
    {
      drawBitmap(images.three, x, y, ALIGN_LEFT);
      break;
    }
    case 4:
    {
      drawBitmap(images.four, x, y, ALIGN_LEFT);
      break;
    }
    case 5:
    {
      drawBitmap(images.five, x, y, ALIGN_LEFT);
      break;
    }
    case 6:
    {
      drawBitmap(images.six, x, y, ALIGN_LEFT);
      break;
    }
    case 7:
    {
      drawBitmap(images.seven, x, y, ALIGN_LEFT);
      break;
    }
    case 8:
    {
      drawBitmap(images.eight, x, y, ALIGN_LEFT);
      break;
    }
    case 9:
    {
      drawBitmap(images.nine, x, y, ALIGN_LEFT);
      break;
    }
  }
}

//////////////////////////////////////////////////////////////////

int number_of_digits(int number)
{
  int digits = 0;
  do{ 
    number /= 10; digits++; 
  } while (number != 0);

  return digits;
}

//////////////////////////////////////////////////////////////////

void reset_score_counter()
{
  score_counter = 0;
}

//////////////////////////////////////////////////////////////////

void reset_score_to_print()
{
  score_to_print = 0;
}

//////////////////////////////////////////////////////////////////

void get_powerup()
{
  if (pp.lb && images.pointer->colided)
	{
		score_counter += 100;
    click = true;
    score_to_print = 5;
	}

  images.pointer->colided = false;
}

//////////////////////////////////////////////////////////////////

void print_sentence(char string[], int x, int y)
{
  for (unsigned int i = 0; i < strlen(string); i++, x += 28)
  {
    print_letter(string[i], x, y);
  }
}

//////////////////////////////////////////////////////////////////

void print_letter(char c, int x, int y)
{
  switch (c)
  {
    case 'a':
    {
      drawBitmap(images.a, x, y, ALIGN_LEFT);
      break;
    }
    case 'b':
    {
      drawBitmap(images.b, x, y, ALIGN_LEFT);
      break;
    }
    case 'c':
    {
      drawBitmap(images.c, x, y, ALIGN_LEFT);
      break;
    }
    case 'd':
    {
      drawBitmap(images.d, x, y, ALIGN_LEFT);
      break;
    }
    case 'e':
    {
      drawBitmap(images.e, x, y, ALIGN_LEFT);
      break;
    }
    case 'f':
    {
      drawBitmap(images.f, x, y, ALIGN_LEFT);
      break;
    }
    case 'g':
    {
      drawBitmap(images.g, x, y, ALIGN_LEFT);
      break;
    }
    case 'h':
    {
      drawBitmap(images.h, x, y, ALIGN_LEFT);
      break;
    }
    case 'i':
    {
      drawBitmap(images.i, x, y, ALIGN_LEFT);
      break;
    }
    case 'j':
    {
      drawBitmap(images.j, x, y, ALIGN_LEFT);
      break;
    }
    case 'k':
    {
      drawBitmap(images.k, x, y, ALIGN_LEFT);
      break;
    }
    case 'l':
    {
      drawBitmap(images.l, x, y, ALIGN_LEFT);
      break;
    }
    case 'm':
    {
      drawBitmap(images.m, x, y, ALIGN_LEFT);
      break;
    }
    case 'n':
    {
      drawBitmap(images.n, x, y, ALIGN_LEFT);
      break;
    }
    case 'o':
    {
      drawBitmap(images.o, x, y, ALIGN_LEFT);
      break;
    }
    case 'p':
    {
      drawBitmap(images.p, x, y, ALIGN_LEFT);
      break;
    }
    case 'q':
    {
      drawBitmap(images.q, x, y, ALIGN_LEFT);
      break;
    }
    case 'r':
    {
      drawBitmap(images.r, x, y, ALIGN_LEFT);
      break;
    }
    case 's':
    {
      drawBitmap(images.s, x, y, ALIGN_LEFT);
      break;
    }
    case 't':
    {
      drawBitmap(images.t, x, y, ALIGN_LEFT);
      break;
    }
    case 'u':
    {
      drawBitmap(images.u, x, y, ALIGN_LEFT);
      break;
    }
    case 'v':
    {
      drawBitmap(images.v, x, y, ALIGN_LEFT);
      break;
    }
    case 'w':
    {
      drawBitmap(images.w, x, y, ALIGN_LEFT);
      break;
    }
    case 'x':
    {
      drawBitmap(images.x, x, y, ALIGN_LEFT);
      break;
    }
    case 'y':
    {
      drawBitmap(images.y, x, y, ALIGN_LEFT);
      break;
    }
    case 'z':
    {
      drawBitmap(images.z, x, y, ALIGN_LEFT);
      break;
    }
  }
}

Player players[MAX_PLAYER_SLOTS];
int player_counter = 0;

//////////////////////////////////////////////////////////////////

void save_score()
{
  if (players[get_last_player_index()].score > score_counter && player_counter == MAX_PLAYER_SLOTS)
  {
    return;
  }

  if (player_counter == MAX_PLAYER_SLOTS)
  {
    //printf("this one");
    int index = get_last_player_index();
    strcpy(players[index].name, player_name);
    players[index].score = score_counter;
    players[index].rank = rank();
    //printf("%s ", players[index].name);
    //printf("%d ", players[index].score);
  }
  else
  {
    //printf("me");
    strcpy(players[player_counter].name, player_name);
    players[player_counter].score = score_counter;
    players[player_counter].rank = rank() + 1;

    player_counter++;
  }

    /*for (int j = 0; j < player_counter; j++)
    {
      printf("%s ", players[j].name);
      printf("%d ", players[j].score);
      printf("%d\n", players[j].rank);
    }*/
}

//////////////////////////////////////////////////////////////////

void save_score_to_file()
{
  FILE *f = fopen("/home/lcom/labs/proj/highscores.txt", "w");

  if (f == NULL)
  {
    print_sentence("Error opening file!", 300, 300);
    exit(1);
  }

  /* print in rank order*/
  for (int i = 1; i <= player_counter; i++)
  {
    for (int j = 0; j < player_counter; j++)
    {
      if (players[j].rank == i)
      {
        fprintf(f, "%s ", players[j].name);
        fprintf(f, "%d\r\n", players[j].score);
      }
    }
  }

  fclose(f);
}

//////////////////////////////////////////////////////////////////

void load_score_from_file()
{
  FILE *f = fopen("/home/lcom/labs/proj/highscores.txt", "r");
  char buff[30];
  int buff2;

  if (f == NULL)
  {
    print_sentence("Error opening file!", 300, 300);
    exit(1);
  }

  /* read */
  while (fscanf(f, "%s", buff) == 1)
  {
    if (feof(f)) 
    {
      break;
    }

    strcpy(players[player_counter].name, buff);

    fscanf(f, "%d", &buff2);
    players[player_counter].score = buff2;
    players[player_counter].rank = (player_counter + 1);

    player_counter++;
  }

  fclose(f);
}

//////////////////////////////////////////////////////////////////

int get_last_player_index()
{
  int l = 0, cScore = players[0].score;

  for (int i = 1; i < player_counter; i++)
  {
    if (players[i].score < cScore)
    {
      l = i;
    }
  }
  
  return l;
}

//////////////////////////////////////////////////////////////////

int rank()
{
  int r = player_counter;

  //printf("r_a: %d\n", r);

  for (int j = (player_counter - 1); j >= 0; j--)
  {
    if (players[j].score < score_counter)
    {
      players[j].rank++;
      r--;
    }
  }

  //printf("r_d: %d\n", r);

  return r;
}

//////////////////////////////////////////////////////////////////

void print_high_scores()
{
  int x_pos = 100, x_name = 300, x_score = 900;
  int y_pos = 140, y_name = 170;

  for (int i = 1; i <= player_counter; i++)
  {
    for (int j = 0; j < player_counter; j++)
    {
      if (players[j].rank == i)
      {
        print_digit(i, x_pos, y_pos);
        print_sentence(players[j].name, x_name, y_name);
        show_score(players[j].score, x_score, y_pos);
        y_pos += 90;
        y_name += 90;
      }
    }
  }
}

//////////////////////////////////////////////////////////////////

void set_current_player_name(char name[25])
{
  strcpy(player_name, name);
}
