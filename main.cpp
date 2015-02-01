/*include header files*/
#include <allegro.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "fighter.h"
#include "mage.h"

/*define macros*/
#ifdef SCREEN_W                 //screen size
#undef SCREEN_W
#undef SCREEN_H
#endif

#define SCREEN_W	1024
#define SCREEN_H	768

#define FPS		48              //game speed (frames per second)

#ifdef FALSE                     //conditions (FALSE, TRUE, REVERT, COOLDOWN)
#undef FALSE
#undef TRUE
#endif

#define FALSE	    -1		        //if an action does not occur
#define TRUE	    0		        //if an action occurs
#define REVERT	    1		        //if an action is blocked
#define COOLDOWN	2		        //if an action is cooling down

                                //macro function for platforms
#define PlatformFunc(a,b,c)	a*b + c

/*declare constants*/
const float VyO = -150;					//original jump velocity
const float aY = 9.80665;  				//gravitational acceleration
const float aX = 0.99;      			//air pressure
const float punchMultiplier = 0.75; 	//damage multiplier for punch
const float kickMultiplier = 0.99;		//damage multiplier for kick
const float blockMultiplier = 1.15;		//damage multiplier for block
const int mana_reduce = 20;				//loss of mana per special move
const int stunTime = 5;					//stun time after being hit

/*set speed counter & function*/
volatile long fpsCounter = 0;
void increment_fpsCounter()
{
	fpsCounter ++;
}END_OF_FUNCTION(increment_fpsCounter);

/*define character data type*/
typedef struct Character
{
 	char Name[31];          //Character Name
 	float Speed;            //Character Agility/Speed
 	float Damage;           //Maximum Character Damage
 	int Health;             //Total Character Health
 	float Defense;          //Character Defense Against Attacks
 	float Special;          //Damage of Special Attack
 	int Mana;               //Amount of Mana (energy)
 	DATAFILE *Sprites;      //Character Sprites
};

/*declare globals*/
int framecounter = 0;           //game variables
int mapSelect = 0;

double player1X_old = 0;        //player 1 position variables
double player1X_new = 0;
double player1Y_old = 0;
double player1Y_new = 0;
int player1X_round = 0;
int player1Y_round = 0;

int p1_BB[4] = {0};             //player 1 bounding box variables
int p1_BBXhalf = 0;

float player1_Vy = -1;          //player 1 jump variables
double p1JumpTime = 1;

float player1Health_temp = 0;   //player 1 health temp

int p1Right = TRUE;            //player 1 action conditions
int p1Left = FALSE;
int p1Punch = FALSE;
int p1Kick = FALSE;
int p1Special = FALSE;
int p1Block = FALSE;
int p1Stun = FALSE;
int p1Combo = FALSE;

int p1Punch_length = 0;         //player 1 action lengths
int p1Kick_length = 0;
int p1Special_length = 0;
int p1_specialTime = 0;         //player 1 action cooldown times
int p1_blockTime = 0;
int p1_stunTime = 0;
int p1_kickTime = 0;
int p1ManaTemp = 0;
int p1dmg = 0;
int p1Punch_delay = 0;
int p1PunchCount = 0;           //player 1 action count variable

int p1GroundCollision = FALSE;   //player 1 collision conditions
int p1PunchCollision = FALSE;
int p1KickCollision = FALSE;
int p1SpecialCollision = FALSE;

int p1_x_dist;                  //other player 1 variables
int p1_y_dist;
int p1_takedmg_cd;
int p1_jumpaction;
int p1_blocklength;
int p1_punchTime;
int P1ManaRegen = 0;
int p1_champ = 0;

double player2X_old = 0;        //player 2 position variables
double player2X_new = 0;
double player2Y_old = 0;
double player2Y_new = 0;
int player2X_round = 0;
int player2Y_round = 0;

int p2_BB[4] = {0};             //player 2 bounding box variables
int p2_BBXhalf = 0;

float player2_Vy = 0;           //player 2 jump variables
double p2JumpTime = 0;

float player2Health_temp = 0;   //player 2 health variable

int p2Right = FALSE;             //player 2 action conditions
int p2Left = TRUE;
int p2Punch = FALSE;
int p2Kick = FALSE;
int p2Special = FALSE;
int p2Block = FALSE;
int p2Stun = FALSE;
int p2Combo = FALSE;

int p2Punch_length = 0;         //player 2 action lengths
int p2Kick_length = 0;
int p2Special_length = 0;
int p2Block_length = 0;
int p2_specialTime = 0;         //player 2 action cooldown times
int p2_blockTime = 0;
int p2_stunTime = 0;
int p2_kickTime = 0;
int p2ManaTemp = 0;
int p2dmg = 0;
int p2Punch_delay = 0;
int p2PunchCount = 0;           //player 2 action count variable

int p2GroundCollision = FALSE;   //player 2 action collision conditions
int p2PunchCollision = FALSE;
int p2KickCollision = FALSE;
int p2SpecialCollision = FALSE;

int p2_x_dist;                  //other player 2 variables
int p2_y_dist;
int p2_takedmg_cd;
int p2_jumpaction;
int p2_punchTime;
int P2ManaRegen = 0;
int p2_champ = 0;

int Floor = 0;                  //floor and platform position variables
double Platform1[4] = {0};
double Platform2[4] = {0};
double Platform3[4] = {0};
double Platform4[4] = {0};
double Platform5[4] = {0};
double Platform6[4] = {0};

/*prototype functions*/
void MapSelect(BITMAP *map);                                            //select map
struct Character CharacterLoad(struct Character P1, int champ);         //load character file
void player1Movement(struct Character P1, BITMAP *player1);             //Player 1 movement
void player2Movement(struct Character P2, BITMAP *player2);             //Player 2 movement
void CollisionCheck(BITMAP *player1, BITMAP *player2);                  //Checks for various collisions
int p1_Damage_calc(Character Player1, Character Player2);               //Player 1 Damage Calculation
int p2_Damage_calc(Character Player1, Character Player2);               //Player 2 Damage Calculation
int Player1_SpriteSelect(Character P1, BITMAP *temp, int dist);         //Player 1 Sprite Selection
int Player2_SpriteSelect(Character P2, BITMAP *temp, int dist);         //Player 2 Sprite Selection
int P1_get_height(Character P1, BITMAP *temp, int dist);                //Player 1 Sprite Height Calculation
int P2_get_height(Character P2, BITMAP *temp, int dist);                //Player 2 Sprite Height Calculation
int datafileSelect(DATAFILE* Player, int current, int sprite_select);   //Datafile Pointer Adjustment (for Sprite Selection)
int health_interface(int Health, int max_health);                       //Health bar calculation
int mana_interface(int Mana, int max_mana);                             //Mana bar calculation
int end_sprite_select(BITMAP* end_screen, int p1Health, int p2Health); //Victory Screen Selection


/**main function**/
int main(int argc, char *argv[])
{
    /*install allegro and keyboard, timer & sound modules*/
    allegro_init();
    install_keyboard();
    install_timer();
    int sound = install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, 0);

   	/*set screen & speed settings*/
 	LOCK_VARIABLE(fpsCounter);
 	LOCK_FUNCTION(increment_fpsCounter);
 	install_int_ex(increment_fpsCounter, BPS_TO_TIMER(FPS));

    set_color_depth(desktop_color_depth());
	set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, 1024,768,0,0);

	/*declare title/instruction sequence bitmaps*/
	BITMAP *title1 = load_bitmap("title1.bmp", NULL);                   //background bitmaps
	BITMAP *title2 = load_bitmap("title2.bmp", NULL);
	BITMAP *instructions1 = load_bitmap("instructions1.bmp", NULL);
    BITMAP *instructions2 = load_bitmap("instructions2.bmp", NULL);
	BITMAP *instructions3 = load_bitmap("instructions3.bmp", NULL);
	BITMAP *heroselect = load_bitmap("heroselect.bmp", NULL);
	BITMAP *pointer = load_bitmap("cursor.bmp", NULL);                  //foreground bitmaps
	BITMAP *p1 = load_bitmap("player1.bmp", NULL);
	BITMAP *p2 = load_bitmap("player2.bmp", NULL);
	BITMAP *mm_icon = load_bitmap("mm.icon.bmp", NULL);
	BITMAP *mm_back_icon = load_bitmap("mm.back.icon.bmp", NULL);
	BITMAP *mage_icon = load_bitmap("mage.icon.bmp", NULL);
	BITMAP *mage_back_icon = load_bitmap("mage.back.icon.bmp", NULL);
	BITMAP *enter = load_bitmap("enter.bmp", NULL);

	/*load gameplay variables*/
	BITMAP* buffer = create_bitmap(SCREEN_W, SCREEN_H);     //buffers
 	BITMAP* p1_buf = create_bitmap(505,250);
 	BITMAP* p2_buf = create_bitmap(505,250);
 	BITMAP* map = create_bitmap(SCREEN_W, SCREEN_H);        //background bitmaps
 	BITMAP* end_screen = NULL;
 	BITMAP* hp = load_bitmap("hp.bmp", NULL);               //interface bitmaps
 	BITMAP* mana = load_bitmap("mana.bmp", NULL);
 	BITMAP* p1_display = load_bitmap("p1.bmp", NULL);
 	BITMAP* p2_display = load_bitmap("p2.bmp", NULL);
    BITMAP* p1_marker = load_bitmap("player1.bmp", NULL);
    BITMAP* p2_marker = load_bitmap("player2.bmp", NULL);

	/*load other variables*/
	int my_pic_x = 0;
	int my_pic_y = 0;
	int cursor = 0;
	int current = 1;
	int score = 0;

	/*declare character structures for Players 1 and 2*/
    struct Character Player1;
    struct Character Player2;

	/*declare & assign values to health variables*/
 	player1Health_temp = Player1.Health;
 	player2Health_temp = Player2.Health;
 	int p1_max_hp = Player1.Health;
 	int p2_max_hp = Player2.Health;
 	int p1Temp[2] = {0}, p2Temp[2] = {0}, i = 0;
 	float p1_health_display = Player1.Health;
 	float p2_health_display = Player2.Health;

	/*load game music*/
	SAMPLE *music = load_sample("game_music.wav");

	if (!music) //return error if music not found
	{
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        allegro_message("**ERROR: MUSIC NOT FOUND**");
        exit(EXIT_FAILURE);
    }

                //play music in continuous loop
	int play_music = play_sample(music, 255, 128, 1000, -1);

    /*game play*/
	while(!key[KEY_ESC])
	{
        /*title screen 1*/
        if (current == 1)
        {
            blit(title1, screen, 0,0,0,0,1024, 768);
            readkey();
            current += 1;
        }

        /*title screen 2*/
        if (current == 2)
        {
            clear_bitmap(buffer);
            draw_sprite(buffer, title2, 0, 0);

            //draw cursor on screen
            if (cursor == 1 || cursor == 0)
            {
                masked_blit(pointer, buffer, 0,0,350,320,1024,768);
            }
            if (cursor == 2)
            {
                masked_blit(pointer, buffer, 0,0,280,495,1024,768);
            }
            blit(buffer, screen, 0,0,0,0,1024,768);

            //key commands to choose path (game OR instructions)
            if (key[KEY_ENTER] && (cursor == 1))
            {
                current += 1;
            }
            if (key[KEY_ENTER] && cursor == 2)
            {
                current += 2;
            }
            if (key[KEY_ENTER] && (cursor == 0))
            {
                cursor = 1;
                readkey();
            }
            if (key[KEY_DOWN] && (cursor == 1 || cursor == 0))
            {
                cursor = 2;
            }
            if (key[KEY_UP] && cursor == 2)
            {
                cursor = 1;
            }
        }

        /*instructions page 1*/
        if (current == 4)
        {
            draw_sprite(buffer, instructions1, 0, 0);
            blit(buffer, screen, 0,0,0,0,1024,768);
            readkey();
            //key commands to continue to next page or return to title screen 2
            if (key[KEY_RIGHT])
            {
                current += 1;
            }
            if (key[KEY_LEFT])
            {
                current = 2;
            }
        }

        /*instructions page 2*/
        if (current == 5)
        {
            clear_bitmap(screen);
            draw_sprite(buffer, instructions2, 0, 0);
            blit(buffer, screen, 0,0,0,0,1024,768);
            readkey();
            //key commands to either go to next page or to previous page
            if (key[KEY_RIGHT])
            {
                current += 1;
            }
            if (key[KEY_LEFT])
            {
                current -= 1;
            }
        }

        /*instructions page 3*/
        if (current == 6)
        {
            clear_bitmap(screen);
            draw_sprite(buffer, instructions3, 0, 0);
            blit(buffer, screen, 0,0,0,0,1024,768);
            readkey();
            //key commands to go either to title screen or previous page
            if (key[KEY_RIGHT])
            {
                current = 2;
            }
            if (key[KEY_LEFT])
            {
                current -= 1;
            }
        }

        /*champion select (game preparation)*/
        if (current == 3)
        {
            draw_sprite(buffer, heroselect, 0, 0);

            //if either (or no) character is selected, draw respective icon in the player's selection box
            if (p1_champ == 0)      ///no character
            {
                masked_blit(p1, buffer, 0,0,420,440,1024,768);
            }
            if (p2_champ == 0)      ///no character
            {
                masked_blit(p2, buffer, 0,0,550,440,1024,768);
            }
            if (p1_champ == 1)      ///fighter
            {
                masked_blit(mm_icon, buffer, 0,0,60,60,1024,768);
                masked_blit(p1, buffer, 0,0,65,510,1024,768);
            }
            if (p1_champ == 2)      ///fighter
            {
                masked_blit(mage_icon, buffer, 0,0,60,60,1024,768);
                masked_blit(p1, buffer, 0,0,790,525,1024,768);
            }
            if (p2_champ == 1)      ///mage
            {
                masked_blit(mm_back_icon, buffer, 0,0,760,60,1024,768);
                masked_blit(p2, buffer, 0,0,220,510,1024,768);
            }
            if (p2_champ == 2)      ///mage
            {
                masked_blit(mage_back_icon, buffer, 0,0,760,60,1024,768);
                masked_blit(p2, buffer, 0,0,935,525,1024,768);
            }

            //allow users to continue to game only after both have selected a character
            if (p1_champ != 0 && p2_champ != 0)
            {
                masked_blit(enter, buffer, 0,0,285,450,1024,768);
                if (key[KEY_ENTER])
                {
                    //set variables for game, load map and character attributes
                    MapSelect(map);
                    Player1 = CharacterLoad(Player1, p1_champ);
                    Player2 = CharacterLoad(Player2, p2_champ);
 	                p1_max_hp = Player1.Health;
 	                p2_max_hp = Player2.Health;
 	                p1_health_display = p1_max_hp;
 	                p2_health_display = p2_max_hp;
                    player1Health_temp = p1_max_hp;
 	                player2Health_temp = p2_max_hp;
 	                p1ManaTemp = Player1.Mana;
 	                p2ManaTemp = Player2.Mana;
 	                player1Y_new = 0;
 	                player2Y_new = 0;
 	                player1X_new = 14;
 	                player2X_new = 870;
 	                p1Right = TRUE;
 	                p1Left = FALSE;
 	                p2Right = FALSE;
 	                p2Left = TRUE;

 	                //move to game play
                    current = 7;
                }
            }

            //character selection key commands
            if (key[KEY_A])
            {
                p1_champ = 1;
            }
            if (key[KEY_D])
            {
                p1_champ = 2;
            }
            if (key[KEY_LEFT])
            {
                p2_champ = 1;
            }
            if (key[KEY_RIGHT])
            {
                p2_champ = 2;
            }
            blit(buffer, screen, 0,0,0,0,1024,768);
        }

        /*game play*/
        if (current == 7)
        {
            //game plays when both players are alive
            if (Player1.Health > 0 && Player2.Health > 0)
            {
                while (fpsCounter > 0)
                {
                    //clear character buffers
                    clear_bitmap(p1_buf);
                    clear_bitmap(p2_buf);

                    //movement, sprite selection, collision checking, and damage calculation
                    player1Movement(Player1, p1_buf);
                    player2Movement(Player2, p2_buf);
                    p1_x_dist = Player1_SpriteSelect(Player1, p1_buf, p1_x_dist);
                    p1_y_dist = P1_get_height(Player1, p1_buf, p1_y_dist);
                    p2_x_dist = Player2_SpriteSelect(Player2, p2_buf, p2_x_dist);
                    p2_y_dist = P2_get_height(Player2, p2_buf, p2_y_dist);
                    CollisionCheck(p1_buf, p2_buf);
                    Player2.Health = p1_Damage_calc(Player1, Player2);
                    Player1.Health = p2_Damage_calc(Player1, Player2);

                    fpsCounter --;
                    framecounter ++;

                    //move framecounter back to 0 after 1 second passes
                    if (framecounter > 48)
                    {
                        framecounter = 0;
                    }
                }

                //round players' X and Y coordinates
                player1X_round = (int)round(player1X_new);
                player1Y_round = (int)round(player1Y_new);
                player2X_round = (int)round(player2X_new);
                player2Y_round = (int)round(player2Y_new);

                //reset action collisions
                p1PunchCollision = FALSE;
                p2PunchCollision = FALSE;
                p1KickCollision = FALSE;
                p2KickCollision = FALSE;
                p1SpecialCollision = FALSE;
                p2SpecialCollision = FALSE;

                //clear screen buffer & draw map to it
                clear_bitmap(buffer);
                blit(map, buffer, 0, 0, 0, 0, 1024, 768);

                //correct X-value discrepancies for left-facing actions
                if (p1Punch == TRUE && p1Left == TRUE && p1Punch_length <= 12)
                {
                    if (strcmp(Player1.Name, "Mage") == 0 && p1PunchCount == 2) player1X_round -= 300;
                    if (strcmp(Player1.Name, "Mage") == 0 && p1PunchCount != 2) player1X_round -= 90;
                    else player1X_round -= 40;
                }
                if (p1Kick == TRUE && p1Left == TRUE && p1GroundCollision == TRUE && p1_jumpaction == 0)
                {
                    if (strcmp(Player1.Name, "Mage") == 0 && p1Kick_length <= 12) player1X_round -= 140;
                    if (strcmp(Player1.Name, "Fighter") == 0) player1X_round -= 40;
                }
                if (p1Kick == TRUE && p1Left == TRUE && p1GroundCollision == FALSE) player1X_round -= 40;
                if (p1Special == TRUE && p1Left == TRUE && strcmp(Player1.Name, "Fighter") == 0) player1X_round -= 40;

                if (p2Punch == TRUE && p2Left == TRUE && p2Punch_length <= 12)
                {
                    if (strcmp(Player2.Name, "Mage") == 0 && p2PunchCount == 2) player2X_round -= 300;
                    if (strcmp(Player2.Name, "Mage") == 0 && p2PunchCount != 2) player2X_round -= 90;
                    else player2X_round -= 40;
                }
                if (p2Kick == TRUE && p2Left == TRUE && p2GroundCollision == TRUE && p2_jumpaction == 0)
                {
                    if (strcmp(Player2.Name, "Mage") == 0 && p2Kick_length <= 12) player2X_round -= 140;
                    if (strcmp(Player2.Name, "Fighter") == 0) player2X_round -= 40;
                }
                if (p2Kick == TRUE && p2Left == TRUE && p2GroundCollision == FALSE) player2X_round -= 40;
                if (p2Special == TRUE && p2Left == TRUE && strcmp(Player2.Name, "Fighter") == 0) player2X_round -= 40;

                //draw player buffers to screen buffer
                masked_blit(p1_buf, buffer, 0,0, player1X_round, player1Y_round, 1024, 768);
                masked_blit(p2_buf, buffer, 0,0, player2X_round -(505 - p2_x_dist), player2Y_round, 1024, 768);

                //draw interface (display, health & mana bars, markers);
                masked_blit(p1_marker, buffer, 0, 0, player1X_round + (p1_x_dist/2 - 20), player1Y_round - 25, 1024, 768);
                masked_blit(p2_marker, buffer, 0, 0, player2X_round + (p2_x_dist/2 - 20), player2Y_round - 25, 1024, 768);
                masked_blit(p1_display, buffer, 0, 0, 12, 12, 1024, 768);
                masked_blit(p2_display, buffer, 0, 0, 524, 12, 1024, 768);
                p1Temp[0] = health_interface(Player1.Health, p1_max_hp);
                p1Temp[1] = mana_interface(p1ManaTemp, Player1.Mana);
                p2Temp[0] = health_interface(Player2.Health, p2_max_hp);
                p2Temp[1] = mana_interface(p2ManaTemp, Player2.Mana);
                for (i = 0; i < p1Temp[0]; i++) masked_blit(hp, buffer, 0, 0, 125+(i*5), 55, 1024, 768);
                for (i = 0; i < p1Temp[1]; i++) masked_blit(mana, buffer, 0, 0, 125+(i*5), 107, 1024, 768);
                for (i = 0; i < p2Temp[0]; i++) masked_blit(hp, buffer, 0, 0, 637+(i*5), 55, 1024, 768);
                for (i = 0; i < p2Temp[1]; i++) masked_blit(mana, buffer, 0, 0, 637+(i*5), 107, 1024, 768);

                //draw buffer to screen
                blit(buffer, screen, 0, 0, 0, 0, 1024, 768);

                //update old X values
                player1X_old = player1X_new;
                player2X_old = player2X_new;
                player1Y_old = player1Y_new;
                player2Y_old = player2Y_new;
            }

            //choose end screen once a player wins
            score = end_sprite_select(end_screen, Player1.Health, Player2.Health);

            // if enter key is pressed while game over, restore values and return to champ select screen
            if (key[KEY_ENTER] && score == 1)
            {
                current = 3;
                p1_champ = 0;
                p2_champ = 0;
            }
        }
    }

    /*stop playing of game music, destroy pointer*/
    stop_sample(music);
    destroy_sample(music);

    /*destroy bitmap pointers*/
    destroy_bitmap(title1);
    destroy_bitmap(title2);
    destroy_bitmap(instructions1);
    destroy_bitmap(instructions2);
    destroy_bitmap(instructions3);
    destroy_bitmap(pointer);
    destroy_bitmap(heroselect);
    destroy_bitmap(p1);
    destroy_bitmap(p2);
    destroy_bitmap(buffer);
    destroy_bitmap(mage_icon);
    destroy_bitmap(mage_back_icon);
    destroy_bitmap(mm_icon);
    destroy_bitmap(mm_back_icon);
    destroy_bitmap(enter);
   	destroy_bitmap(map);
 	destroy_bitmap(p1_buf);
 	destroy_bitmap(p2_buf);
 	destroy_bitmap(end_screen);
 	destroy_bitmap(hp);
 	destroy_bitmap(mana);
 	destroy_bitmap(p1_display);
 	destroy_bitmap(p2_display);
 	destroy_bitmap(p1_marker);
 	destroy_bitmap(p2_marker);

 	/*unload datafiles*/
 	unload_datafile(Player2.Sprites);
 	unload_datafile(Player1.Sprites);

    /*remove sound module, exit game*/
 	remove_sound();
    return(0);
}
END_OF_MAIN()

