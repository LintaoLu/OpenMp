#include<stdlib.h>
#include<stdio.h>
#include <omp.h>

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

/**
 * Jacboi solution.
 * Need two parameters: thread number and precision.
 * Calculate and print all variable based on precision.
 * 
 * @author Lintao Lu
 * @date 10/7/2019
*/


int row = 3, col = 3;
float constants[] = { 12.0/5.0, -25.0/8.0, 6.0/4.0 };
float coefficient[3][3] = {
  { 0.0, 1.0/5.0, -2.0/5.0 },
  { -3.0/8.0, 0, 2.0/8.0},
  { -1.0/4.0, -1.0/4.0, 0.0}
};
float values[] = { 0.0, 0.0, 0.0 };
float last_values[] = { 0.0, 0.0, 0.0 };

void calculate(int num_of_thread, float precision);
int isSame(float* value, float* last_value, float precision);

int main(int argc, char *argv[]){
  if(argc <= 1){
    printf("You did not feed me arguments! Need 2: threads, precision\n");
    exit(1);
  }
  int num_of_thread = atoi(argv[1]);
  float precision = (float)atof(argv[2]);
  calculate(num_of_thread, precision);

  return 0;
}

void calculate(int num_of_thread, float precision){
  //if thread number is greater than row, only create row threads.
  num_of_thread = MIN(num_of_thread, row);
  int task_size = row / num_of_thread;
  int* task_position = (int*)malloc(sizeof(int)*num_of_thread);
  int* task_end = (int*)malloc(sizeof(int)*num_of_thread);
  for (int i = 0; i < num_of_thread; i++){
    task_position[i] = i*task_size;
    task_end[i] = (i+1)*task_size;
  }
  task_end[num_of_thread-1] = col;  

  omp_set_num_threads(num_of_thread);

  do{
    //use multiple threads to copy array in parallel
    #pragma omp parallel
    {
      int ID = omp_get_thread_num();
      //save current values
      for (int i = task_position[ID]; i < task_end[ID]; i++)
      { last_values[i] = values[i]; } 
    }
  
    //multiple threads to calculate 1 or several rows
    #pragma omp parallel
    {
      int ID = omp_get_thread_num();
      for (int i = task_position[ID]; i < task_end[ID]; i++){
        float newVal = constants[i];
        for (int j = 0; j < col; j++){
          newVal += coefficient[i][j] * last_values[j];
        }
        //write directly to current value
        values[i] = newVal;
      }
    }
  } while (isSame(values, last_values, precision) != 1);

  for (int i = 0; i < col; i++) printf("%f\t", values[i]);
  printf("\n");
}

int isSame(float* value, float* last_value, float precision){
  for (int i = 0; i < col; i++){
    float a = (100.0 * value[i]) / 100.0;
    float b = (100.0 * last_value[i]) / 100.0;
    if (a - b > precision) return -1;
  }
  return 1;
}
