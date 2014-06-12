#include <stdio.h>
#include <stdlib.h>
#include <string.h>  

typedef struct Coord{
	int x;
	int y;
} Coord;

typedef struct Robot{
	Coord *coords;
	Coord ** vectors;
	int vector_w;
	int covers;
	int id;
	int max_x;
	int min_x;
	int max_y;
	int min_y;
} Robot;

typedef struct Move{
	char direction;
	int steps;
} Move;

typedef struct MergedRobot{
	Coord ** vectors;
	int vector_w;
	int covers;
	int *ids;
	int idIndex;
	int merged;
}MergedRobot;

int height;
int width;
int robotsNumber;
int toCover;
Robot *robots;
Robot *robots2;
MergedRobot * mergedRobots;
Move *moves;
int max_x = 0;
int min_x = 0;
int max_y = 0;
int min_y = 0;
int robotCovers = 0;
int howMany = 0;
int merged = 0;

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

//extracts width height toCover and robotsNumber
int extractData(){
	int tmp = 1;

	tmp = scanf("%d %d", &width, &height);
	tmp = scanf("%d", &toCover);
	tmp = scanf("%d", &robotsNumber);

	return tmp;
}

//extracts direction and steps and returns it as Move structure
Move getMove(){
	int tmp = 1;

	Move move;
	move.steps = tmp;

	tmp = scanf("\n%c %d", &move.direction, &move.steps);

	return move;	
}

//Extracts borders from min_x, max_x, max_y, min_y
void extractBorders(Robot * robot){
	robot->max_x = (height - 1) - max_x;
	robot->min_x = 0 - min_x;
	robot->max_y = (width - 1) - max_y;
	robot->min_y = 0 - min_y;
}

//Checks if given coordinates are already in coords array - returns 1 if so
int isCordIn(Coord coord, Coord * coords, int coordsLength){
	int i = 0;
	for(i = 0; i < coordsLength ; i++)
		if(coords[i].x == coord.x && coords[i].y == coord.y)
			return 0;

	return 1;
}

//Counts coordinates from moves
void getCoords(Robot * robot, Move *moves, int numberOfMoves, int count){
	robot->coords = malloc( count * sizeof(Coord) );
	Coord coord; 
	coord.x = 0;
	coord.y = 0;
	int i = 0;
	int j = 0;
	int c = 0;
	robot->coords[c++] = coord;
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
	     		if( isCordIn(coord, robot->coords, c  ) ){
	     			robot->coords[c++] = coord;
	     			robotCovers++;
	     			if(max_y < coord.y )
	     				max_y = coord.y;
	     		}
	     	}
	        break;
	     case 'W' :
	     	for(j = 0; j < moves[i].steps; j++){
	     		coord.y -= 1;
				if( isCordIn(coord, robot->coords, c ) ){
	     			robot->coords[c++] = coord;
	     			robotCovers++;
	     			if(min_y > coord.y)
	     				min_y = coord.y;
				}
	     	}
	        break;
	     case 'N' :
	     	for(j = 0; j < moves[i].steps; j++){
	     		coord.x -= 1;
				if( isCordIn(coord, robot->coords, c ) ){
	     			robot->coords[c++] = coord;
	     			robotCovers++;
	     			if(min_x > coord.x)
	     				min_x = coord.x;
				}
	     	}
	     	break;
	     case 'S':
	     	for(j = 0; j < moves[i].steps; j++){
	     		coord.x += 1;
				if( isCordIn(coord, robot->coords, c ) ){
	     			robot->coords[c++] = coord;
	     			robotCovers++;
	     			if(max_x < coord.x)
	     				max_x = coord.x;
				}
	     	}
	        break;
	    default :
	    	printf("Shouln't have happend\n");
	    	exit(EXIT_FAILURE);
        	break;
		}
	}
	robot->coords = realloc(robot->coords, c * sizeof(Coord) );
	robot->covers = c;
	extractBorders(robot);
}

//extracts Robot: id, coords, covers, max_x, max_y, min_x, min_y
Robot extractRobot(){
	int tmp = 1;
	int i = tmp;
	int numberOfMoves = 0;
	int count = 1;
	Robot robot;

	tmp = scanf("%d", &robot.id);
	tmp = scanf("%d", &numberOfMoves);

	moves = realloc( moves, numberOfMoves * sizeof (Move) );

	for(i = 0; i < numberOfMoves; i++){
		moves[i] = getMove();
		count += moves[i].steps;
	}

	Robot *robotPtr = &robot;
	getCoords(robotPtr, moves, numberOfMoves, count);
	robot.covers = robotCovers;

	return robot;
}