/**function to select map**/
void MapSelect(BITMAP *map)
{
	/*declare and initialize local variables*/
    int i = 0;
	FILE *finput = NULL;
	char strTemp[25][30]={"0"};
	BITMAP* map1 = load_bitmap("Map1.bmp", NULL);
	BITMAP* map2 = load_bitmap("Map2.bmp", NULL);

	/*seed random number generator*/
	srand(time(NULL));

	/*randomly select map to be used*/
	mapSelect = rand()%2 + 1;

    /*open map files*/
	switch(mapSelect)
	{
		case 1:     //Map 1 - Bunker
     		blit(map1, map, 0, 0, 0, 0, 1024, 768);
			finput = fopen("Map1.txt", "r");
			break;

		case 2:     //Map 2 - Dungeon
			blit(map2, map, 0, 0, 0, 0, 1024, 768);
			finput = fopen("Map2.txt", "r");
			break;

		default:    //No map - game error
			set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
			allegro_message("**ERROR: MAP NOT FOUND**");
			exit(EXIT_FAILURE);
	}

                   //No map file - game error
	if (!finput)
	{
         set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
         allegro_message("**ERROR: GAME FILES NOT FOUND**");
         exit(EXIT_FAILURE);
    }

    /*read map information from file to array of strings*/
	while (i < 25)
	{
		fgets(strTemp[i], 29, finput);

        //break loop if end of file is reached
		if (feof(finput))
		{
			break;
		}
		i++;
	}

    /*convert textfile strings into integers & floats (platform and floor positions)*/
	Floor = atoi(strTemp[0]);
	Platform1[0] = atof(strTemp[1]);        //Platform arrays:
	Platform1[1] = atof(strTemp[2]);            ///0: Platform X-start
	Platform1[2] = atof(strTemp[3]);            ///1: Platform X-end
	Platform1[3] = atof(strTemp[4]);            ///2: Platform function coefficient (linear function)
	Platform2[0] = atof(strTemp[5]);            ///3: Platform function constant (linear function)
	Platform2[1] = atof(strTemp[6]);
	Platform2[2] = atof(strTemp[7]);
	Platform2[3] = atof(strTemp[8]);
	Platform3[0] = atof(strTemp[9]);
	Platform3[1] = atof(strTemp[10]);
	Platform3[2] = atof(strTemp[11]);
	Platform3[3] = atof(strTemp[12]);
	Platform4[0] = atof(strTemp[13]);
	Platform4[1] = atof(strTemp[14]);
	Platform4[2] = atof(strTemp[15]);
	Platform4[3] = atof(strTemp[16]);
	Platform5[0] = atof(strTemp[17]);
	Platform5[1] = atof(strTemp[18]);
	Platform5[2] = atof(strTemp[19]);
	Platform5[3] = atof(strTemp[20]);
	Platform6[0] = atof(strTemp[21]);
	Platform6[1] = atof(strTemp[22]);
	Platform6[2] = atof(strTemp[23]);
	Platform6[3] = atof(strTemp[24]);

    /*close file, destroy temporary bitmaps*/
	fclose(finput);
	destroy_bitmap(map1);
	destroy_bitmap(map2);
}END_OF_FUNCTION(MapSelect);

/**function to load character information and sprites to program**/
Character CharacterLoad(struct Character P1, int champ)
{
	/*declare local variables*/
	FILE *fp1;
	char temp1[7][30];
	char *newline;
	int i = 0, j = 0;

    /*load character information according to selection*/
    if (champ == 1)
    {
        fp1 = fopen("char1.txt", "r");
        P1.Sprites = load_datafile("fighter.dat");
    }

	else if (champ == 2)
    {
        fp1 = fopen("char2.txt", "r");
        P1.Sprites = load_datafile("mage.dat");
    }

	if (!fp1)   //no file found - game error
    {
        set_gfx_mode(GFX_TEXT,0,0,0,0);
        allegro_message("**ERROR: CHARACTER FILES NOT FOUND**");
        exit(EXIT_FAILURE);
    }

    /*read in values from file, store each line in a string*/
	while (i < 7)
    {
        fgets(temp1[i], 29, fp1);

        //break loop if end of file reached
        if (feof(fp1))
        {
            break;
        }

        //remove newline character from string if it exists
        newline = strrchr(temp1[i], '\n');
        if (newline)
        {
            *newline = '\0';
        }

        newline = NULL;

        i++;
    }

    /*copy values from temp storage to character structure, converting to corresponding data type*/
    strcpy(P1.Name, temp1[0]);
    P1.Speed = atof(temp1[1]);
    P1.Damage = atof(temp1[2]);
    P1.Health = atoi(temp1[3]);
    P1.Defense = atof(temp1[4]);
    P1.Special = atof(temp1[5]);
    P1.Mana = atoi(temp1[6]);

    /*return structure*/
    return P1;
}END_OF_FUNCTION(CharacterLoad);

/**function for player 1 movement**/
void player1Movement(struct Character P1, BITMAP *player1)
{
	/*action cooldown & mana regeneration time reduction*/
	if (p1_punchTime > 0)   p1_punchTime --;

	if (p1Punch_delay > 0) p1Punch_delay --;

	if (P1ManaRegen > 0)    P1ManaRegen --;

	if (p1_blockTime > 0)   p1_blockTime --;

	if (p1_specialTime > 0) p1_specialTime --;

	if (p1_kickTime >0)     p1_kickTime--;

	if (p1Punch_delay == 0) p1PunchCount = 1;

	/*increase mana until it reaches maximum*/
	if (P1ManaRegen == 0)
	{
		if (p1ManaTemp < P1.Mana)
		{
			p1ManaTemp ++;
			P1ManaRegen = 48;       //1 second cooldown time per mana increase
		}
	}

	/*start or end action cooldowns*/
	if (p1Punch == TRUE)       //Punch
	{
		if (p1Punch_length > 0)                         ///decrease remaining action length
		{
			p1Punch_length --;
		}
		else
		{
            if (strcmp(P1.Name, "Fighter") == 0 && p1dmg == 1)       //increase punch combo if playing as fighter
            {
                p1PunchCount += 1;
                p1Punch_delay = 60;
            }
			p1Punch = COOLDOWN;                                                 //start cooldown
			if (strcmp(P1.Name, "Mage") == 0) p1_punchTime = 30;
			else if (strcmp(P1.Name, "Fighter") == 0) p1_punchTime = 10;
		}
	}

	else if (p1Punch == COOLDOWN && p1_punchTime == 0)  ///after cooldown finishes, action may occur again
	{
		 p1Punch = FALSE;
	}

	else if (p1Block == TRUE)  //Block
	{
         if (p1_blocklength > 0) p1_blocklength --;
         else
         {
		     p1Block = COOLDOWN;
		     p1_blockTime = 180;			//set cooldown time to 180 frames (3 seconds)
	     }
    }

	else if (p1Kick == TRUE)   //Kick
	{
		if (p1Kick_length > 0)
		{                              //proceed if still kicking
			p1Kick_length --;
		}

		else
		{
			p1Kick = COOLDOWN;
			p1_kickTime = 180;        //set cooldown time to 180 frames (3 seconds)
		}
	}

	else if (p1Special == TRUE)
	{
		if (p1Special_length > 0)
		{
			p1Special_length --;
			if (strcmp("Fighter", P1.Name) == 0 && p1Right == TRUE) player1X_new += 15;
			if (strcmp("Fighter", P1.Name) == 0 && p1Left == TRUE) player1X_new -= 15;
		}
		else
		{
		    if (strcmp("Mage", P1.Name) == 0 && p1Right == TRUE)
            {
                player1X_new += 300;
                //p1PunchCount = 2;     //
                p1Punch_delay = 250;
            }
			else if (strcmp("Mage", P1.Name) == 0 && p1Left == TRUE)
            {
                player1X_new -= 300;
                //p1PunchCount = 2;
                p1Punch_delay = 250;
            }
			p1Special = COOLDOWN;
	       	p1_specialTime = 480;		//set cooldown time to 480 frames (10 seconds)
		}
        //keep within stage area after special
		if (mapSelect == 1 && (p1_BB[3] > 567 - 250 + p1_y_dist) && player1X_new <= 192) player1X_new = 193;
		else if (player1X_new < 0) player1X_new = 0;
		if (mapSelect == 1 && (p1_BB[3] > 573 - 250 + p1_y_dist) && player1X_new + p1_x_dist >= 845) player1X_new = 844 - p1_x_dist;
		else if (player1X_new > 886) player1X_new = 886;
	}

	else if (p1Special == COOLDOWN && p1_specialTime == 0)
	{
		p1Special = FALSE;
	}

	else if (p1Kick == COOLDOWN && p1_kickTime == 0) p1Kick = FALSE;

	else if (p1Block == COOLDOWN && p1_blockTime == 0)
	{
		p1Block = FALSE;
	}

	//reduce stun time if player is stunned
	if (p1_stunTime > 0)
	{
		p1_stunTime --;
	}
	//otherwise allow for movement
	else if (p1_stunTime <= 0)
	{
		if (p1Stun == TRUE)
		{
            p1Stun = FALSE;
        }

        //effecting of gravity if player is in air
		if (p1GroundCollision == FALSE)
		{
			player1_Vy += (aY * p1JumpTime);

			player1Y_new = player1Y_old + (player1_Vy * p1JumpTime) + (0.5 * aY * pow(p1JumpTime, 2));
			p1JumpTime += 0.0167777;
		}

		//reset gravity algorithm if player is on ground
		else if (p1GroundCollision == TRUE)
		{
			p1JumpTime = 0;
			player1_Vy = 0;
		}

		//move left within screen
		if (key[KEY_A])
		{
			if (p1Left != TRUE)
			{
				p1Left = TRUE;		//change character to face left
				p1Right = FALSE;
			}
			if (player1X_old > 0)
			{
				player1X_new = player1X_old - (P1.Speed * aX);
			}
		}

		//move right within screen
		if (key[KEY_D])
		{
			if (p1Right != TRUE)
			{
				p1Right = TRUE;	//change character to face right
				p1Left = FALSE;
			}
			if (player1X_old < (SCREEN_W - p1_x_dist))
			{
				player1X_new = player1X_old + (P1.Speed * aX);
			}
		}

		//initiate jump (gravity) if player is on ground
		if (key[KEY_W])
		{
			if (p1GroundCollision == TRUE)
			{
				player1_Vy = -70;
				p1JumpTime = 1/60;
				player1Y_new = player1Y_old + player1_Vy;
			}
		}

		//block if it has not just occurred and cool-down time has passed
		if (key[KEY_S])
		{
			if (p1Block != TRUE && p1_blockTime == 0 && (p1ManaTemp - 5) >= 0)
			{
                p1ManaTemp -= 5;
				p1Block = TRUE;
				p1_blocklength = 35;
			}
		}

		//punch command if there is enough mana and cooldown time has passed
		if (key[KEY_J])
		{
			if (p1Punch == FALSE && (p1ManaTemp - (mana_reduce/10)) >= 0)
			{
				p1Punch = TRUE;
				p1ManaTemp -= 2;
				if (strcmp(P1.Name, "Fighter") == 0)
				{
					p1Punch_length = 12;
				}

				else
				{
					p1Punch_length = 24;
				}
				if (p1GroundCollision == FALSE) p1_jumpaction = 1;
			    if (p1GroundCollision == TRUE) p1_jumpaction = 0;
			}
		}

		//kick command if there is enough mana and cooldown time has passed
		else if (key[KEY_K])
		{
			if (p1Kick == FALSE && ((p1ManaTemp - mana_reduce/5)) >= 0)
			{
                p1ManaTemp -= 4;
				p1Kick = TRUE;
				p1Kick_length = 24;
				if (p1GroundCollision == FALSE) p1_jumpaction = 1;
			    if (p1GroundCollision == TRUE) p1_jumpaction = 0;
			}
		}

		//special command if there is enough mana and cooldown time has passed
		else if (key[KEY_L])
		{
			if (p1Special != TRUE && p1_specialTime == 0 && (p1ManaTemp - 15) >= 0)
			{
                p1ManaTemp -= 15;
				p1Special = TRUE;
				if (strcmp(P1.Name, "Fighter") == 0)
				{
					p1Special_length = 12;
				}
				else
				{
					p1Special_length = 36;
                }
            }
		}
	}
}END_OF_FUNCTION(player1Movement);

