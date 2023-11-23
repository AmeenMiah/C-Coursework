#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

int gcd(int a, int b);

/* The RGB values of a pixel. */
struct Pixel {
    int red, green, blue;
};

struct Pixel *makeVector(int n)
{
    return malloc(n * sizeof(struct Pixel));
}

struct Pixel **makeMatrix(int m, int n)
{
    struct Pixel **newm = malloc(m * sizeof(struct Pixel *));
    for (int i = 0; i < m; i++)
        newm[i] = makeVector(n);
    return newm;
}


struct Comment {
    char * comment;
    struct Comment *nextComment;
    
};

/* An image loaded from a PPM file. */
struct PPM {
    int width;
    int height;
    int max;


    struct Pixel **allPixels;
    struct Comment *baseComment;  
};



void append(struct Comment** head, char* newComment)
{
    //make a new comment
    struct Comment* newNode = malloc(sizeof(struct Comment));

    //Used to iterate through the linked list
    struct Comment *last = *head;  

    //Make the new comment structure hold the comment string
    newNode->comment = newComment;
  
    //Make that the new comment points to nothing
    newNode->nextComment = NULL;
    
    //If the head isn't pointing to something then make head point to the new comment
    if (*head == NULL)
    {
       *head = newNode;
       return;
    }  
    /* 5. Else traverse till the last node */
    while (last->nextComment != NULL)
        last = last->nextComment;
   
    /* 6. Change the next of last node */
    last->nextComment = newNode;
    return;    
}


/* Reads an image from an open PPM file.
 * Returns a new struct PPM, or NULL if the image cannot be read. */
struct PPM *getPPM(FILE * f)
{
    if (f == NULL)
        return NULL;

    char * line = NULL;
    size_t len = 0;
    size_t read;
    struct Pixel p;

    struct PPM *file = malloc(sizeof(struct PPM));

    //Get the first line
    char p1 = 'a';
    int num1, num2 = 0;
    if (fscanf(f, "%c%d\n", &p1, &num1) !=2)
        return NULL;
    
    //setbuf(stdout, NULL);

    //If it isn't a plain PPM file then return NULL
    if (p1 != 'P' || num1 != 3) {
        return NULL;
    }

    //Add comment
    char *width; 
    char *temp;
    char *height;
    char * checker;
    file->baseComment = NULL;
    //Check if the line is the width and height, if not then must be a comment, store the comment in a linked list of comments, keep doing this until you reach the width and height
    while (fscanf(f, "%d %d\n", &file->width, &file->height) != 2) {
        checker = malloc(100 * sizeof(char *));
        fgets(checker, 100, f);
        
        if (checker[0] = '#')
        {
            append(&file->baseComment, checker);
        }
        else
        {
            //If it is neither a comment or the width or height return NULL
            return NULL;
        }
    }
    checker = "";

    //Get max value of the rgb values
    if (fscanf(f, "%d\n", &file->max) != 1) {
        return NULL;
    }

    //Store all the pixels in a 2d array
    file->allPixels = makeMatrix(file->height, file->width);
    for(int x=0; x<(file->height); x++) {
           for(int y = 0; y <(file->width); y++) {
                fscanf(f, "%d", &file->allPixels[x][y].red);
                fscanf(f, "%d", &file->allPixels[x][y].green);
                fscanf(f, "%d", &file->allPixels[x][y].blue);

           }
    }
    
    return file;

}

/* Write img to stdout in PPM format. */
void showPPM(const struct PPM *img)
{
    printf("P3\n");
    //Print all the comments by going through the linked list
    if (img->baseComment != NULL){
            struct Comment *currentComment = img->baseComment;
            while(currentComment != NULL)
            {
                
                printf("%s", currentComment->comment);
                currentComment = currentComment->nextComment;

            }
    }
    //Print out the width, height and max
    printf("%d %d\n", img->width,img->height);
    printf("%d\n", img->max);

    //Print out all the pixels
    for(int x=0; x<(img->height); x++) {
            for(int y = 0; y <(img->width); y++) {
                    printf("%d %d %d\n",img->allPixels[x][y].red,img->allPixels[x][y].green,img->allPixels[x][y].blue);

            }
    }
}