//sorts robots according to how many fields robot covers
void QuickSortRobots(Robot *T, int Lo, int Hi){
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
   if (Lo < j) QuickSortRobots(T, Lo, j);
   if (Hi > i) QuickSortRobots(T, i, Hi);
}

//initialize memory and extracts data to robot
void initializeRobots(){
	int i = 0;
	robots = malloc(robotsNumber * sizeof (Robot) ); 
	robots2 = malloc(robotsNumber * sizeof (Robot) );
	howMany = (1 + (robotsNumber -1) ) * 13 / 2;
	mergedRobots =  malloc( howMany * sizeof (MergedRobot));

	for(i = 0 ; i < robotsNumber; i++){
		robots[i] = extractRobot();
		robots2[i] = robots[i];
	}
}

//checks if single robot solves if so returns its id
int checkIfSingleSolves(){
	int i = 0;
	for(i = robotsNumber - 1 ; i >= 0; i--)
		if(robots[i].covers >= toCover)
			return i;
	
	return -1;
}

//moving coords +x and +y and saves to vectors
void  moveCoords(Robot * robot, int x, int y, int v){
	int i = 0;

	for(i = 0; i < robot->covers ; i++){
		robot->vectors[v][i].x = robot->coords[i].x + x;
		robot->vectors[v][i].y = robot->coords[i].y + y;
	}
}

//sets all possible vectors for robot and saves in vectors
void setVectors(Robot * robot){
	int i = 0;
	robot->vector_w = (robot->max_x - robot->min_x + 1) * (robot->max_y - robot->min_y + 1);
	int x = 0;
	int y = 0;
	int v = 0;

	if(robot->vector_w < 0 )robot->vector_w = 0;

	robot->vectors =  (Coord **) malloc ( robot->vector_w * sizeof (Coord *) );
	for (i = 0; i < robot->vector_w; i++){
		robot->vectors[i] = (Coord*) malloc(robot->covers * sizeof (Coord));
	}
	
	for(x = robot->min_x; x <= robot->max_x; x++){
		for(y = robot->min_y; y <= robot->max_y; y++){
			moveCoords(robot, x , y, v++);
		}
	}
	if(v == 0 )	robot->covers = -1;
}

//sets vectors for all robots
void setAllVectors(){
	int i = 0;
	for(i = robotsNumber - 1 ; i >= 0; i--){
		Robot * robotPtr;
		robotPtr = &robots[i];
		setVectors(robotPtr);
	}
}

//returns 1 if coords do not collide
int compareCoords(Coord * cords1, Coord *cords2, int h1, int h2){

	int k = 0;
	int m = 0;

	for(m = 0 ; m < h1; m++)
		for(k = 0 ; k < h2; k++)
			if(cords1[m].x == cords2[k].x && cords1[m].y == cords2[k].y)
				return 0;

	return 1;
}

//merges to vectors in mergedVector arg
void mergeVectors(Coord * mergedVector, Coord * vector1, Coord * vector2, int l1 , int l2){
	int i = 0;
	int k = 0;

	for(i = 0; i < l1; i++)
		mergedVector[ k++ ] = vector1[i];

	for(i = 0; i < l2; i++)
		mergedVector[ k++ ] = vector2[i];
}

//frees memory - robots, moves
void freeMemory(){
	int i = 0;
	int j = 0;
	
	for(i = 0 ; i < robotsNumber; i++){
		for (j = 0; j < robots[i].vector_w; j++){
			free(robots[i].vectors[j]);
		}
		free(robots[i].vectors);
		free(robots[i].coords);
	}

	for(i = 0; i < merged; i++){
		free(mergedRobots[i].ids);
		for(j = 0; j < mergedRobots[i].merged; j++)
			free(mergedRobots[i].vectors[j]);
		free(mergedRobots[i].vectors);
	}



	free(robots);
	free(robots2);
	free(moves);
	free(mergedRobots);
}

