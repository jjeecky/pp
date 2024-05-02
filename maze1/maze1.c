#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_DIM 100
#define MIN_DIM 5

#define EXIT_SUCCESS 0
#define EXIT_ARG_ERROR 1
#define EXIT_FILE_ERROR 2
#define EXIT_MAZE_ERROR 3

typedef struct __Coord
{
    int x;
    int y;
} coord;

typedef struct __Maze
{
    char **map;
    int height;
    int width;
    coord start;
    coord end;
} maze;

int create_maze(maze *this, int height, int width)
{
    this->height = height;
    this->width = width;
    this->map = (char **)malloc(sizeof(char *) * height);
    if (this->map == NULL)
    {
        printf("Error: Memory allocation failed.\n");
        return EXIT_MAZE_ERROR;
    }

    for (int i = 0; i < height; i++)
    {
        this->map[i] = (char *)malloc(sizeof(char) * (width + 1)); // +1 for null terminator
        if (this->map[i] == NULL)
        {
            printf("Error: Memory allocation failed.\n");
            for (int j = 0; j < i; j++)
                free(this->map[j]);
            free(this->map);
            return EXIT_MAZE_ERROR;
        }
    }

    return EXIT_SUCCESS;
}

void free_maze(maze *this)
{
    for (int i = 0; i < this->height; i++)
        free(this->map[i]);
    free(this->map);
}

int read_maze(maze *this, const char *f)
{
    FILE *file = fopen(f, "r");
    if (file == NULL)
    {
        printf("Error: Failed to open file %s.\n", f);
        return EXIT_FILE_ERROR;
    }

    int width, height;
    if (fscanf(file, "%d %d", &width, &height) != 2)
    {
        printf("Error: Failed to read width and height from maze file.\n");
        fclose(file);
        return EXIT_MAZE_ERROR;
    }

    if (width < MIN_DIM || width > MAX_DIM || height < MIN_DIM || height > MAX_DIM)
    {
        printf("Error: Invalid width or height.\n");
        fclose(file);
        return EXIT_MAZE_ERROR;
    }

    if (create_maze(this, height, width) != EXIT_SUCCESS)
    {
        fclose(file);
        return EXIT_MAZE_ERROR;
    }

    for (int i = 0; i < height; i++)
    {
        if (fscanf(file, "%s", this->map[i]) != 1)
        {
            printf("Error: Failed to read maze layout.\n");
            free_maze(this);
            fclose(file);
            return EXIT_MAZE_ERROR;
        }
        if (strlen(this->map[i]) != width)
        {
            printf("Error: Maze layout width does not match specified width.\n");
            free_maze(this);
            fclose(file);
            return EXIT_MAZE_ERROR;
        }
        for (int j = 0; j < width; j++)
        {
            if (this->map[i][j] == 'S')
            {
                this->start.x = j;
                this->start.y = i;
            }
            else if (this->map[i][j] == 'E')
            {
                this->end.x = j;
                this->end.y = i;
            }
        }
    }

    fclose(file);
    return EXIT_SUCCESS;
}


int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <maze_file>\n", argv[0]);
        return EXIT_ARG_ERROR;
    }

    maze this_maze;
    int result = read_maze(&this_maze, argv[1]);
    if (result != EXIT_SUCCESS)
    {
        printf("Error: ");
        if (result == EXIT_FILE_ERROR)
            printf("Failed to open or read maze file.\n");
        else if (result == EXIT_MAZE_ERROR)
            printf("Invalid maze file.\n");
        return result;
    }

    // Maze reading successful, proceed with the rest of the program
    return EXIT_SUCCESS;
}
