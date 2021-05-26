#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "mpi.h"

/*
problem:

*/
int main(int argc , char * argv[])
{
    int my_rank;		/* rank of process	*/
    int p;			/* number of process	*/
    int portionSize;
    int i;
    int* arr;
    int* arr2;
    int globalSum = 0;
    int localSum = 0;
    int n;
    double globalMean=0.0;
    double localSumOfSquDiff=0.0;
    double globalSumOfSquDiff=0.0;
    double theMean=0.0;
    double result;



	MPI_Status status;	/* return status for 	*/
				/* recieve		*/

	/* Start up MPI */
	MPI_Init( &argc , &argv );

	/* Find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* Find out number of process */
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	if( my_rank == 0)
	{
	    //read n
	    scanf("%d",&n);
    }

    //broadcast n to slave process
    MPI_Bcast(&n , 1 , MPI_INT, 0,MPI_COMM_WORLD);

    if( my_rank != 0)
    {

        arr=malloc (n * sizeof (int));

        //generate random number + calculate local sum of generated element

        srand(time(NULL)+my_rank);
        i = 0;
        for(;i<n;i++){
            arr[i]=rand()%100;
            //calculate local sum
            localSum += arr[i];
            printf("arr %d ,rank %d\n",arr[i],my_rank);
        }
	}
	//share local sum
    MPI_Allreduce(&localSum,&globalSum,1,MPI_INT,MPI_SUM,MPI_COMM_WORLD);


    if(my_rank!=0){
        //calculate global mean
        globalMean=(double)globalSum/(n*(p-1));
        /*printf("global sum: %d\n",globalSum);
        printf("global mean:  %f\n",globalMean);*/


        srand(time(NULL)+my_rank);
        i=0;
        for(;i<n;i++)
        {

            double v=rand()%100;
            /*printf("%.3lf  ",v);
            printf("- %.3lf  ",globalMean);
            printf("=  %.3lf\n",globalMean-v);*/

            // Calculate local sum of squared differences from the mean
            localSumOfSquDiff+=pow((v-globalMean),2.0);
        }
        //printf("localSumOfSquDiff %f\n",localSumOfSquDiff);
    }

    // Share this local sum of squared differences with the master process
    MPI_Reduce(&localSumOfSquDiff,&globalSumOfSquDiff,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);

    if (my_rank == 0 ){
        theMean=globalSumOfSquDiff/(n*(p-1));
        //Calculate the square root of the mean of squared differences.
        theMean=sqrt((double)theMean);
        printf("SUM at rank %d : %f\n",my_rank,theMean );
    }
	/* shutdown MPI */
	MPI_Finalize();
    return 0;
}
