#include <stdio.h>
#include <stdlib.h>
#include <string.h>  
#include <strings.h>  

#define BUFLEN 100

typedef struct WidthHeight{
	int width;
	int height;
} WidthHeight;

typedef struct Move{
	char direction;
	int steps;
} Move;

typedef struct Coord{
	int x;
	int y;
} Coord;

typedef struct Robot{
	struct Coord *coords;
	Coord ** vectors;
	int vector_h;
	int vector_w;
	int numberOfMoves;
	int steps;
	int covers;
	int id;
	int max_x;
	int min_x;
	int max_y;
	int min_y;
} Robot;

typedef struct MergedRobot{
	Coord ** vectors;
	int vector_h;
	int vector_w;
	int covers;
	int max_x;
	int min_x;
	int max_y;
	int min_y;
	int *ids;
}MergedRobot;

//global variables
int width = 0;
int height = 0;
int robotsNumber = 0;
int toCover = 0;
int robotCovers = 0;
Move * moves;
int max_x = 0;
int min_x = 0;
int max_y = 0;
int min_y = 0;
int limit = 0;
int vector_w = 0;
int vector_h = 0;



Coord *  moveCoords(int x, int y, Coord * coords, int covers){
	int i = 0;
	struct Coord * coordsTMP = malloc(covers * sizeof * coordsTMP);
	memcpy(coordsTMP, coords, covers * sizeof(Coord));

	for(i = 0; i < covers; i++){
		coordsTMP[i].x += x;
		coordsTMP[i].y += y;
	}

	return coordsTMP;
}

int compareCoords(Coord * cords1, Coord *cords2, int h1, int h2){

	int k = 0;
	int m = 0;

	for(m = 0 ; m < h1; m++){
		for(k = 0 ; k < h2; k++){
			if(cords1[m].x == cords2[k].x && cords1[m].y == cords2[k].y){
				return 0;
			}
		}	
	}
	return 1;
}

MergedRobot mergeRobots(Robot1, Robot2){
	
}

int checkIfCollides(Coord ** vectors1, 	Coord ** vectors2, int h1, int w1, int h2, int w2){
	int i = 0;
	int j = 0;
	for (i = 0; i < w1 ; i++){
		for(j = 0 ; j < w2; j++){
			if ( compareCoords(vectors1[i], vectors2[j], h1, h2) == 1){
				return 0;
			}
		}
	}
	return 1;
}


struct Robot setVectors(Robot robot){
	int i = 0;
	double **table;
	int possibilities = (robot.max_x - robot.min_x + 1) * (robot.max_y - robot.min_y + 1);
	int x = 0;
	int y = 0;
	int v = 0;

	robot.vectors =  (Coord **) malloc ( possibilities * sizeof (Coord *) );
	for (i = 0; i < possibilities; i++){
		robot.vectors[i] = (Coord*) malloc(robot.covers * sizeof (Coord));
	}
	vector_w = possibilities;
	vector_h = robot.covers;

	for(x = robot.min_x; x <= robot.max_x; x++){
		for(y = robot.min_y; y <= robot.max_y; y++){
			robot.vectors[v++] = moveCoords(x , y, robot.coords, robot.covers);
		}
	}

	return robot;
}

int minRobotsNumber(Robot *robots){
	int i = 0;
	int covers = 0;
	int howMany = 1;
	int found = 0;

	for(i = robotsNumber -1 ; i >= 0; i--){
		covers += robots[i].covers ;
		if(covers < toCover){
			howMany++;
		}else{
			if(robots[i].covers < (toCover / howMany) ){
				limit = i + 1;
				return howMany == 1 ? 1 : howMany-1;
			}
		}
	}
}

//sorts robots according to how many fields robot covers
void QuickSort(Robot *T, int Lo, int Hi){
   int i,j;
   int x;
   x = T[(Lo+Hi)>>1].covers;
   i = Lo;
   j = Hi;
   do
   {
      while (T[i].covers < x) ++i;
      while (T[j].covers > x) --j;
      if (i<=j)
      {
         Robot tmp = T[i];
         T[i] = T[j];
         T[j] = tmp;
         ++i; --j;
      }
   } while(i < j);
   if (Lo < j) QuickSort(T, Lo, j);
   if (Hi > i) QuickSort(T, i, Hi);
}

