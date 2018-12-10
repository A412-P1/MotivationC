#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MIN(x, y) (x < y ? x : y)
#define days 30
#define RATING_INTERVAL 7

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
    int rating;
} user_stats;

/* Fraction Types*/
typedef enum {residual, paper, plastic, metal} fractiontype;

/* PROTOTYPES*/
int load_wastedata(char * file, fraction_state *waste_data);
int new_input(int argc, char const *argv[], fraction_state *waste_data, int s);
int IsToday(date date);
void UpdateEntry(fractiontype fraction, int weight, fraction_state *waste_data, int s);
void CreateEntry(fractiontype fraction, int weight, fraction_state *waste_data, int s);
void AddWasteData(fractiontype fraction, int weight, fraction_state *waste_data, int s);
void ShiftData(fraction_state *waste_data, int s);
void AddDate(date *date);
void resetdata(fraction_state *waste_data);
void motivational_modules(fraction_state *wastedata, int s);
void scoreboard(fraction_state *wastedata, int s);
double sorted_garbage_percentage(fraction_state *waste_data, int s);
void ShiftRating(user_stats *rating, int s);
double Rating_Points(double SGP);
int Rating_Decay(int rating);
int load_userstats(char * file, user_stats *rating);
int time_for_rating(void);
int new_rating(user_stats *rating, int k, fraction_state *waste_data, int s);
void save_userstats(char * file, const user_stats *rating, int s);
void save_wastedata(char * file, const fraction_state *waste_data, int s);
void print_all(fraction_state *waste_data, int s);
void load_toFraction(const char * fraction, const char * weight);
int WhichFractionType(const char * fraction);
int is_residual(const char * fraction);
int is_plastic(const char * fraction);
int is_metal(const char * fraction);
int is_paper(const char * fraction);



/* 	Load Waste_Data from Save
	Add new input - if any - to Waste_Data
	Run motivational modules
	Save Waste_Data
	Exit. */
int main(int argc, char const *argv[]){
	int s; /* Size of Waste Data */
	fraction_state *waste_data = malloc(days * sizeof(fraction_state));
	s = load_wastedata("save", waste_data);
	s = new_input(argc, argv, waste_data, s);
	motivational_modules(waste_data, s);
	save_wastedata("save", waste_data, s);
	free(waste_data);
	return 0;
}

