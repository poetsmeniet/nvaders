#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#define DELAY1 10000
#define DELAY2 30000
#define MVSTEP 0.1

struct f{
    int x;
    int y;
};
struct e{
    int x;
    int y;
    int hits;
    char body[35];
};
struct p{
    int x;
    int y;
    int speed;
    char body[4];
    int used;
};

void setStage(struct f *ship, struct e *enemy, struct p *p, int max_x, int max_y);
void printObjects(struct f * ship, struct e *enemy, struct p *pulse, int max_x, int max_y);
void moveShip(struct f *ship, struct p *pulse, int mv, int max_x, int max_y);
void moveEnemy(struct e *enemy, int max_x, int max_y, int d);
void shoot(struct p *pulse);
void moveProjectiles(struct p *pulse, int ship_x, int ship_y, int maxy_y);

int main(int argc, char *argv[]) {
    initscr();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(FALSE);
    timeout(0); 

    int max_y = 0; int max_x = 0;
    int mv;int d = 0;

    getmaxyx(stdscr, max_y, max_x);

    struct f ship;
    ship.x=0;
    ship.y=0;

    struct e enemy;
    enemy.x=0;
    enemy.y=0;
    enemy.hits=0;
    snprintf(enemy.body, sizeof enemy.body,"_______(%i)_______",0);

    struct p pulse[5];
    pulse[0].x=0;
    pulse[0].y=max_y;
    pulse[0].speed=1;
    pulse[0].used=1;
    strcpy(pulse[0].body,"|||");
    struct p *pulseP=pulse;
   
    setStage(&ship, &enemy, pulseP, max_x, max_y);//move to on winch
   
    while(1) {
        getmaxyx(stdscr, max_y, max_x);
        mv=getch();

        moveShip(&ship, pulseP, mv, max_x, max_y);

        moveProjectiles(pulseP, ship.x, ship.y, max_y);

        if(enemy.x == (max_x - 18)){
            d=1;
            enemy.y++;
        }
        if(enemy.x == 0){
            enemy.y++;
            d=0;
        }

        moveEnemy(&enemy, max_x, max_y, d);

        printObjects(&ship, &enemy, pulseP, max_x, max_y);
   
        usleep(DELAY2);
    }
    
    endwin();
}

void moveShip(struct f *ship, struct p *pulse, int mv, int max_x, int max_y){
        if(mv == KEY_LEFT && ship->x > 4){
            ship->x-=4;
        }else if(mv == KEY_RIGHT && ship->x < max_x - 14){
            ship->x+=4;
        }
        if(mv == KEY_UP){
            ship->y-=4;
        }else if(mv == KEY_DOWN){
            ship->y+=4;
        }
        if(mv == ' ')
            shoot(pulse);
        if(mv == 'p')
            ship->y+=4;
}

void moveEnemy(struct e *enemy, int max_x, int max_y, int d){
    if(d)
        enemy->x--;
    else
        enemy->x++;
    //sprintf(enemy->body,"*****(%d)",d);
}

void printObjects(struct f * ship, struct e *enemy, struct p *pulse, int max_x, int max_y){
    clear();
    mvprintw(enemy->y, enemy->x, enemy->body);
    
    mvprintw(max_y-4+ship->y, ship->x, "    | ");
    mvprintw(max_y-3+ship->y, ship->x, "   (0) ");
    mvprintw(max_y-2+ship->y, ship->x, "_/[{X}]\\_");
    mvprintw(max_y-1+ship->y, ship->x, "   ^ ^   ");

    mvprintw(pulse->y, pulse->x, pulse->body);

    // hits
    if(pulse->y <= enemy->y && \
       pulse->x >= (enemy->x - 8) && \
       pulse->x <= (enemy->x + 8))
    
    {
        enemy->hits++;
        snprintf(enemy->body, sizeof enemy->body,"_______(%i)%d______",enemy->hits,ship->y);
        pulse->used=1;
        pulse->y=ship->y;
        pulse->x=ship->x;
    }

    //detect game over, enemy collides with ship
    //if(enemy->y == max_y - 4 && enemy->x >= ship->x - 8 && enemy->x <= ship->x + 8){
    if(enemy->y == max_y - 4 && enemy->x >= ship->x - 8 && enemy->x <= ship->x + 8){
        snprintf(enemy->body, sizeof enemy->body,"GAME OVER. %i hits",enemy->hits);
        mvprintw(enemy->y, enemy->x, enemy->body);
        refresh();
        sleep(60);
    }
            
    
    refresh();
}

void setStage(struct f *ship, struct e *enemy, struct p *p, int max_x, int max_y){
    while(1) {
        enemy->x++;

        if(ship->x == (max_x / 2)){
            break;
        }
        if(ship->x >= (max_x / 2))
            ship->x--;
        else
            ship->x++;
        printObjects(ship, enemy, p, max_x, max_y);

        usleep(DELAY1);
    }
}

void shoot(struct p *pulse){
    pulse[0].used=0;
}

void moveProjectiles(struct p *pulse, int ship_x, int ship_y, int max_y){
    //pulse[0].y=20;
    if(pulse[0].used == 0){
        pulse[0].x=ship_x + 3;
        pulse[0].y=pulse[0].y -= pulse[0].speed;

        if(pulse[0].y <= 0){
            pulse[0].used=1;
            pulse[0].y=max_y + ship_y;
            //snprintf(pulse[0].body, sizeof pulse[0].body, "%i",ship_y);
        }
    }else{
        pulse[0].x=ship_x + 3;
        pulse[0].y=max_y + ship_y;
    }
}
