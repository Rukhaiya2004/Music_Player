#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> // header file for directory , for directory related functions
#include <windows.h> //for windows api functions
#include <MMsystem.h> //headers file for music , for multimedia functions

// Forward declaration of struct node
struct node;

// Function prototypes
void initialize();
void printNewDirectoryContents(const char *directoryPath);
void insertSong();
struct node* insert(char data[], int count);
int substring(const char *string, const char *substring);

void play(int index, struct node *temp);
void next();
void previous();
void stop();
void printDirectoryContents();
void deleteSong();

#define MAX 50
#define DIRECTORY_PATH "Enter new directory path from where you want to add new songs"  //make sure the directory where the program file is present should also have 2 to 3 songs present of .wav 

struct node
{
    int index;
    struct node *prev;
    char data[MAX];
    struct node *next;
};

struct node *front = NULL, *temp, *new;
struct node *currentSong = NULL;

void initialize()  //This function initializes the music player by scanning the current directory for ".wav" files and adding them to the playlist.
{
    int indexCount = 1; //to keep track of index of songs
    DIR *dir;
    struct dirent *file; //represents a directory entry
    dir = opendir("."); // open current directory
    if (dir)  // checks if the directory was successfully opened, to read its contents
    {
        while ((file = readdir(dir)) != NULL) //loops untill there are no more entries
        {
            if (substring(file->d_name, ".wav"))   //checks if the file name of the current entry has a ".wav" substring. d_name is not explicitly initialized in the code; rather, it gets its values from the directory entries when the directory is being read.  
            {
                insert(file->d_name, indexCount);
                indexCount++;
            }
        }
        closedir(dir);
    }
}



int substring(const char *string, const char *substring)
{
    int stringLength = strlen(string), substringLength = strlen(substring);
    int i = 0, flag = 0;  //i is a counter used for iterating through the characters of string , flag = to indicate whether substring is found or not

    while (string[i] != '\0')
    {
        if (i + substringLength > stringLength)
        {
            break;
        }

        if (strncmp(string + i, substring, substringLength) == 0)
        {
            flag = 1;
            break;
        }

        i++;
    }

    return flag;
}

struct node* insert(char data[], int count)
{
    new = (struct node *)malloc(sizeof(struct node));
    strcpy(new->data, data);
    new->index = count;
    new->next = NULL;

    if (front == NULL)
    {
        new->prev = NULL;
        front = new;
    }
    else
    {
        temp = front;
        while (temp->next != NULL)
            temp = temp->next;
        new->prev = temp;
        temp->next = new;
    }

    return new; //pointer to newly inserted node
}


void play(int index, struct node *temp)
{
    if (index != 0)
    {
        int i;
        currentSong = temp;
        for (i = 1; i < index; i++)
        {
            currentSong = currentSong->next;
        }
    }

    char *fileName = strrchr(currentSong->data, '\\');
    if (fileName != NULL)
    {
        fileName++;
    }
    else
    {
        fileName = currentSong->data; 
    }

    puts(fileName);

    PlaySound(TEXT(currentSong->data), NULL, SND_FILENAME | SND_ASYNC); //windows api function, SND_ASYNC:allows the program to continue without waiting for the sound to finish
}

void next()
{
    if (currentSong != NULL) 
    {
        struct node *temp = currentSong->next;
        if (temp != NULL)
        {
            currentSong = temp;
            play(0, currentSong); 
        }
        else
        {
            currentSong = front;
            play(0, currentSong);
        }
    }
}

void previous()
{
    if (currentSong != NULL)
    {
        if (currentSong->prev != NULL)
        {
            currentSong = currentSong->prev;
        }
        else
        {
            currentSong = front;
            while (currentSong->next != NULL)
            {
                currentSong = currentSong->next;
            }
        }

        play(0, currentSong);
    }
    else
    {
        puts("No file before");
    }
}

void stop()
{
    PlaySound(NULL, 0, 0);  //no source file should be played
    puts("Stopped!!!");
}

