#include "knn.h"

// Makefile included in starter:
//    To compile:               make
//    To decompress dataset:    make datasets
//
// Example of running validation (K = 3, 8 processes):
//    ./classifier 3 datasets/training_data.bin datasets/testing_data.bin 8

/*****************************************************************************/
/* This file should only contain code for the parent process. Any code for   */
/*      the child process should go in `knn.c`. You've been warned!          */
/*****************************************************************************/

/**
 * main() takes in 4 command line arguments:
 *   - K:  The K value for kNN
 *   - training_data: A binary file containing training image / label data
 *   - testing_data: A binary file containing testing image / label data
 *   - num_procs: The number of processes to be used in validation
 *
 * You need to do the following:
 *   - Parse the command line arguments, call `load_dataset()` appropriately.
 *   - Create the pipes to communicate to and from children
 *   - Fork and create children, close ends of pipes as needed
 *   - All child processes should call `child_handler()`, and exit after.
 *   - Parent distributes the testing set among childred by writing:
 *        (1) start_idx: The index of the image the child should start at
 *        (2)    N:      Number of images to process (starting at start_idx)
 *     Each child should gets N = ceil(test_set_size / num_procs) images
 *      (The last child might get fewer if the numbers don't divide perfectly)
 *   - Parent waits for children to exit, reads results through pipes and keeps
 *      the total sum.
 *   - Print out (only) one integer to stdout representing the number of test
 *      images that were correctly classified by all children.
 *   - Free all the data allocated and exit.
 */