//Check if first line is ok and extracts width and height
struct WidthHeight checkAndGetWidthHight(char * buf){

	int i = 0;
	int j = 0;
	int width = 0; //width == 0 -> start, width == 1 extracting width, width == 2 width extracted
	int height = 0; //height == 0 -> start, height == 1 extracting height, height == 2 height extracted

	char charWidth[BUFLEN];
	bzero(charWidth, BUFLEN);

	char charHeight[BUFLEN];
	bzero(charHeight, BUFLEN);

	for(i = 0; i < strlen(buf); i++){
		if(buf[i] != ' '){
			if(isdigit(buf[i])){
				//extracting width
				if(width == 0 || width == 1){
					charWidth[j++] = buf[i];	
					width = 1;
				} 

				//when width extracted (width ==2) -> extracting height
				if(width == 2 && (height == 0 || height == 1)){
					charHeight[j++] = buf[i];
					height = 1;
				}
			}
			//checks if other character than digit or '\n'
			else if(buf[i] != '\n'){ 
				printf("There should be only digits in first line but is: %c\n", buf[i]);
				exit(EXIT_FAILURE);
			}

			//checks if two arguments in line
			if(width == 2 && height == 2 && buf[i] != '\n'){
				printf("There should be two arguments in first line, width and height\n");
				exit(EXIT_FAILURE);
			}
		}
		else{
			//width extracted: width = 2;
			 if (width == 1){
				width = 2;
				j = 0;
			 }
			//height extracted: height =2;
			 if (height == 1) height = 2;
		}
	}

	WidthHeight widthHeight;
	widthHeight.width=atoi(charWidth); //must be a number checked earlier
	widthHeight.height=atoi(charHeight); //must be a number checked earlier
	return widthHeight;
}

//Checks if line is ok and extracts single number in line
int checkAndExtractInt(char * buf){
	char singleIntChar[BUFLEN];
	bzero(singleIntChar, BUFLEN);

	int i = 0;
	int j = 0;
	int singleInt = 0;//singleInt == 0 -> start, singleInt == 1 extracting singleInt, singleInt == 2 singleInt extracted

	for(i = 0; i < strlen(buf); i++){
		if(buf[i] != ' '){
			if(isdigit(buf[i])){
				//extracting singleInt
				if(singleInt == 0 || singleInt == 1){
					singleIntChar[j++] = buf[i];
					singleInt = 1;
				}
			}
			//checks if other character than digit or '\n'
			else if(buf[i] != '\n'){ 
				printf("There should be only a digit in second line but is: %c\n", buf[i]);
				exit(EXIT_FAILURE);
			}

			//checks if one argument in line
			if(singleInt == 2 && buf[i] != '\n'){
				printf("There should be one argument in second line, tosingleInt\n");
				exit(EXIT_FAILURE);
			}
		}
		else{
			//singleInt extracted: singleInt =2;
			if(singleInt == 1){
				singleInt = 2;
			}
		}
	}
	return atoi(singleIntChar);
}

//Extracts first Three lines and saves width, height, robotsNumber, toCover to global variables
void extractFirstThree(FILE *file){
	char buf[BUFLEN];

	if(fgets(buf, BUFLEN, file) == NULL){
   		perror("Error while reading line");
   		exit(EXIT_FAILURE);
	}
   	WidthHeight widthHeight = checkAndGetWidthHight(buf);
	width = widthHeight.width;
	height = widthHeight.height;

	if(fgets(buf, BUFLEN, file) == NULL){
   		perror("Error while reading line");
   		exit(EXIT_FAILURE);
    }
	toCover = checkAndExtractInt(buf);

	if(fgets(buf, BUFLEN, file) == NULL){
   		perror("Error while reading line");
   		exit(EXIT_FAILURE);
	}
	robotsNumber = checkAndExtractInt(buf);
}

