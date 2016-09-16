
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// at equator:
// arc-second of longitude: 30.87 m
// arc-second of latitude: 30.87 * cos(35) // at 35N


// An HGT file covers an area of 1°x1°. Its south western corner can be deduced from its file name: for example, n51e002.hgt covers the area between N 51° E 2° and N 52° E 3°, and s14w077.hgt covers S 14° W 77° to S 13° W 76°. The fils size depends on the resolution. If this is 1", there are 3601 rows of 3601 cells each; if it is 3", there are 1201 rows of 1201 cells each. The rows are laid out like text on a page, starting with the northernmost row, with each row reading from west to east. Each cell has two bytes, and the elevation at that cell is 256*(1st byte) + (2nd byte). It follows that a 3" HGT file has a file length of 2 x 1201 x 1201. SRTM 3" cells are calculated by calculating the mean of 1" cells and their eight neighbors. It follows that the highest local point is likely to higher than the highest SRTM 3" cell. The difference should vary with the steepness of the local relief.


// rows 1001- 1201
// columns 800-1000


int main(int argc, char *argv[])
{
    char path[999];
    getwd(path);
    printf("wd: %s\n", path);
    
    FILE* in = fopen("GL.app/Contents/Resources/N35W086.hgt", "r");

    if (in == NULL)
    {
        perror("cannot open file\n");
        exit(-1);
    }

 
    short data[1201][1201];

    short min=99999;
    short max=0;
    
    int mini,minj,maxi,maxj;
    


    for (int i=0; i<1201; i++)
    {
        for (int j=0; j<1201; j++)
        {
//            fread(&data[i][j], sizeof(short), 1, in);

            unsigned char unpack[2];

            
            fread(&unpack[1], 1, 1, in);
            fread(&unpack[0], 1, 1, in);

            data[i][j]=*(short*)unpack;
        }
    }

    
    for (int i=1000; i<1201; i++)
    {
        for (int j=800; j<1000; j++)
        {

            if (data[i][j] > max)
            {
                max = data[i][j];
                maxi=i;
                maxj=j;
            }
            

            if (data[i][j] < min)
            {
                min = data[i][j];
                mini=i;
                minj=j;
            }
            

//            printf("(%d,%d) %d\n", i, j, data[i][j]);
        }
    }

    printf("min: %d (%d,%d) -- max: %d (%d,%d)\n", min, mini, minj, max, maxi, maxj);
    
    


    return 0;
}