//function for player 2 movement
void player2Movement(struct Character P2, BITMAP *player2)
{
	//reduce cooldown time for block and special commands
	if (p2_punchTime > 0) p2_punchTime --;

	if (p2Punch_delay > 0) p2Punch_delay --;

	if (P2ManaRegen > 0) P2ManaRegen --;

	if (p2_blockTime > 0)
	{
		p2_blockTime --;
	}

	if (p2_specialTime > 0)
	{
		p2_specialTime --;
	}

	if (p2_kickTime > 0) p2_kickTime --;

	if (p2Punch_delay == 0) p2PunchCount = 1;

	//increase mana every 5 seconds until it reaches maximum
	if (P2ManaRegen == 0)
	{
		if (p2ManaTemp < P2.Mana)
		{
			p2ManaTemp ++;
			P2ManaRegen = 40;
		}
	}

    //start or end action cooldowns
	if (p2Punch == TRUE)
	{
		if (p2Punch_length > 0)
		{
			p2Punch_length --;
		}

		else
		{
            if (strcmp(P2.Name, "Fighter") == 0 && p2dmg == 1)          //increase punch combo if playing as fighter
            {
                p2PunchCount += 1;
                p2Punch_delay = 60;
            }
			p2Punch = COOLDOWN;
			if (strcmp(P2.Name, "Mage") == 0) p2_punchTime = 30;               ///start cooldown
			else if (strcmp(P2.Name, "Fighter") == 0) p2_punchTime = 10;
        }
	}

	else if (p2Punch == COOLDOWN && p2_punchTime == 0)
	{
		p2Punch = FALSE;
	}

	else if (p2Kick == TRUE)
	{
		if (p2Kick_length > 0)
		{
			p2Kick_length --;
		}

		else
		{
			p2Kick = COOLDOWN;
			p2_kickTime = 180;
		}
	}

	else if (p2Kick == COOLDOWN && p2_kickTime == 0)
	     p2Kick = FALSE;

	else if (p2Special == TRUE)
	{
		if (p2Special_length > 0)
		{
			p2Special_length --;
			if (strcmp("Fighter", P2.Name) == 0 && p2Right == TRUE) player2X_new += 15;
			if (strcmp("Fighter", P2.Name) == 0 && p2Left == TRUE) player2X_new -= 15;
		}
		else
		{
			p2Special = COOLDOWN;
			p2_specialTime = 480;		//set cooldown time to 480 frames (10 seconds)
            if (strcmp("Mage", P2.Name) == 0 && p2Right == TRUE)
            {
                player2X_new += 300;
                //p2PunchCount = 2;
                p2Punch_delay = 250;
            }
			else if (strcmp("Mage", P2.Name) == 0 && p2Left == TRUE)
            {
                player2X_new -= 300;
                //p2PunchCount = 2;
                p2Punch_delay = 250;
            }
		}
		//keep within stage area after special
		if (mapSelect == 1 && (p2_BB[3] > 567 - 250 + p2_y_dist) && player2X_new <= 192) player2X_new = 193;
		else if (player2X_new < 0) player2X_new = 0;
		if (mapSelect == 1 && (p2_BB[3] > 573 - 250 + p2_y_dist) && player2X_new + p2_x_dist >= 845) player2X_new = 844 - p2_x_dist;
		else if (player2X_new > 886) player2X_new = 886;
	}

	else if (p2Special == COOLDOWN && p2_specialTime == 0)
	{
		p2Special = FALSE;
	}

	else if (p2Block == TRUE)
	{
	    if (p2Block_length > 0)	p2Block_length --;
		else
        {
            p2Block = COOLDOWN;
            p2_blockTime = 180;			//set cooldown time to 180 frames (3 seconds)
        }
	}

	else if (p2Block == COOLDOWN && p2_blockTime == 0)
	{
		p2Block = FALSE;
	}

	//reduce stun time if player is stunned
	if (p2_stunTime > 0)
	{
		p2_stunTime --;
	}

	else if (p2_stunTime <= 0)
	{
         if (p2Stun == TRUE)
         {
             p2Stun = FALSE;
         }

		//effect gravity if player is in air
		if (p2GroundCollision == FALSE)
		{
			player2_Vy += (aY * p2JumpTime);
			player2Y_new = player2Y_old + (player2_Vy * p2JumpTime) + (0.5 * aY * pow(p2JumpTime, 2));
			p2JumpTime += 0.0167777;
		}

		//reset gravity algorithm if player is on ground
		else if (p2GroundCollision == TRUE)
		{
			p2JumpTime = 0;
			player2_Vy = 0;
		}

		//move left within screen
		if (key[KEY_LEFT])
		{
			if (p2Left != TRUE)
			{
				p2Left = TRUE;		//change character to face left
				p2Right = FALSE;
			}
			if (player2X_old > 0)
			{
				player2X_new = player2X_old - (P2.Speed * aX);
			}
		}

		//move right within screen
		if (key[KEY_RIGHT])
		{
			if (p2Right != TRUE)
			{
				p2Right = TRUE;	//change character to face left
				p2Left = FALSE;
			}
			if (player2X_old < (SCREEN_W - p2_x_dist))
			{
				player2X_new = player2X_old + (P2.Speed * aX);
			}
		}

		//initiate jump if player is on ground
		if (key[KEY_UP])
		{
			if (p2GroundCollision == TRUE)
			{
			    player2_Vy = -70;
				p2JumpTime = 1/60;
				player2Y_new = player2Y_old + player2_Vy;
			}
		}

		//block command if block has not been performed and cooldown is complete
		if (key[KEY_DOWN])
		{
			if (p2Block != TRUE && p2_blockTime == 0 && (p2ManaTemp - (mana_reduce/4)) >= 0)
			{
                p2ManaTemp -= 5;
				p2Block = TRUE;
				p2Block_length = 35;
			}
		}

		//punch command if cooldown is complete and there is enough mana
	 	if (key[KEY_1_PAD])
		{
			if (p2Punch == FALSE && (p2ManaTemp - (mana_reduce/10)) >= 0)
			{
				p2Punch = TRUE;
				p2ManaTemp -= 2;
				if (strcmp(P2.Name, "Fighter") == 0)
				{
					p2Punch_length = 12;
				}

				else
				{
					p2Punch_length = 24;
				}
				if (p2GroundCollision == FALSE) p2_jumpaction = 1;
			    if (p2GroundCollision == TRUE) p2_jumpaction = 0;
			}
		}

		//kick command if cooldown is complete and there is enough mana
		else if (key[KEY_2_PAD])
		{
			if (p2Kick == FALSE && (p2ManaTemp - (mana_reduce/5)) >= 0)
			{
                p2ManaTemp -= 4;
				p2Kick = TRUE;
				p2Kick_length = 24;
				if (p2GroundCollision == FALSE) p2_jumpaction = 1;
			    if (p2GroundCollision == TRUE) p2_jumpaction = 0;
			}
		}

		//special command if cooldown is complete and there is enough mana
		else if (key[KEY_3_PAD])
		{
			if (p2Special != TRUE && p2_specialTime == 0 && (p2ManaTemp - 15) >= 0)
			{
				p2Special = TRUE;
				p2ManaTemp -= 15;
				if (strcmp(P2.Name, "Fighter") == 0)
				{
					p2Special_length = 12;
				}

				else
				{
					p2Special_length = 36;
					p2PunchCount = 0;
				}
			}
		}
	}
}END_OF_FUNCTION(player2Movement);

