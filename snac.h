#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define true 1
#define false 0

//struct for map
typedef struct{
    //number of row & column
    int row,column;
    //spawn position
    int spawnr,spawnc;
    int spawnr2,spawnc2;
    //store character from file
    char map_show[100][100];
    //position
    int player1[100][100];
    int player2[100][100];
    int obstacle[100][100];
    int food[100][100];
}MAP;

//struct for player & bot
typedef struct{
    //position
    int r,c;
    //speed : sr = speed row, sc = speed column
    int sr,sc;
    //length of the snake
    int length;
    //direction that snake is moving
    char dir;
}PLAYER;

//struct for food
typedef struct{
    //position
    int r,c;
    //status : eaten or not
    int show;
}DOT;

//struct for score
typedef struct{
    //player name
    char name[30];
    //score
    int score;
}SCORE;

//clear everything on the screen
void clear(){
    system("CLS");
    return;
}
//hide/show cursor
void hidecursor(int hide){

   HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
   CONSOLE_CURSOR_INFO info;
   info.dwSize = 100;

   //set visibility
   if(hide) info.bVisible = FALSE;
   else info.bVisible = TRUE;
   SetConsoleCursorInfo(consoleHandle, &info);
   return;
}

//read file from name and print everything
void print_file(char name[100]){
    hidecursor(true);
    FILE * fp = fopen(name,"r");
    char c;
    int idx = 0;
    char out[100];
    while(fscanf(fp,"%c",&c) != EOF ){
         if(c == '\n'){
            out[idx] = '\0';
            printf("%s\n",out);
            idx = 0;
         }
         else{
            out[idx++] = c;
         }
    }
    printf("%s",out);
    fclose(fp);
    return;
}

//turn int to string
void int2str(int num, char *res){
    sprintf(res, "%d", num);
    return;
}

//print sequences of files
int print_all_files(char name[100],int from,int to,int sleep,int repeat,int reverse){

    char newname[1000];
    char num[1000];

    int end = 0;
    int add = 1;
    if(to < from){
        add*=-1;
    }

    while(!end){

        for(int i = from;i!=to;i+=add){
            int2str(i,num);
            strcpy(newname,name);
            clear();
            strcat(newname,num);
            strcat(newname,".txt");
            print_file(newname);
            Sleep(sleep);
            if(kbhit()){
                char c = getch();
                end = 1;
                clear();
                return 1;
            }
        }

        if(!repeat) break;
        if(reverse){
            int temp = to;
            to = from;
            from = temp;
            add*=-1;
        }
    }

    clear();
    return 0;
}

//read map and properties from file return struct of map
MAP read_map(char name[100]){
    MAP ret;
    DOT dot;
    FILE * fp = fopen(name,"r");
    fscanf(fp," %d %d %d %d %d %d",&ret.row,&ret.column,&ret.spawnr,&ret.spawnc,&ret.spawnr2,&ret.spawnc2);
    char c;
    fscanf(fp,"%c",&c);
    for(int i = 1;i<=ret.row;i++){
        for(int j = 1;j<=ret.column;j++){
            int color;
            fscanf(fp,"%c",&c);

            if(c == '[' || c == ']') ret.obstacle[i][j] = 1;
            else if( c == '_') ret.obstacle[i][j] = -1;
            else ret.obstacle[i][j] = 0;

            ret.food[i][j] = 0;
            ret.player1[i][j] = 0;

            ret.map_show[i][j] = c;
            ret.player2[i][j] = 0;
        }
        fscanf(fp,"%c",&c);
    }

    return ret;
}

//choosing items by showing different files
int choose(char nname[100],int now,int Min,int Max){

    clear();
    char num[100];
    char name[100];
    int  idx = now;

    strcpy(name,nname);
    int2str(idx,num);
    strcat(name,num);
    strcat(name,".txt");

    print_file(name);

    while(1){
        char c = 0;
        if(kbhit()){
            c = getch();
            if(c == 72) idx--;
            else if( c == 80) idx++;
            else if(c == 75) idx--;
            else if( c == 77) idx++;
            else if(c == 13) break;
            else if(c == 8){
                clear();
                return -1;
            }
            if(idx > Max) idx = Min;
            if(idx < Min) idx = Max;

            clear();
            strcpy(name,nname);
            int2str(idx,num);
            strcat(name,num);
            strcat(name,".txt");

            print_file(name);
        }
    }
    clear();
    return idx;
}

