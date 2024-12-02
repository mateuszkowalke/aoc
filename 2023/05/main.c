#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  long long start;
  long long len;
  bool processed;
} range;

int main() {
  FILE *f;
  if ((f = fopen("input.txt", "r")) == NULL) {
    perror("can't open input.txt");
    exit(1);
  }
  char *l = NULL;
  char *line = NULL;
  size_t n = 0;
  long long res = 0;
  range **seed_ranges = malloc(sizeof(range *) * 64);
  size_t seed_ranges_size = 64;
  size_t seed_ranges_len = 0;
  size_t seed_range_i = 0;
  bool in_map = false;
  long long **maps = malloc(sizeof(uint *) * 64);
  size_t maps_i = 0;
  while (getline(&l, &n, f) != -1) {
    line = l;
    if (strstr(l, "seeds:") != NULL) {
      while (*line++ != ':')
        ;
      bool is_range = false;
      long long base;
      long long range_len;
      char buf[32];
      size_t buf_len = 0;
      while (*line++ != '\0') {
        if (isdigit(*line)) {
          buf[buf_len++] = *line;
        } else if (buf_len > 0) {
          buf[buf_len] = '\0';
          buf_len = 0;
          if (!is_range) {
            base = atoll(buf);
          }
          if (is_range) {
            range_len = atoll(buf);
            range *curr_range = malloc(sizeof(range));
            curr_range->start = base;
            curr_range->len = range_len;
            curr_range->processed = false;
            seed_ranges[seed_range_i++] = curr_range;
            seed_ranges_len++;
            if (seed_ranges_len == seed_ranges_size) {
              seed_ranges_size *= 2;
              seed_ranges =
                  realloc(seed_ranges, sizeof(range) * seed_ranges_size);
            }
            base = 0;
          }
          is_range = !is_range;
        }
      }
      seed_range_i = 0;
      for (int i = 0; i < seed_ranges_len; i++) {
        printf("%lld, %lld\n", seed_ranges[i]->start, seed_ranges[i]->len);
      }
      continue;
    }
    if (strlen(l) < 2) {
      printf("ranges_len: %ld\n", seed_ranges_len);
      for (int i = 0; i < seed_ranges_len; i++) {
        printf("%lld, %lld\n", seed_ranges[i]->start, seed_ranges[i]->len);
      }
      for (int j = 0; j < maps_i; j++) {
        printf("map[%2d]: %10lld, %10lld, %10lld\n", j, maps[j][0], maps[j][1],
               maps[j][2]);
      }
      in_map = false;
      for (int i = 0; i < seed_ranges_len; i++) {
        for (int j = 0; j < maps_i; j++) {
          long long low = maps[j][1];
          long long high = maps[j][1] + maps[j][2];
          if (!seed_ranges[i]->processed && seed_ranges[i]->start >= low &&
              seed_ranges[i]->start + seed_ranges[i]->len <= high) {
            seed_ranges[i]->start =
                seed_ranges[i]->start - maps[j][1] + maps[j][0];
            seed_ranges[i]->processed = true;
          } else if (!seed_ranges[i]->processed &&
                     seed_ranges[i]->start >= low &&
                     seed_ranges[i]->start < high) {
            long long diff = seed_ranges[i]->start + seed_ranges[i]->len - high;
            seed_ranges[i]->start =
                seed_ranges[i]->start - maps[j][1] + maps[j][0];
            seed_ranges[i]->len -= diff;
            seed_ranges[i]->processed = true;
            range *new_range = malloc(sizeof(range));
            new_range->start = high;
            new_range->len = diff;
            new_range->processed = true;
            seed_ranges[seed_ranges_len++] = new_range;
            if (seed_ranges_len == seed_ranges_size) {
              seed_ranges_size *= 2;
              seed_ranges =
                  realloc(seed_ranges, sizeof(range) * seed_ranges_size);
            }
          } else if (!seed_ranges[i]->processed &&
                     seed_ranges[i]->start + seed_ranges[i]->len > low &&
                     seed_ranges[i]->start + seed_ranges[i]->len <= high) {
            long long diff = low - seed_ranges[i]->start;
            range *new_range = malloc(sizeof(range));
            new_range->start = low;
            new_range->len = seed_ranges[i]->len - diff;
            new_range->start = new_range->start - maps[j][1] + maps[j][0];
            new_range->processed = true;
            seed_ranges[i]->len = diff;
            seed_ranges[i]->processed = true;
            seed_ranges[seed_ranges_len++] = new_range;
            if (seed_ranges_len == seed_ranges_size) {
              seed_ranges_size *= 2;
              seed_ranges =
                  realloc(seed_ranges, sizeof(range) * seed_ranges_size);
            }
          }
        }
      }
      for (int i = 0; i < seed_ranges_len; i++) {
        seed_ranges[i]->processed = false;
      }
      maps_i = 0;
      seed_range_i = 0;
    }
    if (in_map) {
      char buf[32];
      size_t buf_len = 0;
      long long *map = malloc(sizeof(uint) * 3);
      size_t map_i = 0;
      char c;
      while ((c = *line++) != '\0') {
        if (isdigit(c)) {
          buf[buf_len++] = c;
        } else if (buf_len > 0) {
          buf[buf_len] = '\0';
          buf_len = 0;
          map[map_i++] = atoll(buf);
        }
      }
      maps[maps_i++] = map;
    }
    if (strstr(l, "map") != NULL) {
      in_map = true;
    }
  }
  for (int i = 0; i < seed_ranges_len; i++) {
    printf("%lld, %lld\n", seed_ranges[i]->start, seed_ranges[i]->len);
  }
  for (int i = 0; i < seed_ranges_len; i++) {
    if (res == 0) {
      res = seed_ranges[i]->start;
    } else if (res > seed_ranges[i]->start) {
      res = seed_ranges[i]->start;
    }
  }
  printf("\n");
  printf("%lld", res);
  printf("\n");
  fclose(f);
  if (l) {
    free(l);
  }
}
