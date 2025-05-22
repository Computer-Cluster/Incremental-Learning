/* 
	Tutorial 02
	
Simplest EA (simpleEA.c)
// Victor Landassuri-Moreno
// School of Computer Science
// University of Birmingham
// Oct-2008

// function to optimize : x*x
// population size 10
// size of the genotype --> 11 bits
// this algorithm only consider positive numbers

// this code is only for teaching purpose and must be
// use in the same way!!
// compile as: cc simpleEA.c -o simpleEA
// alternate if troubles: cc simpleEA.c -O -lm -o simpleEA 
// run: ./simpleEA  (depends of the linux version)

// This is the corrected version of the code, form file: simpleEA.c
*/



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

//global variables/////////
int population = 10;
int size_ind = 11;
int toSelect = 10;				//parents to select
int maxGen  = 1000000;
int band = 0;					//change to 1 if you want to show all message in screen
//population 

char Population[10][11];		//the actual population
char Offspring[10][11];			//the new population
double fitness_Pop[10];			//to save the fitness of all individuals
double fitness_Pop_inv[10];		//the inverse of the fitness -> to minimize
///////////////////////////


#define rando() ((double)rand()/RAND_MAX)		//random number

// functions ///////////////
void initpop(void);
void eval_fitness(void);
void print_pop(void);
void print_fitness(void);
void selection(void);
///////////////////////////


int main(void)
{
    int i,j;
    int generation = 0;
    int minimum;
    double min_temp;
    int pos_min;
    
    srand(520);                               //init random
    
    
    //generate initial population
    initpop();
    
    //pirnt initial population
    print_pop();
	
    
    do
    {
        printf("gen %d\n",generation);
        
        
        //evaluate fitness of each individual, only the first time
        eval_fitness();
        

		//find the minimum
        min_temp = fitness_Pop[0];
        pos_min = 0;
        for(i=1; i< population; i++)
        {
            if(fitness_Pop[i] < min_temp)
            {
                min_temp = fitness_Pop[i];
                pos_min = i;
            }
        }
        
        // printf the best individual found    
        printf("fitness best %f\n",min_temp);
        printf("pos min %d\n Best individual found\n",pos_min);
        for(i=0; i<size_ind; i++)
            printf("%c ",Population[pos_min][i]);
        printf("\n");

		if((generation > maxGen) || (min_temp == 0))  
			break;									//stop the algorithm



        //print the fitness of each individual
        if(band==1)print_fitness();
        
        //select parents, apply crossover and mutate
        selection();
        
        //replace the old population by the offspring
        for(i=0; i<population; i++)
        {
            for(j=0; j<size_ind; j++)
            {
                Population[i][j] = Offspring[i][j];
            }
        }
        
        generation++;
    }while(1);		
    
    return 0;
}

void initpop(void)
{
    int i,j;
    double rand_temp;
    for(i=0; i<population; i++)
    {
        for(j=0; j<size_ind; j++)
        {
            rand_temp = rando();
            if (rand_temp < 0.5)
                Population[i][j] = '0';
            else
                Population[i][j] = '1';
        }
    }
            
}

void eval_fitness(void)					//from binary to decimal
{
    int i;
    for(i=0; i<population; i++)
    {
        fitness_Pop[i] = 0;				//clear previous values
        //form binary to dec
        if(Population[i][10] == '1')
            fitness_Pop[i] += 1;
        if(Population[i][9] == '1')
            fitness_Pop[i] += 2;
        if(Population[i][8] == '1')
            fitness_Pop[i] += 4;
        if(Population[i][7] == '1')
            fitness_Pop[i] += 8;
        if(Population[i][6] == '1')
            fitness_Pop[i] += 16;
        if(Population[i][5] == '1')
            fitness_Pop[i] += 32;
        if(Population[i][4] == '1')
            fitness_Pop[i] += 64;
        if(Population[i][3] == '1')
            fitness_Pop[i] += 128;
        if(Population[i][2] == '1')
            fitness_Pop[i] += 256;
        if(Population[i][1] == '1')
            fitness_Pop[i] += 512;
        if(Population[i][0] == '1')
            fitness_Pop[i] += 1024;
        
        //given the fitness function f(x)= x^2
        fitness_Pop[i] *= fitness_Pop[i];
		if(fitness_Pop[i] != 0)
			fitness_Pop_inv[i] = 1/fitness_Pop[i]; //because we want to minimize
		
		else
			fitness_Pop_inv[i] = 0;
			
		printf("fitness_Pop_inv[%d] = %f\n",i,fitness_Pop_inv[i]);
    }
}