//random speed for snake bot
void bot_random(MAP * map,PLAYER * bot){
    while(1){

        if(map->obstacle[bot->r+bot->sr][bot->c+bot->sc] == 0){
            if(rand()%2) break;
        }
        if(rand()%2){
            if(rand()%2){
                bot->sc = 1;
            }
            else bot->sc = -1;
                bot->sr = 0;
        }
        else{
            if(rand()%2){
                bot->sr = 1;
            }
            else bot->sr = -1;
                bot->sc = 0;
            }
    }
}

//more clever algorithm for bot speed
void bot_ai_1(MAP * map,PLAYER * bot,DOT * dot){
    int dunno= 0;
    while(1){
        int idx = rand()%3;
        if(idx <= 0){
            if(dot->c > bot->c){
                bot->sc = 1;
                bot->sr = 0;
            }
            else{
                bot->sc = -1;
                bot->sr = 0;
            }
        }
        else if(idx <= 1){
            if(dot->r > bot->r){
                bot->sc = 0;
                bot->sr = 1;
            }
            else{
                bot->sc = 0;
                bot->sr = -1;

                }
        }

        if(map->obstacle[bot->r+bot->sr][bot->c+bot->sc] == 0){
            return;
        }
        else dunno++;

        if(dunno >= 2){
            bot_random(map,bot);
            return;
        }
    }
}

//ask name and store in input address
void ask_name(char * ret){

    print_all_files("Name/name",1,9,200,false,false);
    print_file("Name/name9.txt");
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = {13, 11};
    SetConsoleCursorPosition(hConsole, pos);
    hidecursor(false);
    fgets(ret,25,stdin);
    hidecursor(true);
    return;
}

//update scoreboard from player score
void update_scoreboard(int score,int map,int mode){
    //set console size
    system("MODE CON:cols=55 lines=29");

    char nname[100] = "scoreboard/scoreboard";
    char num[100];
    char name[100];

    //create file path from map number(1,2,...) and mode(1,2) [1 player, 2 players]
    strcpy(name,nname);
    int2str(map,num);
    strcat(name,num);
    strcat(name,"_");
    int2str(mode,num);
    strcat(name,num);
    strcat(name,".bin");

    //check the score : higher than anyone or not
    FILE * fp = fopen(name,"rb");
    SCORE scores[10];
    int is_highscore = 0;
    for(int i = 0;i<5;i++){
        fread(&scores[i],sizeof(SCORE),1,fp);
        if(score > scores[i].score) is_highscore  = 1;
    }
    fclose(fp);


    //if the score is higher than someone : ask player's name
    if(is_highscore){
        hidecursor(false);
        ask_name(scores[5].name);

        if(scores[5].name[strlen(scores[5].name)-1] == '\n'){
            scores[5].name[strlen(scores[5].name)-1] = '\0';
        }
        scores[5].score = score;

        //check the name: if the name already on scoreboard keep the highest score
        for(int i = 0;i<5;i++){
            if(strcmp(scores[5].name,scores[i].name) == 0){
                scores[i].score = max(scores[i].score,scores[5].score);
                is_highscore = 0;
                scores[5].score = -1;
            }
        }

        //sort the score & name
        for(int i = 0;i<6;i++){
            for(int j = i+1;j<6;j++){
                if(scores[i].score < scores[j].score){
                    SCORE temp = scores[i];
                    scores[i] = scores[j];
                    scores[j] = temp;
                }
            }
        }

        //update scoreboard file
        FILE * fp = fopen(name,"wb");
        for(int i = 0;i<5;i++){
            fwrite(&scores[i],sizeof(SCORE),1,fp);
        }
        fclose(fp);
    }
    else{
        print_all_files("Dead/dead",17,20,200,false,false);
    }

    hidecursor(true);
    return;
}

