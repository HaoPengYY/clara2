/**
 * Copyright 2014-2016 Richard Pausch
 *
 * This file is part of Clara 2.
 *
 * Clara 2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Clara 2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Clara 2.
 * If not, see <http://www.gnu.org/licenses/>.
 */


#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "parallel_jobs.h"
#include "all_directions.hpp"

/////////////////// modified by PENGHAO////////////////////
#include "settings.hpp"
/////////////////// modified by PENGHAO////////////////////


int main(void)
{
  printf("start\n");
  using namespace std;
  int numtasks, rank;

  /** the function 'start_array()' calls the parallelization procedure
   * defined in 'parallel_jobs.h'. The variables 'numtasks' and 'rank'
   * are set by this function and allow to use the parallel distributed
   * indexes to be used.
   **/
  if(start_array(&numtasks, &rank) != 0)
    return 1;

  //////////////////////////////////////////////////
  //    do some work here
  //////////////////////////////////////////////////

  /* hostname of the machine this task is running on */
  char * pHost;
  pHost = getenv("MYHOSTNAME");


  /* template for standard and error output file names */
  const char output_file[] = "my_output.txt-%d";
  const char error_file[] = "my_error.txt-%d";


  int return_value; /* return value for 'all_directions()' or 'all_directions_uop()' */
  char dump[1024] ={0}; /* used for final filenames stderr stdout */

  const unsigned int N_max = 1; /* max number of trajectories */
  unsigned int i; /* index */
  for(i=rank; i<N_max; i += numtasks)
  {
    /** this for loop runs over all tasks assigned to the mpi/array process
     * it starts with its first assigned id which is its rank. Then it
     * completes more tasks by going in strides of the total number of
     * processes to the next task. This is done until N_max is reached.
     **/

    /* print info on tasks performed to stdout */
    printf("this is job %5d of %5d jobs in the array (on %s = rank: %d)\n", i, N_max, pHost, rank);

    /* redirect output stdout to file */
    sprintf(dump, output_file, i);
    freopen(dump, "w", stdout);
    /* redirect output stderr to file */
    sprintf(dump, error_file, i);
    freopen(dump, "w", stderr);

/////////////////// modified by PENGHAO////////////////////
    /* calculate the radiation of a single trajectory for all
     * directions of interest 
     * if USE_uop == false, calculate the far-field spectrum
     * else calculate the electrical field on the user-defined
     * observation plane
     */
    // return_value = all_directions(i);
    if (!param::USE_uop)
      return_value = all_directions(i);
    else
      return_value = all_directions_uop(i);
/////////////////// modified by PENGHAO////////////////////

    /* set output back to stdout stderr
     * ISSUE: THIS DOES NOT WORK !!!
     */
    fclose(stdout);
    fclose(stderr);
    freopen("/dev/tty", "w", stdout);
    freopen("/dev/tty", "w", stderr);

    if(return_value != 0)
      std::cerr << "error occured in rank " << i << std::endl;

    /* this commented-out section is a method to stop the calculation if a certain
     * file is found on the hard drive - currently not used
     */

    /*
      if(check_break())
      {
        printf("break point was set: i= %d  rank=%d  numtasks=%d \n", i, rank,  numtasks);
        break;
      }
    */

  }



  ///////////////////////////////////////////////////
  //    work is done
  //////////////////////////////////////////////////

  /* clean up parallelization if needed */
  end_array();


  return 0;
}