//merges two robots into MergedRobot
MergedRobot mergeRobots(Robot robot1,Robot robot2){
	MergedRobot mergedRobot;
	int i = 0;
	int j = 0;
	mergedRobot.merged = 0;
	mergedRobot.idIndex = 0;
	mergedRobot.covers = -1;

	mergedRobot.vectors =   malloc ( robot1.vector_w * robot2.vector_w * sizeof (Coord *) );
	mergedRobot.vector_w = robot1.vector_w * robot2.vector_w;

	mergedRobot.ids =  malloc( robotsNumber * sizeof(int) );
	mergedRobot.ids[ mergedRobot.idIndex++ ] = robot1.id;
	mergedRobot.ids[ mergedRobot.idIndex++ ] = robot2.id;


	for(i = 0; i < robot1.vector_w; i++){
		for(j = 0; j < robot2.vector_w; j++){
			if ( compareCoords(robot1.vectors[i], robot2.vectors[j], robot1.covers, robot2.covers) == 1){
				mergedRobot.vectors[ mergedRobot.merged++ ] = malloc( (robot1.covers + robot2.covers ) * sizeof (Coord));
				mergeVectors(mergedRobot.vectors[ mergedRobot.merged - 1 ], robot1.vectors[i], robot2.vectors[j], robot1.covers, robot2.covers);
				mergedRobot.covers = (robot1.covers + robot2.covers );			
			}
		}
	}

	mergedRobot.vectors =  realloc (mergedRobot.vectors , mergedRobot.merged * sizeof (Coord *) );

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

	Coord ** vectors = malloc (limit * robot2.vector_w * sizeof (Coord*) );

	for(j = 0; j < robot2.vector_w; j++){
		for(i = 0; i < limit; i++){
			if ( compareCoords(robot1->vectors[i], robot2.vectors[j], robot1->covers, robot2.covers) == 1 ){
				vectors[ robot1->merged++ ] = malloc( (robot1->covers + robot2.covers ) * sizeof(Coord) );
				mergeVectors(vectors[ robot1->merged - 1 ], robot1->vectors[i], robot2.vectors[j], robot1->covers, robot2.covers);
				flag = 1;
			}
		}
	}

	if(flag == 1){
		vectors = realloc (vectors , robot1->merged * sizeof ( Coord *) );
		for(k = 0; k < limit; k++){
			free(robot1->vectors[k]);
		}
		free(robot1->vectors);

		robot1->ids[ robot1->idIndex++ ] = robot2.id;
		robot1->vectors = vectors;
		robot1->covers = ( robot1->covers + robot2.covers );
	}else{
		robot1->merged = limit;
		free(vectors);
	}
}

void printSollution(int m){
	int k = 0;
	int j = 0;
	printf("%d\n", mergedRobots[m].idIndex);
	while(k < mergedRobots[m].idIndex ){
		printf("%d ",mergedRobots[m].ids[k] );
		printf("%d ", mergedRobots[m].vectors[0][j].y + 1);
		printf("%d\n", mergedRobots[m].vectors[0][j].x + 1);
		j += robots2[ mergedRobots[m].ids[ k++ ] - 1 ].covers;
	}
}

int mergeInPairs(){
	int i = 0;
	int j = 0;
	int start = 0;
	int m = -1;

	for(i = 0; i < robotsNumber ; i++){
		start++;
		for(j = start; j < robotsNumber; j++){
			mergedRobots[merged++] = mergeRobots(robots[i], robots[j]);	

			if(mergedRobots[ merged - 1 ].covers >= toCover){
				m = merged - 1;
				break;
			}
		}
		if(m != -1 && mergedRobots[m].covers >= toCover){
			break;
		}
	}

	if(m != -1){
		printSollution(m);
		return 1;
	} 
	return -1;
}

//Checks if id is in array of id - returns 1 if so
int idIsIn(int id, int * ids, int limit){
	int i = 0;

	for(i = 0; i < limit; i++)
		if(id == ids[i])
			return 1;
	return 0;
}

void mergeTillSollution(){
	QuickSortM(mergedRobots, 0, merged -1);
	int m = 0;
	int i = 0;

	for(m = merged - 1 ; m >= 0  ; m--){
		for(i = robotsNumber - 1; i >= 0; i--){
			if( mergedRobots[m].covers > 0 && !idIsIn(robots[i].id, mergedRobots[m].ids, mergedRobots[m].idIndex) ){
				MergedRobot * mergedRobotPtr = &mergedRobots[m];
				mergeMergedWithRobot(mergedRobotPtr, robots[i]);

				if(mergedRobots[m].covers >= toCover) break;
			}
			if(mergedRobots[m].covers == -1) break;		
		}
		if(mergedRobots[m].covers >= toCover) break;
	}

	if(mergedRobots[m].covers >= toCover)
		printSollution(m);
	else{
		printf("1\n");
		printf("1 1 1\n");		
	}

}

void printSollutionForOne(int m){
	printf("1 \n" );
	printf("%d ", robots[m].id);
	printf("%d ", robots[m].vectors[0][0].y + 1);
	printf("%d\n", robots[m].vectors[0][0].x + 1);
}

int main(){
	int solved = -1;
	extractData();

	initializeRobots();
	QuickSortRobots(robots, 0, robotsNumber -1);

	setAllVectors();

	solved = checkIfSingleSolves();
	if(solved != -1){
		printSollutionForOne(solved);
		freeMemory();
		return 0;
	}

	solved = mergeInPairs();
	if( solved == 1){
		freeMemory();
		return 0;
	}
	mergeTillSollution();

	//printRobotsCoords();
	//printMerged();
	freeMemory();
	return 0;
}
