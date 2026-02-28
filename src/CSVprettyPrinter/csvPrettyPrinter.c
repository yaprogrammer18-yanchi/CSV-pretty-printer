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
отсутствие пустых ячеек ",," в заголовках
*/

int readFile(char** data, const char* filepath)
{
    FILE* file = fopen(filepath, "r");
    if (file == NULL) {
        printf("file not found!\n");
        return false;
    }
    int linesRead = 0;
    char temporaryBuffer[256] = { 0 };
    while ((linesRead < 100) && (fgets(temporaryBuffer, sizeof(temporaryBuffer), file) != NULL)) {
        size_t indexOfLineBreak = strcspn(temporaryBuffer, "\n");
        if (indexOfLineBreak < sizeof(temporaryBuffer)) {
            temporaryBuffer[indexOfLineBreak] = '\0';
        } else {
            temporaryBuffer[sizeof(temporaryBuffer) - 1] = '\0';
        }
        char* buffer = malloc(strlen(temporaryBuffer) + 1);
        if (buffer == NULL) {
            printf("Ошибка выделения памяти!\n");
            break;
        }
        strlcpy(buffer, temporaryBuffer, 256);
        data[linesRead] = buffer;
        linesRead++;
    }
    fclose(file);
    return linesRead;
}

int quantityOfCols(char** data)
{
    int columsQuantity = 0;
    if (data[0] == 0) {
        return columsQuantity;
    }

    size_t len = strlen(data[0]);
    for (size_t i = 0; i < len; i++) {
        if (data[0][i] == ',') {
            columsQuantity += 1;
        }
    }
    columsQuantity += 1;
    return columsQuantity;
}

bool arrWithAllTheWords(char** data, char** allWords, int rows, int maxWords)
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
                if (wordIndex >= maxWords) {
                    printf("Ошибка: превышено максимальное количество слов\n");
                    return false;
                }
                allWords[wordIndex] = malloc(sizeof(char) * (j + 1));
                if (allWords[wordIndex] != NULL) {
                    strncpy(allWords[wordIndex], tempWordBuf, j + 1);
                } else {
                    printf("Ошибка выделения памяти\n");
                    return false;
                }
                j = 0;
                i++;
                wordIndex++;
            }
        }
        if (j > 0) {
            tempWordBuf[j] = '\0';
            if (wordIndex >= maxWords) {
                printf("Ошибка: превышено максимальное количество слов\n");
                return false;
            }
            allWords[wordIndex] = malloc(sizeof(char) * (j + 1));
            strncpy(allWords[wordIndex], tempWordBuf, j + 1);
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
            if (allWords[word] == NULL) {
                continue;
            }
            int len = (int)strlen(allWords[word]);
            if (len > maxLength) {
                maxLength = len;
            }
        }
        maxLengthArr[col] = maxLength;
    }
    return true;
}

bool drawLines(char symbol, FILE* outputFile, int cols, const int* maxLengths)
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

bool isNumber(const char* num)
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

bool writeTableInFile(char** allWords, const int* maxLengths, int cols, int rows, const char* filepath)
{
    FILE* out = fopen(filepath, "w");
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
        arr[i] = NULL;
    }
    return true;
}

bool algorithm(const char* filepathInput, const char* filepathOutput)
{
    char* data[100] = { 0 };
    int rows = readFile(data, filepathInput);

    if (rows <= 0) {
        printf("Ошибка\n");
        freeArrInArr(data, rows);
        return false;
    }

    int cols = quantityOfCols(data);
    if (cols <= 0) {
        printf("Ошибка\n");
        freeArrInArr(data, rows);
        return false;
    }
    int words = rows * cols;
    char** allWords = calloc(words, sizeof(char*));

    if (allWords == NULL) {
        printf("Ошибка\n");
        freeArrInArr(data, rows);
        free(allWords);
        return false;
    }
    arrWithAllTheWords(data, allWords, rows, words);
    int* maxLengthArr = malloc(sizeof(int) * cols);

    if (maxLengthArr == NULL) {
        printf("Ошибка\n");
        free(maxLengthArr);
        freeArrInArr(data, rows);
        freeArrInArr(allWords, words);
        free(allWords);
        return false;
    }
    getMaxLengthOfColInArr(allWords, cols, rows, maxLengthArr);
    writeTableInFile(allWords, maxLengthArr, cols, rows, filepathOutput);
    free(maxLengthArr);
    freeArrInArr(data, rows);
    freeArrInArr(allWords, words);
    free(allWords);
    return true;
}

//--------------------------------tests---------------------------------------------------

