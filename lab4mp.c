
/*
*Colton Rivard
*sec 021
*Lab2 Stack Machine 
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <time.h>


int main(int argc, char* argv[]) {
    int row, col, row1, col1, row2, col2;
    struct timespec newTime;

    if (argc != 3) {
        printf("you either put too many or too little arguments \n");
         exit(EXIT_FAILURE);
    }
    FILE *matrix1 = fopen(argv[1], "r");
    fscanf(matrix1, "%d", &row);
    fscanf(matrix1, "%d", &col);
	
	 row1 = row;
	 col1 = col; 
	
    int **matrixArray1 = malloc(row * col * sizeof(int));
    int **matrixArray2 = malloc(row * col * sizeof(int));
    int fd_shm = shm_open("/resultmemory",O_RDWR|O_CREAT, S_IRUSR | S_IWUSR);
    if(fd_shm == -1){
    	printf("COULD NOT OPEN SHARED MEMORY SEGMENT\n");
    	exit(EXIT_FAILURE);
    }
    
    ftruncate(fd_shm, row * col);
    int *resultMatrix = (int *)mmap(NULL, row*col ,PROT_READ | PROT_WRITE, MAP_SHARED,fd_shm,0);
   
    if(resultMatrix == MAP_FAILED){
    printf("COULD NOT MAP");
    exit(EXIT_FAILURE);
    }




    //puts data into matrix 1
    for (int i = 0; i < row; i++) {
        matrixArray1[i] = malloc(col * sizeof(int));
        for (int j = 0; j < col; j++) {
            int value;
            fscanf(matrix1, "%d", &value);
            matrixArray1[i][j]= value;

        }
    }
    fclose(matrix1);

    FILE *matrix2 = fopen(argv[2], "r");
    fscanf(matrix2, "%d", &row);
    fscanf(matrix2, "%d", &col);
    
     row2 = row;
     col2 = col;
    
    if((row1 != row2) || (col1 != col2)){
	printf("\n the matricies do not match, they must have the same number of cols and rows\n");
	 exit(EXIT_FAILURE);
	}

    //puts data into matrix2
    for (int i = 0; i < row; i++) {
        matrixArray2[i] = malloc(col * sizeof(int));
        for (int j = 0; j < col; j++) {
            int value;
            fscanf(matrix2, "%d", &value);
            matrixArray2[i][j] = value;
        }
    }
    fclose(matrix2);



    //prints out matrix 1
    printf("\n Matrix1 \n");
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            printf("%d ", matrixArray1[i][j]);
        }
        printf("\n");
    }

    // prints out matrix2
    printf("\n Matrix2 \n");
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            printf("%d ", matrixArray2[i][j]);
        }
        printf("\n");
    }



    //child process
    //does the matrix math

    pid_t *pids = malloc(row * sizeof(pid_t));
    
     clock_gettime(CLOCK_REALTIME, &newTime);
    long beforeTime = newTime.tv_nsec;
    for (int i = 0; i < row; i++) {
	
        pid_t pid = fork();
        if(pid < 0){
        printf("COULD NOT FORK");
        exit(EXIT_FAILURE);
        }
        if(pid == 0){
            
            for (int j = 0; j < col; j++) {
                resultMatrix[i * row + j] = matrixArray1[i][j] + matrixArray2[i][j];
            }
            
            
            
            munmap(NULL,sizeof(resultMatrix));
            close(fd_shm);
            free(pids);
            exit(0);


            //parent process
        } if(pid > 0){
            pids[i] = pid;
        }


    }
     clock_gettime(CLOCK_REALTIME, &newTime);
     long afterTime = newTime.tv_nsec;
     printf("\nit took :%ld ns for the matrix to work\n", afterTime - beforeTime );
     
    for (int i = 0; i < row; i++){
        int status;
        waitpid(pids[i], &status, 0);
    }

    munmap(NULL,sizeof(resultMatrix));
    close(fd_shm);
    free(pids);

	printf("\n The Result is \n");
    for (int x = 0; x < row; x++) {
        for (int y = 0; y < col; y++) {
            printf("%d ", resultMatrix[x * row + y]);

        }
        printf("\n");
    }
    shm_unlink("/resultmemory");
    

    for(int i = 0; i < row; i++){
    	free(matrixArray2[i]);
    }
     for(int i = 0; i < row; i++){
    	free(matrixArray1[i]);
    }
   
    free(matrixArray1);
    free(matrixArray2);





}



