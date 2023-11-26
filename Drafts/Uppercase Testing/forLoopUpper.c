#include <stdio.h>
#include <string.h>
//https://www.javatpoint.com/convert-uppercase-to-lowercase-in-c

int main()
{
    char str[30];
    int i;
    printf(" Enter the string: ");
    scanf(" %s", &str); // take a string

    // use for loop to change string from upper case to lower case
    for (i = 0; i <= strlen(str); i++)
    {
        // The ASCII value of A is 65 and Z is 90
        // if (str[i] >= 65 && str[i] <= 90)
        if (str[i] >= 97 && str[i] <= 122) // ascii val of a is 97 and z is 122
            str[i] = str[i] - 32; /* remove 32 to string character to convert from lower case. */
    }

    /*
    97 	61 	01100001 	&#97; 	a

    101 	65 	01100101 	&#101; 	e

    105 	69 	01101001 	&#105; 	i

    111 	6F 	01101111 	&#111; 	o

    117 	75 	01110101 	&#117; 	u

    convert this program so that it will loop through the string, then store the data where each of the specific values are in a different array, then have different threads to each array specifically go and change the values at those positions, one after another, then have that data print and return!
    */

    printf(" \n Upper Case to Lower case string is: %s \n", str);
    return 0;
}