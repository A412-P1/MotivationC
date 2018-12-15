#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <windows.h>


#define CHEAT_MODE 1
#define MIN(x, y) (x < y ? x : y)
#define DAYS 30
#define RATING_SIZE 30
#define RATING_INTERVAL 7
#define GRAPH_INTERVAL 30
#define ANIMATE_ENABLE 0
#define LOCAL_USERS 1500
#define WAVELENGHT sqrt(20*20 + 2*2)


typedef struct {
	int day;
	int month;
	int year;
} date;

typedef struct {
	int residual;
	int paper;
	int plastic;
	int metal;
	date date;
} fraction_state;

typedef struct {
    date date;
    double SGP;
    double rating;
} user_stats;



/* Fraction Types*/
typedef enum {RESIDUAL, PAPER, PLASTIC, METAL} fractiontype;

/* Ranks */
typedef enum {BRONZE = 19, SILVER = 39, GOLD = 59, PLATINUM = 79, DIAMOND = 100} RANKINGGROUP;

/* PROTOTYPES*/
int load_wastedata(char * file, fraction_state *waste_data);
void reset_screen(int n);
void userinterface(fraction_state *waste_data, int s);
void set_screenlayout(char screen[29][119]);
int new_input(char *type, int weight, fraction_state *waste_data, int s);
int IsToday(date date);
void update_entry(fractiontype fraction, int weight, fraction_state *waste_data, int s);
void create_entry(fractiontype fraction, int weight, fraction_state *waste_data, int s);
void AddWasteData(fractiontype fraction, int weight, fraction_state *waste_data, int s);
void shift_data(fraction_state *waste_data, int s);
void AddDate(date *date);
void resetdata(fraction_state *waste_data);
void update_screen(char screen[29][119], fraction_state *waste_data, int s);
void performancegraph(char screen[29][119], user_stats *rating, int k);
void histogram(char screen[29][119], fraction_state *waste_data, int s);
double fraction_percentage(fractiontype fraction, fraction_state *waste_data, int s);
int round_number(double input);
int ratingsystem(fraction_state *wastedata, int s, user_stats *rating);
void shift_rating(user_stats *rating, int s);
double rating_points(double x);
double rating_decay(double x);
int load_userstats(char * file, user_stats *rating);
int time_for_rating(user_stats *rating, int k, fraction_state *waste_data, int s);
int new_rating(user_stats *rating, int k, fraction_state *waste_data, int s);
void save_userstats(char * file, const user_stats *rating, int s);
void rankstatistics(char screen[29][119], user_stats *rating);
int rank(char screen[29][119], int rating);
void top_percentage(char screen[29][119], user_stats *rating);
int binarysearch_rating(user_stats *all_user_ratings, int i, int s, double k);
void local_area_sorted(char screen[29][119]);
int load_alluserstats(char * file, user_stats *all_user_ratings);
void load_toFraction(const char * fraction, const char * weight);
int WhichFractionType(const char * fraction);
int is_residual(const char * fraction);
int is_plastic(const char * fraction);
int is_metal(const char * fraction);
int is_paper(const char * fraction);
void delay(unsigned int mseconds);
void trashbin(int space);
void animatetrashbin(int speed, int count, int space);
void save_wastedata(char * file, const fraction_state *waste_data, int s);

/* 	Load Waste_Data from Save
	Run User Interface */
int main(void){
	int s; /* Size of Waste Data */
	fraction_state *waste_data = malloc(DAYS * sizeof(fraction_state));
	s = load_wastedata("save", waste_data);
	userinterface(waste_data, s);
	free(waste_data);
	return 0;
}

/*	Load data from waste fractions to Waste_Data array, then return amount of entries */
int load_wastedata(char * file, fraction_state *waste_data){
	int i = 0,
	n = 0;
	char line[256];
	FILE *savefile = fopen(file, "r");

	if (savefile == NULL){
		printf("No save file detected!\n");
	} else { 

		/* Count Entries */
		while(fscanf(savefile, "%[^\n]\n", line) != EOF){
		n++;
		}

		fseek(savefile, 0, SEEK_SET);

		/* Load DATA*/
		while (i < n){
			fscanf(savefile, " %d.%d.%d | RESIDUAL: %d, PAPER: %d, PLASTIC: %d, METAL: %d.",
				&waste_data[i].date.day,
				&waste_data[i].date.month,
				&waste_data[i].date.year,
				&waste_data[i].residual,
				&waste_data[i].paper,
				&waste_data[i].plastic,
				&waste_data[i].metal);
			i++;
		}
	}
	fclose(savefile);
	return n;
}

