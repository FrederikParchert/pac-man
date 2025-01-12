#include <stdio.h>
#include <stdlib.h>
#include <conio.h> // Für getch() und kbhit() (nur auf Windows verfügbar)
#include <stdbool.h>
#include <time.h>
#include <windows.h> // Für Sleep()

#define WIDTH 40
#define HEIGHT 20
#define MAX_GHOSTS 4
#define HIGHSCORE_FILE "highscores.txt"
#define HIGHSCORE_COUNT 10

char grid[HEIGHT][WIDTH]; // Spielfeld
typedef struct 
{
    int x, y;
} 
Position;

typedef struct 
{
    char name[20];
    int score;
} 
HighscoreEntry;

Position pacman;
Position ghosts[MAX_GHOSTS];
char ghost_prev_char[MAX_GHOSTS]; // Speichert, was unter jedem Geist war
int score = 0;
bool running = true;

// Spielfeld initialisieren
void initializeGrid() 
{
    char labyrinth[HEIGHT][WIDTH] = 
    {
        "########################################",
        "#.............................#........#",
        "#......##.....#########.......#..####..#",
        "#......##..#......#.....###............#",
        "#......##..#..##..#...........#...#....#",
        "#...####...#......###..####...#........#",
        "#............##...P......#....#......###",
        "########.................#....####.....#",
        "#........###....########......####.....#",
        "#..###....................##...........#",
        "#.......#####....###....########.......#",
        "#...........#.............##........####",
        "#....####...#......###..........##.....#",
        "#....#...........##############........#",
        "#...........#....#.....#............####",
        "#....########..........#.###....#......#",
        "#...............####...#......#####....#",
        "#.....########.........#........#......#",
        "#...............####...................#",
        "########################################"
    };

    for (int i = 0; i < HEIGHT; i++) 
    {
        for (int j = 0; j < WIDTH; j++) 
        {
            grid[i][j] = labyrinth[i][j];
            if (labyrinth[i][j] == 'P') 
            {
                pacman.x = j;
                pacman.y = i;
                grid[i][j] = 'P';
            }
        }
    }

    // Geister initialisieren
    srand(time(NULL));
    for (int i = 0; i < MAX_GHOSTS; i++) 
    {
        int gx, gy;
        do 
        {
            gx = rand() % WIDTH;
            gy = rand() % HEIGHT;
        } 
        while (grid[gy][gx] != '.' && grid[gy][gx] != ' ');
        ghosts[i].x = gx;
        ghosts[i].y = gy;
        ghost_prev_char[i] = grid[gy][gx]; // Speichert das ursprüngliche Zeichen
        grid[gy][gx] = 'G';
    }
}

// Spielfeld anzeigen
void drawGrid() {
    COORD cursorPosition = {0, 0};
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(console, cursorPosition); // Cursor zurücksetzen

    for (int i = 0; i < HEIGHT; i++) 
    {
        for (int j = 0; j < WIDTH; j++) 
        {
            printf("%c", grid[i][j]);
        }
        printf("\n");
    }
    printf("Score: %d\n", score);
}

// Prüfen, ob alle Punkte eingesammelt wurden
bool checkWinCondition() 
{
    for (int i = 0; i < HEIGHT; i++) 
    {
        for (int j = 0; j < WIDTH; j++) 
        {
            if (grid[i][j] == '.') 
            {
                return false; // Es gibt noch Punkte
            }
        }
    }
    return true; // Alle Punkte eingesammelt
}

// Pac-Man bewegen
void movePacman(int dx, int dy) 
{
    int newX = pacman.x + dx;
    int newY = pacman.y + dy;

    // Kollision mit Wänden verhindern
    if (grid[newY][newX] == '#') 
    {
        return;
    }

    // Punkt sammeln
    if (grid[newY][newX] == '.') 
    {
        score++;
    }

    // Prüfen, ob ein Geist erreicht wurde
    if (grid[newY][newX] == 'G') 
    {
        running = false;
        return;
    }

    // Aktuelle Position leeren
    grid[pacman.y][pacman.x] = ' ';

    // Neue Position setzen
    pacman.x = newX;
    pacman.y = newY;
    grid[pacman.y][pacman.x] = 'P';
}

// Geister bewegen
void moveGhosts() 
{
    for (int i = 0; i < MAX_GHOSTS; i++) 
    {
        int direction = rand() % 4;
        int dx = 0, dy = 0;
        switch (direction) 
        {
            case 0: dy = -1; break; // Hoch
            case 1: dy = 1; break;  // Runter
            case 2: dx = -1; break; // Links
            case 3: dx = 1; break;  // Rechts
        }

        int newX = ghosts[i].x + dx;
        int newY = ghosts[i].y + dy;

        // Kollision mit Wänden oder anderen Geistern verhindern
        if (grid[newY][newX] == '#' || grid[newY][newX] == 'G') 
        {
            continue;
        }

        // Prüfen, ob ein Geist Pac-Man erreicht
        if (newX == pacman.x && newY == pacman.y) 
        {
            running = false;
            return;
        }

        // Aktuelle Position des Geistes zurücksetzen
        grid[ghosts[i].y][ghosts[i].x] = ghost_prev_char[i];

        // Neue Position speichern und das vorherige Zeichen merken
        ghost_prev_char[i] = grid[newY][newX];
        ghosts[i].x = newX;
        ghosts[i].y = newY;
        grid[ghosts[i].y][ghosts[i].x] = 'G';
    }
}

// Eingabe verarbeiten
void handleInput() 
{
    if (kbhit()) 
    {
        char input = getch();
        switch (input) 
        {
            case 'w': movePacman(0, -1); break; // Hoch
            case 's': movePacman(0, 1); break;  // Runter
            case 'a': movePacman(-1, 0); break; // Links
            case 'd': movePacman(1, 0); break;  // Rechts
            case 'q': running = false; break;   // Spiel beenden
        }
    }
}

int main() 
{
    initializeGrid();

    while (running) 
    {
        drawGrid();
        handleInput();
        moveGhosts();

        // Gewinnen prüfen
        if (checkWinCondition()) 
        {
            printf("You Won!\n");
            running = false;
            break;
        }

        Sleep(100); // Flüssigere Darstellung mit kürzeren Pausen
    }

    printf("Game Over!\n");
    return 0;
}