int main(int argc, char *argv[]) {
  // TODO: Handle command line arguments
    
    // nww
    
    if (argc != 5)
    {
        fprintf(stderr, "Number of arguments passed are not 4\n");
        exit(1);
    }
    
    Dataset *training_list = load_dataset(argv[2]);
    
    
    if(training_list == NULL){
        fprintf(stderr,"Memory not enough to contain the training list\n");
        exit(1);
    }
    Dataset *testing_list = load_dataset(argv[3]);
    
    if(testing_list == NULL){
        fprintf(stderr,"Memory not enough to contain the testing list\n");
        exit(1);
    }
    
    int K;
    K = atoi(argv[1]);
    if (K == 0)
    {
        fprintf(stderr, "K value is not of type int or K value is 0\n");
        exit(1);
    }
    if (K<0){
        fprintf(stderr, "Value of K is less than 0\n");
        exit(1);
    }
    
    int num_procs;
    num_procs = atoi(argv[4]);
    
    if (num_procs == 0)
    {
        fprintf(stderr, "Number of Process value is not of type int or value zero process should be called 0\n");
        exit(1);
    }
    if (num_procs<0){
        fprintf(stderr, "Value of Number of Process is less than 0\n");
        exit(1);
    }
    
    
    
    // Creating pipe for communication
    
    int pipe_ptoc[num_procs][2];
    int pipe_ctop[num_procs][2];
    
    for(int p=0;p<num_procs;p++){
        
        if (pipe(pipe_ptoc[p]) == -1){
            perror("pipe error");
            exit(1);
        }
        
        if (pipe(pipe_ctop[p]) == -1){
            perror("pipe error");
            exit(1);
        }
        
    }
    
    
    // TODO: Spawn `num_procs` children

    
    for(int i=0; i<num_procs;i++){
        

        int r = fork();
        
        if(r < 0){
            
            perror("fork");
            exit(1);
            
            
        }
        
        else if(r == 0) //Child
        {
            

            if(close(pipe_ptoc[i][1]) == -1){
                
                perror("Closing writing end of Parent to child pipe");
                exit(1);
    
            }
            if(close(pipe_ctop[i][0]) == -1){
                
                perror("Closing reading end of child to pipe");
                exit(1);
    
            }
            // closing everthing for rest of the child
            
            for (int x = 0; x < num_procs; x++)
            {
                
                
                if(x != i){
                    if(close(pipe_ptoc[x][0]) == -1){
                    
                        perror("Closing Reading end of Parent to child pipe");
                        exit(1);

                    }
                    if(close(pipe_ptoc[x][1]) == -1){
                    
                        perror("Closing writing end of Parent to child pipe");
                        exit(1);

                    }
                    if(close(pipe_ctop[x][0]) == -1){
                    
                        perror("Closing Reading end of child to Parent pipe");
                        exit(1);

                    }
                    if(close(pipe_ctop[x][1]) == -1){
                    
                        perror("Closing writing end of child to Parent pipe");
                        exit(1);

                    }
                    
                }
            }



            child_handler(training_list, testing_list, K, pipe_ptoc[i][0], pipe_ctop[i][1]);
            
            if(close(pipe_ptoc[i][0]) == -1){
                
                perror("Closing Reading end of Parent to child pipe");
                exit(1);

            }
            if(close(pipe_ctop[i][1]) == -1)
            {
                perror("Closing writing end of child to Parent pipe");
                exit(1);
            }
            
            free_dataset(training_list);
            free_dataset(testing_list);
            
            exit(0);
        }
    }
        
    
    

    // TODO: Send information to children

    // TODO: Compute the total number of correct predictions from returned values

    // Only parent reach here right

    // closing everthing for rest of the child
    
    
    
    for(int r=0; r<num_procs;r++){
        
        if(close(pipe_ptoc[r][0]) == -1){
            
            perror("Closing reading end of Parent to child pipe");
            exit(1);

        }
        
        if(close(pipe_ctop[r][1]) == -1){
            
            perror("Closing writing end of child to Parent pipe");
            exit(1);

        }
    
    }
    
    
    for(int c=0;c<num_procs;c++){
        
        int start_idx = 0;
        
        int N = ceil(16/(float)num_procs);
        start_idx = c*N;
        int temp = 16 - start_idx;
        
        if(temp < N && temp > 0){
            
            N = temp;
            
            if(write(pipe_ptoc[c][1], &start_idx,sizeof(int)) != sizeof(int)){
                      
                perror("Writing of Start index to Parent to child Pipe end not successfull");
                exit(1);
                        
            }
            if(write(pipe_ptoc[c][1], &N,sizeof(int)) != sizeof(int)){
                      
                perror("Writing of N to Parent to child Pipe end not successfull");
                exit(1);
                    
            }
            
        }
        else if(temp < 0){
            
            N = 0;
            
            if(write(pipe_ptoc[c][1], &start_idx,sizeof(int)) != sizeof(int)){
                      
                perror("Writing of Start index to Parent to child Pipe end not successfull");
                exit(1);
                        
            }
            if(write(pipe_ptoc[c][1], &N,sizeof(int)) != sizeof(int)){
                      
                perror("Writing of N to Parent to child Pipe end not successfull");
                exit(1);
                    
            }
        }
        else
        {
            
            if(write(pipe_ptoc[c][1], &start_idx,sizeof(int)) != sizeof(int)){
                      
                perror("Writing of Start index to Parent to child Pipe end not successfull");
                exit(1);
                        
            }
            if(write(pipe_ptoc[c][1], &N,sizeof(int)) != sizeof(int)){
                      
                perror("Writing of N to Parent to child Pipe end not successfull");
                exit(1);
                    
            }
        }
    }
    

    
    for(int d=0; d<num_procs;d++){
        
        if(close(pipe_ptoc[d][1]) == -1){
            
            perror("Closing writing end of Parent to child pipe");
            exit(1);

        }
        
    }
    
    
    
    int total_correct = 0;
    int correct = 0;
    
    
    for(int j=0;j<num_procs;j++){
        
        
        if(read(pipe_ctop[j][0], &correct, sizeof(int)) == -1){
            
            perror("Reading of Total Correct from child to Parent was not successfull");
            exit(1);
        }
        
        total_correct += correct;
        

        
        if(close(pipe_ctop[j][0]) == -1){
            
            perror("Closing reading end of child to Parent pipe");
            exit(1);

        }


    }

    // Print out answer

    printf("%d\n", total_correct);
    
    // free space
    
    free_dataset(training_list);
    free_dataset(testing_list);


    

    return 0;
    
}
