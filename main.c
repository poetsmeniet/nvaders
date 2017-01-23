#include <ncurses.h>
#include <unistd.h>

#define DELAY1 10000
#define DELAY2 60000
#define MVSTEP 0.1

struct f{
    int x;
    int y;
};
struct e{
    int x;
    int y;
};

void setStage(struct f *ship, struct e *enemy, int max_x, int max_y);
void printObjects(struct f * ship, struct e *enemy, int max_x, int max_y);
void moveShip(struct f *ship, int mv);
void moveEnemy(struct e *enemy, int max_x, int max_y, int d);

int main(int argc, char *argv[]) {
    initscr();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(FALSE);
    timeout(0); /* set delay non blocking */

    int max_y = 0; int max_x = 0;
    int mv, d;

    struct f ship;
    ship.x=0;
    ship.y=0;

    struct e enemy;
    enemy.x=0;
    enemy.y=0;
   
    // Global var `stdscr` is created by the call to `initscr()`
    getmaxyx(stdscr, max_y, max_x);
    getmaxyx(stdscr, max_y, max_x);//move to on winch
    setStage(&ship, &enemy, max_x, max_y);//move to on winch
   
    while(1) {
        mv=getch();

        moveShip(&ship, mv);

        if(enemy.x == (max_x - 10))
            d=1;
        if(enemy.x == 0)
            d=0;
        moveEnemy(&enemy, max_x, max_y, d);

        printObjects(&ship, &enemy, max_x, max_y);
   
        usleep(DELAY2);
    }
    
    endwin();
}

void moveShip(struct f *ship, int mv){
        if(mv == KEY_LEFT){
            ship->x--;
        }else if(mv == KEY_RIGHT){
            ship->x++;
        }else{
            //ship.x=x;
        }
}

void moveEnemy(struct e *enemy, int max_x, int max_y, int d){
    if(d)
        enemy->x--;
    else
        enemy->x++;
}

void printObjects(struct f * ship, struct e *enemy, int max_x, int max_y){
    clear();
    mvprintw(enemy->y, enemy->x, "0000000000");
    
    mvprintw(max_y-4, ship->x, "    | ");
    mvprintw(max_y-3, ship->x, "   (0) ");
    mvprintw(max_y-2, ship->x, "_/[{X}]\\_");
    mvprintw(max_y-1, ship->x, "   ^ ^   ");
    refresh();
}

void setStage(struct f *ship, struct e *enemy, int max_x, int max_y){
    while(1) {
        enemy->x++;

        if(ship->x == (max_x / 2)){
            break;
        }
        if(ship->x >= (max_x / 2))
            ship->x--;
        else
            ship->x++;
        
        printObjects(ship, enemy, max_x, max_y);

        usleep(DELAY1);
    }
}