//code for 1 player mode
void oneplayer(char * map_name,int map_index){
    //for randoming
    srand(time(0));
    //food
    DOT dot;
    dot.show = 0;

    //store the frames before for replaying and updating screen
    char replay[5][100][100];
    memset(replay,0,sizeof(replay));

    //initialize map & players
    MAP map;
    PLAYER player;
    PLAYER bot;

    map = read_map(map_name);

    player.r = map.spawnr;
    player.c = map.spawnc;
    player.sr = 0;
    player.sc = 0;
    player.length = 2;
    player.dir = 'n';

    bot.r = map.spawnr2;
    bot.c = map.spawnc2;
    bot.sr = 0;
    bot.sc = 0;
    bot.length = 2;

    int start = 0;
    int end = 0;

    while(!end){

        //for checking keyboard hit
        int all_key[4] = {0x57,0x53,0x41,0x44};
        int speed_r[4] = {-1,1,0,0};
        int speed_c[4] = {0,0,-1,1};
        char dirs[4] = {'w','s','a','d'};
        char tdirs[4] = {'ä','Ë','¿','¡'};
        char opps[4] = {'s','w','d','a'};

        int msec = 0, trigger = 175;
        clock_t before = clock();
        do {
            if(kbhit()){
                char c;
                c = getch();
                for(int ii = 0;ii<4;ii++){
                    if((c == dirs[ii] || c == tdirs[ii]) && player.dir != opps[ii]){
                        player.sr = speed_r[ii];
                        player.sc = speed_c[ii];
                        start = 1;
                    }
                }
            }


          clock_t difference = clock() - before;
          msec = difference * 1000 / CLOCKS_PER_SEC;

        } while ( msec < trigger );

        //update food
        while(!dot.show){
            dot.r = rand()%map.row + 1;
            dot.c = rand()%map.column + 1;
            if(map.food[dot.r][dot.c] == 0 && map.obstacle[dot.r][dot.c] == 0){
                map.food[dot.r][dot.c] = 1;
                dot.show = 1;
                break;
            }
        }

        //get bot direction from speed
        if(start) bot_ai_1(&map,&bot,&dot);

        //update position of player and bot
        player.r += player.sr;
        player.c += player.sc;
        //update direction
        for(int i = 0;i<4;i++){
            if(player.sr == speed_r[i] && player.sc == speed_c[i]){
                player.dir = dirs[i];
            }
        }

        bot.r += bot.sr;
        bot.c += bot.sc;

        //check if player or bot hits anything or not
        if(map.obstacle[player.r][player.c] == 1) end = 1;
        if(map.player2[player.r][player.c] != 0) end = 1;
        if(map.player1[player.r][player.c] != 0 && start) end = 1;
        if(map.player1[bot.r][bot.c] != 0){

            bot.r = map.spawnr2;
            bot.c = map.spawnc2;
            player.length += bot.length-2;
            bot.length = 2;

        }
        if(player.r == dot.r && player.c == dot.c){
            player.length++;
            map.food[player.r][player.c] = 0;
            dot.show = 0;
        }
        else if(bot.r == dot.r && bot.c == dot.c){
            bot.length++;
            map.food[bot.r][bot.c] = 0;
            dot.show = 0;
        }

        //update tail
        for(int i = 1;i<=map.row;i++){
            for(int j = 1;j<=map.column;j++){
                if(map.player1[i][j] > 0) map.player1[i][j]--;
            }
        }
        for(int i = 1;i<=map.row;i++){
            for(int j = 1;j<=map.column;j++){
                if(map.player2[i][j] > 0) map.player2[i][j]--;
            }
        }
        //set new position for head
        map.player2[bot.r][bot.c] = bot.length;
        map.player1[player.r][player.c] = player.length;

        //generate the map appearance
        for(int i = 1;i<=map.row;i++){
            char out[100];
            int idx = 0;
            for(int j = 1;j<=map.column;j++){
                if(map.food[i][j] == 1){
                    out[idx++] = '*';
                }
                else if(map.obstacle[i][j] == -1){
                     out[idx++] = ' ';
                }
                else if(map.player1[i][j] == player.length && player.length != 0){
                    out[idx++] = 'O';
                }
                else if(map.player1[i][j] > 0){
                    out[idx++] = 'o';
                }
                else if(i == bot.r && j == bot.c){
                    out[idx++] = 'X';
                }
                else if(map.player2[i][j] > 0){
                    out[idx++] = 'o';
                }
                else{
                    out[idx++] = map.map_show[i][j];
                }
            }
            out[idx] = '\0';
            strcpy(replay[4][i-1],out);
        }

        //update the shown map
        for(int i = 0;i<map.row;i++){
            for(int j = 0;j<map.column;j++){
                if(replay[4][i][j] != replay[3][i][j]){
                    hidecursor(true);
                    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
                    COORD pos = {j, i};
                    SetConsoleCursorPosition(hConsole, pos);
                    printf("%c",replay[4][i][j]);
                }
            }
        }

        //update the stored frames
        for(int i = 0;i<=3;i++){
            for(int j = 0;j<map.row;j++){
                strcpy(replay[i][j],replay[i+1][j]);
            }
        }
    }

    //play replay
    Sleep(1000);
    for(int i = 0;i<=3;i++){
        clear();
        for(int j = 0;j<map.row;j++){
            printf("%s",replay[i][j]);
            if(j!=map.row-1) printf("\n");
        }
        Sleep(500);
    }
    Sleep(500);
    clear();

    //set console size
    system("MODE CON:cols=55 lines=29");
    //play dead animation
    print_all_files("Dead/dead",1,16,200,false,false);
    //update score
    update_scoreboard(player.length,map_index,1);
}

