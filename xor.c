/**
 * 
 * Jayson Mourier - 2022
 * jayson.mourier@parisnanterre.fr 
 *
*/  

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <omp.h>

#define BUFFER_SIZE 255
#define XOR_VALUE '\x35'
#define READ_BINARY "rb+"
#define WRITE_BINARY "wb+"
#define SOURCE_FILE "file.txt"
#define OUTPUT_FILE "file.txt.xor"

// load a file from disk
FILE *openFile(const char *__restrict__ src, const char *__restrict__ mode) 
    __attribute__((nonnull (1)));

// encrypt or decrypt the file and save it to the disk
void xor(FILE *__restrict__ file, FILE *__restrict__ dst) 
    __attribute__((nonnull (1, 2)));

int
main(void)
{
    FILE *src = openFile(SOURCE_FILE, READ_BINARY);
    FILE *dst = openFile(OUTPUT_FILE, WRITE_BINARY);
    
    xor(src, dst);

    fclose(src);
    fclose(dst);
    return 0;
}

FILE *openFile(const char *__restrict__ src, const char *__restrict__ mode)
{
    FILE *tmp = fopen64(src, mode);

    if(NULL == tmp)
    {
        fprintf(stderr, "load file error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return tmp;
}

void xor(FILE *__restrict__ file, FILE *__restrict__ dst)
{
    uint8_t bufferSrc[BUFFER_SIZE];
    uint8_t bufferDst[BUFFER_SIZE];
    long long n;

    memset(bufferSrc, '\0', sizeof(uint8_t) * BUFFER_SIZE);
    memset(bufferDst, '\0', sizeof(uint8_t) * BUFFER_SIZE);

    // n < BUFFER_SIZE
    while ((n = fread(bufferSrc, sizeof(uint8_t), BUFFER_SIZE, file)) > 0)
    {
        #pragma omp parallel for
        for(long long i = 0; i < n; ++i)
        {
            bufferDst[i] = bufferSrc[i] ^ XOR_VALUE;
        }

        // write to the disk
        fwrite(bufferDst, sizeof(uint8_t), n, dst);
        memset(bufferDst, '\0', sizeof(uint8_t) * BUFFER_SIZE);
    }
}
