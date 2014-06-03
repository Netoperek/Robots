#include <stdio.h>
#include <stdlib.h>
#include <string.h>  
#include <strings.h>  

#define BUFLEN 100

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
	int vector_w;
	int covers;
	int *ids;
	int idIndex;
	int merged;
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

//returns 1 if coords do not collide
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

//merges to vectors in mergedVector arg
void mergeVectors(Coord * mergedVector, Coord * vector1, Coord * vector2, int l1 , int l2){
	int i = 0;
	int j = 0;
	int k = 0;

	for(i = 0; i < l1; i++)
		mergedVector[ k++ ] = vector1[i];

	for(i = 0; i < l2; i++)
		mergedVector[ k++ ] = vector2[i];

}

//merges two robots into MergedRobot
struct MergedRobot mergeRobots(Robot robot1,Robot robot2){
	MergedRobot mergedRobot;
	int i = 0;
	int j = 0;
	mergedRobot.merged = 0;
	mergedRobot.idIndex = 0;
	mergedRobot.covers = -1;

	mergedRobot.vectors =  (Coord **) malloc ( robot1.vector_w * robot2.vector_w * sizeof (Coord *) );
	mergedRobot.vector_w = robot1.vector_w * robot2.vector_w;

	mergedRobot.ids = (int*) malloc( robotsNumber * sizeof(int) );
	mergedRobot.ids[ mergedRobot.idIndex++ ] = robot1.id;
	mergedRobot.ids[ mergedRobot.idIndex++ ] = robot2.id;

	for(i = 0; i < robot1.vector_w; i++){
		for(j = 0; j < robot2.vector_w; j++){
			if ( compareCoords(robot1.vectors[i], robot2.vectors[j], robot1.vector_h, robot2.vector_h) == 1){
				mergedRobot.vectors[ mergedRobot.merged++ ] = (Coord*) malloc( (robot1.covers + robot2.covers ) * sizeof (Coord));
				mergeVectors(mergedRobot.vectors[ mergedRobot.merged - 1 ], robot1.vectors[i], robot2.vectors[j], robot1.vector_h, robot2.vector_h);
				mergedRobot.covers = (robot1.covers + robot2.covers );
			}
		}
	}

	mergedRobot.vectors = (Coord **) realloc (mergedRobot.vectors , mergedRobot.merged * sizeof (Coord *) );

	return mergedRobot;
}

//merges Robot and MergedRobot vectors
void mergeMergedWithRobot(MergedRobot * robot1,Robot robot2){
	int i = 0;
	int j = 0;
	int limit = robot1->merged;
	robot1->merged = 0;
	int k = 0;
	int flag = 0;

	Coord ** vectors = (Coord **) malloc ( robot1->vector_w * robot2.vector_w * sizeof (Coord *) );

	for(i = 0; i < limit; i++){
		for(j = 0; j < robot2.vector_w; j++){
			if ( compareCoords(robot1->vectors[i], robot2.vectors[j], robot1->covers, robot2.vector_h) == 1 ){
				vectors[ robot1->merged++ ] = ( Coord* ) malloc( (robot1->covers + robot2.covers ) * sizeof (Coord));
				mergeVectors(vectors[ robot1->merged - 1 ], robot1->vectors[i], robot2.vectors[j], robot1->covers, robot2.vector_h);
				flag = 1;
			}
		}
	}

	vectors = (Coord **) realloc (vectors , robot1->merged * sizeof (Coord *) );

	if(flag == 1){

		free(robot1->vectors);
		robot1->ids[ robot1->idIndex++ ] = robot2.id;
		robot1->vectors = vectors;
		robot1->covers = (robot1->covers + robot2.covers );

	}else{
		robot1->merged = limit;
	}
}

//checks if to vectors collide - not used in sollution
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

//sorts robots according to how many fields robot covers
void QuickSortM(MergedRobot *T, int Lo, int Hi){
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
         MergedRobot tmp = T[i];
         T[i] = T[j];
         T[j] = tmp;
         ++i; --j;
      }
   } while(i < j);
   if (Lo < j) QuickSortM(T, Lo, j);
   if (Hi > i) QuickSortM(T, i, Hi);
}

// extracts width and height
void getWidhAndHeight(char * buf){

	int i = 0;

	width =  buf[ i++ ] - '0';

	while(buf[i] != ' ')
		width = width * 10 + buf[ i ++ ] - '0';  
	

	i++;
	height = buf[ i++ ] - '0';

	while( i < strlen(buf) && buf[i] != '\n')
		height = height * 10 + buf[ i ++ ] - '0';
	
}

//extracts single number in line
int extractInt(char * buf){
	int i = 0;
	int number = 0;

	number =  buf[ i++ ] - '0';

	while( i < strlen(buf) && buf[i] != '\n')
		number = number * 10 + (buf[ i ++ ] - '0');
	
	return number;
}

//Extracts first Three lines and saves width, height, robotsNumber, toCover to global variables
void extractFirstThree(FILE *file){
	char buf[BUFLEN];

	if(fgets(buf, BUFLEN, file) == NULL){
   		perror("Error while reading line");
   		exit(EXIT_FAILURE);
	}
   	getWidhAndHeight(buf);

	if(fgets(buf, BUFLEN, file) == NULL){
   		perror("Error while reading line");
   		exit(EXIT_FAILURE);
    }
	toCover = extractInt(buf);

	if(fgets(buf, BUFLEN, file) == NULL){
   		perror("Error while reading line");
   		exit(EXIT_FAILURE);
	}
	robotsNumber = extractInt(buf);
}

