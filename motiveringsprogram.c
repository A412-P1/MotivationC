#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define EU_RIS 7 * (100/18)
#define EU_SORT 11 * (100/18)
#define days 30
#define rating_interval 7

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
} saveinfo;

typedef enum {residual, paper, plastic, metal} fractiontype;

int load(char * file, saveinfo *waste_data);
int new_input(int argc, char const *argv[], saveinfo *waste_data, int s);
int IsToday(date date);
void UpdateEntry(fractiontype fraction, int weight, saveinfo *waste_data, int s);
void CreateEntry(fractiontype fraction, int weight, saveinfo *waste_data, int s);
void AddWasteData(fractiontype fraction, int weight, saveinfo *waste_data, int s);
void ShiftData(saveinfo *waste_data, int s);
void AddDate(saveinfo *waste_data);
void resetdata(saveinfo *waste_data);
void motivation_modules(saveinfo *wastedata, int s);
void scoreboard(saveinfo *wastedata, int s);
double sorted_garbage_percentage(saveinfo *waste_data, int s);
int SGP_points(double SGP);
void save(char * file, const saveinfo *waste_data, int s);
void print_all(saveinfo *waste_data, int s);
void load_toFraction(const char * fraction, const char * weight);
int WhichFractionType(const char * fraction);
int is_residual(const char * fraction);
int is_plastic(const char * fraction);
int is_metal(const char * fraction);
int is_paper(const char * fraction);


int main(int argc, char const *argv[]){
	int s;
	saveinfo *waste_data = malloc(days * sizeof(saveinfo));
	s = load("save", waste_data);
	s = new_input(argc, argv, waste_data, s);

	motivation_modules(waste_data, s);
	
	save("save", waste_data, s);
	free(waste_data);
	return 0;
}

int load(char * file, saveinfo *waste_data){
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

int new_input(int argc, char const *argv[], saveinfo *waste_data, int s){
	int weight;
	fractiontype fraction;
	if (argc != 3) return 0;

	weight = atoi(argv[2]);
	fraction = WhichFractionType(argv[1]);
	if (IsToday(waste_data[0].date)){
		UpdateEntry(fraction, weight, waste_data, s);
		return s;
	} else {
		CreateEntry(fraction, weight, waste_data, s);
		return s + 1;
	}
	return s;
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

void UpdateEntry(fractiontype fraction, int weight, saveinfo *waste_data, int s){
	AddWasteData(fraction, weight, waste_data, s);
}

void CreateEntry(fractiontype fraction, int weight, saveinfo *waste_data, int s){
	ShiftData(waste_data, s);
	AddDate(waste_data);
    resetdata(waste_data);
	AddWasteData(fraction, weight, waste_data, s);
}

void AddWasteData(fractiontype fraction, int weight, saveinfo *waste_data, int s){
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

void ShiftData(saveinfo *waste_data, int s){
	while(s > 0){
		waste_data[s] = waste_data[s-1];
		s--;
	}

}

void AddDate(saveinfo *waste_data){
	time_t now = time(NULL);
  	struct tm *t = localtime(&now);
  	waste_data[0].date.day = t->tm_mday,
  	waste_data[0].date.month = t->tm_mon+1,
  	waste_data[0].date.year = t->tm_year+1900;
}

void resetdata(saveinfo *waste_data){
    waste_data[0].residual = 0;
    waste_data[0].paper = 0;
    waste_data[0].plastic = 0;
    waste_data[0].metal = 0;
}

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

void print_all(saveinfo *waste_data, int s){
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

void motivation_modules(saveinfo *waste_data, int s){
	scoreboard(waste_data, s);
}

void scoreboard(saveinfo *waste_data, int s){
	double SGP = sorted_garbage_percentage(waste_data, s);
	int total_points = 0;
	total_points += SGP_points(SGP);
	printf("Sorteringsforhold: %.2lf%\n", SGP);
	printf("Total Point = %d\n", total_points);


}

double sorted_garbage_percentage(saveinfo *waste_data, int s){
	int i;
	double sorted_garbage;
	double total_garbage;
	for (i = 0; i < rating_interval && i < s; ++i){
		sorted_garbage += waste_data[i].paper + waste_data[i].plastic + waste_data[i].metal;
		total_garbage += waste_data[i].residual + waste_data[i].paper + waste_data[i].plastic + waste_data[i].metal;
	}
	return sorted_garbage/total_garbage * 100;
}

int SGP_points(double SGP){
	int points = 0;

	if (SGP < 15){
		points += 2;
	} else if (SGP < 30){
		points += 4;
	}  else if (SGP < 50){
		points += 6;
	}  else if (SGP < 60){
		points += 8;
	}  else {
		points += 10;
	}
	return points;
}



void save(char * file, const saveinfo *waste_data, int s){
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