void print_pop(void)
{
    int i,j;
    for(i=0; i<population; i++)    //lines
    {   
        for(j=0; j<size_ind; j++)  //col
        {
            if(band==1)printf("%c ",Population[i][j]);
        }
        if(band==1)printf("\n");
    }
	
}


void print_fitness(void)
{
    int i;
    printf("\n -- Fitness -- \n");
    for(i=0; i<population; i++)
        printf("%f \n",fitness_Pop[i]);
}


void selection(void)
{
    //roulette-wheel selection
    int i,j;
    double sum=0;
    double Prob[population];        //probabilities
    double rand_temp;
    double limit_prob[population];  //to know the limits/border of each individual given the Prob
    int Selected[toSelect];
    int point_2cross;
    int point_2mutate;
    
    
    int a=1;                //not allow to fall in 0
    int b=size_ind-1;       // or 11 (size of genotype) - you can change this and see what happen
    
    // -----------
    for(i=0; i<population; i++){
        sum += fitness_Pop_inv[i];
        Prob[i] = 0;        // init to 0
        limit_prob[i] = 0;  // init to 0
        Selected[i] = 0;    // check here Selected need to be equal to population
    }
    
    if(band==1)printf("\n-- Probabilities --\n");		
    for(i=0; i<population; i++){
        Prob[i] = fitness_Pop_inv[i]/sum;				//calculate probabilities for each individual
        if(band==1)printf("Prob1 %f \n",Prob[i]);        
    }
    
    if(band==1)getchar();          //wait, you can put away this
    
    sum=0;
    if(band==1)printf("\nPrint limits\n");
    for(i=0; i<population; i++){					//to obtain the boundaries of each individual
        sum += Prob[i];
        limit_prob[i] = sum;
        if(band==1)printf("%f \n",limit_prob[i]);
        
    }
    // now each individual has a boundary, with a random we can now choose one parent
    // |-----|------------|----|------------------|--------------| ... --|
    //   ind1    ind2      ind3   ...                                  ind n-1
    
    
    //Selection
    
    //check out where it falls to chose a parent
    
    if(band==1)printf("\n ---Selected ---\n");
    for(j=0; j<toSelect; j++)   // repeat to select n parents
    {
        rand_temp = rando();
        for(i=0; i<population; i++){        //to know where it falls
            if(rand_temp <= limit_prob[i])
            {
               Selected[j] = i;
               break;
            }
        }
        if(band==1)printf("%d\n",Selected[j]);
    }
    
    // now apply crossover between parent chosen 0-1, 2-3, 4-5, 6-7 and 8-9
    
    //flip the bits croosover one-point
    for(i=0; i<population; i+=2)
    {
        point_2cross = (int)(a + (b-a) * rando());  //create a random number between 'a' and 'b'
        for(j=0; j<size_ind; j++)
        {
            if(j < point_2cross)        //copy the original values to offspring
            {
                Offspring[i][j] = Population[Selected[i]][j];
                Offspring[i+1][j] = Population[Selected[i+1]][j];
            }
            else                        //interchange bits
            {
                Offspring[i][j] = Population[Selected[i+1]][j];
                Offspring[i+1][j] = Population[Selected[i]][j];
            }    
        }
    }
    // This section only prints the population////////////////////////
    if(band==1)printf("\n *** Population after crossover *** \n");
    for(i=0; i<population; i++)
    {
        for(j=0; j<size_ind; j++)
        {
            if(band==1)printf("%c ",Offspring[i][j]);
        }
        if(band==1)printf("\n");
    }//////////////////////////////////////////////////////////////////
            
    //Apply mutation, only one bit is change here    
    int aa=0, bb=size_ind-1;				//new values, from aa to bb can falls the random
    
    for(i=0; i<population; i++)
    {
        point_2mutate = (int)(aa + (bb-aa) * rando());
        
        if (Offspring[i][point_2mutate] == '0')
            Offspring[i][point_2mutate] = '1';
        else
            Offspring[i][point_2mutate] = '0';
    }
    //This section only prints the new oofsprings ////////////////////
    if(band==1)printf("\n *** Population after Mutation *** \n");
    for(i=0; i<population; i++)
    {
        for(j=0; j<size_ind; j++)
        {
            if(band==1)printf("%c ",Offspring[i][j]);
        }
        if(band==1)printf("\n");
    }/////////////////////////////////////////////////////////////////
}

/*You can improve this code (maybe optimize) and create more functions as needed
but not that for academic purpose you must create your own implementation*/