/*	Load saved data to Waste_Data array, then return amount of entries */
int load_wastedata(char * file, fraction_state *waste_data){
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

/* 	Add Input to Waste_Data
	*/
int new_input(int argc, char const *argv[], fraction_state *waste_data, int s){
	int weight;
	fractiontype fraction;
	if (argc != 3) return s;
	fraction = WhichFractionType(argv[1]);
	weight = atoi(argv[2]);

	if (IsToday(waste_data[0].date)){
		UpdateEntry(fraction, weight, waste_data, s);
		return s;
	} else {
		CreateEntry(fraction, weight, waste_data, s);
		return s + 1;
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

/* Update Lastest Entry in WasteData Latest Entry == Current Date */
void UpdateEntry(fractiontype fraction, int weight, fraction_state *waste_data, int s){
	AddWasteData(fraction, weight, waste_data, s);
}

/* Create New Entry in WasteData if Current Date != Latest Entry*/
void CreateEntry(fractiontype fraction, int weight, fraction_state *waste_data, int s){
	ShiftData(waste_data, s);
	AddDate(&waste_data[0].date);
    resetdata(waste_data);
	AddWasteData(fraction, weight, waste_data, s);
}

/* Check fraction type, then add weigth to fraction */
void AddWasteData(fractiontype fraction, int weight, fraction_state *waste_data, int s){
	switch(fraction){
		case residual:	
			waste_data[0].residual += weight;
			printf("Added %d weight to Residual. Total = %d\n", weight, waste_data[0].residual); 
			break;
		case paper:		
			waste_data[0].paper += weight;
			printf("Added %d weight to Paper. Total = %d\n", weight, waste_data[0].paper); 
			break;
		case plastic:	
			waste_data[0].plastic += weight;
			printf("Added %d weight to Plastic. Total = %d\n", weight, waste_data[0].plastic); 
			break;
		case metal:		
			waste_data[0].metal += weight;
			printf("Added %d weight to Metal. Total = %d\n", weight, waste_data[0].metal); 
			break;
	}
}

/* Shift WasteData, so element 0 is the latest entry */
void ShiftData(fraction_state *waste_data, int s){
	while(s > 0){
		waste_data[s] = waste_data[s-1];
		s--;
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

/* Reset Data on CreateEntry*/
void resetdata(fraction_state *waste_data){
    waste_data[0].residual = 0;
    waste_data[0].paper = 0;
    waste_data[0].plastic = 0;
    waste_data[0].metal = 0;
}

/* Check fractiontype of Input*/
int WhichFractionType(const char * fraction){
	
	if (is_residual(fraction)) return residual;
	if (is_plastic(fraction)) return plastic;
	if (is_metal(fraction)) return metal;
	if (is_paper(fraction)) return paper;
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

/* Print Content of all Fractions and date for each entry in WasteData */
void print_all(fraction_state *waste_data, int s){
	int i;
	for (i = 0; i < s; ++i){
		printf("%d.%d.%d | residual: %d, paper: %d, plastic: %d, metal: %d.\n",
			waste_data[i].date.day,
			waste_data[i].date.month,
			waste_data[i].date.year,
			waste_data[i].residual,
			waste_data[i].paper,
			waste_data[i].plastic,
			waste_data[i].metal);
	}
}

/* All motivational modules*/
void motivational_modules(fraction_state *waste_data, int s){
	scoreboard(waste_data, s);
}

/* */
void scoreboard(fraction_state *waste_data, int s){
    user_stats *rating = malloc(RATING_INTERVAL * sizeof(user_stats));
    int k = load_userstats("user.stats", rating);
    if (time_for_rating()) k = new_rating(rating, k, waste_data, s);
    save_userstats("user.stats", rating, k);
    free(rating);
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
            fscanf(savefile, " %d.%d.%d | SGP: %lf, RATING: %d.",
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
int time_for_rating(void){

	return 1;
}

/* Calculate New Rating */
int new_rating(user_stats *rating, int k, fraction_state *waste_data, int s){
    double SGP = sorted_garbage_percentage(waste_data, s);
    int LastRating = (k == 0) ? 0 : rating[0].rating;
	ShiftRating(rating, k);
	AddDate(&rating[0].date);
    rating[0].SGP = SGP;
    rating[0].rating = MIN(100, LastRating + Rating_Points(SGP) - Rating_Decay(rating[1].rating));
    return k + 1;
}

/* Calculate Sorted Garbage Percentage in timeperiod RATING_INTERVAL */
double sorted_garbage_percentage(fraction_state *waste_data, int s){
	int i;
	double sorted_garbage;
	double total_garbage;
	for (i = 0; i < RATING_INTERVAL && i < s; ++i){
		sorted_garbage += waste_data[i].paper + waste_data[i].plastic + waste_data[i].metal;
		total_garbage += waste_data[i].residual + waste_data[i].paper + waste_data[i].plastic + waste_data[i].metal;
	}
	return (sorted_garbage/total_garbage * 100.00);
}

/* Shift Rating, so element 0 is the latest rating */
void ShiftRating(user_stats *rating, int s){
	while(s > 0){
		rating[s] = rating[s-1];
		s--;
	}
}

/* Points based on Sorted Garbage Percentage */
double Rating_Points(double SGP){
	double points = 0;
	if (SGP < 15){
		points = 0;
	} else if (SGP < 30){
		points = 4;
	}  else if (SGP < 50){
		points = 6;
	}  else if (SGP < 60){
		points = 8;
	}  else {
		points = 10;
	}
	return points;
}

/* Point Decay based on rating */
int Rating_Decay(int rating){
	int decay = 0;
	if (rating < 19){
		decay = 0;
	} else if (rating < 40){
		decay = 2;
	}  else if (rating < 60){
		decay = 4;
	}  else if (rating < 80){
		decay = 6;
	}  else {
		decay = 8;
	}
	return decay;
}

void save_userstats(char * file, const user_stats *rating, int s){
	int i = 0;
	FILE *savefile = fopen(file, "w");

	while(i < s){
		fprintf(savefile, "%d.%d.%d | SGP: %.2lf, RATING: %d.\n",
                rating[i].date.day,
                rating[i].date.month,
                rating[i].date.year,
                rating[i].SGP,
                rating[i].rating);
		i++;
	}
	fclose(savefile);
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