//function to check for various collisions
void CollisionCheck(BITMAP *player1, BITMAP *player2)
{
	//declare and initialize local variables
	int CollBorder_left = 0;
	int CollBorder_right = 0;
	int CollBorder_top = 0;
	int CollBorder_bottom = 0;
	int p1_platformstatus = 0;
	int p2_platformstatus = 0;

	double P1_platformY = 0;
	int P1_platformY_round = 0;

	double P2_platformY = 0;
	int P2_platformY_round = 0;

    //reset damage calculations
	p1dmg = 0;
	p2dmg = 0;

	//update bounding boxes
	player1X_round = (int)round(player1X_new);
	player1Y_round = (int)round(player1Y_new);
	player2X_round = (int)round(player2X_new);
	player2Y_round = (int)round(player2Y_new);

    //determine previous Y coordinate
	int p1Bottom_old = p1_BB[3];
	int p2Bottom_old = p2_BB[3];

    //find player 1 bounding boxes if facing right
	if (p1Right == TRUE)
	{
        p1_BB[0] = player1X_round;
	    p1_BB[1] = player1Y_round;
	    p1_BB[2] = p1_BB[0] + p1_x_dist;
	    p1_BB[3] = p1_BB[1] + p1_y_dist;
    }

    //find player 1 bounding boxes if facing left
    else if (p1Left == TRUE)
    {
        p1_BB[0] = player1X_round - (p1_x_dist - 138);
        p1_BB[1] = player1Y_round;
        p1_BB[2] = p1_BB[0] + p1_x_dist;
        p1_BB[3] = p1_BB[1] + p1_y_dist;
    }

    //find player 2 bounding boxes if facing right
    if (p2Right == TRUE)
	{
        p2_BB[0] = player2X_round;
        p2_BB[1] = player2Y_round;
        p2_BB[2] = p2_BB[0] + p2_x_dist;
        p2_BB[3] = p2_BB[1] + p2_y_dist;
    }

    //find player 2 bounding boxes if facing left
    else if (p2Left == TRUE)
    {
        p2_BB[0] = player2X_round - (p2_x_dist - 138);
        p2_BB[1] = player2Y_round;
        p2_BB[2] = p2_BB[0] + p2_x_dist;
        p2_BB[3] = p2_BB[1] + p2_y_dist;
    }

    //find midpoint of each player (X-coordinate)
    p1_BBXhalf = (p1_BB[0]+p1_BB[2])/2;
    p2_BBXhalf = (p2_BB[0]+p2_BB[2])/2;

	//reset collisions
	p1GroundCollision = FALSE;
	p2GroundCollision = FALSE;

    /*check Player1 for ground collisions (floor & platforms)*/
    //maintain player 1 collision with platform 1
    if (p1_BBXhalf >= Platform1[0] && p1_BBXhalf <=Platform1[1])
	{
		P1_platformY = PlatformFunc(Platform1[2], player1X_new, Platform1[3]);
		P1_platformY_round  = (int)round(P1_platformY);
		if ((p1_BB[3] >= P1_platformY_round - 250 + p1_y_dist) && (p1Bottom_old <= P1_platformY_round - 250 + p1_y_dist))
		{
			p1GroundCollision = TRUE;
			player1Y_new = (P1_platformY_round - 250);
        }
    }
    //maintain player 1 collision with platform 2
	else if (player1X_new >= Platform2[0] && player1X_new <= Platform2[1])
	{
		P1_platformY = PlatformFunc(Platform2[2], player1X_new, Platform2[3]);
		P1_platformY_round  = (int)round(P1_platformY);
		if ((p1_BB[3] >= P1_platformY_round - 250 + p1_y_dist) && (p1Bottom_old <= P1_platformY_round - 250 + p1_y_dist))
		{
			p1GroundCollision = TRUE;
			player1Y_new = (P1_platformY_round - 250);
		}
		if (mapSelect == 1 && (p1_BB[3] > 567 - 250 + p1_y_dist) && player1X_new <= 192) player1X_new = 193;
    }
    //maintain player 1 collision with platform 3
	else if (player1X_new + p1_x_dist >= Platform3[0] && player1X_new + p1_x_dist <= Platform3[1] && player1Y_new > 576 - p1_y_dist)
	{
		P1_platformY = PlatformFunc(Platform3[2], player1X_new, Platform3[3]);
		P1_platformY_round  = (int)round(P1_platformY);
		if ((p1_BB[3] >= P1_platformY_round - 250 + p1_y_dist) && (p1Bottom_old <= P1_platformY_round - 250 + p1_y_dist))
		{
			p1GroundCollision = TRUE;
			player1Y_new = (P1_platformY_round - 250);
		}
		if (mapSelect == 1 && (p1_BB[3] > 573 - 250 + p1_y_dist) && player1X_new + p1_x_dist >= 845) player1X_new = 844 - p1_x_dist;
	}
    //maintain player 1 collision with platform 4
	else if (p1_BBXhalf >= Platform4[0] && player1X_new + p1_x_dist <= Platform4[1] && player1Y_new <= 576 - p1_y_dist && player1Y_new > 346 - p1_y_dist)
	{
		P1_platformY = PlatformFunc(Platform4[2], player1X_new, Platform4[3]);
		P1_platformY_round  = (int)round(P1_platformY);
		if ((p1_BB[3] >= P1_platformY_round - 250 + p1_y_dist) && (p1Bottom_old <= P1_platformY_round - 250 + p1_y_dist))
		{
			p1GroundCollision = TRUE;
			player1Y_new = (P1_platformY_round - 250);
		}
	}
    //maintain player 1 collision with platform 5
	else if (p1_BBXhalf >= Platform5[0] && p1_BBXhalf <= Platform5[1] && player1Y_new <= 494 - p1_y_dist)
	{
		P1_platformY = PlatformFunc(Platform5[2], player1X_new, Platform5[3]);
		P1_platformY_round  = (int)round(P1_platformY);
		if ((p1_BB[3] >= P1_platformY_round - 250 + p1_y_dist)&& (p1Bottom_old <= P1_platformY_round - 250 + p1_y_dist))
		{
			p1GroundCollision = TRUE;
			player1Y_new = (P1_platformY_round - 250);
		}
	}
    //maintain player 1 collision with platform 6
	else if (p1_BBXhalf >= Platform6[0] && p1_BBXhalf <= Platform6[1] && player1Y_new <= 346 - p1_y_dist)
	{
		P1_platformY = PlatformFunc(Platform6[2], player1X_new, Platform6[3]);
		P1_platformY_round  = (int)round(P1_platformY);
		if ((p1_BB[3] >= P1_platformY_round - 250 + p1_y_dist) && (p1Bottom_old <= P1_platformY_round - 250 + p1_y_dist))
		{
			p1GroundCollision = TRUE;
			player1Y_new = (P1_platformY_round - 250);
		}
    }
    //maintain player 1 floor collision
 	if ((p1_BB[3] >= Floor - 250 + p1_y_dist) && p1_platformstatus == 0)
	{
		p1GroundCollision = TRUE;
		player1Y_new = (Floor - 250);
	}

	/*check Player2 for ground collisions (floor & platforms)*/
	//maintain player 2 collision with platform 1
	if (p2_BBXhalf >= Platform1[0] && p2_BBXhalf <=Platform1[1])
	{
		P2_platformY = PlatformFunc(Platform1[2], player2X_new, Platform1[3]);
		P2_platformY_round  = (int)round(P2_platformY);
		if ((p2_BB[3] >= P2_platformY_round - 250 + p2_y_dist) && (p2Bottom_old <= P2_platformY_round - 250 + p2_y_dist))
		{
			p2GroundCollision = TRUE;
			player2Y_new = (P2_platformY_round - 250);
        }
    }
    //maintain player 2 collision with platform 2
	else if (player2X_new >= Platform2[0] && player2X_new <= Platform2[1])
	{
		P2_platformY = PlatformFunc(Platform2[2], player2X_new, Platform2[3]);
		P2_platformY_round  = (int)round(P2_platformY);
		if ((p2_BB[3] >= P2_platformY_round - 250 + p2_y_dist) && (p2Bottom_old <= P2_platformY_round - 250 + p2_y_dist))
		{
			p2GroundCollision = TRUE;
			player2Y_new = (P2_platformY_round - 250);
		}
		if (mapSelect == 1 && (p2_BB[3] > 567 - 250 + p2_y_dist) && player2X_new <= 192) player2X_new = 193;
    }
    //maintain player 2 collision with platform 3
	else if (player2X_new + p2_x_dist >= Platform3[0] && player2X_new + p2_x_dist <= Platform3[1] && player2Y_new > 576 - p2_y_dist)
	{
		P2_platformY = PlatformFunc(Platform3[2], player2X_new, Platform3[3]);
		P2_platformY_round  = (int)round(P2_platformY);
		if ((p2_BB[3] >= P2_platformY_round - 250 + p2_y_dist) && (p2Bottom_old <= P2_platformY_round - 250 + p2_y_dist))
		{
			p2GroundCollision = TRUE;
			player2Y_new = (P2_platformY_round - 250);
		}
		if (mapSelect == 1 && (p2_BB[3] > 573 - 250 + p2_y_dist) && player2X_new + p2_x_dist >= 845) player2X_new = 844 - p2_x_dist;
	}
    //maintain player 2 collision with platform 4
	else if (p2_BBXhalf >= Platform4[0] && player2X_new + p2_x_dist <= Platform4[1] && player2Y_new <= 576 - p2_y_dist && player2Y_new > 346 - p2_y_dist)
	{
		P2_platformY = PlatformFunc(Platform4[2], player2X_new, Platform4[3]);
		P2_platformY_round  = (int)round(P2_platformY);
		if ((p2_BB[3] >= P2_platformY_round - 250 + p2_y_dist) && (p2Bottom_old <= P2_platformY_round - 250 + p2_y_dist))
		{
			p2GroundCollision = TRUE;
			player2Y_new = (P2_platformY_round - 250);
		}
	}
    //maintain player 2 collision with platform 5
	else if (p2_BBXhalf >= Platform5[0] && p2_BBXhalf <= Platform5[1] && player2Y_new <= 494 - p2_y_dist)
	{
		P2_platformY = PlatformFunc(Platform5[2], player2X_new, Platform5[3]);
		P2_platformY_round  = (int)round(P2_platformY);
		if ((p2_BB[3] >= P2_platformY_round - 250 + p2_y_dist)&& (p2Bottom_old <= P2_platformY_round - 250 + p2_y_dist))
		{
			p2GroundCollision = TRUE;
			player2Y_new = (P2_platformY_round - 250);
		}
	}
    //maintain player 2 collision with platform 6
    else if (p2_BBXhalf >= Platform6[0] && p2_BBXhalf <= Platform6[1] && player2Y_new <= 346 - p2_y_dist)
    {
        P2_platformY = PlatformFunc(Platform6[2], player2X_new, Platform6[3]);
        P2_platformY_round  = (int)round(P2_platformY);
        if ((p2_BB[3] >= P2_platformY_round - 250 + p2_y_dist) && (p2Bottom_old <= P2_platformY_round - 250 + p2_y_dist))
        {
			p2GroundCollision = TRUE;
			player2Y_new = (P2_platformY_round - 250);
        }
    }
    //maintain player 2 floor collision
 	if ((p2_BB[3] >= Floor - 250 + p2_y_dist) && p2_platformstatus == 0)
	{
		p2GroundCollision = TRUE;
		player2Y_new = (Floor - 250);
	}

	/*check for action collisions (punch, kick, special, block)*/
	//fine tune bounding boxes for mage special
	if (p1Special == TRUE && p1Special_length < 12 && p1Left == TRUE && p1_champ == 2)  p1_BB[2] += 41;
    if (p2Special == TRUE && p2Special_length < 12 && p2Left == TRUE && p2_champ == 2)  p2_BB[2] += 41;
    if (p1Special == TRUE && p1Special_length < 12 && p1Right == TRUE && p1_champ == 2)  p1_BB[0] -= 100;
    if (p2Special == TRUE && p2Special_length < 12 && p2Right == TRUE && p2_champ == 2)  p2_BB[0] -= 100;

    //case one
	if (p2_BB[0] > p1_BB[0] && p2_BB[0] < p1_BB[2])
	{
        //assign boundaries for box created by bounding box overlap
	    CollBorder_left = p2_BB[0];
	    CollBorder_right = p1_BB[2];
	    CollBorder_bottom = MAX(p1_BB[1], p2_BB[1]);
	    CollBorder_top = MIN(p1_BB[3], p2_BB[3]);
	    if (p1Right == TRUE||(p1Left == TRUE && p1Special==TRUE)) p1dmg = 1;      //mark collision
	    if (p2Left == TRUE) p2dmg = 1;

    }

    //case two
    else if ((p1_BB[0] > p2_BB[0] && p1_BB[0] < p2_BB[2]))
    {
        //assign boundaries for box created by bounding box overlap
        CollBorder_left = p1_BB[0];
        CollBorder_right = p2_BB[2];
        CollBorder_bottom = MAX(p1_BB[1], p2_BB[1]);
        CollBorder_top = MIN(p1_BB[3], p2_BB[3]);
        if (p1Left == TRUE) p1dmg = 1;                                            //mark collision
        if (p2Right == TRUE||(p2Left == TRUE && p2Special==TRUE)) p2dmg = 1;
    }

    //go through collisions and assign damage taken or stun time for player 1 actions
	if (p1dmg == 1)
	{
	    if (p1Punch == TRUE)
	    {
            if (((p1_BB[3]-85)<p2_BB[3]) && ((p1_BB[3]-85)>p2_BB[1]) && p1Punch_length <= 12 && p1GroundCollision == FALSE) //if punching right
            {
                //revert action collision if other player blocks at the same time and stun attacker
                if (p2Block == TRUE)
	            {
	                p1PunchCollision = REVERT;
					p1Stun = TRUE;
					p1_stunTime = 10 * stunTime;
	            }
	            //if not, collision occurs normally
	            else if (p1_stunTime < 40)
	            {
					p1PunchCollision = TRUE;
					p2Stun = TRUE;
					p2_stunTime = stunTime;
	            }
            }

	        else if (((p1_BB[3]-100)<p2_BB[3]) && ((p1_BB[3]-100)>p2_BB[1]) && p1Punch_length <= 12 && p1GroundCollision == TRUE) //if punching left
            {
                //revert action collision if other player blocks at the same time and stun attacker
	            if (p2Block == TRUE)
	            {
					p1PunchCollision = REVERT;
					p1Stun = TRUE;
					p1_stunTime = 10 * stunTime;
	            }

                //if not, collision occurs normally
                else if (p1_stunTime < 40)
                {
                    p1PunchCollision = TRUE;
					p2Stun = TRUE;
                    p2_stunTime = stunTime;
                }
            }
        }
        else if (p1Kick == TRUE)
        {
            if ((p1_BB[3]<p2_BB[3]) && (p1_BB[3]>p2_BB[1]) && p1GroundCollision == FALSE)
            {
                //if player one is kicking midair
                if ((p1Right == TRUE && p1_BB[2] > p2_BB[0] + 5 && p2Left == TRUE)||(p1Left == TRUE && p1_BB[0] < p2_BB[2] - 5 && p2Right == TRUE)||(p1Right == TRUE && p2Right == TRUE && p1_BB[2] > p2_BB[0] + 25)||(p1Left == TRUE && p2Left == TRUE && p1_BB[0] < p2_BB[2] - 75))
                {
                    //revert action collision if other player blocks at the same time and stun attacker
					if (p2Block == TRUE)
					{
					    p1KickCollision = REVERT;
					    p1Stun = TRUE;
					    p1_stunTime = 10 * stunTime;
					}
                    //if not, collision occurs normally
                    else if (p1_stunTime < 40)
                    {
                        p1KickCollision = TRUE;
                        p2Stun = TRUE;
                        p2_stunTime = stunTime;
                    }
                }
            }
            if (p1GroundCollision == TRUE && (p1_BB[3] < player2Y_new + 250)) //if kicking from the ground
            {
                //assign damage and stun to player 2
                if (p1_stunTime < 40)
                {
                    p1KickCollision = TRUE;
                    p2Stun = TRUE;
                    p2_stunTime = stunTime;
                }
            }
        }

        else if (p1Special == TRUE && p1Special_length < 12) //if using special
        {
             //revert action collision if other player blocks at the same time and stun attacker
            if (p2Block == TRUE)
            {
                p1SpecialCollision = REVERT;
                p1Stun = TRUE;
                p1_stunTime = 10 * stunTime;
            }
            //if not, collision occurs normally
            else if (p1_stunTime < 40)
            {
                p1SpecialCollision = TRUE;
                p2Stun = TRUE;
                p2_stunTime = 2 * stunTime;
            }
        }
    }

    //go through collisions and assign damage taken or stun time for player 2 actions
    if (p2dmg ==1)
    {
        if (p2Punch == TRUE)
        {
            if (((p2_BB[3]-85) < p1_BB[3]) && ((p2_BB[3]-85) > p1_BB[1]) && p2Punch_length <= 12 && p2GroundCollision == FALSE)   //if punching right
            {
                //revert action collision if other player blocks at the same time and stun attacker
                if (p1Block == TRUE)
                {
                    p2PunchCollision = REVERT;
                    p2Stun = TRUE;
                    p2_stunTime = 10 * stunTime;
                }
                //if not, collision occurs normally
                else if (p2_stunTime < 40)
                {
                    p2PunchCollision = TRUE;
                    p1Stun = TRUE;
                    p1_stunTime = stunTime;
                }
            }
            else if (((p2_BB[3]-100)<p1_BB[3]) && ((p2_BB[3]-100)>p1_BB[1]) && p2Punch_length <= 12 && p2GroundCollision == TRUE)      // if punching left
            {
                 //revert action collision if other player blocks at the same time and stun attacker
                if (p1Block == TRUE)
                {
                    p2PunchCollision = REVERT;
                    p2Stun = TRUE;
                    p2_stunTime = 10 * stunTime;
                }
                //if not, collision occurs normally
                else if (p2_stunTime < 40)
                {
                    p2PunchCollision = TRUE;
                    p1Stun = TRUE;
                    p1_stunTime = stunTime;
                }
            }
        }

        else if (p2Kick == TRUE)
        {
            if ((p2_BB[3] >= p1_BB[1]) && (p2_BB[3] <= p1_BB[3]) && p2GroundCollision == FALSE)
            {
                //if kicking from midair
                if ((p2Right == TRUE && p2_BB[2] > p1_BB[0] + 5 && p1Left == TRUE)||(p2Left == TRUE && p2_BB[0] < p1_BB[2] - 5 && p1Right == TRUE)||(p2Right == TRUE && p1Right == TRUE && p2_BB[2] > p1_BB[0] + 25)||(p2Left == TRUE && p1Left == TRUE && p2_BB[0] < p1_BB[2] - 75))
                {
                    //revert action collision if other player blocks at the same time and stun attacker
                    if (p1Block == TRUE)
                    {
                        p2KickCollision = REVERT;
                        p2Stun = TRUE;
                        p2_stunTime = 10 * stunTime;
                    }
                    //if not, collision occurs normally
                    else if (p2_stunTime < 40)
                    {
                        p2KickCollision = TRUE;
                        p1Stun = TRUE;
                        p1_stunTime = stunTime;
                    }
                }
            }
            //if kicking from the ground
            else if (p2GroundCollision == TRUE && (p2_BB[3] < player1Y_new + 250))
            {
                //assign damage and stun time
                if (p2_stunTime < 40)
                {
                    p2KickCollision = TRUE;
                    p1Stun = TRUE;
                    p1_stunTime = stunTime;
                }
            }
        }

        else if (p2Special == TRUE && p2Special_length < 12)  //if casting specials
        {
            //revert action collision if other player blocks at the same time and stun attacker
            if (p1Block == TRUE)
            {
                p2SpecialCollision = REVERT;
                p2Stun = TRUE;
                p2_stunTime = 4 * stunTime;
            }
            //if not, collision occurs normally
            else if (p2_stunTime < 40)
            {
                p2SpecialCollision = TRUE;
                p1Stun = TRUE;
                p1_stunTime = 2 * stunTime;
            }
        }
    }

    //update X and Y coordinates
	player1X_round = (int)round(player1X_new);
	player1Y_round = (int)round(player1Y_new);
	player2X_round = (int)round(player2X_new);
	player2Y_round = (int)round(player2Y_new);
}END_OF_FUNCTION(CollisionCheck);

/*function to calculate damage done by player 1 to player 2(change in health)*/
int p1_Damage_calc(Character Player1, Character Player2)
{
	//declare local variables
	float P1_damage_dealt = 0;
	int health_round = 0;
	player2Health_temp = Player2.Health;
	p2_takedmg_cd--;

	//Calculate damage for player 1 hit scenarios
	if (p2_takedmg_cd <= 0)
	{
	if (p1PunchCollision != FALSE)
	{
		P1_damage_dealt = (punchMultiplier * Player1.Damage) / Player2.Defense;   //calculate punch damage

		if (p1PunchCollision == TRUE)		//if punch hits, reduce player 2 health
		{
            if (p1PunchCount % 4 == 0)
            {
                P1_damage_dealt *= 2;       //assign bonus damage and knockup if on fighter's fourth chained hit
                player2_Vy = -30;
				p2JumpTime = 1/60;
				player2Y_new = player2Y_old + player2_Vy;
            }
			player2Health_temp -= P1_damage_dealt;
		}
	}

	else if (p1KickCollision != FALSE)
	{
		P1_damage_dealt = (kickMultiplier * Player1.Damage) / Player2.Defense;   //calculate kick damage

		if (p1KickCollision == TRUE)	//if kick hits, reduce player 2 health
		{
			player2Health_temp -= P1_damage_dealt;
		}
	}

	else if (p1SpecialCollision != FALSE)
	{
		P1_damage_dealt = Player1.Special / (Player2.Defense / 2);    //calculate special damage

		if (p1SpecialCollision == TRUE)	//if special hits, reduce player 2 health
		{
			player2Health_temp -= P1_damage_dealt;
		}
	}
}
    //assign brief invulnerability after taking damage
    if (P1_damage_dealt > 0)
	{
        p2_takedmg_cd = 25;
    }

 	if (player2Health_temp < 0) player2Health_temp = 0;           //update health
 	health_round = (int)round(player2Health_temp);
	return(health_round);
}END_OF_FUNCTION(p1_Damage_calc);

