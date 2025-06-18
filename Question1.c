#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>

// Struct to hold log entries
typedef struct LogEntry
{
    int id;
    float temperature;
    float humidity;
    float pressure;
    struct LogEntry *prev;
    struct LogEntry *next;
} LogEntry;

LogEntry *head = NULL;
LogEntry *tail = NULL;
LogEntry *current = NULL;

typedef enum
{
    PAUSED,
    PLAYING,
    STOPPED
} SystemState;

SystemState state = PAUSED;

// Create and append a log entry
LogEntry *createLog(int id, float temp, float humidity, float pressure)
{
    LogEntry *entry = malloc(sizeof(LogEntry));
    entry->id = id;
    entry->temperature = temp;
    entry->humidity = humidity;
    entry->pressure = pressure;
    entry->prev = entry->next = NULL;
    return entry;
}

void appendLog(int id, float temp, float humidity, float pressure)
{
    LogEntry *entry = createLog(id, temp, humidity, pressure);
    if (!head)
    {
        head = tail = entry;
    }
    else
    {
        tail->next = entry;
        entry->prev = tail;
        tail = entry;
    }
}

void displayLog(LogEntry *log)
{
    if (!log)
    {
        printf("No log to display.\n");
        return;
    }
    printf("Log ID: %d | Temp: %.2f°C | Humidity: %.2f%% | Pressure: %.2f hPa\n",
           log->id, log->temperature, log->humidity, log->pressure);
}

void systemStartup()
{
    current = head;
    state = PAUSED;
    printf("System started. Displaying the oldest log:\n");
    displayLog(current);
}

// Set terminal to raw mode to capture keystrokes
struct termios orig_termios;

void disableRawMode()
{

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    printf("\n[Terminal restored]\n");
    fflush(stdout);
}

void enableRawMode()
{
    struct termios raw;
    raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO); // Disable echo and canonical mode
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

// Command loop with live mode that allows pause
void commandLoop()
{
    char input[20];
    fd_set readfds;
    struct timeval timeout;

    while (state != STOPPED)
    {
        if (state == PLAYING)
        {
            enableRawMode();
            FD_ZERO(&readfds);
            FD_SET(STDIN_FILENO, &readfds);
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;

            int ready = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);

            if (ready > 0)
            {
                char c;
                read(STDIN_FILENO, &c, 1);
                if (c == 'p')
                {
                    printf("\nPlayback paused.\n");
                    state = PAUSED;
                    disableRawMode();
                    continue;
                }
                else if (c == 's')
                {
                    printf("\nSystem stopped.\n");
                    state = STOPPED;
                    disableRawMode();
                    continue;
                }
            }

            // Advance to next log
            if (current->next)
            {
                current = current->next;
                displayLog(current);
            }
            else
            {
                printf("Reached latest log. Pausing.\n");
                state = PAUSED;
                disableRawMode();
            }
        }
        else
        {
            printf("\nCommand (n: next, p: prev, play, pause, stop): ");
            scanf("%s", input);

            if (strcmp(input, "n") == 0)
            {
                if (current && current->next)
                {
                    current = current->next;
                    displayLog(current);
                }
                else
                {
                    printf("No newer log.\n");
                }
            }
            else if (strcmp(input, "p") == 0)
            {
                if (current && current->prev)
                {
                    current = current->prev;
                    displayLog(current);
                }
                else
                {
                    printf("No older log.\n");
                }
            }
            else if (strcmp(input, "play") == 0)
            {
                if (state != PLAYING)
                {
                    printf("Live playback started. Press 'p' to pause, 's' to stop.\n");
                    state = PLAYING;
                }
            }
            else if (strcmp(input, "pause") == 0)
            {
                printf("Already paused.\n");
            }
            else if (strcmp(input, "stop") == 0)
            {
                printf("System stopped.\n");
                state = STOPPED;
            }
            else
            {
                printf("Invalid command.\n");
            }
        }
    }
}

void cleanup()
{
    LogEntry *temp = head;
    while (temp)
    {
        LogEntry *next = temp->next;
        free(temp);
        temp = next;
    }
}

int main()
{
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);
    // Example logs
    appendLog(1, 22.5, 40.2, 1011.3);
    appendLog(2, 23.1, 42.0, 1010.1);
    appendLog(3, 24.0, 41.5, 1009.5);
    appendLog(4, 25.2, 43.1, 1008.8);
    appendLog(5, 26.0, 44.0, 1007.9);

    systemStartup();
    commandLoop();
    cleanup();
    return 0;
}