void printDirectoryContents()
{
    int indexCount = 1;
    temp = front;
    while (temp != NULL)
    {
        char *fileName = strrchr(temp->data, '\\');
        if (fileName != NULL)
        {
            fileName++;
        }
        else
        {
            fileName = temp->data;
        }

        char *dot = strrchr(fileName, '.');  //Searches for the last occurrence of the dot ('.') character in the file name.
        if (dot != NULL)  //If a dot is found in the file name, the dot is replaced with a null terminator ('\0'). This effectively removes the file extension.
        {
            *dot = '\0';
        }

        printf("%d. %s\n", indexCount++, fileName);
        temp = temp->next;
    }
}

void insertSong()
{
    char songName[MAX];
    int indexCount = 1;

    // Display the available files in the directory without the ".wav" extension
    printf("Available songs in the directory:\n");
    printNewDirectoryContents(DIRECTORY_PATH);

    // Ask the user to enter the name of the new song
    printf("Enter the name of the new song: ");
    scanf("%s", songName);

    char fullPath[MAX];
    snprintf(fullPath, sizeof(fullPath), "%s\\%s.wav", DIRECTORY_PATH, songName); //snprintf is to prevent buffer overflow 

    // Check if the entered song name exactly matches one of the available ".wav" files
    if (substring(fullPath, ".wav") && access(fullPath, F_OK) != -1) //F_OK is a constant indicating that the function should check for the existence of the file.
    {
        struct node *newSong = insert(fullPath, indexCount + 1);
        printf("Song '%s' has been added to the playlist.\n", songName);
        currentSong = newSong;
    }
    else
    {
        printf("Invalid file name. The entered song is not available in the directory.\n");
    }
}

void printNewDirectoryContents(const char *directoryPath)
{
    DIR *dir;
    struct dirent *entry;

    dir = opendir(DIRECTORY_PATH);

    if (dir == NULL)
    {
        perror("Error opening directory");
        return;
    }

    // Read directory entries
    while ((entry = readdir(dir)) != NULL)
    {
        // Exclude entries with "." and ".."
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            // Display the file name without the ".wav" extension
            char *dot = strrchr(entry->d_name, '.');
            if (dot != NULL )
            {
                *dot = '\0'; // Null-terminate the string at the dot to remove the extension
            }

            printf("%s\n", entry->d_name);
        }
    }
    // Close the directory
    closedir(dir);
}


void deleteSong()
{
    int index;
    printDirectoryContents();
    printf("Enter the index of the song to delete: ");
    scanf("%d", &index);

    temp = front;
    struct node *prev = NULL;

    for (int i = 1; i < index && temp != NULL; i++)
    {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL)
    {
        puts("Invalid index");
        return;
    }

    if (prev != NULL)
    {
        prev->next = temp->next;
    }
    else
    {
        front = temp->next;
    }

    if (temp->next != NULL)
    {
        temp->next->prev = prev;
    }

    printf("Song '%s' has been deleted from the playlist.\n", temp->data);
    free(temp);
}



int main()
{
    // intializers
    initialize();
    puts("\t\t\tWelcome to iMusic");
    system("color 4f"); //sets the console background color to red and the text color to white

    // for our refernce for deallocating memorr
    struct node *reference = front;

    int choice;
    while (1)
    {
        printf("\n \n");
        puts("1. Play");
        puts("2. Next");
        puts("3. Previous");
        puts("4. Stop");
        puts("5. Current Directory Contents");
        puts("6. To insert a song");
        puts("7. To delete a song");
        puts("8. Exit");

        scanf("%d", &choice);
        system("cls");
        switch (choice)
        {
        case 1:
            printDirectoryContents();
            printf("Which song :");
            scanf("%d", &choice);
            system("cls");
            play(choice, reference);
            break;

        case 2:
            next();
            break;

        case 3:
            previous();
            break;

        case 4:
            stop();
            break;

        case 5:
            printDirectoryContents();
            break;

        case 6:
            insertSong();
            break;
             
        case 7:
            deleteSong();
            break;  

        case 8:
            printf("BYE!!!");
            temp = reference;
            while (temp != NULL)
            {
                front = temp;
                temp = temp->next;
                free(front);
            }
            exit(1);


        default:
            printf("Enter a valid option!");
            break;
        }
    }
    return 0;
}