/*function to calculate damage done by player 2 to player 1(change in health)*/
int p2_Damage_calc(Character Player1, Character Player2)
{
    //declare local variables
    float P2_damage_dealt = 0;
    int health_round = 0;
	player1Health_temp = Player1.Health;
	p1_takedmg_cd--;

if (p1_takedmg_cd <= 0)
{
	if (p2PunchCollision != FALSE)
	{
		P2_damage_dealt = (punchMultiplier * Player2.Damage) / Player1.Defense;           //calculate punch damage

		if (p2PunchCollision == TRUE)      //if punch hits, reduce player 1 health
		{
            if (p2PunchCount % 4 == 0)
            {
                P2_damage_dealt *= 2;      //apply bonus damage and knockup for fourth chained fighter punch
                player1_Vy = -30;
				p1JumpTime = 1/60;
				player1Y_new = player1Y_old + player1_Vy;
            }
			player1Health_temp -= P2_damage_dealt;
		}
	}

	else if (p2KickCollision != FALSE)
	{
		P2_damage_dealt = (punchMultiplier * Player2.Damage) / Player1.Defense;        //calculate kick damage

		if (p2KickCollision == TRUE)       //if kick hits, reduce player 1 health
		{
			player1Health_temp -= P2_damage_dealt;
		}
	}

	else if (p2SpecialCollision != FALSE)
	{
		P2_damage_dealt = Player2.Special / (Player1.Defense / 2);       //calculate special damage

		if (p2SpecialCollision == TRUE)   //if special hits, reduce player 1 health
		{
			player1Health_temp -= P2_damage_dealt;
		}
	}
	if (P2_damage_dealt > 0)      //assign brief invulnerability after taking damage
	{
        p1_takedmg_cd = 25;
    }
}
	if (player1Health_temp < 0) player1Health_temp = 0;          //update health
	health_round = (int)round(player1Health_temp);
	return(health_round);

}END_OF_FUNCTION(p2_Damage_calc);

