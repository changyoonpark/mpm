#include <iostream>
#include "include/initer.h"
#include "include/particle.h"
#include "include/grid.h"
#include "include/constants.h"

#include "include/simpleViewer.h"

#include <assert.h>

// #include <omp.h>


int main (int argc, char* argv[]){

    InitData    idat("../input/input.in");
    Constants   constants("../input/box.in");
    ParticleSet pSet(constants,  idat);
    Grid        grid(constants, &pSet);

    SimpleView sv = SimpleView(&pSet, &grid, &constants);
    
    sv.init();
    sv.start();



// int nthreads, tid;

// /* Fork a team of threads giving them their own copies of variables */
// #pragma omp parallel for num_threads(4)
//     for(int i = 1; i < 13; i++)
//     {
//     //    c[i] = a[i] + b[i];
//        printf("%d from %d\n",i,omp_get_thread_num());
//     }


    return 0;

}