/* USER INTERFACE 
   Print Screen then wait for input */
void userinterface(fraction_state *waste_data, int s){
	int weight;
	char type[15];
	char screen[29][119] = {' '};

	/* Set Static Screen Layout */
	set_screenlayout(screen);
	do {

		/* Update Screen with new info on each loop*/
		update_screen(screen, waste_data, s);

		/* Wait for user input*/
		printf(">> ");
		scanf(" %s %d", type, &weight);
		s = new_input(type, weight, waste_data, s);

		/* Animate trashbin */
		if (ANIMATE_ENABLE) animatetrashbin(80, 1, 4);

		/* Backup WasteData */
		save_wastedata("save", waste_data, s);
	} while (strcmp("EXIT", type));
}

void reset_screen(int n){
		printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
}

/* Set Static ScreenLayout */
void set_screenlayout(char screen[29][119]){
	int i;
	int j;
	int x;
	char *performancegraph = "Performance Graph";
	char *performance = "PERFORMANCE";
	char *time = "T I M E";
	char *histogram = "Histogram";
	char *fractiontypes = "Residual   Paper   Plastic   Metal";
	char *rankstats = "Rank Statistics";
	char *rank = "RANK:";

	/* General  Layout*/
	for (j = 0; j < 119; ++j)
	{
		screen[21][j] = '-';
	}

	screen[21][40] = '|';
	screen[21][79] = '|';


	/* Populate Performance Graph Layout*/
	for (j = 0; j < strlen(performancegraph); ++j)
	{
		screen[8][12+j] = performancegraph[j];
	}

	for (i = 0; i < strlen(performance); ++i)
	{
		screen[10+i][3] = performance[i];
		screen[10+i][5] = '|';
	}

	for (j = 0; j < strlen(time); ++j)
	{
		screen[22][15 + j] = time[j];
	}

	/* Populate Histogram Layout*/

	for (j = 0; j < strlen(histogram); ++j)
	{
		screen[8][52+j] = histogram[j];
	}

	for (j = 0; j < strlen(fractiontypes); ++j)
	{
		screen[22][43+j] = fractiontypes[j];
	}

	j = 46;
	for (i = 0; i < strlen(performance); ++i)
	{
		screen[20-i][j] = '|';
		screen[20-i][j+2] = '|';
		screen[20-i][j+9] = '|';
		screen[20-i][j+11] = '|';
		screen[20-i][j+18] = '|';
		screen[20-i][j+20] = '|';
		screen[20-i][j+27] = '|';
		screen[20-i][j+29] = '|';
	}


	/* Populate STATISTICS Layout*/
	for (j = 0; j < strlen(rankstats); ++j)
	{
		screen[8][91+j] = rankstats[j];
	}

	for (i = 0; i < strlen(rank); ++i)
	{
		screen[10][83+i] = rank[i];
	}
}

/*  Update all Screen Modules:
	Performance graph
	Histogram 
	Rankstatistics */
void update_screen(char screen[29][119], fraction_state *waste_data, int s){
	int i;
	int j;
	user_stats *rating = malloc(RATING_SIZE * sizeof(user_stats));

	/* Calculate Rating*/
	int k = ratingsystem(waste_data, s, rating);

	/* Update Screen Modules */
	performancegraph(screen, rating, k);
	histogram(screen, waste_data, s);
	rankstatistics(screen, rating);


	/* Print Screen*/
	for (i = 0; i < 29; ++i)
	{
		for (j = 0; j < 119; ++j)
		{
			printf("%c", screen[i][j]);
		}
		printf("\n");
	}

	free(rating);
}

/*  Calculate Rating:
	Load old ratings,
	Calculate new rating, if timeperiod has passed
	Save new rating */