int Player1_SpriteSelect(Character P1, BITMAP *temp, int dist)
{
    clear_to_color(temp, makecol(255, 0, 255));
    BITMAP *char_buf = create_bitmap(505,240);
    const char *name;
    int width, height;
    int i, dif;
    int current = 0;
    clear_to_color(char_buf, makecol(255, 0, 255));
	if (strcmp(P1.Name, "Fighter") == 0)  ///Fighter character
	{
		if (p1Punch == TRUE && p1Stun != TRUE)
		{
			if (p1Right == TRUE)
			{
				if (p1GroundCollision == TRUE)
				{
					switch (p1PunchCount % 4)
					{
						case 1:
							blit((BITMAP*)P1.Sprites[6].dat,char_buf, 0, 0, 0, 0, 1024, 768);      //Select Punch1_right bitmap
					            dif = datafileSelect(P1.Sprites, current, 6);                      //move pointer to sprite in datafile
                                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                                current = 6;
							break;

						case 2:
							blit((BITMAP*)P1.Sprites[8].dat,char_buf, 0, 0, 0, 0, 1024, 768);      //Select Punch2_right bitmap
					            dif = datafileSelect(P1.Sprites, current, 8);                      //move pointer to sprite in datafile
                                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                                current = 8;
							break;

						case 3:
							blit((BITMAP*)P1.Sprites[10].dat,char_buf, 0, 0, 0, 0, 1024, 768);     //Select Punch3_right bitmap
					            dif = datafileSelect(P1.Sprites, current, 10);                     //move pointer to sprite in datafile
                                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                                current = 10;
							break;

						case 0:
							if (p1Punch_length > 6)
							{
								blit((BITMAP*)P1.Sprites[12].dat,char_buf, 0, 0, 0, 0, 1024, 768); //select Combo1_right bitmap
					            dif = datafileSelect(P1.Sprites, current, 12);                     //move pointer to sprite in datafile
                                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                                current = 12;
							}
							else
							{
								blit((BITMAP*)P1.Sprites[14].dat,char_buf, 0, 0, 0, 0, 1024, 768); //select Combo2_right Bitmap
					            dif = datafileSelect(P1.Sprites, current, 14);                     //move pointer to sprite in datafile
                                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                                current = 14;
							}
							break;

						default:
							set_gfx_mode(GFX_TEXT,0,0,0,0);
							allegro_message("The program entered a bug and needs to close.");
							exit (EXIT_FAILURE);
					}
				}

				else
				{
					blit((BITMAP*)P1.Sprites[20].dat,char_buf, 0, 0, 0, 0, 1024, 768);             //select JumpPunch_right Bitmap
					            dif = datafileSelect(P1.Sprites, current, 20);                     //move pointer to sprite in datafile
                                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                                current = 20;
				}
			}

			else if (p1Left == TRUE)
			{
				if (p1GroundCollision == TRUE)
				{
					switch (p1PunchCount % 4)
					{
						case 1:
							blit((BITMAP*)P1.Sprites[7].dat,char_buf, 0, 0, 0, 0, 1024, 768);      //select Punch1_left bitmap
					            dif = datafileSelect(P1.Sprites, current, 7);                      //move pointer to sprite in datafile
                                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                                current = 7;
							break;

						case 2:
							blit((BITMAP*)P1.Sprites[9].dat,char_buf, 0, 0, 0, 0, 1024, 768);      //select punch2_left bitmap
					        dif = datafileSelect(P1.Sprites, current, 9);                          //move pointer to sprite in datafile
                            if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                            else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                            current = 9;
							break;

						case 3:
							blit((BITMAP*)P1.Sprites[11].dat,char_buf, 0, 0, 0, 0, 1024, 768);     //select Punch3_left bitmap
					        dif = datafileSelect(P1.Sprites, current, 11);                         //move pointer to sprite in datafile
                            if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                            else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                            current = 11;
							break;

						case 0:
							if (p1Punch_length > 6)
							{
								blit((BITMAP*)P1.Sprites[13].dat,char_buf, 0, 0, 0, 0, 1024, 768); //select Combo1_left bitmap
					            dif = datafileSelect(P1.Sprites, current, 13);                     //move pointer to sprite in datafile
                                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                                current = 13;
							}
							else
							{
								blit((BITMAP*)P1.Sprites[15].dat,char_buf, 0, 0, 0, 0, 1024, 768); //select Combo2_left bitmap
					            dif = datafileSelect(P1.Sprites, current, 15);                     //move pointer to sprite in datafile
                                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                                current = 15;

							}
							break;
						default:
							set_gfx_mode(GFX_TEXT,0,0,0,0);
							allegro_message("The program entered a bug and needs to close.");
							exit (EXIT_FAILURE);
					}
				}

				else
				{
					blit((BITMAP*)P1.Sprites[21].dat,char_buf, 0, 0, 0, 0, 1024, 768);       //select JumpPunch_left bitmap
					dif = datafileSelect(P1.Sprites, current, 21);                           //move pointer to sprite in datafile
                    if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                    else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                    current = 21;
				}
			}
		}

		if (p1Kick == TRUE)
		{
			if (p1GroundCollision == TRUE)
			{
				if (p1Right == TRUE)
				{
                            if (p1_jumpaction == 0)
                            {
					{
						blit((BITMAP*)P1.Sprites[16].dat,char_buf, 0, 0, 0, 0, 1024, 768);   //select Kick_right bitmap
						dif = datafileSelect(P1.Sprites, current, 18);                       //move pointer to sprite in datafile
                        if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                        else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                        current = 16;
					}
				}
				else if (p1_jumpaction == 1)
				{
				    blit((BITMAP*)P1.Sprites[0].dat,char_buf, 0, 0, 0, 0, 1024, 768);        //select idle_face_right bitmap
                    dif = datafileSelect(P1.Sprites, current, 0);                            //move pointer to sprite in datafile
                    if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                    else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                    current = 0;
                }
            }

				else if (p1Left == TRUE)
				{
                     if (p1_jumpaction == 0)
                     {

						blit((BITMAP*)P1.Sprites[17].dat,char_buf, 0, 0, 0, 0, 1024, 768);   //select Kick_left bitmap
						dif = datafileSelect(P1.Sprites, current, 17);                       //move pointer to sprite in datafile
                      if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                      else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                      current = 17;
				}
							else if (p1_jumpaction == 1)
			{
				blit((BITMAP*)P1.Sprites[1].dat,char_buf, 0, 0, 0, 0, 1024, 768);            //select idle_face_left bitmap
				p1Kick = FALSE;
				dif = datafileSelect(P1.Sprites, current, 1);                                //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 1;
			}
			}
        }

			else if (p1Right == TRUE)
			{
				blit((BITMAP*)P1.Sprites[22].dat,char_buf, 0, 0, 0, 0, 1024, 768);           //select jumpKick_right bitmap
				dif = datafileSelect(P1.Sprites, current, 22);                               //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 22;
			}
			else if (p1Left == TRUE)
						{
                        if (p1GroundCollision == FALSE)
                        {
				            blit((BITMAP*)P1.Sprites[23].dat,char_buf, 0, 0, 0, 0, 1024, 768);      //select jumpKick_left bitmap
				            dif = datafileSelect(P1.Sprites, current, 23);                          //move pointer to sprite in datafile
                            if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                            else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                            current = 23;
                     }
                     else if (p1GroundCollision == TRUE) //return to regular face_left
                     {
                          p1_kickTime = 180;
                          p1Kick = COOLDOWN;
                          p1Kick_length = 0;
                          blit((BITMAP*)P1.Sprites[1].dat,char_buf, 0, 0, 0, 0, 1024, 768);         //select idle_face_left bitmap
				          dif = datafileSelect(P1.Sprites, current, 1);                             //move pointer to sprite in datafile
                          if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                          else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                          current = 1;
                      }
			}
		}

		else if (p1Special == TRUE)
		{
			if (p1Right == TRUE)
			{
				blit((BITMAP*)P1.Sprites[18].dat,char_buf, 0, 0, 0, 0, 1024, 768); //select special_right bitmap
				dif = datafileSelect(P1.Sprites, current, 18);                     //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 18;
			}

			else if (p1Left == TRUE)
			{
				blit((BITMAP*)P1.Sprites[19].dat,char_buf, 0, 0, 0, 0, 1024, 768); //select Special_left bitmap
				dif = datafileSelect(P1.Sprites, current, 19);                     //move pointer to sprite in datafile
                        if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                        else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                        current = 19;
			}
		}

		else if (p1Block == TRUE)
		{
			if (p1Right == TRUE)
			{
                     if (p1_blocklength > 0 && key[KEY_S])     //while blocking
                     {
				        blit((BITMAP*)P1.Sprites[26].dat,char_buf, 0, 0, 0, 0, 1024, 768);        //select block_right bitmap
				        dif = datafileSelect(P1.Sprites, current, 26);                            //move pointer to sprite in datafile
                        if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                        else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                        current = 26;
                     }
                     else       //after blocking
                     {
                         p1Block = COOLDOWN;
                         p1_blocklength = 0;
                         p1_blockTime = 180;
                         blit((BITMAP*)P1.Sprites[0].dat,char_buf, 0, 0, 0, 0, 1024, 768);        //select idle_face_right bitmap
				         dif = datafileSelect(P1.Sprites, current, 0);                            //move pointer to sprite in datafile
                         if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                         else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                         current = 0;
                     }
			}

			else if (p2Left == TRUE)
			{
                 if (p2Block_length > 0 && key[KEY_S]) //while blocjking
                 {
                     blit((BITMAP*)P1.Sprites[27].dat,char_buf, 0, 0, 0, 0, 1024, 768);           //select block_left bitmap
				     dif = datafileSelect(P1.Sprites, current, 27);                               //move pointer to sprite in datafile
                     if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                     else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                     current = 27;
                 }
                 else        //after blocking
                 {
                     p1Block = COOLDOWN;
                     p1_blocklength = 0;
                     p1_blockTime = 180;
                     blit((BITMAP*)P1.Sprites[1].dat,char_buf, 0, 0, 0, 0, 1024, 768);            //select idle_face_left bitmap
				     dif = datafileSelect(P1.Sprites, current, 1);                                //move pointer to sprite in datafile
                     if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                     else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                     current = 1;
                 }
			}
		}
		else if (p1Stun == TRUE)
		{
			if (p1Right == TRUE)
			{
				blit((BITMAP*)P1.Sprites[24].dat,char_buf, 0, 0, 0, 0, 1024, 768);                //select stun_right bitmap
				dif = datafileSelect(P1.Sprites, current, 24);                                    //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 24;
			}

			else if (p1Left == TRUE)
			{
				blit((BITMAP*)P1.Sprites[25].dat,char_buf, 0, 0, 0, 0, 1024, 768);                //select stun_left bitmap
				dif = datafileSelect(P1.Sprites, current, 25);                                    //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 25;
			}
		}
		else if (p1GroundCollision == FALSE)
		{
			if (p1Right == TRUE && p1Punch != TRUE && p1Kick != TRUE && p1Block != TRUE && p1Special != TRUE)
			{
				blit((BITMAP*)P1.Sprites[4].dat,char_buf, 0, 0, 0, 0, 1024, 768);       //select Jump_right bitmap
				dif = datafileSelect(P1.Sprites, current, 4);                           //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 4;
			}

			else if (p1Left == TRUE && p1Punch != TRUE && p1Kick != TRUE && p1Block != TRUE && p1Special != TRUE)
			{
				 blit((BITMAP*)P1.Sprites[5].dat,char_buf, 0, 0, 0, 0, 1024, 768);     //select Jump_left bitmap
				dif = datafileSelect(P1.Sprites, current, 5);                          //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 5;
			}

		}

		else if (p1Right == TRUE && p1Punch != TRUE && p1Kick != TRUE && p1Block != TRUE && p1Special != TRUE)
		{
			if (key[KEY_D]) //while walking right
            {
			if (framecounter < 24 && framecounter > 0)
			{
				blit((BITMAP*)P1.Sprites[2].dat,char_buf, 0, 0, 0, 0, 1024, 768);        //select walk_right Bitmap
				dif = datafileSelect(P1.Sprites, current, 2);
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 2;
            }
			else
			{
				blit((BITMAP*)P1.Sprites[0].dat,char_buf, 0, 0, 0, 0, 1024, 768);        //select idle_face_right bitmap
				dif = datafileSelect(P1.Sprites, current, 0);
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 0;
			}
   		    }

		    else
		    {
            blit((BITMAP*)P1.Sprites[0].dat,char_buf, 0, 0, 0, 0, 1024, 768);            //select idle_face_right bitmap
		    dif = datafileSelect(P1.Sprites, current, 0);                                //move pointer to sprite in datafile
            if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
            else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
            current = 0;
            }
		}

		else if (p1Left == TRUE && p1Punch != TRUE && p1Kick != TRUE && p1Block != TRUE && p1Special != TRUE)
		{
        if(key[KEY_A])  //while walking left
        {
			if (framecounter < 24 && framecounter > 0)
			{
				blit((BITMAP*)P1.Sprites[3].dat,char_buf, 0, 0, 0, 0, 1024, 768);  //select walk_right bitmap
				dif = datafileSelect(P1.Sprites, current, 3);                      //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 3;
			}

			else
			{
				blit((BITMAP*)P1.Sprites[1].dat,char_buf, 0, 0, 0, 0, 1024, 768);  //select idle_face_right bitmap
                dif = datafileSelect(P1.Sprites, current, 1);                      //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 1;
			}
		}

		else
		{
            blit((BITMAP*)P1.Sprites[1].dat,char_buf, 0, 0, 0, 0, 1024, 768);      //select idle_face_right bitmap
            dif = datafileSelect(P1.Sprites, current, 1);                          //move pointer to sprite in datafile
            if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
            else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
            current = 1;
        }
	}
	}

	else if (strcmp(P1.Name, "Mage") == TRUE) ///Mage Character
	{
		if (p1Punch == TRUE)
		{
			if (p1GroundCollision == TRUE)
			{
				if (p1Right == TRUE)
				{
					if (p1PunchCount == 2)
					{
						if (p1Punch_length > 12)
						{
							blit((BITMAP*)P1.Sprites[12].dat,char_buf, 0, 0, 0, 0, 1024, 768);    //select SpecPunch1_right bitmap
							dif = datafileSelect(P1.Sprites, current, 12);                        //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 12;
                p1PunchCount = 0;
						}

						else
						{
							blit((BITMAP*)P1.Sprites[14].dat,char_buf, 0, 0, 0, 0, 1024, 768);    //select SpecPunch2_right bitmap
							dif = datafileSelect(P1.Sprites, current, 14);                        //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 14;
						}
					}

					else
					{
                        if (p1_jumpaction == 0)
                        {
						if (p1Punch_length > 12)
						{
							blit((BITMAP*)P1.Sprites[8].dat,char_buf, 0, 0, 0, 0, 1024, 768);     //select Attack1_right bitmap
							dif = datafileSelect(P1.Sprites, current, 8);                         //move pointer to sprite in datafile
                            if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                            else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                            current = 8;
						}

						else
						{
							blit((BITMAP*)P1.Sprites[10].dat,char_buf, 0, 0, 0, 0, 1024, 768);    //select Attack2_right bitmap
							dif = datafileSelect(P1.Sprites, current, 10);                        //move pointer to sprite in datafile
                            if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                            else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                            current = 10;
						}
					}
				else if (p1_jumpaction == 1)
				{
                    blit((BITMAP*)P1.Sprites[0].dat,char_buf, 0, 0, 0, 0, 1024, 768);             //select idle_face_right bitmap
					dif = datafileSelect(P1.Sprites, current, 0);                                 //move pointer to sprite in datafile
                    if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                    else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                    current = 0;
                }
                }
            }

				else if (p1Left == TRUE)
				{
					if (p1PunchCount == 2)
					{
						if (p1Punch_length > 12)
						{
							blit((BITMAP*)P1.Sprites[13].dat,char_buf, 0, 0, 0, 0, 1024, 768);    //select SpecPunch1_left bitmap
							dif = datafileSelect(P1.Sprites, current, 13);                        //move pointer to sprite in datafile
                             if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                             else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                             current = 13;
						}

						else
						{
							blit((BITMAP*)P1.Sprites[15].dat,char_buf, 0, 0, 0, 0, 1024, 768);    //select SpecPunch2_left bitmap
							dif = datafileSelect(P1.Sprites, current, 15);                        //move pointer to sprite in datafile
                            if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                            else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                            current = 15;
						}
					}

					else
					{
                        if (p1_jumpaction == 0)
                        {
						if (p1Punch_length > 12)
						{
							blit((BITMAP*)P1.Sprites[9].dat,char_buf, 0, 0, 0, 0, 1024, 768);     //select Attack1_left bitmap
							dif = datafileSelect(P1.Sprites, current, 12);                        //move pointer to sprite in datafile
                            if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                            else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                            current = 12;
						}

						else
						{
							blit((BITMAP*)P1.Sprites[11].dat,char_buf, 0, 0, 0, 0, 1024, 768);    //select Attack2_left bitmap
							dif = datafileSelect(P1.Sprites, current, 11);                        //move pointer to sprite in datafile
                            if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                            else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                            current = 11;
						}
					}
			if (p1_jumpaction == 1)
			{
                p1Punch_length = 0;
                p1_punchTime = 30;
                p1Punch = COOLDOWN;
                blit((BITMAP*)P1.Sprites[1].dat,char_buf, 0, 0, 0, 0, 1024, 768);    //select idle_face_left bitmap
							dif = datafileSelect(P1.Sprites, current, 1);            //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 1;
                        }
                    }
            }
        }

			else
			{
				if (p1Right == TRUE)
				{
					if (p1Punch_length > 12)
					{
						blit((BITMAP*)P1.Sprites[24].dat,char_buf, 0, 0, 0, 0, 1024, 768);  //select JumpPunch1_right bitmap
						dif = datafileSelect(P1.Sprites, current, 24);                      //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 24;
					}

					else
					{
						blit((BITMAP*)P1.Sprites[26].dat,char_buf, 0, 0, 0, 0, 1024, 768);  //select JumpPunch2_right bitmap
						dif = datafileSelect(P1.Sprites, current, 26);                      //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 26;
					}
				}

				else if (p1Left == TRUE)
				{
					if (p1Punch_length > 12)
					{
						blit((BITMAP*)P1.Sprites[25].dat,char_buf, 0, 0, 0, 0, 1024, 768);  //select JumpPunch1_left bitmap
						dif = datafileSelect(P1.Sprites, current, 25);                      //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 25;
					}
					else
					{
						blit((BITMAP*)P1.Sprites[27].dat,char_buf, 0, 0, 0, 0, 1024, 768);  //select JumpPunch2_left bitmap
						dif = datafileSelect(P1.Sprites, current, 27);                      //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 27;
					}
				}
			}
		}

		else if (p1Kick == TRUE)
		{
			if (p1GroundCollision == TRUE)
			{
				if (p1Right == TRUE)
				{
                            if (p1_jumpaction == 0)
                            {
					if (p1Kick_length > 12)
					{
						blit((BITMAP*)P1.Sprites[16].dat,char_buf, 0, 0, 0, 0, 1024, 768);  //select Kick1_right bitmap
						dif = datafileSelect(P1.Sprites, current, 16);                      //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 16;
					}


					else
					{
						blit((BITMAP*)P1.Sprites[18].dat,char_buf, 0, 0, 0, 0, 1024, 768);  //select Kick2_right bitmap
						dif = datafileSelect(P1.Sprites, current, 18);                      //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 18;
					}
				}
				else if (p1_jumpaction == 1)
				{
                     blit((BITMAP*)P1.Sprites[0].dat,char_buf, 0, 0, 0, 0, 1024, 768);      //select idle_face_right bitmap
						dif = datafileSelect(P1.Sprites, current, 0);                       //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 0;
                  }
            }

				else if (p1Left == TRUE)
				{
                     if(p1_jumpaction == 0)
                     {
					if (p1Kick_length > 12)
					{
						blit((BITMAP*)P1.Sprites[17].dat,char_buf, 0, 0, 0, 0, 1024, 768);  //select Kick1_left bitmap
						dif = datafileSelect(P1.Sprites, current, 17);                      //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 17;
					}

					else
					{
						blit((BITMAP*)P1.Sprites[19].dat,char_buf, 0, 0, 0, 0, 1024, 768);  //select Kick2_left bitmap
						dif = datafileSelect(P1.Sprites, current, 19);                      //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 19;
					}
				}
			else if (p1_jumpaction == 1)
			{
                 blit((BITMAP*)P1.Sprites[1].dat,char_buf, 0, 0, 0, 0, 1024, 768);          //select idle_face_left bitmap
						dif = datafileSelect(P1.Sprites, current, 1);                       //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 1;
             }
        }
      }
			else if (p1Right == TRUE)
			{

				blit((BITMAP*)P1.Sprites[28].dat,char_buf, 0, 0, 0, 0, 1024, 768);          //select JumpKick_right bitmap
				dif = datafileSelect(P1.Sprites, current, 28);                              //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 28;
			}
			else if (p1Left == TRUE)
			{
				blit((BITMAP*)P1.Sprites[29].dat,char_buf, 0, 0, 0, 0, 1024, 768);          //select JumpKick_left bitmap
				dif = datafileSelect(P1.Sprites, current, 29);                              //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 29;
			}
		}

		else if (p1Special == TRUE)
		{
			if (p1Right == TRUE)
			{
				if (p1Special_length > 18)
				{
					blit((BITMAP*)P1.Sprites[20].dat,char_buf, 0, 0, 0, 0, 1024, 768);      //select Special1_right bitmap
					dif = datafileSelect(P1.Sprites, current, 20);                          //move pointer to sprite in datafile
                 if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                 else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                 current = 20;
				}

				else
				{
					blit((BITMAP*)P1.Sprites[22].dat,char_buf, 0, 0, 0, 0, 1024, 768);      //select Special2_right bitmap
					dif = datafileSelect(P1.Sprites, current, 22);                          //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 22;
				}
			}

			else if (p1Left == TRUE)
			{
				if (p1Special_length > 18)
				{
					blit((BITMAP*)P1.Sprites[21].dat,char_buf, 0, 0, 0, 0, 1024, 768);      //select Special1_left bitmap
					dif = datafileSelect(P1.Sprites, current, 21);                          //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 21;
				}

				else
				{
					blit((BITMAP*)P1.Sprites[23].dat,char_buf, 0, 0, 0, 0, 1024, 768);      //select Special2_left bitmap
					dif = datafileSelect(P1.Sprites, current, 23);                          //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 23;
				}
			}
		}

		else if (p1Stun == TRUE)
		{
			if (p1Right == TRUE)
			{
				blit((BITMAP*)P1.Sprites[30].dat,char_buf, 0, 0, 0, 0, 1024, 768);          //select Stun_right bitmap
				dif = datafileSelect(P1.Sprites, current, 30);                              //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 30;
			}

			else if (p1Left == TRUE)
			{
				blit((BITMAP*)P1.Sprites[31].dat,char_buf, 0, 0, 0, 0, 1024, 768);          //select stun_left bitmap
				dif = datafileSelect(P1.Sprites, current, 31);                              //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 31;
			}
		}

		else if (p1Block == TRUE)
		{
			if (p1Right == TRUE)
			{
                        if (p1_blocklength > 0 && key[KEY_S]) //while blocking
                        {
				blit((BITMAP*)P1.Sprites[32].dat,char_buf, 0, 0, 0, 0, 1024, 768);          //select Block_right bitmap
				dif = datafileSelect(P1.Sprites, current, 32);
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 32;
			}
			else        //after blocking
			{
                p1Block = COOLDOWN;
                p1_blocklength = 0;
                p1_blockTime = 180;
                blit((BITMAP*)P1.Sprites[0].dat,char_buf, 0, 0, 0, 0, 1024, 768);           //select idle_face_right bitmap
				dif = datafileSelect(P1.Sprites, current, 0);                               //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 0;
            }
        }
			else if (p1Left == TRUE)
			{
                 if (p1_blocklength > 0 && key[KEY_S]) //while blocking
                 {
				blit((BITMAP*)P1.Sprites[33].dat,char_buf, 0, 0, 0, 0, 1024, 768);          //select block_left bitmap
				dif = datafileSelect(P1.Sprites, current, 33);                              //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 33;
                 }
        	else  //after blocking
			{
                p1Block = COOLDOWN;
                p1_blocklength = 0;
                p1_blockTime = 180;
                blit((BITMAP*)P1.Sprites[1].dat,char_buf, 0, 0, 0, 0, 1024, 768);           //select idle_face_left bitmap
				dif = datafileSelect(P1.Sprites, current, 1);                               //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 1;
            }
			}
		}

		else if (p1GroundCollision == FALSE)
		{
			if (p1Right == TRUE)
			{
				if (p1JumpTime == 1/60)
				{
					blit((BITMAP*)P1.Sprites[4].dat,char_buf, 0, 0, 0, 0, 1024, 768);       //select jump1_right bitmap
					dif = datafileSelect(P1.Sprites, current, 4);                           //move pointer to sprite in datafile
                 if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                 else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                 current = 4;
				}

				else
				{
					blit((BITMAP*)P1.Sprites[6].dat,char_buf, 0, 0, 0, 0, 1024, 768);       //select jump2_right bitmap
					dif = datafileSelect(P1.Sprites, current, 6);                           //move pointer to sprite in datafile
                    if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                    else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                    current = 6;
				}
			}

			else if (p1Left == TRUE)
			{
				if (p1JumpTime == 1/60)
				{
                     blit((BITMAP*)P1.Sprites[5].dat,char_buf, 0, 0, 0, 0, 1024, 768);      //select jump1_left bitmap
                     dif = datafileSelect(P1.Sprites, current, 5);                          //move pointer to sprite in datafile
                     if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                     else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                     current = 5;
				}

				else
				{
					blit((BITMAP*)P1.Sprites[7].dat,char_buf, 0, 0, 0, 0, 1024, 768);  //select jump2_left bitmap
				    dif = datafileSelect(P1.Sprites, current, 7);                      //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 7;
				}
			}
		}

		else if (p1Right == TRUE)
		{
        if (key[KEY_D])  //while walking right
        {
			if (framecounter < 24 && framecounter > 0)
			{
				blit((BITMAP*)P1.Sprites[2].dat,char_buf, 0, 0, 0, 0, 1024, 768);      //select walk_right bitmap
				dif = datafileSelect(P1.Sprites, current, 2);                          //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 2;
			}

			else
			{
				blit((BITMAP*)P1.Sprites[0].dat,char_buf, 0, 0, 0, 0, 1024, 768);      //select idle_face_right bitmap
				dif = datafileSelect(P1.Sprites, current, 0);                          //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 0;
			}
		}
		else
		{
            blit((BITMAP*)P1.Sprites[0].dat,char_buf, 0, 0, 0, 0, 1024, 768);          //select idle_face_right bitmap
				dif = datafileSelect(P1.Sprites, current, 0);                          //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 0;
        }
    }

		else if (p1Left == TRUE)
		{
        if (key[KEY_A]) //while walking left
        {
			if (framecounter < 24 && framecounter > 0)
			{
				blit((BITMAP*)P1.Sprites[3].dat,char_buf, 0, 0, 0, 0, 1024, 768);      //select walk_left bitmap
				dif = datafileSelect(P1.Sprites, current, 3);                          //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 3;
			}

			else
			{
				blit((BITMAP*)P1.Sprites[1].dat,char_buf, 0, 0, 0, 0, 1024, 768);      //select idle_face_left bitmap
				dif = datafileSelect(P1.Sprites, current, 1);                          //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 1;
			}
		}
		else
		{
                blit((BITMAP*)P1.Sprites[1].dat,char_buf, 0, 0, 0, 0, 1024, 768);      //select idle_face_left bitmap
				dif = datafileSelect(P1.Sprites, current, 1);                          //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P1.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P1.Sprites--;
                current = 1;
        }
    }
	}

    name = get_datafile_property(P1.Sprites, DAT_ID('X','S','I','Z'));   //retrieve height and width properties
    width = atoi(name);
    name = get_datafile_property(P1.Sprites, DAT_ID('Y','S','I','Z'));
    height = atoi(name);
    blit(char_buf, temp, 0, 0, 0, (250 - height), 1024, 768);         //blit sprite to buffer and destroy temporary sprite bitmap
    destroy_bitmap(char_buf);
    dist = width;                                                     //return width
    return (dist);
}END_OF_FUNCTION (Player1_SpriteSelect);

/**retrieves actual height of selected sprite for player 1**/
int P1_get_height(Character P1, BITMAP *temp, int dist){
    const char* name;
    name = get_datafile_property(P1.Sprites, DAT_ID('Y','S','I','Z'));
    dist = atoi(name);
    return (dist);
}END_OF_FUNCTION (P1_get_height);