void writePPM(const struct PPM *img)
{
    //Print the PPM file to file.PPM in the current directory or if it is NULL then print out an error message
    if (img == NULL)
    {
        printf("Cannot print image\n");
    }
    else {
        FILE *f = fopen("file.ppm", "w");
        fprintf(f,"P3\n");
        if (img->baseComment != NULL){
            struct Comment *currentComment = img->baseComment;
            while(currentComment != NULL)
            {
                
                fprintf(f, "%s", currentComment->comment);
                currentComment = currentComment->nextComment;

            }
        }
            
        fprintf(f,"%d %d\n", img->width,img->height);
        fprintf(f,"%d\n", img->max);

        for(int x=0; x<(img->height); x++) {
            for(int y = 0; y <(img->width); y++) {
                    fprintf(f, "%d %d %d\n",img->allPixels[x][y].red,img->allPixels[x][y].green,img->allPixels[x][y].blue);

            }
        }

        fclose(f);
    }
    
}

/* Opens and reads a PPM file, returning a pointer to a new struct PPM.
 * On error, prints an error message and returns NULL. */
struct PPM *readPPM(const char *filename)
{
    /* Open the file for reading */
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        fprintf(stderr, "File %s could not be opened.\n", filename);
        return NULL;
    }

    /* Load the image using getPPM */
    struct PPM *img = getPPM(f);

    /* Close the file */
    fclose(f);

    if (img == NULL) {
        fprintf(stderr, "File %s could not be read.\n", filename);
        return NULL;
    }

    return img;
}

/* Encode the string text into the red channel of image img.
 * Returns a new struct PPM, or NULL on error. */
struct PPM *encode(const char *text, const struct PPM *img)
{
    struct PPM *file = malloc(sizeof(struct PPM));
    
    file = img;
    int textSize = strlen(text);
    int lastLetterIndex = textSize - 1;
    if (textSize == 0) {
        return NULL;
    }

    if (file->max < 126) {
        return NULL;
    }
    int x = 0;
    int y = 0;
    if (file->height > file->width){
        if (file->height <= lastLetterIndex) {
            return NULL;
        }
        int spliter = gcd(file->height, textSize);
        spliter = spliter/textSize;
        int totalSpliter = spliter;
        int previous = 0;

        for (int i = lastLetterIndex;i >= 0;i--)
        {
            //Chose a random number from previous to totalSplitter-1  (due to the fact it is going to be used for an array)
            int x = (rand() % ((totalSpliter-1) - previous + 1)) + previous; 
            file->allPixels[x][0].red = text[i];
            previous = totalSpliter;
            totalSpliter += spliter;
        }

        return file;
        
    }
    else if (file->height < file->width){
        if (file->width <= lastLetterIndex) {
            return NULL;
        }
        int spliter = gcd(file->width, textSize);
        spliter = spliter/textSize;
        int totalSpliter = spliter;
        int previous = 0;

        
        for (int i = lastLetterIndex;i >= 0;i--)
        {
            int y = (rand() % ((totalSpliter-1) - previous + 1)) + previous; 
            file->allPixels[0][y].red = text[i];
            previous = totalSpliter;
            totalSpliter += spliter;
        }

        return file;
    }
    else if (file->height == file->width) {
        if (file->width <= lastLetterIndex) {
            return NULL;
        }
        int spliter = gcd(file->width, textSize);
        spliter = spliter/textSize;
        int totalSpliter = spliter;
        int previous = 0;

        for (int i = lastLetterIndex;i >= 0;i--)
        {
            int y = (rand() % ((totalSpliter-1) - previous + 1)) + previous; 
            file->allPixels[0][y].red = text[i];
            previous = totalSpliter;
            totalSpliter += spliter;
        }

        return file;
    }
    else{
        return NULL;    
    }
    
}

//If a can be divided by b then return a but if it isn't then return the biggest number from a that is divisible by b
int gcd(int a, int b)
{
    if (a%b == 0)
        return a;
    else
        return ((int) round(a/b))*b;
}

/* Extract the string encoded in the red channel of newimg, by comparing it
}

 * with oldimg. The two images must have the same size.
 * Returns a new C string, or NULL on error. */