//Checks if line is ok and extracts Move: direction(char) and steps(int)
struct Move checkAndGetMove(char * buf){

	int i = 0;
	int j = 0;

	char direction = '0' ; 
	int steps = 0; //steps == 0 -> start, steps == 1 extracting steps, steps == 2 steps extracted

	char charSteps[BUFLEN];
	bzero(charSteps, BUFLEN);

	for(i = 0; i < strlen(buf); i++){
		if(buf[i] != ' '){
			if(direction == '0'){
				if(buf[i] == 'W' || buf[i] == 'E' || buf[i] == 'S' || buf[i] == 'N'){
					direction = buf[i];
				}else{
					printf("Direction sholud be W or E or S or W but is: %c\n", buf[i]);
					exit(EXIT_FAILURE);
				}	
			}else{
				if(steps == 0 || steps == 1){
					if(isdigit(buf[i])){
						charSteps[j++] = buf[i];	
						steps = 1;
					}else if(buf[i] != '\n'){
						printf("Steps should be a number but one of digits is: %c\n", buf[i]);
						exit(EXIT_FAILURE);
					}
				} 
			}
			//checks if other character than digit or '\n'
			if(buf[i] != '\n' && direction != '0' && steps == 2){ 
				printf("There should be only digits in first line but is: %c\n", buf[i]);
				exit(EXIT_FAILURE);
			}

		}else if(steps == 1){
			steps = 2;
		}
	}

	Move move;
	move.direction = direction; 
	move.steps=atoi(charSteps); //must be a number checked earlier
	return move;
}

//Checks if given coordinates are already in coords array
int isCordIn(Coord coord, Coord * coords, int coordsLength){
	int i = 0;
	for(i = 0; i < coordsLength ; i++){
		if(coords[i].x == coord.x && coords[i].y == coord.y){
			return 0;
		}
	}
	return 1;
}

//Extracts borders from min_x, max_x, max_y, min_y
void extractBorders(){
	max_x = (height - 1) - max_x;
	min_x = 0 - min_x;
	max_y = (width - 1) - max_y;
	min_y = 0 - min_y;
}

//Counts coordinates from moves
struct Coord * getCoords(struct Move *moves, int numberOfMoves, int count){
	Coord *coords = malloc( count* sizeof * coords);
	Coord coord; 
	coord.x = 0;
	coord.y = 0;
	int i = 0;
	int j = 0;
	int c = 0;
	coords[c++] = coord;
	robotCovers = 1;
	max_x = 0;
	min_x = 0;
	max_y = 0;
	min_y = 0;

	for(i = 0 ; i < numberOfMoves; i++){
		switch (moves[i].direction) {
	     case 'E' :
	     	for(j = 0; j < moves[i].steps; j++){
	     		coord.y += 1;
	     		if( isCordIn(coord, coords, count  ) ){
	     			coords[c++] = coord;
	     	/*		printf("coord  x %d  ", coords[c - 1].x);
	     			printf("coord  y %d\n", coords[c - 1].y);*/
	     			robotCovers++;
	     			if(max_y < coord.y ){
	     				max_y = coord.y;
	     			}
	     		}
	     	}
	        break;
	     case 'W' :
	     	for(j = 0; j < moves[i].steps; j++){
	     		coord.y -= 1;
				if( isCordIn(coord, coords, count ) ){
	     			coords[c++] = coord;
				/*	printf("coord  x %d  ", coords[c - 1].x);
	     			printf("coord  y %d\n", coords[c - 1].y);*/
	     			robotCovers++;
	     			if(min_y > coord.y){
	     				min_y = coord.y;
	     			}
				}
	     	}
	        break;
	     case 'N' :
	     	for(j = 0; j < moves[i].steps; j++){
	     		coord.x -= 1;
				if( isCordIn(coord, coords, count ) ){
	     			coords[c++] = coord;
				/*	printf("coord  x %d  ", coords[c - 1].x);
	     			printf("coord  y %d\n", coords[c - 1].y);*/
	     			robotCovers++;
	     			if(min_x > coord.x){
	     				min_x = coord.x;
	     			}
				}
	     	}
	     	break;
	     case 'S':
	     	for(j = 0; j < moves[i].steps; j++){
	     		coord.x += 1;
				if( isCordIn(coord, coords, count ) ){
	     			coords[c++] = coord;
				/*	printf("coord  x %d ", coords[c - 1].x);
	     			printf("coord  y %d\n", coords[c - 1].y);*/
	     			robotCovers++;
	     			if(max_x < coord.x){
	     				max_x = coord.x;
	     			}
				}
	     	}
	        break;
	    default :
	    	printf("Shouln't have happend\n");
	    	exit(EXIT_FAILURE);
        	break;
		}
	}
	extractBorders();