int  ratingsystem(fraction_state *waste_data, int s, user_stats *rating){
    int k = load_userstats("user.stats", rating);
    if (time_for_rating(rating, k, waste_data, s)) k = new_rating(rating, k, waste_data, s);
    save_userstats("user.stats", rating, k);
    return k;
}

/*	Load userstats from save into ratings array, then return amount of entries */
int load_userstats(char * file, user_stats *rating){
    int i = 0,
    	n = 0;
    char line[256];
    FILE *savefile = fopen(file, "r");

    if (savefile == NULL){
        printf("No save file detected!\n");
    } else { 
        while(fscanf(savefile, "%[^\n]\n", line) != EOF){
        n++;
        }
        fseek(savefile, 0, SEEK_SET);
        while (i < n){
            fscanf(savefile, " %d.%d.%d | SGP: %lf, RATING: %lf.",
                &rating[i].date.day,
                &rating[i].date.month,
                &rating[i].date.year,
                &rating[i].SGP,
                &rating[i].rating);
            i++;
        }
    }
    fclose(savefile);
    return n;
}

/* Check if it is time for rating */
int time_for_rating(user_stats *rating, int k, fraction_state *waste_data, int s){
	int wd_day = waste_data[RATING_INTERVAL-1].date.day;
	int wd_month = waste_data[RATING_INTERVAL-1].date.month;
	int wd_year = waste_data[RATING_INTERVAL-1].date.year;
	int rating_day = rating[0].date.day;
	int rating_month = rating[0].date.month;
	int rating_year = rating[0].date.year;
	if (s < RATING_INTERVAL) return 0;
	return (wd_day == rating_day && wd_month == rating_month && wd_year == rating_year) ? 1 : 0;

}

/* Calculate New Rating */
int new_rating(user_stats *rating, int k, fraction_state *waste_data, int s){
	/* If no previous ratings Lastrating 0, else set to latest rating */
    double LastRating = (k == 0) ? 0 : rating[0].rating;

    /* Calculate Sorted Percentage for the last 7 DAYS */
    double SGP = fraction_percentage(PAPER, waste_data, s) + fraction_percentage(PLASTIC, waste_data, s) + fraction_percentage(METAL, waste_data, s);
    
    /* Shift ratings array so rating[1] = LastRating and Rating[0] = new rating */
    if (k) shift_rating(rating, k);
	
    /* Add Date of Rating calculation to New Rating*/
	AddDate(&rating[0].date);
    rating[0].SGP = SGP;
    rating[0].rating = LastRating + rating_points(SGP) - rating_decay(LastRating);
    return min(k + 1, DAYS);
}

/* Shift Rating, so element 0 is the latest rating */
void shift_rating(user_stats *rating, int s){
	int i = MIN(s, RATING_SIZE-1);
	while(i > 0){
		rating[i] = rating[i-1];
		i--;
	}	
}

/* Points based on Sorted Garbage Percentage */
double rating_points(double x){
	return x/10;
}

/* Point Decay based on last rating */
double rating_decay(double x){
	return (   0.3 * cos((2*x*M_PI)/WAVELENGHT) + x/10 - 0.3  );
}

void save_userstats(char * file, const user_stats *rating, int s){
	int i = 0;
	FILE *savefile = fopen(file, "w");

	while(i < s){
		fprintf(savefile, "%d.%d.%d | SGP: %.2lf, RATING: %.2lf.\n",
                rating[i].date.day,
                rating[i].date.month,
                rating[i].date.year,
                rating[i].SGP,
                rating[i].rating);
		i++;
	}
	fclose(savefile);
}


/* Insert ratings into screen as coordinates */
void performancegraph(char screen[29][119], user_stats *rating, int k){
	int x = 0;
	int y = 0;
	int n = MIN(k, GRAPH_INTERVAL) - 1;
	int interval = 6 + n;

	/* RESET GRAPH AREA*/
	for (x = 6; x < interval; ++x)
	{	
		for (y = 10; y < 21; ++y)
		{
			screen[y][x] = ' ';
		}
	}

	/* FILL GRAPH */
	for (x = 6; x < interval; ++x)
	{	
		y = 20 - round_number(rating[n].rating);
		screen[y][x] = '.';
		n--;
	}
}