//code for 2 players mode
void twoplayers(char * map_name,int map_index){
    //for randoming
    srand(time(0));

    //food
    DOT dot;
    dot.show = 0;

    //store the frames before for replaying and updating screen
    char replay[5][100][100];
    memset(replay,0,sizeof(replay));

    //initialize map & players
    MAP map;
    PLAYER player;
    PLAYER player2;

    map = read_map(map_name);

    player.r = map.spawnr;
    player.c = map.spawnc;
    player.sr = 0;
    player.sc = 0;
    player.length = 2;
    player.dir = 'n';

    player2.r = map.spawnr2;
    player2.c = map.spawnc2;
    player2.sr = 0;
    player2.sc = 0;
    player2.length = 2;
    player2.dir = 'n';

    int start = 0;
    int start2 = 0;
    int end = 0;

    while(!end){

        //for checking keyboard hit
        int all_key[8] = {0x57,0x53,0x41,0x44,0x49,0x4B,0x4A,0x4C};
        int speed_r[4] = {-1,1,0,0};
        int speed_c[4] = {0,0,-1,1};
        char dirs[8] = {'w','s','a','d','i','k','j','l'};
        char tdirs[8] = {'ä','Ë','¿','¡','Ã','Ò','è','Ê'};
        char opps[4] = {'s','w','d','a'};


        int msec = 0, trigger = 175;
        clock_t before = clock();
        do {
            if(kbhit()){
                char c;
                c = getch();
                for(int ii = 0;ii<4;ii++){
                    if((c == dirs[ii] || c == tdirs[ii])&& player.dir != opps[ii]){
                        player.sr = speed_r[ii];
                        player.sc = speed_c[ii];
                        start = 1;
                    }
                }
                for(int ii = 0;ii<4;ii++){
                    if((c == dirs[ii+4] || c == tdirs[ii+4])&& player2.dir != opps[ii]){
                        player2.sr = speed_r[ii];
                        player2.sc = speed_c[ii];
                        start2 = 1;
                    }
                }
            }


            clock_t difference = clock() - before;
            msec = difference * 1000 / CLOCKS_PER_SEC;

        } while ( msec < trigger );
        //update food
        while(!dot.show){
            dot.r = rand()%map.row + 1;
            dot.c = rand()%map.column + 1;
            if(map.food[dot.r][dot.c] == 0 && map.obstacle[dot.r][dot.c] == 0){
                map.food[dot.r][dot.c] = 1;
                dot.show = 1;
                break;
            }
        }
        //update direction
        for(int i = 0;i<4;i++){
            if(player.sr == speed_r[i] && player.sc == speed_c[i]){
                player.dir = dirs[i];
            }
        }

        for(int i = 0;i<4;i++){
            if(player2.sr == speed_r[i] && player2.sc == speed_c[i]){
                player2.dir = dirs[i];
            }
        }
        //update position
        player.r += player.sr;
        player.c += player.sc;
        player2.r += player2.sr;
        player2.c += player2.sc;

        //check if players hit anything or not
        if(map.obstacle[player.r][player.c] == 1) end = 1;
        if(map.obstacle[player2.r][player2.c] == 1) end = 1;
        if(map.player2[player.r][player.c] != 0) end = 1;
        if(map.player1[player2.r][player2.c] != 0) end = 1;

        if(map.player1[player.r][player.c] != 0 && start) end = 1;
        if(map.player2[player2.r][player2.c] != 0 && start2) end = 1;

        if(player.r == dot.r && player.c == dot.c){
            player.length++;
            map.food[player.r][player.c] = 0;
            dot.show = 0;
        }

        if(player2.r == dot.r && player2.c == dot.c){
            player2.length++;
            map.food[player2.r][player2.c] = 0;
            dot.show = 0;
        }
        //update tail
        for(int i = 1;i<=map.row;i++){
            for(int j = 1;j<=map.column;j++){
                if(map.player1[i][j] > 0) map.player1[i][j]--;
                if(map.player2[i][j] > 0) map.player2[i][j]--;
            }
        }

        //set new position for head
        map.player2[player2.r][player2.c] = player2.length;
        map.player1[player.r][player.c] = player.length;

        //generate the map appearance
        for(int i = 1;i<=map.row;i++){
            char out[100];
            int idx = 0;
            for(int j = 1;j<=map.column;j++){
                if(map.food[i][j] == 1){
                    out[idx++] = '*';
                }
                else if(map.obstacle[i][j] == -1){
                     out[idx++] = ' ';
                }
                else if(map.player1[i][j] == player.length && player.length != 0){
                    out[idx++] = 'O';
                }
                else if(map.player1[i][j] > 0){
                    out[idx++] = 'o';
                }
                else if(map.player2[i][j] == player2.length && player2.length != 0){
                    out[idx++] = 'X';
                }
                else if(map.player2[i][j] > 0){
                    out[idx++] = 'o';
                }
                else{
                    out[idx++] = map.map_show[i][j];
                }
            }
            out[idx] = '\0';
            strcpy(replay[4][i-1],out);
        }

        //update the shown map
        for(int i = 0;i<map.row;i++){
            for(int j = 0;j<map.column;j++){
                if(replay[4][i][j] != replay[3][i][j]){
                    hidecursor(true);
                    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
                    COORD pos = {j, i};
                    SetConsoleCursorPosition(hConsole, pos);
                    printf("%c",replay[4][i][j]);
                }
            }
        }

        //update the stored frames
        for(int i = 0;i<=3;i++){
            for(int j = 0;j<map.row;j++){
                strcpy(replay[i][j],replay[i+1][j]);
            }
        }
    }

    //play replay
    Sleep(1000);
    for(int i = 0;i<=3;i++){
        clear();
        for(int j = 0;j<map.row;j++){
            printf("%s",replay[i][j]);
            if(j!=map.row-1) printf("\n");
        }
        Sleep(600);
    }
    Sleep(500);
    clear();
    //set console size
    system("MODE CON:cols=55 lines=29");
    //play dead animation
    print_all_files("Dead/dead",1,16,200,false,false);
    //update score
    update_scoreboard(max(player.length,player2.length),map_index,2);

}
//wait for keyboard to be hit and return the key value
char wait_for_kbhit(){
    char c;
    c = getch();
    return c;
}

