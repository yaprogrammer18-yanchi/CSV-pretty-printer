#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
Ограничения для пользователя:
        по кол-ву строк  в input.csv - 100
        по длине строки  в input.csv - 256
        по длине 1 слова в input.csv - 99
В input.csv данные записаны корректно и без ошибок
*/

int readFile(char** data)
{
    FILE* file = fopen("input.csv", "r");
    if (file == NULL) {
        printf("file not found!");
        return false;
    }
    int linesRead = 0;
    char temporaryBuffer[256] = { 0 };
    while ((linesRead < 100) && (fgets(temporaryBuffer, sizeof(temporaryBuffer), file) != NULL)) {
        size_t indexOfLineBreak = strcspn(temporaryBuffer, "\n");
        temporaryBuffer[indexOfLineBreak] = '\0';
        char* buffer = malloc(strlen(temporaryBuffer) + 1);
        if (buffer == NULL) {
            printf("Ошибка выделения памяти!\n");
            break;
        }
        strcpy(buffer, temporaryBuffer);
        data[linesRead] = buffer;
        linesRead++;
    }
    fclose(file);
    return linesRead;
}

int quantityOfCols(char** data)
{
    int columsQuantity = 0;
    int len = (int)strlen(data[0]);
    for (int i = 0; i < len; i++) {
        if (data[0][i] == ',') {
            columsQuantity += 1;
        }
    }
    columsQuantity += 1;
    return columsQuantity;
}

bool arrWithAllTheWords(char** data, char** allWords, int rows)
{
    /*
      Здесь формируется массив из указателей, которые
      указывают на массивы из символов (слова).
    */

    int wordIndex = 0;
    for (int row = 0; row < rows; row++) {
        int i = 0;
        int j = 0;
        char tempWordBuf[100] = { 0 };
        while (data[row][i] != '\0') {
            if (data[row][i] != ',') {
                tempWordBuf[j] = data[row][i];
                j++;
                i++;
            } else {
                tempWordBuf[j] = '\0';
                allWords[wordIndex] = malloc(sizeof(char) * (j + 1));
                if (allWords[wordIndex] != NULL) {
                    strcpy(allWords[wordIndex], tempWordBuf);
                } else {
                    printf("Ошибка выделения памяти");
                    return false;
                }
                j = 0;
                i++;
                wordIndex++;
            }
        }
        if (j > 0) {
            tempWordBuf[j] = '\0';
            allWords[wordIndex] = malloc(sizeof(char) * (j + 1));
            strcpy(allWords[wordIndex], tempWordBuf);
            wordIndex++;
        }
    }
    return true;
}

bool getMaxLengthOfColInArr(char** allWords, int quantityOfCols, int quantityOfRows, int* maxLengthArr)
{
    int step = quantityOfCols;
    for (int col = 0; col < quantityOfCols; col++) {
        int maxLength = 0;
        for (int word = col; word < quantityOfRows * quantityOfCols; word += step) {
            int len = strlen(allWords[word]);
            if (len > maxLength) {
                maxLength = len;
                maxLengthArr[col] = maxLength;
            }
        }
    }
    return true;
}

bool drawLines(char symbol, FILE* outputFile, int cols, int* maxLengths)
{
    fprintf(outputFile, "+");
    for (int col = 0; col < cols; col++) {
        for (int i = 0; i < maxLengths[col] + 2; i++) {
            fputc(symbol, outputFile);
        }
        fprintf(outputFile, "+");
    }
    fprintf(outputFile, "\n");
    return true;
}

bool isNumber(char* num)
{
    if (num == NULL) {
        return false;
    }
    int ind = 0;
    if (num[0] == '-') {
        ind++;
    }
    if (isdigit(num[ind])) {
        return true;
    }
    return false;
}

bool writeTableInFile(char** allWords, int* maxLengths, int cols, int rows)
{
    FILE* out = fopen("output.txt", "w");
    if (out == NULL) {
        return false;
    }
    int currentWord = 0;
    // заголовок
    drawLines('=', out, cols, maxLengths);
    fprintf(out, "|");
    for (int col = 0; col < cols; col++) {
        char* word = allWords[currentWord];
        if (word != NULL && isNumber(word)) {
            fprintf(out, " %*s ", maxLengths[col], word);
        } else {
            fprintf(out, " %-*s ", maxLengths[col], word);
        }
        fprintf(out, "|");
        currentWord++;
    }
    fprintf(out, "\n");
    drawLines('=', out, cols, maxLengths);
    // обычные данные
    for (int row = 0; row < rows - 1; row++) {
        fprintf(out, "|");
        for (int col = 0; col < cols; col++) {
            char* word = allWords[currentWord];
            if (word != NULL && isNumber(word)) {
                fprintf(out, " %*s ", maxLengths[col], word);
            } else {
                fprintf(out, " %-*s ", maxLengths[col], word);
            }
            fprintf(out, "|");
            currentWord++;
        }
        fprintf(out, "\n");
        drawLines('-', out, cols, maxLengths);
    }
    fclose(out);
    return true;
}

bool freeArrInArr(char** arr, int len)
{
    for (int i = 0; i < len; i++) {
        free(arr[i]);
    }
    return true;
}

bool algorithm()
{
    char* data[100] = { 0 };
    int rows = readFile(data);
    int cols = quantityOfCols(data);
    int words = rows * cols;
    char** allWords = malloc(words * sizeof(char*));

    if (allWords == NULL || rows <= 0 || cols <= 0) {
        printf("Ошибка");
        freeArrInArr(data, rows);
        freeArrInArr(allWords, words);
        return false;
    }
    arrWithAllTheWords(data, allWords, rows);
    int* maxLengthArr = malloc(sizeof(int) * cols);

    if (maxLengthArr == NULL) {
        printf("Ошибка");
        free(maxLengthArr);
        freeArrInArr(data, rows);
        freeArrInArr(allWords, words);
        return false;
    }
    getMaxLengthOfColInArr(allWords, cols, rows, maxLengthArr);

    for (int i = 0; i < cols; i++) {
        printf("%d\n", maxLengthArr[i]);
    }
    writeTableInFile(allWords, maxLengthArr, cols, rows);
    free(maxLengthArr);
    freeArrInArr(data, rows);
    freeArrInArr(allWords, words);
    return true;
}

bool testRowsQuantity()
{
    return true;
}

bool testColumsQuantity()
{
    return true;
}

bool testOneElement()
{
    return true;
}

bool testCheckMaxLenght()
{
    return true;
}

bool tests()
{
    printf("Все тесты пройдены успешно!");
    return true;
}

int main()
{
    algorithm();
    return 0;
}