char *decode(const struct PPM *oldimg, const struct PPM *newimg)
{
    if (newimg->height != oldimg->height)
        return NULL;

    if (newimg->width != oldimg->width)
        return NULL;

    if (newimg->max != oldimg->max)
        return NULL;

    char *string = malloc(100 * sizeof(char *));
    memset(string, 0, 100 * sizeof(char *));
    char *head = malloc(2 * sizeof(char *));
    memset(head, 0, 2 * sizeof(char *));
    char *temp = malloc(100 * sizeof(char *));
    memset(temp, 0, 100 *sizeof(char *));
    char c;
    for(int x=0; x<(oldimg->height); x++) {
           for(int y = 0; y <(oldimg->width); y++) {
                if (newimg->allPixels[x][y].red !=  oldimg->allPixels[x][y].red)
                {
                    if(newimg->allPixels[x][y].red > 126) 
                        return NULL;
                    else{
                        c = newimg->allPixels[x][y].red;
                        
                        head[0] = c;
                        sprintf(temp, "%s%s", head, string);

                        strcpy(string, temp);
                    }
                        

                }
                
           }
    }
    free(temp);
    if (strcmp(string, "") == 0)
        return NULL;
    else
        return string;
    /* TODO: Question 2d */

}

/* TODO: Question 3 */
int main(int argc, char *argv[])
{
    /* Initialise the random number generator, using the time as the seed */
    srand(time(NULL));

    /* Parse command-line arguments */
    if (argc == 3 && strcmp(argv[1], "t") == 0) {
        /* Mode "t" - test PPM reading and writing */

        struct PPM *img = readPPM(argv[2]);
        showPPM(img);
        free(img);
    } else if (argc == 3 && strcmp(argv[1], "wt") == 0) {
        /* Mode "w" - write to a file in the local directory called file.ppm */
        /* Mode "t" - test PPM reading and writing */
        struct PPM *img = readPPM(argv[2]);
        showPPM(img);
        writePPM(img);
        free(img);
    
    } else if (argc == 3 && strcmp(argv[1], "e") == 0) {
        /* Mode "e" - encode PPM */

        struct PPM *oldimg = readPPM(argv[2]);

        

        /* prompt for a message from the user, and read it into a string */
        printf("Enter a text \n");
        int inputLimit = 100;
        char * input = malloc(inputLimit);
        fgets(input, inputLimit, stdin);
        input[strcspn(input, "\n")] = 0;

        
        /* encode the text into the image with encode, and assign to newimg */
        struct PPM *newimg = encode(input, oldimg);
        
        /* write the image to stdout with showPPM */
        if (newimg != NULL) {
            showPPM(newimg);
        }
        else {
            printf("Encoding failed\n");
        }
        


        free(oldimg->allPixels);
    } else if (argc == 3 && strcmp(argv[1], "we") == 0) {
        /* Mode "w" - write to a file in the local directory called file.ppm */
        /* Mode "e" - encode PPM */
       
        struct PPM *oldimg = readPPM(argv[2]);

        

        /* TODO: prompt for a message from the user, and read it into a string */
        printf("Enter a text \n");
        int inputLimit = 100;
        char * input = malloc(inputLimit);
        fgets(input, inputLimit, stdin);
        input[strcspn(input, "\n")] = 0;

        
        /* TODO: encode the text into the image with encode, and assign to newimg */
        struct PPM *newimg = encode(input, oldimg);
        
        /* TODO: write the image to stdout with showPPM */
        if (newimg != NULL) {
            writePPM(newimg);
            //showPPM(newimg);
            printf("Encoding successful, check file.ppm for the encoded file\n");
        }
        else {
            printf("Encoding failed\n");
        }
        


        free(oldimg->allPixels);
    } else if (argc == 4 && strcmp(argv[1], "d") == 0) {
        /* Mode "d" - decode PPM */

        /* TODO: get original file filename from argv, load it with
           readPPM, then assign to oldimg */
        struct PPM *oldimg = readPPM(argv[2]);
        /* TODO: get encoded file filename from argv, load it with
           readPPM, then assign to newimg */
        struct PPM *newimg = readPPM(argv[3]);
        char *message = malloc(sizeof(char *));
        /* TODO: decode the encodedPPM with the comparisonPPM and assign to message */
        message = decode(oldimg, newimg);
        free(oldimg->allPixels);
        /* TODO: print the decoded message to stdout */
        if (message != NULL)
            printf("The message is %s\n", message);
        else
            printf("Failed to decode");
        free(message);
        

    } else {
        fprintf(stderr, "Unrecognised or incomplete command line.\n");
        return 1;
    }

    return 0;
}