//show intro of the game from files
void show_intro(){
    if(print_all_files("splash/intro",1,27,200,false,false) == 0){
        print_all_files("splash/intro",28,41,200,true,true);
    }
    return;
}

//show scoreboard that can choose map and mode
void show_scoreboard(int map,int mode,int min,int max){
    //animation
    print_all_files("scoreboard/scoreboard_intro",1,7,200,false,false);
    while(1){
        char nname[100] = "scoreboard/scoreboard";
        clear();
        char num[100];
        char name[100];

        //make the path from map number and mode number
        strcpy(name,nname);
        int2str(map,num);
        strcat(name,num);
        strcat(name,"_");
        int2str(mode,num);
        strcat(name,num);
        strcat(name,".bin");

        //open - read - close scoreboard file
        FILE * fp = fopen(name,"rb");
        SCORE scores[10];
        for(int i = 0;i<5;i++){
            fread(&scores[i],sizeof(SCORE),1,fp);
        }
        fclose(fp);

        //show scoreboard
        print_file("scoreboard/scoreboard_upper.txt");
        printf("\n");
        int idx = 0;
        for(int i = 1;i<=16;i++){
            if(i == 2){
                if(mode == 1) printf("#|%13s[MAP %2d]%7s[1 PLAYER]%13s|#\n","",map,"","");
                else printf("#|%13s[MAP %2d]%6s[2 PLAYERS]%13s|#\n","",map,"","");
            }
            else if(i>=3 && (i-3)%3 == 0){
                 printf("#|%12s%1d. %-20.20s%4d%12s|#\n","",idx+1,scores[idx].name,scores[idx].score,"");
                 idx++;
            }
            else printf("#|                                                   |#\n");

        }
        print_file("scoreboard/scoreboard_lower.txt");

        char c = 0;
        if(c = wait_for_kbhit()){
            if(c == 72) mode--;
            else if( c == 80) mode++;
            else if( c == 75) mode--;
            else if( c == 77) mode++;
            else if( c > 0  ) break;

            if(mode == 0){
                map--;
                mode = 2;
            }
            else if(mode == 3){
                map++;
                mode = 1;
            }
            if(map > max) map = min;
            if(map < min) map = max;

            clear();
            strcpy(name,nname);
            int2str(idx,num);
            strcat(name,num);
            strcat(name,".txt");
        }

    }
    print_all_files("scoreboard/scoreboard_intro",7,1,200,false,false);
    return;
}

