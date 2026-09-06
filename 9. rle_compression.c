#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RUN 255

static int compress_file(const char *in_path, const char *out_path) {
    FILE *in = fopen(in_path, "rb");
    if (!in) { perror("Error opening input file"); return 1; }
    FILE *out = fopen(out_path, "wb");
    if (!out) { perror("Error opening output file"); fclose(in); return 1; }

    long original_size = 0, compressed_size = 0;

    int c = fgetc(in);
    while (c != EOF) {
        unsigned char current_byte = (unsigned char)c;
        unsigned char run_length = 1;
        original_size++;

        int next;
        while (run_length < MAX_RUN && (next = fgetc(in)) != EOF) {
            original_size++;
            if ((unsigned char)next == current_byte) {
                run_length++;
            } else {
                
                fputc(run_length, out);
                fputc(current_byte, out);
                compressed_size += 2;
                current_byte = (unsigned char)next;
                run_length = 1;
            }
        }
        fputc(run_length, out);
        fputc(current_byte, out);
        compressed_size += 2;

        c = fgetc(in);
    }

    fclose(in);
    fclose(out);

    printf("Compressed '%s' (%ld bytes) -> '%s' (%ld bytes)\n",
           in_path, original_size, out_path, compressed_size);
    if (original_size > 0) {
        printf("Compression ratio: %.2f%%\n",
               100.0 * (1.0 - (double)compressed_size / (double)original_size));
    }
    return 0;
}

static int decompress_file(const char *in_path, const char *out_path) {
    FILE *in = fopen(in_path, "rb");
    if (!in) { perror("Error opening input file"); return 1; }
    FILE *out = fopen(out_path, "wb");
    if (!out) { perror("Error opening output file"); fclose(in); return 1; }

    long compressed_size = 0, decompressed_size = 0;
    int count_byte, value_byte;

    while ((count_byte = fgetc(in)) != EOF) {
        value_byte = fgetc(in);
        if (value_byte == EOF) {
            fprintf(stderr, "Warning: truncated RLE stream (odd byte count).\n");
            break;
        }
        compressed_size += 2;
        for (int i = 0; i < count_byte; i++) {
            fputc(value_byte, out);
            decompressed_size++;
        }
    }

    fclose(in);
    fclose(out);

    printf("Decompressed '%s' (%ld bytes) -> '%s' (%ld bytes)\n",
           in_path, compressed_size, out_path, decompressed_size);
    return 0;
}

static void print_usage(const char *prog_name) {
    printf("Usage:\n");
    printf("  %s compress   <input_file> <output_file>\n", prog_name);
    printf("  %s decompress <input_file> <output_file>\n", prog_name);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        
        printf("(No arguments supplied - running a built-in demo instead.)\n\n");
        print_usage(argv[0]);
        printf("\n--- Demo ---\n");

        const char *demo_in = "demo_input.txt";
        const char *demo_rle = "demo_compressed.rle";
        const char *demo_out = "demo_output.txt";

        FILE *f = fopen(demo_in, "w");
        fprintf(f, "aaaaaaaaaabbbbbccccccccccccccccccccddddddddddeeeeeeeeeeeeeeee");
        fclose(f);

        compress_file(demo_in, demo_rle);
        decompress_file(demo_rle, demo_out);

        
        FILE *a = fopen(demo_in, "rb");
        FILE *b = fopen(demo_out, "rb");
        int ca, cb, mismatch = 0;
        do {
            ca = fgetc(a);
            cb = fgetc(b);
            if (ca != cb) { mismatch = 1; break; }
        } while (ca != EOF);
        fclose(a);
        fclose(b);

        printf("Round-trip check: %s\n", mismatch ? "MISMATCH!" : "OK, files match.");
        return 0;
    }

    const char *mode = argv[1];
    const char *input = argv[2];
    const char *output = argv[3];

    if (strcmp(mode, "compress") == 0) {
        return compress_file(input, output);
    } else if (strcmp(mode, "decompress") == 0) {
        return decompress_file(input, output);
    } else {
        fprintf(stderr, "Unknown mode '%s'.\n", mode);
        print_usage(argv[0]);
        return 1;
    }
}
