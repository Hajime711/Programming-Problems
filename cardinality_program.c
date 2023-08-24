#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define TOTALNUMBERS 1000


//Converts Integer to ASCII Code
int integerToAscii(int number) {
   return '0' + number;
}

//Converts ASCII code back to integer
int asciiToInteger(int number) {
   return number - '0';
}

int main()
{
    int i,j,randon_number;

    FILE *outfile;
    outfile = fopen("numbers.txt","w");

    printf("Writing to file...!\n");
    //Write 1000 lines in file
    for (i = 0; i < TOTALNUMBERS; i++){
        //Generate 4 Random numbers String with spaces in between
        char singleLine[60]; //Stores a single line that will be written to file
        int pos = 0; //This used form remembering current position of cursor in singleLine
        for (j = 0; j < 4; j++){
            //Getting a random number
            randon_number =(int)(10.0*rand()/(RAND_MAX+ 1.0));

            //Converts to ASCII code and write to file
            //Just concatenating ASCII value of random number ti single line string
            pos += sprintf(&singleLine[pos], "%d ", integerToAscii(randon_number));
        }

        //Writing generated single line to the file
        fprintf (outfile, "%s\n", singleLine);
    }

    fclose(outfile);
    printf("File saved successfully...\n");


    //Starting file reading
    printf("Reading From file...!\n");

    FILE* inFile;
    char ch;

    inFile = fopen("numbers.txt", "r");

    //If file does not exists or cannot be opened
    if (NULL == inFile) {
        printf("Error occurred while opening this file. \n");
        exit(1);
    }

    //Array to store digit count in a single line
    int digitCount[10] = { 0 };

    //Array to store actual cardinalities
    int cardinalities[5] = { 0 };

    int asciiValue = 0;
    int count = 0;

    //Read ASCII code from while
    fscanf (inFile, "%d", &asciiValue);

    //Loop to read all ASCII codes from file
    while (!feof (inFile))
    {
        int realValue = asciiToInteger(asciiValue);
        digitCount[realValue]++; //Incrementing digit count

        //When 4 ASCII code already read e.g at the end of line
        //Now we will try yo find the most repeated digit
        if(count % 4 == 0){
            //First checking for special case: if it contains 2 pairs
            int twoCount = 0;
            for(int i = 0; i < 10; i++){
                if(digitCount[i] == 2)
                    twoCount++;
            }

            //If special case i.e. got two pairs
            if(twoCount == 2){
                cardinalities[0]++; //Cardinality for special case is stored at index 0
            }
            else{
                //Now, trying to get a digit which is repeated the most
                int largest = 0; //Stores the digit which most repeated e.g in 3331 the largest will store 3
                for(int i = 0; i < 10; i++){
                    if(digitCount[i] > largest){
                        largest = digitCount[i];
                    }
                }
                cardinalities[largest]++; //Increment the cardinality of respective index according to number repetition
            }

            //Resetting entire digit count array to zero
            for(int i = 0; i < 10; i++){
                digitCount[i] = 0; //Setting it to zero for next line iteration
            }
        }

        fscanf (inFile, "%d", &asciiValue);
        count++;
    }



    fclose(inFile);



    //Print Cardinalities
    printf("\n----------------------------\nCardinalities are:\n----------------------------\n");
    printf("i)   All digits same: %d\n", cardinalities[4]);
    printf("ii)  3 digits same:   %d\n", cardinalities[3]);
    printf("iii) 2 pairs:         %d\n", cardinalities[0]);
    printf("iv)  1 pairs:         %d\n", cardinalities[2]);
    printf("v)   Non identical:   %d\n", cardinalities[1]);

    system("pause");

    return 0;
}
