/**********************************************************************
 * Student 1 Last Name:     Woods-Smith
 * Student 1 First Name:    Zachary
 * Student 1 ID:            30086474
 * Student 2 Last Name:     Kaur
 * Student 2 First Name:    Amandeep
 * Student 2 ID:            30153923
 * Course:        CPSC 359
 * Assignment:    2
 * 
 * File Name:     game.c
 * 
 * 
 * This program is used to simulate Tic-Tac-Toe with 3 threads. The first thread is the main (tid = 0) instance of the program (i.e., thread initialized to main() function as that's how C works). 
 *  The second(tid = 1) and third (tid = 2) threads are the player threads who will be playing the game setup and displayed by main thread.
 * 
 **********************************************************************/
#include <pthread.h> 
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

struct shared {
    char board[9];
    //CHAR 9x9 SHARED ARRAY FOR THE GAMEBOARD
    int turn;
    int lastTurn; 
    int boardCount; 
    //Turn 0 = MAIN THREAD
    //Turn 1 = PLAYER 1
    //Turn 2 = PLAYER 2
};

struct shared gameBoard; 

bool determineWinner(char playerChar)
{
    //check rows {(0,1,2), (3,4,5), (6,7,8)}
    if  (
        (gameBoard.board[0] == playerChar && gameBoard.board[1] == playerChar && gameBoard.board[2] == playerChar) || 
        (gameBoard.board[3] == playerChar && gameBoard.board[4] == playerChar && gameBoard.board[5] == playerChar) ||
        (gameBoard.board[6] == playerChar && gameBoard.board[7] == playerChar && gameBoard.board[8] == playerChar)
        )
    {
        return true; 
    }

    
    //check cols {(1,4,7), (0,3,6), (2,5,8)}
    if  (
        (gameBoard.board[1] == playerChar && gameBoard.board[4] == playerChar && gameBoard.board[7] == playerChar) || 
        (gameBoard.board[0] == playerChar && gameBoard.board[3] == playerChar && gameBoard.board[6] == playerChar) ||
        (gameBoard.board[2] == playerChar && gameBoard.board[5] == playerChar && gameBoard.board[8] == playerChar)
        )
    {
        return true; 
    }
    //check diag {(0,4,8), (2,4,6)}
    if  (
        (gameBoard.board[0] == playerChar && gameBoard.board[4] == playerChar && gameBoard.board[8] == playerChar) || 
        (gameBoard.board[2] == playerChar && gameBoard.board[4] == playerChar && gameBoard.board[6] == playerChar)
        )
    {
        return true; 
    }

    return false; 
}

void setupWinnerCalc(bool * GG)
{
    //check if player 1 is the winner
    if(determineWinner('X'))
    {
        printf("Player 1 wins\n");
        *GG = true; 
        return;
        

    }
    //check if player 2 is winner
    else if(determineWinner('O'))
    {
        printf("Player 2 wins\n");
        * GG = true; 
        return;
    }
    //check if board full but no winners
    else if(gameBoard.boardCount == 9)
    {
        printf("There was no winner; TIE.\n");
        *GG = true; 
        return;
    }
}


void makePlay(char *board, int playerID)
{
    int checkIfPlayed = -1; 

    while(checkIfPlayed == -1)
    {
        //Get 'random' number 0 to 8 
        srand(time(NULL));
        checkIfPlayed = rand() % 9; 

        //Check if this random number place in Char array is actually open for a move
        if(board[checkIfPlayed] == ' ') ////See if we can make a move anywhere (if == ' ' we know its empty and can be filled)
        {
            if(playerID == 1)
            {
                
                board[checkIfPlayed] = 'X'; //Make a move in empty space checked above
                gameBoard.boardCount++;     //increment counter for checking for ties
            }
            else
            {
                board[checkIfPlayed] = 'O'; //Make a move in empty space checked above
                gameBoard.boardCount++;     //increment counter for checking for ties
            }
        }
        else //otherwise that position in table was filled so we keep this at -1 and keep checking (as we cant check a full board shouldn't end up with infinite loop issues)
        {
            checkIfPlayed = -1; 
        }
    }
}

void * playGame(void* player)
{
    //check which player it is based on threadid/playerid
    int thisPlayer = atoi(player);
    while(1)
    {
        //Wait while it is not your turn
        while(thisPlayer != gameBoard.turn);
        //make a play depending whose turn it is. 
        makePlay(gameBoard.board, atoi(player));
        //set back to main thread turn to display board and determine next turn and such
        gameBoard.turn = 0; 
    }

}

void setPlayerTurn(int *currentTurn)
{
    //if the current turn is 1, we should set it to 2 obviously, 
    if(*currentTurn == 1)
    {
        *currentTurn = 2; 
    }
    //if the current turn is anything other than 1 (i.e., main or player 2 would go back to 1)
    else
    {
        *currentTurn = 1; 
    }
}


//Print out in a nice format
void printBoard(char board[9])
{
    printf("+-----+-----+-----+\n");
    printf("|  %c  |  %c  |  %c  |\n",board[0], board[1], board[2]);
    printf("+-----+-----+-----+\n");
    printf("|  %c  |  %c  |  %c  |\n",board[3], board[4], board[5]);
    printf("+-----+-----+-----+\n");
    printf("|  %c  |  %c  |  %c  |\n",board[6], board[7], board[8]);
    printf("+-----+-----+-----+\n");
}

//Fill board with empty characters, probably could've used NULL but unsure if it would've worked honestly. 
void generateBoard(char board[9])
{
    for(int i = 0; i < 9; i++)
    {
        board[i] = ' ';
    }
}

int main(){
    //initialize struct variables
    gameBoard.turn = 0;
    gameBoard.lastTurn = -1; // to keep track of who has gone after we go back to the main to show the board and such
    gameBoard.boardCount = 0; //Count for tie case 
    //generate board
    generateBoard(gameBoard.board);

    //Thread variable creation 
    pthread_t tid_1, tid_2; 
    pthread_attr_t attr; 

    //Initialize attributes for threads
    pthread_attr_init(&attr); 
    //Initialize threads to playGame function with their respective thread IDS (i.e., "1" and "2"). 
    pthread_create(&tid_1, &attr, playGame, "1"); 
    pthread_create(&tid_2, &attr, playGame, "2"); 



    bool gameOver = false; 

    //infinite loop for game (ends when either board full or winner; whichever happens first)
    while(1)
    {
        //Wait until it is turn 0, this just does nothing while it isn't 0
        while(gameBoard.turn != 0) ;

        //When it's my turn print the game board
        printBoard(gameBoard.board);
        //Check if there are any winners once we get turn back at main prior to attempting to give another player a turn.
        setupWinnerCalc(&gameOver); 

        if(gameOver)
        {
            //Cleanup/cancel threads
            pthread_cancel(tid_1);
            pthread_cancel(tid_2);
            
            //exit game
            exit(0);


        }
        else//Else the game isn't over so we need to decide whose turn it is
        {
            if(gameBoard.lastTurn == -1) // i.e., no turns made
            {
                //set first turn to player 1
                gameBoard.lastTurn = 1; 
                gameBoard.turn = 1;
            }
            else if (gameBoard.lastTurn == 1) //Player 1 had the last turn prior to main getting the process control back so we should set the turn to 2 and the last turn to 2 (for when we hit here again)
            {
                gameBoard.lastTurn = 2; 
                gameBoard.turn = 2; 
            }
            else //Same logic as the last condition, however, this is when either main or player2 was the last person to play of course. 
            {
                gameBoard.lastTurn = 1; 
                gameBoard.turn = 1; 
            }
        }



    }

    return 0; 
}