/* Histogram Last 7 Days */
void histogram(char screen[29][119], fraction_state *waste_data, int s){

	/* Percentage Garbage for each fraction*/
	double residual_percentage = fraction_percentage(RESIDUAL, waste_data, s),
		   paper_percentage = fraction_percentage(PAPER, waste_data, s),
		   plastic_percentage = fraction_percentage(PLASTIC, waste_data, s),
		   metal_percentage = fraction_percentage(METAL, waste_data, s);

	/* Amount of residual starts pr. percentage*/
	int i,
	    residual_stars = round_number(residual_percentage),
	    paper_stars = round_number(paper_percentage),
	    plastic_stars = round_number(plastic_percentage),
	    metal_stars = round_number(metal_percentage);

	/* Fill stars into screen*/
	for (i = 0; i < 10; ++i)
	{	
		/* */
		screen[20-i][47] = (residual_stars-- > 0) ? '*' : ' ';
     	screen[20-i][56] = (paper_stars-- > 0) ? '*' : ' ';
    	screen[20-i][65] = (plastic_stars-- > 0) ? '*' : ' ';
   		screen[20-i][74] = (metal_stars-- > 0) ? '*' : ' ';
	}
}

/* Calculate fraction percentage for FRACTIONTYPE*/
double fraction_percentage(fractiontype fraction, fraction_state *waste_data, int s){
	int i;
	double	total = 0,
		residual = 0,
		paper = 0,
		plastic = 0,
		metal = 0;

	for (i = 0; i < RATING_INTERVAL && i < s; ++i){
		residual += waste_data[i].residual;
		paper += waste_data[i].paper;
		plastic += waste_data[i].plastic;
		metal += waste_data[i].metal;
		total += residual + paper + plastic + metal;
	}

	if(total == 0) return 0;
/* Return fraction percentage based on fractiontype */
	switch(fraction){
	case RESIDUAL:	
		return residual/total * 100;
		break;
	case PAPER:
		return paper/total * 100;
		break;
	case PLASTIC:
		return plastic/total * 100;
		break;
	case METAL:
		return metal/total * 100;
		break;
	}
}

/* Round number from 10s to 1s */
int round_number(double input){
	return (input / 10 >= 0.5) ? ceil(input / 10) : floor(input / 10); 
}

/* Calculate Rank Statistics*/
void rankstatistics(char screen[29][119], user_stats *rating){

	/* Set RANK */
	rank(screen, rating[0].rating);

	/* Set top percentage of local area */
	top_percentage(screen, rating);

	/* Set local area sorted percentage*/
	local_area_sorted(screen);
}

/* */
int rank(char screen[29][119], int rating){
	int i;
	char rank[10];
	RANKINGGROUP r;

	if (rating < BRONZE)
	{
		strcpy(rank, "BRONZE");
		r = BRONZE;
	} else if (rating < SILVER){
		strcpy(rank, "SILVER");
		r = SILVER;
	} else if (rating < GOLD){
		strcpy(rank, "GOLD");
		r = GOLD;
	} else if (rating < PLATINUM){
		strcpy(rank, "PLATINUM");
		r = PLATINUM;
	} else if (rating < DIAMOND){
		strcpy(rank, "DIAMOND");
		r = DIAMOND;
	}

	for (i = 0; i < 10; ++i)
	{
		screen[10][90+i] = ' ';
	}

	for (i = 0; i < strlen(rank); ++i)
	{
		screen[10][90+i] = rank[i];
	}
	return r;
}

void top_percentage(char screen[29][119], user_stats *rating){
	user_stats *all_user_ratings = malloc(LOCAL_USERS * sizeof(user_stats));
	int k = load_alluserstats("alluser.stats", all_user_ratings);
	int i;
	int s;
	int n;
	double x;
	char percentage[45];

	x = (double) binarysearch_rating(all_user_ratings, 0, LOCAL_USERS-1, rating[0].rating);
	x = ((x+1)/LOCAL_USERS) * 100;

	sprintf(percentage, "You are top %.0lf%% in your local area.", x);

	for (i = 0; i < strlen(percentage); ++i)
	{
		screen[13][83+i] = percentage[i];
	}
	free(all_user_ratings);
}

