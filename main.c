#include <ncurses.h>
#include <unistd.h>

#define DELAY1 10000
#define DELAY2 30000
#define MVSTEP 0.1

struct fighter{
    int x;
    int y;
};
struct enemy{
    int x;
    int y;
};

void setStage(struct fighter *ship1, struct enemy *enemy, int max_x, int max_y);
void printObjects(struct fighter * ship1, struct enemy *enemy, int max_x, int max_y);
void moveShip(struct fighter *ship, int mv);
void moveEnemy(struct enemy *enemy);

int main(int argc, char *argv[]) {
    initscr();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(FALSE);

    int max_y = 0; int max_x = 0;
    int mv;

    struct fighter ship1;
    ship1.x=0;
    ship1.y=0;

    struct enemy enemy;
    enemy.x=0;
    enemy.y=0;
   
    // Global var `stdscr` is created by the call to `initscr()`
    getmaxyx(stdscr, max_y, max_x);
    getmaxyx(stdscr, max_y, max_x);//move to on winch
    setStage(&ship1, &enemy, max_x, max_y);//move to on winch
   
    while(1) {
        mv=getch();

        moveShip(&ship1, mv);
        moveEnemy(&enemy);

        printObjects(&ship1, &enemy, max_x, max_y);
   
        usleep(DELAY2);
    }
    
    endwin();
}

void moveShip(struct fighter *ship1, int mv){
        if(mv == KEY_LEFT){
            ship1->x--;
        }else if(mv == KEY_RIGHT){
            ship1->x++;
        }else{
            //ship1.x=x;
        }
}

void moveEnemy(struct enemy *enemy){
    enemy->x++;
}
void printObjects(struct fighter * ship1, struct enemy *enemy, int max_x, int max_y){
    clear();
    mvprintw(enemy->y, enemy->x, "^^^^^^^^^^^^^^^");
    mvprintw(max_y-1, ship1->x, "[{O}]");
    refresh();
}

void setStage(struct fighter *ship1, struct enemy *enemy, int max_x, int max_y){
    while(1) {
        enemy->x++;

        if(ship1->x == (max_x / 2)){
            break;
        }
        if(ship1->x >= (max_x / 2))
            ship1->x--;
        else
            ship1->x++;
        
        printObjects(ship1, enemy, max_x, max_y);

        usleep(DELAY1);
    }
}