/**Selects and blits to buffer the appropriate bitmap for Player 2. Also opens up properties of the selected bitmap.**/
int Player2_SpriteSelect(Character P2, BITMAP *temp, int dist)
{
    clear_to_color(temp, makecol(255, 0, 255));
    BITMAP *char_buf = create_bitmap(505,240);
    const char *name;
    int width, height;
    int i, dif;
    int current = 0;
    clear_to_color(char_buf, makecol(255, 0, 255));
	if (strcmp(P2.Name, "Fighter") == 0)
	{
        if (p2Punch == TRUE && p2Stun != TRUE)
		{
			if (p2Right == TRUE)
			{
				if (p2GroundCollision == TRUE)
				{
					switch (p2PunchCount % 4)
					{
						case 1:
							blit((BITMAP*)P2.Sprites[6].dat,char_buf, 0, 0, 0, 0, 1024, 768);      //select Punch1_right sprite
                            dif = datafileSelect(P2.Sprites, current, 6);                          //move pointer to sprite in datafile
                            if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                            else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                            current = 6;
							break;

						case 2:
							blit((BITMAP*)P2.Sprites[8].dat,char_buf, 0, 0, 0, 0, 1024, 768);     //select Punch2_right sprite
                            dif = datafileSelect(P2.Sprites, current, 8);                         //move pointer to sprite in datafile
                            if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                            else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                            current = 8;
							break;

						case 3:
							blit((BITMAP*)P2.Sprites[10].dat,char_buf, 0, 0, 0, 0, 1024, 768);              //select Punch3_right sprite
                            dif = datafileSelect(P2.Sprites, current, 10);                                  //move pointer to sprite in datafile
                            if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                            else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                            current = 10;
							break;

						case 0:
							if (p2Punch_length > 6)
							{
								blit((BITMAP*)P2.Sprites[12].dat,char_buf, 0, 0, 0, 0, 1024, 768);          //select Combo1_right sprite
					            dif = datafileSelect(P2.Sprites, current, 12);                              //move pointer to sprite in datafile
                                if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                                else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                                current = 12;
							}

							else
							{
								blit((BITMAP*)P2.Sprites[14].dat,char_buf, 0, 0, 0, 0, 1024, 768);          //select Combo2_right sprite
					            dif = datafileSelect(P2.Sprites, current, 14);                              //move pointer to sprite in datafile
                                if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                                else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                                current = 14;
							}
							break;

						default:
							set_gfx_mode(GFX_TEXT,0,0,0,0);
							allegro_message("The program entered a bug and needs to close.");
							exit (EXIT_FAILURE);
					}
				}

				else
				{
					blit((BITMAP*)P2.Sprites[20].dat,char_buf, 0, 0, 0, 0, 1024, 768);       //select JumpPunch_right sprite
                    dif = datafileSelect(P2.Sprites, current, 20);                           //move pointer to sprite in datafile
                    if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                    else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                    current = 20;
				}
			}

			else if (p2Left == TRUE)
			{
				if (p2GroundCollision == TRUE)
				{
					switch (p2PunchCount % 4)
					{
						case 1:
							blit((BITMAP*)P2.Sprites[7].dat,char_buf, 0, 0, 0, 0, 1024, 768);     //select Punch1_left bitmap
                            dif = datafileSelect(P2.Sprites, current, 7);                         //move pointer to sprite in datafile
                            if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                            else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                            current = 7;
							break;

						case 2:
							blit((BITMAP*)P2.Sprites[9].dat,char_buf, 0, 0, 0, 0, 1024, 768);     //select Punch2_left bitmap
					        dif = datafileSelect(P2.Sprites, current, 9);                         //move pointer to sprite in datafile
                            if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                            else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                            current = 9;
							break;

						case 3:
							blit((BITMAP*)P2.Sprites[11].dat,char_buf, 0, 0, 0, 0, 1024, 768);    //select Punch3_left bitmap
					        dif = datafileSelect(P2.Sprites, current, 11);                        //move pointer to sprite in datafile
                            if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                            else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                            current = 11;
							break;

						case 0:
							if (p2Punch_length > 6)
							{
								blit((BITMAP*)P2.Sprites[13].dat,char_buf, 0, 0, 0, 0, 1024, 768);   //select Combo1_left bitmap
					            dif = datafileSelect(P2.Sprites, current, 13);                       //move pointer to sprite in datafile
                                if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                                else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                                current = 13;
							}

							else
							{
								blit((BITMAP*)P2.Sprites[15].dat,char_buf, 0, 0, 0, 0, 1024, 768);   //select Combo2_left bitmap
					            dif = datafileSelect(P2.Sprites, current, 15);                       //move pointer to sprite in datafile
                                if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                                else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                                current = 15;

							}
							break;

						default:
							set_gfx_mode(GFX_TEXT,0,0,0,0);
							allegro_message("The program entered a bug and needs to close.");
							exit (EXIT_FAILURE);
                    }
				}

				else
				{
					blit((BITMAP*)P2.Sprites[21].dat,char_buf, 0, 0, 0, 0, 1024, 768);       //select JumpPunch_left bitmap
					dif = datafileSelect(P2.Sprites, current, 21);                           //move pointer to sprite in datafile
                    if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                    else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                    current = 21;
				}
			}
		}

		if (p2Kick == TRUE)
		{
			if (p2GroundCollision == TRUE)
			{
				if (p2Right == TRUE)
				{
                    if (p2_jumpaction == 0)
                    {
						blit((BITMAP*)P2.Sprites[16].dat,char_buf, 0, 0, 0, 0, 1024, 768);   //select kick_right bitmap
						dif = datafileSelect(P2.Sprites, current, 18);                       //move pointer to sprite in datafile
                        if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                        else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                        current = 16;
                    }

                    else if (p2_jumpaction == 1)
                    {
                        blit((BITMAP*)P2.Sprites[0].dat,char_buf, 0, 0, 0, 0, 1024, 768);    //select idle_face_right bitmap
                        dif = datafileSelect(P2.Sprites, current, 0);                        //move pointer to sprite in datafile
                        if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                        else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                        current = 0;
                    }
                }

				else if (p2Left == TRUE)
				{
                     if (p2_jumpaction == 0)
                     {
						blit((BITMAP*)P2.Sprites[17].dat,char_buf, 0, 0, 0, 0, 1024, 768);   //select kick_left bitmap
						dif = datafileSelect(P2.Sprites, current, 17);                       //move pointer to sprite in datafile
                        if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                        else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                        current = 17;
                    }

                    else if (p2_jumpaction == 1)
                    {
                        blit((BITMAP*)P2.Sprites[1].dat,char_buf, 0, 0, 0, 0, 1024, 768);    //select idle_face_left bitmap
                        p2Kick = FALSE;
                        dif = datafileSelect(P2.Sprites, current, 1);                        //move pointer to sprite in datafile
                        if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                        else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                        current = 1;
                    }
                }
            }

			else if (p2Right == TRUE)
			{
				blit((BITMAP*)P2.Sprites[22].dat,char_buf, 0, 0, 0, 0, 1024, 768);       //select JumpKick_right bitap
				dif = datafileSelect(P2.Sprites, current, 22);                           //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                current = 22;
			}

			else if (p2Left == TRUE)
            {
                if (p2GroundCollision == FALSE)
                {
                    blit((BITMAP*)P2.Sprites[23].dat,char_buf, 0, 0, 0, 0, 1024, 768);   //select JumpKick_left bitmap
                    dif = datafileSelect(P2.Sprites, current, 23);                       //move pointer to sprite in datafile
                    if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                    else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                    current = 23;
                }

                else if (p2GroundCollision == TRUE)
                {
                    p2_kickTime = 180;
                    p2Kick = COOLDOWN;
                    p2Kick_length = 0;
                    blit((BITMAP*)P2.Sprites[1].dat,char_buf, 0, 0, 0, 0, 1024, 768); //select idle_face_left bitmap
                    dif = datafileSelect(P2.Sprites, current, 1);                     //move pointer to sprite in datafile
                    if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                    else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                    current = 1;
                }
			}
		}

		else if (p2Special == TRUE)
		{
			if (p2Right == TRUE)
			{
				blit((BITMAP*)P2.Sprites[18].dat,char_buf, 0, 0, 0, 0, 1024, 768);   //select Special_right bitmap
				dif = datafileSelect(P2.Sprites, current, 18);                       //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                current = 18;
			}

			else if (p2Left == TRUE)
			{
				blit((BITMAP*)P2.Sprites[19].dat,char_buf, 0, 0, 0, 0, 1024, 768);   //select Special_left bitmap
				dif = datafileSelect(P2.Sprites, current, 19);                       //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                current = 19;
			}
		}

		else if (p2Block == TRUE)
		{
			if (p2Right == TRUE)  //while blocking
			{
                if (p2Block_length > 0 && key[KEY_DOWN])
                {
                    blit((BITMAP*)P2.Sprites[26].dat,char_buf, 0, 0, 0, 0, 1024, 768);   //select block_right bitmap
                    dif = datafileSelect(P2.Sprites, current, 26);                       //move pointer to sprite in datafile
                    if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                    else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                    current = 26;
                }

                else         //once block finishes
                {
                    p2Block = COOLDOWN;
                    p2Block_length = 0;
                    p2_blockTime = 180;
                    blit((BITMAP*)P2.Sprites[0].dat,char_buf, 0, 0, 0, 0, 1024, 768); //select idle_face_right bitmap
                    dif = datafileSelect(P2.Sprites, current, 0);                     //move pointer to sprite in datafile
                    if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                    else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                    current = 0;
                }
			}

			else if (p2Left == TRUE)
			{
                if (p2Block_length > 0 && key[KEY_DOWN])   //while blocking
                {
                    blit((BITMAP*)P2.Sprites[27].dat,char_buf, 0, 0, 0, 0, 1024, 768);  //select block_left bitmap
                    dif = datafileSelect(P2.Sprites, current, 27);                      //move pointer to sprite in datafile
                    if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                    else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                    current = 27;
                }
                else        //once block finishes
                {
                    p2Block = COOLDOWN;
                    p2Block_length = 0;
                    p2_blockTime = 180;
                    blit((BITMAP*)P2.Sprites[1].dat,char_buf, 0, 0, 0, 0, 1024, 768);    //select idle_face_left bitmap
                    dif = datafileSelect(P2.Sprites, current, 1);                        //move pointer to sprite in datafile
                    if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                    else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                    current = 1;
                }
            }
		}
		else if (p2Stun == TRUE)
		{
			if (p2Right == TRUE)
			{
				blit((BITMAP*)P2.Sprites[24].dat,char_buf, 0, 0, 0, 0, 1024, 768);   //select stun_right bitmap
				dif = datafileSelect(P2.Sprites, current, 24);                       //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                current = 24;
			}

			else if (p2Left == TRUE)
			{
				blit((BITMAP*)P2.Sprites[25].dat,char_buf, 0, 0, 0, 0, 1024, 768);    //select stun_left bitmap
				dif = datafileSelect(P2.Sprites, current, 25);                        //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                current = 25;
			}
		}

		else if (p2GroundCollision == FALSE)
		{
			if (p2Right == TRUE && p2Punch != TRUE && p2Kick != TRUE && p2Block != TRUE && p2Special != TRUE)
			{
				blit((BITMAP*)P2.Sprites[4].dat,char_buf, 0, 0, 0, 0, 1024, 768);   //select jump_right bitmap
				dif = datafileSelect(P2.Sprites, current, 4);                      //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                current = 4;
			}

			else if (p2Left == TRUE && p2Punch != TRUE && p2Kick != TRUE && p2Block != TRUE && p2Special != TRUE)
			{
				 blit((BITMAP*)P2.Sprites[5].dat,char_buf, 0, 0, 0, 0, 1024, 768);   //select jump_left bitmap
				dif = datafileSelect(P2.Sprites, current, 5);                        //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                current = 5;
			}
		}

		else if (p2Right == TRUE && p2Punch != TRUE && p2Kick != TRUE && p2Block != TRUE && p2Special != TRUE)
		{
			if (key[KEY_RIGHT])  //if walking right
            {
                if (framecounter < 24 && framecounter > 0)
                {
                    blit((BITMAP*)P2.Sprites[2].dat,char_buf, 0, 0, 0, 0, 1024, 768);    //select walk_right bitmap
                    dif = datafileSelect(P2.Sprites, current, 2);                        //move pointer to sprite in datafile
                    if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                    else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                    current = 2;
                }

                else
                {
                    blit((BITMAP*)P2.Sprites[0].dat,char_buf, 0, 0, 0, 0, 1024, 768);    //select idle_face_right bitmap
                    dif = datafileSelect(P2.Sprites, current, 0);                        //move pointer to sprite in datafile
                    if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                    else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                    current = 0;
                }
		    }
		    else
		    {
                blit((BITMAP*)P2.Sprites[0].dat,char_buf, 0, 0, 0, 0, 1024, 768);    //select idle_face_right bitmap
                dif = datafileSelect(P2.Sprites, current, 0);                        //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                current = 0;
            }
		}

		else if (p2Left == TRUE && p2Punch != TRUE && p2Kick != TRUE && p2Block != TRUE && p2Special != TRUE)
		{
            if(key[KEY_LEFT])   //if walking left
            {
                if (framecounter < 24 && framecounter > 0)
                {
                    blit((BITMAP*)P2.Sprites[3].dat,char_buf, 0, 0, 0, 0, 1024, 768);   //select walk_left bitmap
                    dif = datafileSelect(P2.Sprites, current, 3);                       //move pointer to sprite in datafile
                    if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                    else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                    current = 3;
                }

                else
                {
                    blit((BITMAP*)P2.Sprites[1].dat,char_buf, 0, 0, 0, 0, 1024, 768);   //select idle_face_left bitmap
                    dif = datafileSelect(P2.Sprites, current, 1);                       //move pointer to sprite in datafile
                    if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                    else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                    current = 1;
                }
            }
            else
            {
                blit((BITMAP*)P2.Sprites[1].dat,char_buf, 0, 0, 0, 0, 1024, 768);    //select idle_face_right bitmap
                dif = datafileSelect(P2.Sprites, current, 1);                        //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                current = 1;
            }
        }
	}

	else if (strcmp(P2.Name, "Mage") == TRUE)                   //if playing mage
	{
		if (p2Punch == TRUE)
		{
			if (p2GroundCollision == TRUE)
			{
				if (p2Right == TRUE)
				{
					if (p2PunchCount == 2)
					{
						if (p2Punch_length > 12)
						{
							blit((BITMAP*)P2.Sprites[12].dat,char_buf, 0, 0, 0, 0, 1024, 768);       //select SpecPunch1_right bitmap;
							dif = datafileSelect(P2.Sprites, current, 12);                           //move pointer to sprite in datafile
                            if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                            else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                            current = 12;
						}

						else
						{
							blit((BITMAP*)P2.Sprites[14].dat,char_buf, 0, 0, 0, 0, 1024, 768);       //select SpecPunch2_right bitmap;
							dif = datafileSelect(P2.Sprites, current, 14);                           //move pointer to sprite in datafile
                            if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                            else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                            current = 14;
						}
					}

					else
					{
                        if (p2_jumpaction == 0)
                        {
                            if (p2Punch_length > 12)
                            {
                                blit((BITMAP*)P2.Sprites[8].dat,char_buf, 0, 0, 0, 0, 1024, 768);    //select Attack1_right bitmap
                                dif = datafileSelect(P2.Sprites, current, 8);                        //move pointer to sprite in datafile
                                if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                                else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                                current = 8;
                            }

                            else
                            {
                                blit((BITMAP*)P2.Sprites[10].dat,char_buf, 0, 0, 0, 0, 1024, 768);   //select Attack2_right bitmap
                                dif = datafileSelect(P2.Sprites, current, 10);                       //move pointer to sprite in datafile
                                if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                                else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                                current = 10;
                            }
                        }
                        if (p2_jumpaction == 1)
                        {
                            blit((BITMAP*)P2.Sprites[0].dat,char_buf, 0, 0, 0, 0, 1024, 768);        //select idle_face_right bitmap
							dif = datafileSelect(P2.Sprites, current, 0);                            //move pointer to sprite in datafile
                            if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                            else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                            current = 0;
                        }
                    }
                }

				else if (p2Left == TRUE)
				{
					if (p2PunchCount == 2)
					{
						if (p2Punch_length > 12)
						{
							blit((BITMAP*)P2.Sprites[13].dat,char_buf, 0, 0, 0, 0, 1024, 768);       //select SpecPunch1_left bitmap
							dif = datafileSelect(P2.Sprites, current, 13);                           //move pointer to sprite in datafile
                            if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                            else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                            current = 13;
						}

						else
						{
							blit((BITMAP*)P2.Sprites[15].dat,char_buf, 0, 0, 0, 0, 1024, 768);       //select SpecPunch2_left bitmap
							dif = datafileSelect(P2.Sprites, current, 15);                           //move pointer to sprite in datafile
                            if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                            else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                            current = 15;
						}
					}

					else
					{
                        if (p2_jumpaction == 0)
                        {
                            if (p2Punch_length > 12)
                            {
                                blit((BITMAP*)P2.Sprites[9].dat,char_buf, 0, 0, 0, 0, 1024, 768);    //select Attack1_left bitmap
                                dif = datafileSelect(P2.Sprites, current, 9);                        //move pointer to sprite in datafile
                                if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                                else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                                current = 9;
                            }

						else
						{
							blit((BITMAP*)P2.Sprites[11].dat,char_buf, 0, 0, 0, 0, 1024, 768);       //select Attack2_left bitmap
							dif = datafileSelect(P2.Sprites, current, 11);                           //move pointer to sprite in datafile
                            if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                            else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                            current = 11;
						}
					}
					else if (p2_jumpaction == 1)
                        {
                            p2Punch_length = 0;
                            p2_punchTime = 30;
                            p2Punch = COOLDOWN;
                            blit((BITMAP*)P2.Sprites[1].dat,char_buf, 0, 0, 0, 0, 1024, 768);        //select idle_face_right bitmap
                            dif = datafileSelect(P2.Sprites, current, 1);                            //move pointer to sprite in datafile
                            if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                            else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                            current = 1;
                        }
                    }
                }
            }

			else
			{
				if (p2Right == TRUE)
				{
					if (p2Punch_length > 12)
					{
						blit((BITMAP*)P2.Sprites[24].dat,char_buf, 0, 0, 0, 0, 1024, 768);           //select JumpPunch1_right bitmap
						dif = datafileSelect(P2.Sprites, current, 24);                               //move pointer to sprite in datafile
                        if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                        else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                        current = 24;
					}

					else
					{
						blit((BITMAP*)P2.Sprites[26].dat,char_buf, 0, 0, 0, 0, 1024, 768);           //select JumpPunch2_right bitmap
						dif = datafileSelect(P2.Sprites, current, 26);                               //move pointer to sprite in datafile
                        if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                        else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                        current = 26;
					}
				}

				else if (p2Left == TRUE)
				{
					if (p2Punch_length > 12)
					{
						blit((BITMAP*)P2.Sprites[25].dat,char_buf, 0, 0, 0, 0, 1024, 768);           //select JumpPunch1_left bitmap
						dif = datafileSelect(P2.Sprites, current, 25);                               //move pointer to sprite in datafile
                        if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                        else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                        current = 25;
					}

					else
					{
						blit((BITMAP*)P2.Sprites[27].dat,char_buf, 0, 0, 0, 0, 1024, 768);           //select JumpPunch2_left bitmap
						dif = datafileSelect(P2.Sprites, current, 27);                               //move pointer to sprite in datafile
                        if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                        else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                        current = 27;
					}
				}
			}
		}

		else if (p2Kick == TRUE)
		{
			if (p2GroundCollision == TRUE)
			{
				if (p2Right == TRUE)
				{
                    if (p2_jumpaction == 0)
                    {
                        if (p2Kick_length > 12)
                        {
                            blit((BITMAP*)P2.Sprites[16].dat,char_buf, 0, 0, 0, 0, 1024, 768);          //select kick1_right bitmap
                            dif = datafileSelect(P2.Sprites, current, 16);                              //move pointer to sprite in datafile
                            if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                            else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                            current = 16;
                        }

                        else
                        {
                            blit((BITMAP*)P2.Sprites[18].dat,char_buf, 0, 0, 0, 0, 1024, 768);          //select kick2_right bitmap
                            dif = datafileSelect(P2.Sprites, current, 18);                              //move pointer to sprite in datafile
                            if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                            else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                            current = 18;
                        }
                    }
                    else if (p2_jumpaction == 1)
                    {
                        blit((BITMAP*)P2.Sprites[0].dat,char_buf, 0, 0, 0, 0, 1024, 768);               //select idle facing right bitmap if landed after midair kick
                        dif = datafileSelect(P2.Sprites, current, 0);                                   //move pointer to sprite in datafile
                        if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                        else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                        current = 0;
                    }
                }

				else if (p2Left == TRUE)
				{
                    if (p2_jumpaction == 0)
                    {
                        if (p2Kick_length > 12)
                        {
                            blit((BITMAP*)P2.Sprites[17].dat,char_buf, 0, 0, 0, 0, 1024, 768);          //select kick1_left bitmap
                            dif = datafileSelect(P2.Sprites, current, 17);                              //move pointer to sprite in datafile
                            if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                            else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                            current = 17;
                        }

                        else
                        {
                            blit((BITMAP*)P2.Sprites[19].dat,char_buf, 0, 0, 0, 0, 1024, 768);          //select kick2_left bitmap
                            dif = datafileSelect(P2.Sprites, current, 19);                              //move pointer to sprite in datafile
                            if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                            else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                            current = 19;
                        }
                    }

                    else if (p2_jumpaction == 1)
                    {
                        blit((BITMAP*)P2.Sprites[1].dat,char_buf, 0, 0, 0, 0, 1024, 768);               //select idle facing left bitmap if landed after midair kick
                        p2Kick = FALSE;                                                                 //disable jump
                        dif = datafileSelect(P2.Sprites, current, 1);                                   //move pointer to sprite in datafile
                        if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                        else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                        current = 1;
                    }
                }
            }

			else if (p2Right == TRUE)
			{
				blit((BITMAP*)P2.Sprites[28].dat,char_buf, 0, 0, 0, 0, 1024, 768);
				dif = datafileSelect(P2.Sprites, current, 28);
                if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                current = 28;
			}
			else if (p2Left == TRUE)
                {
                if (p2GroundCollision == FALSE)
                    {
                        blit((BITMAP*)P2.Sprites[29].dat,char_buf, 0, 0, 0, 0, 1024, 768);    //select jump kick right bitmap
                        dif = datafileSelect(P2.Sprites, current, 29);                        //move pointer to sprite in datafile
                        if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                        else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                        current = 29;
                    }
                    else if (p2GroundCollision == TRUE)
                    {
                        p2_kickTime = 180;                                                    //place kick on cooldown
                        p2Kick = COOLDOWN;
                        p2Kick_length = 0;
                        blit((BITMAP*)P2.Sprites[1].dat,char_buf, 0, 0, 0, 0, 1024, 768);     //select bitmap if idle facing right
                        dif = datafileSelect(P2.Sprites, current, 1);                         //move pointer to sprite in datafile
                        if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                        else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                        current = 1;
                    }
                }
            }

            else if (p2Special == TRUE)
            {
                if (p2Right == TRUE)
                {
                    if (p2Special_length > 18)
                    {
                        blit((BITMAP*)P2.Sprites[20].dat,char_buf, 0, 0, 0, 0, 1024, 768);    //select bitmap if preparing to teleport right (special)
                        dif = datafileSelect(P2.Sprites, current, 20);                        //move pointer to sprite in datafile
                        if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                        else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                        current = 20;
                    }

                    else
                    {
                        blit((BITMAP*)P2.Sprites[22].dat,char_buf, 0, 0, 0, 0, 1024, 768);    //select bitmap if teleporting right (special)
                        dif = datafileSelect(P2.Sprites, current, 22);                        //move pointer to sprite in datafile
                        if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                        else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                        current = 22;
                    }
                }

                else if (p2Left == TRUE)
                {
                    if (p2Special_length > 18)
                    {
                        blit((BITMAP*)P2.Sprites[21].dat,char_buf, 0, 0, 0, 0, 1024, 768);    //select bitmap if preparing to teleport left (special)
                        dif = datafileSelect(P2.Sprites, current, 21);                        //move pointer to sprite in datafile
                        if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                        else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                        current = 21;
                    }

                    else
                    {
                        blit((BITMAP*)P2.Sprites[23].dat,char_buf, 0, 0, 0, 0, 1024, 768);    //select bitmap if teleporting left (special)
                        dif = datafileSelect(P2.Sprites, current, 23);                        //move pointer to sprite in datafile
                        if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                        else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                        current = 23;
                    }
                }
            }

		else if (p2Stun == TRUE)
		{
			if (p2Right == TRUE)
			{
				blit((BITMAP*)P2.Sprites[30].dat,char_buf, 0, 0, 0, 0, 1024, 768);            //select bitmap if stunned facing right
                dif = datafileSelect(P2.Sprites, current, 31);                                //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                current = 30;
			}

			else if (p2Left == TRUE)
			{
				blit((BITMAP*)P2.Sprites[31].dat,char_buf, 0, 0, 0, 0, 1024, 768);            //select bitmap if stunned facing left
                dif = datafileSelect(P2.Sprites, current, 31);                                //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                current = 31;
			}
		}

		else if (p2Block == TRUE)
		{
			if (p2Right == TRUE)
			{
                if (p2Block_length > 0 && key[KEY_DOWN])                                      //if actively blocking
                {
                    blit((BITMAP*)P2.Sprites[32].dat,char_buf, 0, 0, 0, 0, 1024, 768);        //select bitmap for blocking right
                    dif = datafileSelect(P2.Sprites, current, 32);                            //move pointer to sprite in datafile
                    if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                    else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                    current = 32;
                }
                else
                {
                    p2Block = COOLDOWN;                                                       //place block on cooldown
                    p2Block_length = 0;
                    p2_blockTime = 180;
                    blit((BITMAP*)P2.Sprites[0].dat,char_buf, 0, 0, 0, 0, 1024, 768);         //select bitmap for idle facing right
                    dif = datafileSelect(P2.Sprites, current, 0);                             //move pointer to sprite in datafile
                    if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                    else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                    current = 0;
                }
			}

			else if (p2Left == TRUE)
			{
                 if (p2Block_length > 0 && key[KEY_DOWN])                                     //if actively blocking
                 {
                     blit((BITMAP*)P2.Sprites[33].dat,char_buf, 0, 0, 0, 0, 1024, 768);       //select bitmap for blocking left
				     dif = datafileSelect(P2.Sprites, current, 33);                           //move pointer to sprite in datafile
                     if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                     else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                     current = 33;
                 }
                 else
                 {
                     p2Block = COOLDOWN;                                                      //place block on cooldown
                     p2Block_length = 0;
                     p2_blockTime = 180;
                     blit((BITMAP*)P2.Sprites[1].dat,char_buf, 0, 0, 0, 0, 1024, 768);        //select bitmap for idle facing left
				     dif = datafileSelect(P2.Sprites, current, 1);                            //move pointer to sprite in datafile
                     if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                     else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                     current = 1;
                 }
			}
		}

		else if (p2GroundCollision == FALSE)
		{
			if (p2Right == TRUE)
			{
				if (p2JumpTime <= 5/60)
				{
					blit((BITMAP*)P2.Sprites[4].dat,char_buf, 0, 0, 0, 0, 1024, 768);          //select bitmap if jumping up facing right
					dif = datafileSelect(P2.Sprites, current, 4);                              //move pointer to sprite in datafile
                    if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                    else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                    current = 4;
				}

				else
				{
					blit((BITMAP*)P2.Sprites[6].dat,char_buf, 0, 0, 0, 0, 1024, 768);          //select bitmap if falling down facing right
					dif = datafileSelect(P2.Sprites, current, 6);                              //move pointer to sprite in datafile
                    if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                    else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                    current = 6;
				}
			}

			else if (p2Left == TRUE)
			{
				if (p2JumpTime == 1/60)
				{
                     blit((BITMAP*)P2.Sprites[5].dat,char_buf, 0, 0, 0, 0, 1024, 768);         //select bitmap if jumping up facing left
                     dif = datafileSelect(P2.Sprites, current, 5);                             //move pointer to sprite in datafile
                     if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                     else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                     current = 5;
				}

				else
				{
					blit((BITMAP*)P2.Sprites[7].dat,char_buf, 0, 0, 0, 0, 1024, 768);          //select bitmap if falling down facing left
				    dif = datafileSelect(P2.Sprites, current, 7);                              //move pointer to sprite in datafile
                    if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                    else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                    current = 7;
				}
			}
		}

		else if (p2Right == TRUE)
		{
            if (key[KEY_RIGHT])
            {
                if (framecounter < 24 && framecounter > 0)
                {
                    blit((BITMAP*)P2.Sprites[2].dat,char_buf, 0, 0, 0, 0, 1024, 768);          //select bitmap if walking right
                    dif = datafileSelect(P2.Sprites, current, 2);                              //move pointer to sprite in datafile
                    if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                    else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                    current = 2;
                }
                else
                {
                    blit((BITMAP*)P2.Sprites[0].dat,char_buf, 0, 0, 0, 0, 1024, 768);          //select bitmap if walking right and over 23 frames
                    dif = datafileSelect(P2.Sprites, current, 0);                              //move pointer to sprite in datafile
                    if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                    else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                    current = 0;
                }
            }
            else
            {
                blit((BITMAP*)P2.Sprites[0].dat,char_buf, 0, 0, 0, 0, 1024, 768);              //select bitmap if idle facing right
                dif = datafileSelect(P2.Sprites, current, 0);                                  //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                current = 0;
            }
        }

		else if (p2Left == TRUE)
		{
            if(key[KEY_LEFT])
            {
                if (framecounter < 24 && framecounter > 0)
                {
                    blit((BITMAP*)P2.Sprites[3].dat,char_buf, 0, 0, 0, 0, 1024, 768);      //select bitmap if walking left
                    dif = datafileSelect(P2.Sprites, current, 3);                          //move pointer to sprite in datafile
                    if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                    else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                    current = 3;
                }

                else
                {
                    blit((BITMAP*)P2.Sprites[1].dat,char_buf, 0, 0, 0, 0, 1024, 768);     //select bitmap if walking left and over 23 frames
                    dif = datafileSelect(P2.Sprites, current, 1);                         //move pointer to sprite in datafile
                    if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                    else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                    current = 1;
                }
            }
            else
            {
                blit((BITMAP*)P2.Sprites[1].dat,char_buf, 0, 0, 0, 0, 1024, 768);    //select bitmap if idle facing left
                dif = datafileSelect(P2.Sprites, current, 1);                        //move pointer to sprite in datafile
                if (dif > 0) for (i = 0; i < dif; i++) P2.Sprites++;
                else if (dif < 0) for (i = 0; i < dif; i++) P2.Sprites--;
                current = 1;
            }
        }
    }
    name = get_datafile_property(P2.Sprites, DAT_ID('X','S','I','Z'));               //retrieve width property from datafile
    width = atoi(name);
    name = get_datafile_property(P2.Sprites, DAT_ID('Y','S','I','Z'));               //retrieve height property from datafile
    height = atoi(name);
    blit(char_buf, temp, 0, 0, (505 - width), (250 - height), 1024, 768);            //blit select sprite to buffer, then destroy temporary bitmap
    destroy_bitmap(char_buf);
    dist = width;
    return (dist);                                                                   //return width
}END_OF_FUNCTION(Player2_SpriteSelect);