/* Get index of rating HIR in all_user_ratings from interval [i - s]
   HIR <= k < MIR, where k is the current rating of the user and MIR is the rating
   of the previous of the user just below HIR.
   Assumes list is ordered in descending order.
   */
int binarysearch_rating(user_stats *all_user_ratings, int i, int s, double k){
	int halfindex = (i+s)/2;

	/* Rating of halfindex*/
	double HIR = all_user_ratings[halfindex].rating;

	/* Rating of entry just below halfindex */
	double MIR = all_user_ratings[halfindex - 1].rating;
	
	if (HIR <= k && MIR > k){
		while(HIR == k){
			HIR = all_user_ratings[halfindex++].rating;
		}
		return halfindex;
	} else if (HIR < k){
		return binarysearch_rating(all_user_ratings, i, halfindex, k);
	} else if (k < HIR){
		return binarysearch_rating(all_user_ratings, halfindex, s, k);
	}
}

/* Calculate average sorting percentage for entire local area
   Reuses user_stats struct, but SGP is Average SGP! */
void local_area_sorted(char screen[29][119]){
	user_stats *all_user_ratings = malloc(LOCAL_USERS * sizeof(user_stats));
	int k = load_alluserstats("alluser.stats", all_user_ratings);
	int i;
	double x;
	char percentage[45];

	for (i = 0; i < LOCAL_USERS; ++i)
	{
		x += all_user_ratings[i].SGP;
	}

	x = x/LOCAL_USERS;

	sprintf(percentage, "Your local area sorts %.2lf%%.", x);

	/* Fill percentage into RANK STATISTICS AREA*/
	for (i = 0; i < strlen(percentage); ++i)
	{
		screen[14][83+i] = percentage[i];
	}

	free(all_user_ratings);
}

/*	Load userstats from save into ratings array, then return amount of entries */
int load_alluserstats(char * file, user_stats *all_user_ratings){
    int i = 0,
    	n = 0;
    char line[256];
    FILE *savefile = fopen(file, "r");

    if (savefile == NULL){
        printf("No save file detected!\n");
    } else { 
        while(fscanf(savefile, "%[^\n]\n", line) != EOF){
        n++;
        }
        fseek(savefile, 0, SEEK_SET);
        while (i < n){
            fscanf(savefile, " SGP: %lf, RATING: %lf.",
                &all_user_ratings[i].SGP,
                &all_user_ratings[i].rating);
            i++;
        }
    }
    fclose(savefile);
    return n;
}

/* 	Add Input to Waste_Data 
	Check if date is current date, if:
	TRUE: Add input to latest entry
	FALSE: Create New And add input. */
int new_input(char *type, int weight, fraction_state *waste_data, int s){
	fractiontype fraction = WhichFractionType(type);
	if (fraction == -1) return s;
	if (IsToday(waste_data[0].date) && CHEAT_MODE){
		update_entry(fraction, weight, waste_data, s);
		return s;
	} else {
		create_entry(fraction, weight, waste_data, s);
		return min(s + 1, DAYS);;	
	}
}

/* Check if Date is Today for Most Recent Deposit
Input: Date Struct
Output: True, False*/
int IsToday(date MRD){
	time_t now = time(NULL);
  	struct tm *t = localtime(&now);
  	int day = t->tm_mday,
  		month = t->tm_mon+1,
  		year = t->tm_year+1900;
	return (day == MRD.day && month == MRD.month && year == MRD.year);
}

/* Update Lastest Entry in WasteData Latest Entry*/
void update_entry(fractiontype fraction, int weight, fraction_state *waste_data, int s){
	AddWasteData(fraction, weight, waste_data, s);
}

/* Create New Entry in WasteData if Current Date != Latest Entry*/
void create_entry(fractiontype fraction, int weight, fraction_state *waste_data, int s){
	shift_data(waste_data, s);
	AddDate(&waste_data[0].date);
    resetdata(waste_data);
	AddWasteData(fraction, weight, waste_data, s);
}