bool testRowsQuantity(void)
{
    printf("Проверка кол-ва строк...");
    // 6 строк
    char* data1[100] = { 0 };
    int a1 = readFile(data1, "src/CSVprettyPrinter/testInput1.csv");
    if (a1 != 6) {
        freeArrInArr(data1, a1);
        return false;
    }

    // 0 строк
    char* data2[100] = { 0 };
    int a2 = readFile(data2, "src/CSVprettyPrinter/testInput5.csv");
    if (a2 != 0) {
        freeArrInArr(data2, a2);
        return false;
    }

    freeArrInArr(data2, a2);
    freeArrInArr(data1, a1);
    printf("завершена успешно\n");
    return true;
}

bool testColumsQuantity(void)
{
    printf("Проверка кол-ва столбцов...");
    // 5 столбцов
    char* data1[100] = { 0 };
    int a1 = readFile(data1, "src/CSVprettyPrinter/testInput1.csv");
    int b1 = quantityOfCols(data1);
    if (b1 != 5) {
        freeArrInArr(data1, a1);
        return false;
    }

    // 0 столбцов
    char* data2[100] = { 0 };
    int a2 = readFile(data2, "src/CSVprettyPrinter/testInput5.csv");
    int b2 = quantityOfCols(data2);
    if (b2 != 0) {
        freeArrInArr(data2, a2);
        return false;
    }
    freeArrInArr(data2, a2);
    freeArrInArr(data1, a1);
    printf("завершена успешно\n");
    return true;
}

bool testCheckMaxLenght(void)
{
    char* data[100] = { 0 };
    int rows = readFile(data, "src/CSVprettyPrinter/testInput1.csv");
    int cols = quantityOfCols(data);
    int words = rows * cols;
    char** allWords = calloc(words, sizeof(char*));
    arrWithAllTheWords(data, allWords, rows, words);
    int* maxLengthArr = malloc(sizeof(int) * cols);
    getMaxLengthOfColInArr(allWords, cols, rows, maxLengthArr);
    int expected[5];
    expected[0] = 9;
    expected[1] = 6;
    expected[2] = 8;
    expected[3] = 7;
    expected[4] = 6;

    for (int i = 0; i < cols; i++) {
        if (maxLengthArr[i] != expected[i]) {
            free(maxLengthArr);
            free(allWords);
            return false;
        }
    }
    free(maxLengthArr);
    free(allWords);

    return true;
}

bool testSimpleOccasion(void)
{
    printf("Проверка простого случая...");
    algorithm("src/CSVprettyPrinter/testInput2.csv", "src/CSVprettyPrinter/testOutput.txt");
    char* expected[7] = { 0 };
    expected[0] = "+====+========+========+\n";
    expected[1] = "| ID | Name   | Active |\n";
    expected[2] = "+====+========+========+\n";
    expected[3] = "|  1 | Item A | true   |\n";
    expected[4] = "+----+--------+--------+\n";
    expected[5] = "|  2 | Item B | false  |\n";
    expected[6] = "+----+--------+--------+\n";
    FILE* file = fopen("src/CSVprettyPrinter/testOutput.txt", "r");
    if (file == NULL) {
        printf("file not found!\n");
        return false;
    }
    char buffer[256];
    int lineNum = 0;
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        if (lineNum >= 7) {
            break;
        }
        if (strcmp(buffer, expected[lineNum]) != 0) {
            fclose(file);
            return false;
        }
        lineNum++;
    }
    fclose(file);
    printf("завершена успешно!\n");
    return true;
}

int tests(void)
{
    if (!testRowsQuantity()) {
        printf("с ошибкой в подстчете кол-ва строк\n");
        return 1;
    }

    if (!testColumsQuantity()) {
        printf("с ошибкой  в подсчете кол-ва столбцов\n");
        return 1;
    }
    if (!testSimpleOccasion()) {
        printf("с ошибкой  в тестировании простого случая\n");
        return 1;
    }

    if (!testCheckMaxLenght()) {
        printf("с ошибкой в тестировании полученных максимальных длин столбцов\n");
        return 1;
    }

    printf("Все тесты пройдены успешно!\n");
    return 0;
}

int main(int argc, char* argv[])
{
    if (argc > 1 && strcmp(argv[1], "--test") == 0) {
        printf("Запуск тестов...\n");
        tests();
        return 0;
    }
    // пример работы (из условия задачи)
    algorithm("src/CSVprettyPrinter/testInput2.csv", "src/CSVprettyPrinter/output.txt");
    return 0;
}