	return coords;
}

//Extracts Robot's number, number of moves and moves
struct Robot getRobot(FILE *file){
	int robotNumber = 0;
	int numberOfMoves = 0;
	int i = 0;
	char buf[BUFLEN];
	Robot robot;

	if(fgets(buf, BUFLEN, file) == NULL){
   		perror("Error while reading line");
   		exit(EXIT_FAILURE);
	}
	 
	robot.id = checkAndExtractInt(buf);

	if(fgets(buf, BUFLEN, file) == NULL){
   		perror("Error while reading line");
   		exit(EXIT_FAILURE);
	}
	
	robot.numberOfMoves = checkAndExtractInt(buf);

	moves = realloc(moves, robot.numberOfMoves * sizeof * moves);
	robot.coords = malloc(robot.numberOfMoves * sizeof * robot.coords);

	for(i = 0; i < robot.numberOfMoves; i++){
		if(fgets(buf, BUFLEN, file) == NULL){
	   		perror("Error while reading line");
	   		exit(EXIT_FAILURE);
		}
		moves[i] = checkAndGetMove(buf);
	}

	int count = 1;
	int k = 0;
	for(k = 0; k < robot.numberOfMoves; k++){
		count += moves[k].steps;
	}

	robot.coords = getCoords(moves, robot.numberOfMoves, count);
	robot.steps = count;
	robot.covers = robotCovers;
	robot.min_x = min_x;
	robot.min_y = min_y;
	robot.max_x = max_x;
	robot.max_y = max_y;
	return robot;
}


int main(int argc, char **argv){
	FILE *file;
	
	if(argc != 2){
		printf("Usage: ./main <file>\n");
		exit(EXIT_FAILURE);
	}

	if((file=fopen(argv[1], "r")) == NULL) {
	    perror("Error while openning file");
	    exit(EXIT_FAILURE);
	}

	extractFirstThree(file);
	printf("%d %d \n", width, height);
	printf("%d\n", toCover);
	printf("%d\n", robotsNumber);

	int i = 0;
	int j = 0;

	struct Robot *robots = malloc(robotsNumber * sizeof * robots);

	for(i = 0 ; i < robotsNumber; i++){
		robots[i] = getRobot(file);
	}

	QuickSort(robots, 0, robotsNumber -1);

	for(i = 0 ; i < robotsNumber; i++){
		printf("robots id %d ", robots[i].id );
		printf("covers %d\n", robots[i].covers );

		for(j = 0 ; j < robots[i].covers; j++){
			printf("x %d  ", robots[i].coords[j].x);
			printf("y %d\n", robots[i].coords[j].y);
		}
		printf("x_min %d\n", robots[i].min_x);
		printf("x_max %d\n", robots[i].max_x);
		printf("y_min %d\n", robots[i].min_y);
		printf("y_max %d\n", robots[i].max_y);
	}

	printf("min number: %d\n", minRobotsNumber(robots));
	printf("limit %d\n", limit);

	for(i = 0; i < robotsNumber; i++){
		robots[i] = setVectors(robots[i]);
		robots[i].vector_h = vector_h;
		robots[i].vector_w = vector_w;
	}

	i = 0;
	int a = -1;
	for (i = 0; i < robotsNumber; ++i){
		printf("%d ", robots[i].id);
	}
	printf("\n");

	if(fclose (file) != 0){
		perror("Error while closing file");
		exit(EXIT_FAILURE);
	}
	return 0;
}
	