/* Check fraction type, then add weigth to fraction */
void AddWasteData(fractiontype fraction, int weight, fraction_state *waste_data, int s){
	switch(fraction){
		case RESIDUAL:	
			waste_data[0].residual += weight;
			break;
		case PAPER:		
			waste_data[0].paper += weight;
			break;
		case PLASTIC:	
			waste_data[0].plastic += weight;
			break;
		case METAL:		
			waste_data[0].metal += weight;
			break;
	}
}

/* Shift WasteData, so element 0 is the latest entry */
void shift_data(fraction_state *waste_data, int s){
	int i = MIN(s, DAYS - 1);
	while(i > 0){
		waste_data[i] = waste_data[i-1];
		i--;
	}
}

/* Add Date to Created Entry*/
void AddDate(date *date){
	time_t now = time(NULL);
  	struct tm *t = localtime(&now);
  	date->day = t->tm_mday,
  	date->month = t->tm_mon+1,
  	date->year = t->tm_year+1900;
}

/* Reset Data on create_entry*/
void resetdata(fraction_state *waste_data){
    waste_data[0].residual = 0;
    waste_data[0].paper = 0;
    waste_data[0].plastic = 0;
    waste_data[0].metal = 0;
}

/* Check fractiontype of Input*/
int WhichFractionType(const char * fraction){
	
	if (is_residual(fraction)) return RESIDUAL;
	if (is_plastic(fraction)) return PLASTIC;
	if (is_metal(fraction)) return METAL;
	if (is_paper(fraction)) return PAPER;
	return -1;
}

int is_residual(const char * fraction){

	return strcmp(fraction, "residual") ? 0 : 1;
}

int is_plastic(const char * fraction){

	return strcmp(fraction, "plastic") ? 0 : 1;
}

int is_metal(const char * fraction){

	return strcmp(fraction, "metal") ? 0 : 1;
}

int is_paper(const char * fraction){

	return strcmp(fraction, "paper") ? 0 : 1;
}

void delay(unsigned int mseconds){
    clock_t goal = mseconds + clock();
    while (goal > clock());
}

/* Trash bin drawing */
void trashbin(int space){
	int i;
	printf(
		"       =========\n"
		" ______||_____||______\n"
		"|_____________________|\n");
	for ( i = 0; i < space; ++i)
	{
		printf("\n");
	}
	if (space)
	{
		printf(" _____________________\n");
	}
	printf(
		"|                     |\n"
		"|  |   |   |   |   |  |\n"
		"|  |   |   |   |   |  |\n"
		"|  |   |   |   |   |  |\n"
		"|  |   |   |   |   |  |\n"
		"|  |   |   |   |   |  |\n"
		"|  |   |   |   |   |  |\n"
		"|  |   |   |   |   |  |\n"
		"|  |   |   |   |   |  |\n"
		"|  |   |   |   |   |  |\n"
		"|  |   |   |   |   |  |\n"
		"|                     |\n"
		"|_____________________|\n");
}

/*  Animate drawing based on speed, Number of times to animate and space between
	Lid and Can */
void animatetrashbin(int speed, int count, int space){
	int i = 0;
	int j = 0;

	reset_screen(30);
	delay(2000);
	for (i = 0; i < count; ++i){
	
		while( j < space ){
			trashbin(j);
			delay(speed);
			reset_screen(30);
			j++;
		}
		while( j > 0){
			trashbin(j);
			delay(speed);
			reset_screen(30);
			j--;
		}
	}
}

void save_wastedata(char * file, const fraction_state *waste_data, int s){
	int i = 0;
	FILE *savefile = fopen(file, "w");

	while(i < s){
		fprintf(savefile, "%d.%d.%d | RESIDUAL: %d, PAPER: %d, PLASTIC: %d, METAL: %d.\n",
			waste_data[i].date.day,
			waste_data[i].date.month,
			waste_data[i].date.year,
			waste_data[i].residual,
			waste_data[i].paper,
			waste_data[i].plastic,
			waste_data[i].metal);
		i++;
	}
	fclose(savefile);
}