/*
    Convert dex file to mpk
    ----------
    - Only grabs necessary data, doesn't verify, rebuild, etc.
    - Usage:
        ./dex_to_mpk file.dex file.mpk
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEX_HEADER_SIZE 4160
#define MPK_SIZE        131072

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s input.dex output.mpk\n", argv[0]);
        return 1;
    }

    FILE *in = fopen(argv[1], "rb");
    if (!in) {
        perror("fopen input");
        return 1;
    }

    FILE *out = fopen(argv[2], "wb");
    if (!out) {
        perror("fopen output");
        fclose(in);
        return 1;
    }

    fseek(in, 0, SEEK_END);
    long size = ftell(in);
    rewind(in);

    if (size <= DEX_HEADER_SIZE) {
        fprintf(stderr, "File too small to be DexDrive\n");
        return 1;
    }

    long data_size = size - DEX_HEADER_SIZE;

    unsigned char *buf = calloc(1, MPK_SIZE);
    if (!buf) {
        perror("calloc");
        return 1;
    }

    // Read whatever data exists after header
    fseek(in, DEX_HEADER_SIZE, SEEK_SET);
    fread(buf, 1, data_size, in);

    fwrite(buf, 1, MPK_SIZE, out);

    free(buf);
    fclose(in);
    fclose(out);
    return 0;
}
