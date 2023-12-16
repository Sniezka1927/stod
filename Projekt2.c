/*
	Napisz funkcje mzieniajace ciag znakow na liczbe typu double o ile jest to mozliwe. Jesli zamiana nie jest mozliwa wskaz znak kotry ja uniemozliwil.
	Wszystkie tablice alkowane dynamincznie
	Tekst sklada sie z jednej linii
	Dane czytane podawane sa z klawiatury
	Program poprawnie przetwarza polska strone kodowa
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <locale.h>

#pragma warning (disable: 4996)

int has_sign(char ch) {
    return ch == '-';
}

int is_digit_or_dot(char ch) {
    // 48 in Ascii is 0
    // 57 is Ascii is 9
    return ((double)ch >= 48 && (double)ch <= 57) || ch == '.' || ch == '+' || ch == '-';
}

int is_convertable(const char* text, int* dot_index, int* issue_index) {
    int dot_count = 0;

    for (int i = 0; i < strlen(text) - 1; i++) {
        // check if it digit, dot or sign
        if (!is_digit_or_dot(text[i])) {
            *issue_index = i;
            return 0; // Not convertible
        }

        // Check if there is no additional sign in the string
        if (i >= 1 && (text[i] == '+' || text[i] == '-')) {
            *issue_index = i;
            return 0;
        }

        if (text[i] == '.') {
            dot_count++;
            *dot_index = i;
            // If there is more than one dot
            if (dot_count > 1) {
                return 0; 
            }
        }
    }

    return 1; // Convertible
};

double integer_part(const char* text, int* dot_index, int is_signed) {
    // 100 = 0 * 10^0 + 0 * 10^1 + 1 * 10^2
    double sum = 0;
    int power_level = 0;

    for (int i = *dot_index - 1; i >= is_signed; i--) {
        sum += ((double) text[i] - 48) * pow(10., power_level);
        power_level++;
    }
    return sum;
};

double decimals_part(const char* text, int* dot_index) {
    // .01 => 0 * 10^-1 + 1 * 10^-2 ....
    double sum = 0;
    int power_level = -1;
    for (int i = *dot_index + 1; i < strlen(text) - 1; i++) {
        sum += ((double) text[i] - 48) * pow(10., power_level);
        power_level--;
    }
    return sum;
};

int main() {
    setlocale(LC_ALL, "polish");
    printf("Enter a string: ");

    char text[255];
    FILE* file;
    file = fopen("./result.txt", "w+");
    if (file == NULL) {
        printf("We could not find the results file or it could not be opened. Please try again.\n");
        return 1;
    }

    if (fgets(text, sizeof(text), stdin) == NULL) {
        printf("Error reading input.\n");
        return 1;
    }
    
    // Dynamically allocate memory for the character array
    char* received_text = (char*)malloc((strlen(text) + 1) * sizeof(char));
    if (!received_text) {
        printf("There were problem with allocating memory.");
        return 1;
    }

    strcpy(received_text, text);

    // Has sign
    int is_signed = has_sign(received_text[0]);
    
    int dot_index = -1;
    int issue_index = -1;
    // Check if it is convertable
    int can_convert = is_convertable(received_text, &dot_index, &issue_index);
    if (can_convert) {
        if (dot_index == -1) {
            // there is no dot in the string
            dot_index = strlen(received_text) - 1;
        }

        // sum integer part
        double integers = integer_part(received_text, &dot_index, is_signed);
        
        // sum decimal part
        double decimals = decimals_part(received_text, &dot_index);
        
        double result = integers + decimals;
        
        if (is_signed) {
            result *= -1;
        }

        printf("Result is: %lf\n", result);
        fprintf(file, "Result is: %lf\n", result);
    }
    else {
        printf("Could not convert the string to a double because of char %c at index %d\n", received_text[issue_index], issue_index);
    }

    // Free the allocated memory
    free(received_text);
    fclose(file);
    received_text = NULL;

    return 0;
}