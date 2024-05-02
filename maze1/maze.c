#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DIM 100
#define MIN_DIM 5
#define EXIT_SUCCESS 0
#define EXIT_ARG_ERROR 1
#define EXIT_FILE_ERROR 2
#define EXIT_MAZE_ERROR 3

typedef struct {
    int x, y;
} Coord;

typedef struct {
    char **data;
    int rows, cols;
    Coord start, end;
} Maze;

// Function prototypes
int initializeMaze(Maze *m, int rows, int cols);
void freeMaze(Maze *m);
int readMazeFromFile(Maze *m, FILE *fp);
void printMaze(const Maze *m, const Coord *player);
void movePlayer(const Maze *m, Coord *player, char direction);
int detectMazeSize(const char *filename, int *rows, int *cols);

int initializeMaze(Maze *m, int rows, int cols) {
    m->rows = rows;
    m->cols = cols;
    m->data = malloc(rows * sizeof(char *));
    if (!m->data) return 1;

    for (int i = 0; i < rows; ++i) {
        m->data[i] = malloc((cols + 1) * sizeof(char)); // +1 for null terminator
        if (!m->data[i]) {
            while (--i >= 0) free(m->data[i]);
            free(m->data);
            return 1;
        }
    }
    return 0;
}

void freeMaze(Maze *m) {
    for (int i = 0; i < m->rows; ++i) {
        free(m->data[i]);
    }
    free(m->data);
}

int readMazeFromFile(Maze *m, FILE *fp) {
    char buffer[MAX_DIM + 2];
    int row = 0;
    while (fgets(buffer, sizeof(buffer), fp) && row < m->rows) {
        if (buffer[strlen(buffer) - 1] == '\n')
            buffer[strlen(buffer) - 1] = '\0';  // Remove newline character
        strcpy(m->data[row], buffer);
       
        for (int col = 0; col < m->cols; ++col) {
            if (m->data[row][col] == 'S') {
                m->start.x = col;
                m->start.y = row;
            } else if (m->data[row][col] == 'E') {
                m->end.x = col;
                m->end.y = row;
            }
        }
        ++row;
    }
    return 0;
}

void printMaze(const Maze *m, const Coord *player) {
    printf("\n");
    for (int i = 0; i < m->rows; ++i) {
        for (int j = 0; j < m->cols; ++j) {
            if (i == player->y && j == player->x)
                printf("X");
            else
                printf("%c", m->data[i][j]);
        }
        printf("\n");
    }
}

void movePlayer(const Maze *m, Coord *player, char direction) {
    int dx = 0, dy = 0;
    switch (direction) {
        case 'W': case 'w': dy = -1; break;
        case 'S': case 's': dy = 1; break;
        case 'A': case 'a': dx = -1; break;
        case 'D': case 'd': dx = 1; break;
        default: printf("Invalid move.\n"); return;
    }
    int newX = player->x + dx;
    int newY = player->y + dy;
    if (newX >= 0 && newX < m->cols && newY >= 0 && newY < m->rows && m->data[newY][newX] != '#') {
        player->x = newX;
        player->y = newY;
        if (newX == m->end.x && newY == m->end.y) {
            printf("Congratulations! You have reached the end.\n");
            exit(EXIT_SUCCESS);
        }
    } else {
        printf("Move blocked by wall or boundary.\n");
    }
}

int detectMazeSize(const char *filename, int *rows, int *cols) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return 1;
    char line[MAX_DIM + 2];
    *rows = 0;
    int maxWidth = 0;

    while (fgets(line, sizeof(line), fp)) {
        size_t len = strcspn(line, "\n");
        if (line[len] == '\n') line[len] = '\0';
        if (len > maxWidth) maxWidth = len;
        (*rows)++;
    }
    *cols = maxWidth;
    fclose(fp);
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <maze file path>\n", argv[0]);
        return EXIT_ARG_ERROR;
    }

    int rows, cols;
    if (detectMazeSize(argv[1], &rows, &cols)) {
        fprintf(stderr, "Error determining the size of the maze.\n");
        return EXIT_FILE_ERROR;
    }

    Maze m;
    if (initializeMaze(&m, rows, cols)) {
        fprintf(stderr, "Failed to allocate memory for the maze.\n");
        return EXIT_MAZE_ERROR;
    }

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        freeMaze(&m);
        fprintf(stderr, "Failed to open file: %s\n", argv[1]);
        return EXIT_FILE_ERROR;
    }

    if (readMazeFromFile(&m, fp)) {
        fclose(fp);
        freeMaze(&m);
        fprintf(stderr, "Failed to load maze from file.\n");
        return EXIT_MAZE_ERROR;
    }
    fclose(fp);

    Coord player = m.start;
    char move;
    printf("Use W, A, S, D to move. Press Q to quit.\n");
    while (scanf(" %c", &move), move != 'Q' && move != 'q') {
        if (move == 'M' || move == 'm') {
            printMaze(&m, &player);
        } else {
            movePlayer(&m, &player, move);
        }
    }

    freeMaze(&m);
    return EXIT_SUCCESS;
}