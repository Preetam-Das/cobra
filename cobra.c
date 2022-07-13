#include <ncurses.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

int fx, fy, ch, gamerunning = 1, score = 0;
bool foodeaten = true;
float _slptime = 1E5;
WINDOW *gamearea;
enum direction {UP, DOWN, LEFT, RIGHT};
enum direction dir;

struct body_block_st {
    int x, y;
};

struct body_block_st *block;

struct snake_st {
    int head;
    int hy, hx;
    int size;
    enum direction dir;
};

struct snake_st snake;

void printscore()
{
    move(LINES - 1, 1);
    clrtoeol();
    printw("Score: %d", score);
    refresh();
}

void initialize_body()
{
    int y, x;
    y = snake.hy;
    x = snake.hx;

    for (int i = 0; i < snake.size; i++) {
	switch(snake.dir) {
	    case UP:
		block[i].y = y++;
		block[i].x = x;
		break;
	    case DOWN:
		block[i].y = y--;
		block[i].x = x;
		break;
	    case LEFT:
		block[i].y = y;
		block[i].x = x++;
		break;
	    case RIGHT:
		block[i].y = y;
		block[i].x = x--;
		break;
	    default:
		break;
	}
    }
}

void printsnake()
{
    snake.hx = block[0].x;
    snake.hy = block[0].y;

    for (int i = 0; i < snake.size; i++) {
	mvwaddch(gamearea, block[i].y, block[i].x, snake.head);
    } 

    refresh();
    wrefresh(gamearea);
}

void addfood()
{
    if (foodeaten) {
	fx = rand() % gamearea->_maxx;
	fy = rand() % gamearea->_maxy;
	if (fx == 0)
	    fx = 1;
	if (fy == 0)
	    fy = 1;
	foodeaten = false;
    }

    mvwaddch(gamearea, fy, fx, '*');
    refresh();
    wrefresh(gamearea);
}

void clearsnake()
{
    for (int i = 0; i < snake.size; i++) {
	mvwaddch(gamearea, block[i].y, block[i].x, ' ');
    } 
    box(gamearea, 0, 0);
    refresh();
    wrefresh(gamearea);
}

bool checkfoodeaten()
{
    if (snake.hx == fx && snake.hy == fy) {
	foodeaten = true;
	return true;
    }
    return false;
}

void movesnake(enum direction dir)
{
    int hx, hy;
    switch(dir) {
	case UP:
	    hy = snake.hy - 1;
	    hx = snake.hx;
	    break;
	case DOWN:
	    hy = snake.hy + 1;
	    hx = snake.hx;
	    break;
	case LEFT:
	    hy = snake.hy;
	    hx = snake.hx - 1;
	    break;
	case RIGHT:
	    hy = snake.hy;
	    hx = snake.hx + 1;
	    break;
	default:
	    break;
    }

    clearsnake();

    for (int i = snake.size - 1; i >= 0; i--) {
	if (i == 0) {
	    block[i].x = hx;
	    block[i].y = hy;
	}
	else {
	    block[i].x = block[i - 1].x;
	    block[i].y = block[i - 1].y;
	}
    }


    printsnake();
    usleep(_slptime);
}

bool snakeateitself()
{
    for (int i = 1; i < snake.size; i++) {
	if (snake.hx == block[i].x && snake.hy == block[i].y)
	    return true;
    }
    return false;
}

bool detectcollision()
{
    if (snakeateitself() || snake.hx == gamearea->_maxx || snake.hy == gamearea->_maxy || snake.hx == gamearea->_begx - 1 || snake.hy == gamearea->_begy - 1) {
	wclear(gamearea);
	refresh();
	wrefresh(gamearea);
	box(gamearea, 0, 0);
	refresh();
	wrefresh(gamearea);
	wmove(gamearea, gamearea->_maxy/2, gamearea->_maxx/2 - 5);
	wprintw(gamearea, "GAME OVER!!");
	wmove(gamearea, gamearea->_maxy/2 + 1, gamearea->_maxx/2 - 7);
	wprintw(gamearea, "Total Score: %d", score);
	refresh();
	wrefresh(gamearea);
	return true;
    }
    return false;
}

int main()
{
    initscr();
    curs_set(0);
    nodelay(stdscr, 1);
    noecho();
    keypad(stdscr, 1);
    
    gamearea = newwin(LINES - 2, COLS - 2, 1, 1);
    box(gamearea, 0, 0);
    refresh();
    wrefresh(gamearea);

    dir = RIGHT;
    block = calloc(100, sizeof(struct body_block_st));
    srand(time(0));

    snake.hy = gamearea->_maxy/2;
    snake.hx = gamearea->_maxx/2;
    snake.head = '*';
    snake.size = 5;
    snake.dir = dir;


    initialize_body();

    while(gamerunning) {

	printscore();

	if (detectcollision())
	    gamerunning = 0;
	if (checkfoodeaten()) {
	    score++;
	    snake.size++;
	    _slptime /= 1.05;
	}
	addfood();
	movesnake(dir);

	switch ((ch = getch())) {
	    case KEY_UP:
		if (dir == LEFT || dir == RIGHT)
		    _slptime *= 1.5;
		if (dir != DOWN)
		    dir = UP;
		break;
	    case KEY_DOWN:
		if (dir == LEFT || dir == RIGHT)
		    _slptime *= 1.5;
		if (dir != UP)
		    dir = DOWN;
		break;
	    case KEY_LEFT:
		if (dir == UP || dir == DOWN)
		    _slptime /= 1.5;
		if (dir != RIGHT)
		    dir = LEFT;
		break;
	    case KEY_RIGHT:
		if (dir == UP || dir == DOWN)
		    _slptime /= 1.5;
		if (dir != LEFT)
		    dir = RIGHT;
		break;
	    default:
		break;
	}

    }
    
    getchar();
    refresh();
    endwin();

    printf("Total Score: %d", score);
    return 0;
}
