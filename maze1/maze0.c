#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 100

typedef struct {
    int x;
    int y;
} coord;

typedef struct {
    char **map;
    int height;
    int width;
    coord start;
    coord end;
} maze;

int create_maze(maze *this, int height, int width) {
    this->map = (char **)malloc(height * sizeof(char *));
    if (this->map == NULL) {
        return 1; // Allocation failed
    }
    for (int i = 0; i < height; i++) {
        this->map[i] = (char *)malloc(width * sizeof(char));
        if (this->map[i] == NULL) {
            return 1; // Allocation failed
        }
    }
    this->height = height;
    this->width = width;
    return 0; // Success
}

void free_maze(maze *this) {
    for (int i = 0; i < this->height; i++) {
        free(this->map[i]);
    }
    free(this->map);
}

int get_width(FILE *file) {
    int width;
    fscanf(file, "%d", &width);
    if (width < MIN_DIM || width > MAX_DIM) {
        return 0; // Invalid width
    }
    return width;
}

int get_height(FILE *file) {
    int height;
    fscanf(file, "%d", &height);
    if (height < MIN_DIM || height > MAX_DIM) {
        return 0; // Invalid height
    }
    return height;
}

int read_maze(maze *this, FILE *file) {
    fopen(file, r);
    for (int i = 0; i < this->height; i++) {
        fscanf(file, "%s", this->map[i]);
        for (int j = 0; j < this->width; j++) {
            if (this->map[i][j] == 'S') {
                this->start.x = j;
                this->start.y = i;
            } else if (this->map[i][j] == 'E') {
                this->end.x = j;
                this->end.y = i;
            }
        }
    }
    return 0; // Success
}

void print_maze(maze *this, coord *player) {
    printf("\n");
    for (int i = 0; i < this->height; i++) {
        for (int j = 0; j < this->width; j++) {
            if (player->x == j && player->y == i) {
                printf("X");
            } else {
                printf("%c", this->map[i][j]);
            }
        }
        printf("\n");
    }
}

void move(maze *this, coord *player, char direction) {
    int newX = player->x, newY = player->y;
    switch (direction) {
        case 'W':
            newY--;
            break;
        case 'A':
            newX--;
            break;
        case 'S':
            newY++;
            break;
        case 'D':
            newX++;
            break;
        default:
            printf("Invalid move!\n");
            return;
    }
    if (newX >= 0 && newX < this->width && newY >= 0 && newY < this->height && this->map[newY][newX] != '#') {
        player->x = newX;
        player->y = newY;
    } else {
        printf("Invalid move!\n");
    }
}

int has_won(maze *this, coord *player) {
    return player->x == this->end.x && player->y == this->end.y;
}

int main(int argc, char *argv[]) {
    // Check if the correct number of command-line arguments is provided
    if (argc != 2) {
        printf("Usage: %s <maze_file>\n", argv[0]);
        return EXIT_ARG_ERROR;
    }

    FILE *f = fopen(argv[1], "r");
    if (f == NULL) {
        printf("Error opening file!\n");
        return EXIT_FILE_ERROR;
    }

    int width = get_width(f);
    if (width == 0) {
        printf("Invalid width in maze file!\n");
        fclose(f);
        return EXIT_MAZE_ERROR;
    }

    int height = get_height(f);
    if (height == 0) {
        printf("Invalid height in maze file!\n");
        fclose(f);
        return EXIT_MAZE_ERROR;
    }

    maze this_maze;
    if (create_maze(&this_maze, height, width) != 0) {
        printf("Memory allocation error!\n");
        fclose(f);
        return EXIT_MAZE_ERROR;
    }

    if (read_maze(&this_maze, f) != 0) {
        printf("Error reading maze file!\
