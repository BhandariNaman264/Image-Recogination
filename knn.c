#include "knn.h"

/****************************************************************************/
/* For all the remaining functions you may assume all the images are of the */
/*     same size, you do not need to perform checks to ensure this.         */
/****************************************************************************/

/**************************** A1 code ****************************************/

/* Same as A1, you can reuse your code if you want! */
double distance(Image *a, Image *b) {
  // TODO: Return correct distance
    
    int xres;
    int yres;
    double result = 0;
    
    if (a != NULL && b != NULL)
    {
        xres = a->sx;
        yres = a->sy;
        int av;
        int bv;
        
        for(int i = 0; i < xres*yres;i++)
        {
            av = (int)(a->data[i]);
            bv = (int)(b->data[i]);
            result += ((av-bv)*(av-bv));
        }
        result = sqrt(result);
        return result;
    }
    
    return result;
}

/* Same as A1, you can reuse your code if you want! */
int knn_predict(Dataset *data, Image *input, int K) {
  // TODO: Replace this with predicted label (0-9)
    if(input == NULL){
        return 0;
    }
    if(data == NULL){
        return 0;
    }
    
    int n = data->num_items;
    double kmost[K];
    double distances[n][2];
    
    for(int i = 0;i<n;i++){
        
        distances[i][0] = distance(input,&data->images[i]);
        distances[i][1] = data->labels[i];
    }
    
    
    // Sorting array
    
    int x;
    int y;
    double z;
    double z1;
    for(x=0;x<n-1;x++)
    {
        for(y=0;y<n-1;y++)
        {
            if(distances[y][0] > distances[y+1][0])
            {
                
                z = distances[y][0];
                distances[y][0] = distances[y+1][0];
                distances[y+1][0] = z;
                z1 = distances[y][1];
                distances[y][1] = distances[y+1][1];
                distances[y+1][1] = z1;
                
            }
        }
    }
    
    
    // Making a new array contaning K most numbers so as to get an array of k minimum numbers
    
    
    
    for(int j=0;j<K;j++)
    {
        kmost[j] = distances[j][1];
    }
    
    
    // finding mode in that array
    
    
    double mode = 0;
    int max = 0;
    for(int k=0; k<K;k++){
        int mcount = 0;
        for(int l=0; l<K;l++){
            
            if(kmost[k] == kmost[l])
            {
                mcount++;
            }
        }
        if(mcount>max)
        {
            max = mcount;
            mode = kmost[k];
        }
    }
    
    
    int result = (int)mode;
    return result;
    
}

/**************************** A2 code ****************************************/

/* Same as A2, you can reuse your code if you want! */
Dataset *load_dataset(const char *filename) {
    // TODO: Allocate data, read image data / labels, return
    
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL){
        printf("Not enough memory to store Dataset");
        return NULL;
        
    }

      // Finding totol number of images
    int length;
    fread(&length,sizeof(int),1,fp);
      //printf("Total Number of Images are %d\n",length); // comment out

    Dataset *d = malloc(sizeof(Dataset));
    d->num_items = length;
    d->labels = malloc(sizeof(unsigned char)*length);
    d->images = malloc(sizeof(Image)*length);

    if(d->images == NULL){
        printf("Not enough memory to store Images");
        return NULL;

    }
    
    if(d->labels == NULL){
        printf("Not enough memory to store labels");
        return NULL;
          
    }
    
      
    int i;
    for(i = 0;i<length;i++){

        unsigned char label;
        fread(&label,sizeof(unsigned char),1,fp);
        d->labels[i] = label;
          //printf("Image %d label is %d\n",i,d->labels[i]); // comment out
          
        d->images[i].sx = 28;
        d->images[i].sy = 28;
        d->images[i].data = malloc(sizeof(unsigned char)* 784);
          
        if(d->images[i].data == NULL){
            printf("Not enough memory to store Image data");
            return NULL;
            
        }
          
        for(int j = 0;j<784;j++){
            unsigned char buff;
            fread(&buff,sizeof(unsigned char),1,fp);
            d->images[i].data[j] = buff;
              //printf("Image %d data %d is %d\n",i,j,d->images[i].data[j]); // comment out
            
        }
    
    }
    fclose(fp);
      
    return d;

}

/* Same as A2, you can reuse your code if you want! */
void free_dataset(Dataset *data) {
  // TODO: free data
    
    int i = 0;
    while(i<data->num_items){
        
        free(data->images[i].data);
        i++;
        
    }
    free(data->images);
    free(data->labels);
    free(data);
}



/************************** A3 Code below *************************************/

/**
 * NOTE ON AUTOTESTING:
 *    For the purposes of testing your A3 code, the actual KNN stuff doesn't
 *    really matter. We will simply be checking if (i) the number of children
 *    are being spawned correctly, and (ii) if each child is recieving the
 *    expected parameters / input through the pipe / sending back the correct
 *    result. If your A1 code didn't work, then this is not a problem as long
 *    as your program doesn't crash because of it
 */

/**
 * This function should be called by each child process, and is where the
 * kNN predictions happen. Along with the training and testing datasets, the
 * function also takes in
 *    (1) File descriptor for a pipe with input coming from the parent: p_in
 *    (2) File descriptor for a pipe with output going to the parent:  p_out
 *
 * Once this function is called, the child should do the following:
 *    - Read an integer `start_idx` from the parent (through p_in)
 *    - Read an integer `N` from the parent (through p_in)
 *    - Call `knn_predict()` on testing images `start_idx` to `start_idx+N-1`
 *    - Write an integer representing the number of correct predictions to
 *        the parent (through p_out)
 */
void child_handler(Dataset *training, Dataset *testing, int K,
                   int p_in, int p_out) {
    
    // TODO: Compute number of correct predictions from the range of data
    //      provided by the parent, and write it to the parent through `p_out`.
    //    new

    
    int start_idx = 0;
    int N = 0;
    
    if(read(p_in, &start_idx, sizeof(int)) == -1){
        
        perror("Reading of Total Correct from child to Parent was not successfull");
        exit(1);
    }
    if(read(p_in, &N, sizeof(int)) == -1){
        
        perror("Reading of Total Correct from child to Parent was not successfull");
        exit(1);
    }



    int total_correct = 0;
    
    int compare;

    
    for(int i=start_idx; i < (N + start_idx);i++){
            
        compare = knn_predict(training, &(testing->images[i]), K);
        
        if(compare == (int)testing->labels[i]){
            total_correct = total_correct + 1;
        }
        
    }
    
    
    // write total_correct to parent
    
    if(write(p_out, &total_correct,sizeof(int)) != sizeof(int))
    {
        perror("Writing of Total correct in child to Parent Pipe end not successfull");
        exit(1);
    }
    

    return;
    
    
}

