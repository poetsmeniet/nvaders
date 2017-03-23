/*
nvaders

Copyright (c) 2017 Thomas Wink <thomas@geenbs.nl>

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/
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
    int dmg;
};

void setStage(struct f *ship, struct e *enemy, struct p *p, int max_x, int max_y);
void moveProjectiles(struct p *pulse, struct e *enemy, int ship_x, int ship_y, int max_y);
void moveShip(struct f *ship, struct p *pulse, int mv, int max_x);
void moveEnemy(struct e *enemy, int d);
void printObjects(struct f * ship, struct e *enemy, struct p *pulse, int max_y);
void shoot(struct p *pulse);

int main(void) {
    initscr();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(FALSE);
    timeout(0); 

    int max_y = 0; int max_x = 0;
    int mv;int d = 0;

    getmaxyx(stdscr, max_y, max_x);

    //initialize structs, game objects
    struct f ship = {0, 0};
    struct e enemy = {0, 0, 0, "_______(0)_______"};
    struct p p[]={
        {.x=0, .y=max_y, .speed=1, .used=1, .dmg=1, .body=" | "},
        {.x=0, .y=max_y, .speed=1, .used=1, .dmg=2, .body="|||"},
        {.x=0, .y=max_y, .speed=1, .used=1, .dmg=3, .body="|^|"},
        {.x=-1}
    };

    setStage(&ship, &enemy, p, max_x, max_y);//move to on winch
   
    while(1) {
        getmaxyx(stdscr, max_y, max_x);
        mv=getch();

        moveShip(&ship, p, mv, max_x);

        moveProjectiles(p, &enemy, ship.x, ship.y, max_y);

        if(enemy.x == (max_x - 18)){
            d=1;
            enemy.y++;
        }
        if(enemy.x == 0){
            enemy.y++;
            d=0;
        }

        moveEnemy(&enemy, d);

        printObjects(&ship, &enemy, p, max_y);
   
        usleep(DELAY2);
    }
    
    endwin();
}

void moveShip(struct f *ship, struct p *pulse, int mv, int max_x){
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

void moveEnemy(struct e *enemy, int d){
    if(d)
        enemy->x--;
    else
        enemy->x++;
    //sprintf(enemy->body,"*****(%d)",d);
}

void printObjects(struct f * ship, struct e *enemy, struct p *pulse, int max_y){
    clear();
    mvprintw(enemy->y, enemy->x, enemy->body);
    
    mvprintw(max_y-4+ship->y, ship->x, "    | ");
    mvprintw(max_y-3+ship->y, ship->x, "   (0) ");
    mvprintw(max_y-2+ship->y, ship->x, "_/[{X}]\\_");
    mvprintw(max_y-1+ship->y, ship->x, "   ^ ^   ");


    int i;
    for(i=0;pulse[i].x!=-1;i++){
        mvprintw(pulse[i].y, pulse[i].x, pulse[i].body);
        // enemy hits
        if(pulse[i].y <= enemy->y && \
        pulse[i].x >= (enemy->x) && \
        pulse[i].x <= (enemy->x + 19) &&
        pulse[i].used == 0)
        {
            enemy->hits+=pulse[i].dmg;
            snprintf(enemy->body, sizeof enemy->body,"_______(%i)______",enemy->hits);
            pulse[i].used=1;
            pulse[i].y=ship->y;
            pulse[i].x=ship->x;
        }
    }

    //detect game over, enemy collides with ship
    if((enemy->y >= max_y-1) ||
       (ship->x >= enemy->x && 
       ship->x <= enemy->x + 20 &&
       enemy->y >= (ship->y+max_y)-4 && 
       enemy->y <= (ship->y+max_y) )
    ){
        snprintf(enemy->body, sizeof enemy->body,"GAME OVER. %i hits",enemy->hits);
        mvprintw(enemy->y, enemy->x, enemy->body);
        refresh();
        sleep(600);
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
        printObjects(ship, enemy, p, max_y);

        usleep(DELAY1);
    }
}

void shoot(struct p *pulse){
    int i;
    for(i=0;pulse[i].x!=-1;i++){
        if(pulse[i].used==1){ // is available
            pulse[i].used=0; //shoot
            break;
        }
    }
}

void moveProjectiles(struct p *pulse, struct e *enemy, int ship_x, int ship_y, int max_y){
    int i;
    for(i=0;pulse[i].x!=-1;i++){
        if(pulse[i].used == 0){ //in use
            pulse[i].x=ship_x + 3;

            int newY=pulse[i].y - pulse[i].speed;
            pulse[i].y=newY;

            if(pulse[i].y <= 1){ //hit stage
                pulse[i].used=1;
                pulse[i].y=max_y + ship_y;
                enemy->hits--;
                snprintf(enemy->body, sizeof enemy->body,"_______(%i)______",enemy->hits);
            }
        }else{
            pulse[i].x=ship_x + 3;
            pulse[i].y=max_y + ship_y;
        }
    }
}