//Checks if line is ok and extracts Move: direction(char) and steps(int)
struct Move checkAndGetMove(char * buf){

	int i = 0;
	int j = 0;

	char direction = '0' ; 
	int steps = 0; //steps == 0 -> start, steps == 1 extracting steps, steps == 2 steps extracted

	char charSteps[BUFLEN];
	bzero(charSteps, BUFLEN);

	direction = buf[i++];
	i++;
	steps = buf[i++] - '0';

	while( i < strlen(buf) && buf[i] != '\n')
		steps = steps * 10 + (buf[ i ++ ] - '0');

	Move move;
	move.direction = direction; 
	move.steps = steps; //must be a number checked earlier
	return move;
}

//Checks if given coordinates are already in coords array - returns 1 if so
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
	 
	robot.id = extractInt(buf);

	if(fgets(buf, BUFLEN, file) == NULL){
   		perror("Error while reading line");
   		exit(EXIT_FAILURE);
	}
	
	robot.numberOfMoves = extractInt(buf);

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

//Checks if id is in array of id - returns 1 if so
int idIsIn(int id, int * ids, int limit){
	int i = 0;

	for(i = 0; i < limit; i++){
		if(id == ids[i]){
			return 1;
		}
	}
	return 0;
}

//Check if single Robot solves
int checkIfSingleSolves(Robot * robots){
	int i = 0;
	int m = -1;
	for(i = 0 ; i < robotsNumber; i++){
		if(robots[i].covers >= toCover)
			return i;
	}
	return m;
}

//Prints sollution that one robot presents (if one robot solve the puzzle)
void printSollutionForOne(Robot * robots, int m){
	robots[m] = setVectors( robots[m] );
	robots[m].vector_h = vector_h;
	robots[m].vector_w = vector_w;
	printf("1 \n" );
	printf("%d ", robots[m].id);
	printf("%d ", robots[m].vectors[0][0].y + 1);
	printf("%d\n", robots[m].vectors[0][0].x + 1);
	exit(EXIT_SUCCESS);
}

void printSollution(MergedRobot * mergedTable, int m, Robot * robots2){
	int k = 0;
	int j = 0;
	while(k < mergedTable[m].idIndex ){
		printf("%d ",mergedTable[m].ids[k] );
		printf("%d ", mergedTable[m].vectors[0][j].y + 1);
		printf("%d\n", mergedTable[m].vectors[0][j].x + 1);
		j += robots2[ mergedTable[m].ids[ k++ ] - 1 ].covers;
	}
}

int main(int argc, char **argv){
	//FILE *file = stdin;
	FILE * file;
	int i = 0;
	int j = 0;
	int m = 0;
	int flagz = 1;
	int merged = 0;
	int k = 0;

	if(argc != 2){
		printf("Usage: ./main <file>\n");
		exit(EXIT_FAILURE);
	}

	if((file=fopen(argv[1], "r")) == NULL) {
	    perror("Error while openning file");
	    exit(EXIT_FAILURE);
	}

	extractFirstThree(file);

	struct Robot *robots = malloc(robotsNumber * sizeof * robots);
	struct Robot *robots2 = malloc(robotsNumber * sizeof * robots);

	for(i = 0 ; i < robotsNumber; i++){
		robots[i] = getRobot(file);
		robots2[i] = robots[i];
	}

	QuickSort(robots, 0, robotsNumber -1);

	m = checkIfSingleSolves(robots);
	if(m != -1)	printSollutionForOne(robots, m);
	
	//counting vectors for all robots
	for(i = robotsNumber - 1; i >= 0 ; i--){
		robots[i] = setVectors(robots[i]);
		robots[i].vector_h = vector_h;
		robots[i].vector_w = vector_w;
	}

	//merging vectors for all reobots and saving in mergetTable
	MergedRobot * mergedTable = ( MergedRobot* ) malloc( (robotsNumber * (robotsNumber -1) ) * sizeof (MergedRobot));
	for(i = 0; i < robotsNumber ; i++){
		for(j = 0; j < robotsNumber; j++){
			if(i != j){
				mergedTable[ merged++ ] = mergeRobots(robots[i], robots[j]);

			if(mergedTable[ merged - 1 ].covers >= toCover){
				m = merged - 1;
					break;
				}
			}
		}
		if(mergedTable[m].covers >= toCover){
			flagz = 0;
			printf("%d\n", mergedTable[m].idIndex);
			break;
		}
	}

	//merging all vectors till sollution is done;
	while(flagz){
		QuickSortM(mergedTable, 0, merged - 1);
		for(m = merged -1 ; m >= 0; m--){
			for(i = robotsNumber - 1; i >= 0; i--){
				if( mergedTable[m].covers > 0 && !idIsIn(robots[i].id, mergedTable[m].ids, mergedTable[m].idIndex) ){
					MergedRobot * mergedRobotPtr = &mergedTable[m];
					mergeMergedWithRobot(mergedRobotPtr, robots[i]);

					if(mergedTable[m].covers >= toCover) break;
				}
				if(mergedTable[m].covers == -1){
					m = - 1;
					break;
				}
				
			}

			if(mergedTable[m].covers >= toCover) break;
			
		}
		if(mergedTable[m].covers >= toCover) break;
	}


	if(flagz) printf("%d\n", mergedTable[m].idIndex);
	printSollution(mergedTable, m, robots2);

	if(fclose (file) != 0){
		perror("Error while closing file");
		exit(EXIT_FAILURE);
	}

	return 0;
}
	