/*returns the actual height of the current sprite for player 2*/
int P2_get_height(Character P2, BITMAP *temp, int dist){
    const char* name;
    name = get_datafile_property(P2.Sprites, DAT_ID('Y','S','I','Z'));   //retrive property from datafile
    dist = atoi(name);
    return (dist);                                                       //return height
}END_OF_FUNCTION(P2_get_height);

/*returns distance from datafile pointer to desired sprite in datafile*/
int datafileSelect(DATAFILE* Player, int current, int sprite_select)
{
    int dif = 0;                         //declare local variable
    if (current != sprite_select)
    {
        dif = sprite_select - current;   //calculate difference
    }
    return (dif);                        //return difference
}END_OF_FUNCTION(datafileSelect);

/*calculates number of health bars to be drawn*/
int health_interface(int Health, int max_health)
{
    //declare local variable
    int bars;

    //calculate then return the number of bars to be drawn
    bars = (int)((Health*50)/ max_health);
    if (bars == 0 && Health != 0)   bars = 1;
    return(bars);
}END_OF_FUNCTION(health_interface);

/*calculates number of mana bars to be drawn*/
int mana_interface(int Mana, int max_mana)
{
    //declare local variable
    int bars;

    //calculate then return the number of bars to be drawn
    bars = ((Mana*50)/ max_mana);
    return(bars);
}END_OF_FUNCTION(mana_interface);

/*displays the win screen and allows the player to play again*/
int end_sprite_select(BITMAP* end_screen, int p1Health, int p2Health)
{
    //declare local variable
    int score = 0;

    if (end_screen != NULL)
    {
        destroy_bitmap(end_screen);                                  //clear buffer
    }

    if (p1Health == 0)
    {
        if (p2Health == 0)
        {
            end_screen = load_bitmap("null_win.bmp", NULL);          //load tie win screen
            score = 1;
        }

        else
        {
            end_screen = load_bitmap("p2_win.bmp", NULL);            //load player 2 win screen
            score = 1;
        }
    }

    else if (p2Health == 0)
    {
        end_screen = load_bitmap("p1_win.bmp", NULL);                //load player 1 win screen
        score = 1;
    }

    if (end_screen != NULL) masked_blit(end_screen, screen, 0, 0, 0, 0, 1024, 768);   //blit end screen if available

    return (score);                                                                   //return game finished value
}END_OF_FUNCTION(end_sprite_select);
