// so far a copypaste from stackoverflow :P
//

#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#define SIZE 100

void getPassword(char password[]) {
    static struct termios oldt, newt;
    int i = 0;
    int c;

    printf("Please enter your password\n");
    /*saving the old settings of STDIN_FILENO and copy settings for resetting*/
    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;

    /*setting the approriate bit in the termios struct*/
    newt.c_lflag &= ~(ECHO);

    /*setting the new bits*/
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);

    /*reading the password from the console*/
    while ((c = getchar())!= '\n' && c != EOF && i < SIZE){
        password[i++] = c;
    }
    password[i] = '\0';

    /*resetting our old STDIN_FILENO*/
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
}

int main(int argc, char const *argv[]) {
  char pw[SIZE];

  getPassword(pw);

  printf("Password was %s\n", pw);

  return 0;
}