//reset scoreboard from name/path
void reset_scoreboard(char * name){

    FILE * fp = fopen(name,"wb");
    SCORE score;
        for(int i = 0;i<5;i++){
            strcpy(score.name, "------");

            score.score = 0;
            fwrite(&score,sizeof(SCORE),1,fp);
        }

}

void show_mode_intro(){
    print_all_files("mode_splash/intro",1,5,200,false,false);
    return;
}

void show_menu_intro(){
    print_all_files("menu_splash/mintro",1,10,200,false,false);
}

void SNAC(){
    //set console name
    SetConsoleTitle("SNAC");
    //set console size

    hidecursor(true);
    show_intro();
    show_menu_intro();
    int map = 1;
    int opt = 1;
    int b_opt = 1;
    int mode = 1;
    while(1){

        choose_menu:;
        //set console size

        b_opt = opt;
        opt = choose("menu/start_menu",opt,1,3);

        if(opt == 3) return;
        else if(opt == 2){
            show_scoreboard(map,mode,1,3);
        }
        else if(opt == -1){
            opt = b_opt;
        }
        else{
            show_mode_intro();
            choose_mode:;


            mode = choose("modes/mode",mode,1,2);
            if(mode == -1){
                mode = 1;

                print_all_files("mode_splash/intro",5,1,200,false,false);
                goto choose_menu;
            }

            map = choose("maps/intro",map,1,3);

            clear();
            char map_name[100];
            if(map == 1) strcpy(map_name,"maps/map1.txt");
            else if(map == 2) strcpy(map_name,"maps/map2.txt");
            else if(map == 3) strcpy(map_name,"maps/map3.txt");
            else if(map == -1){

                print_all_files("mode_splash/intro",5,4,200,false,false);
                goto choose_mode;
            }

            if(mode == 1) oneplayer(map_name,map);
            else twoplayers(map_name,map);

            //animation
            print_all_files("Dead/dead",21,22,200,false,false);
        }

    }

    Sleep(1000);
    clear();

    printf("Exit");
    return;
}
