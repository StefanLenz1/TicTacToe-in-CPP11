// Benutzung: ./tictactoe [player, easy, hard] [player, easy, hard]
bool activateComDelay = true; // Vor jedem Com Zug wird etwa 1 Sekunde gewartet

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <ctime>

using namespace std;

typedef struct MoveStruct // Nur für minimax
{
    int move;
    int score;
} moveStruct;

const char emptyField = ' '; // Ein Leerzeichen

int (*setDifficulty(string difficulty))(int);
int getMovePlayer(int recentMove);
int getMoveComEasy(int recentMove);

int getMoveComHard(int recentMove);
moveStruct findBestMoveMinimax(int recentMove, bool currentPlayer);
moveStruct minOrMaxMinimax(bool currentPlayer, moveStruct scores[], int amountRemainingFields);

int checkFullFieldMinimax(int recentMove);
vector<int> getRemainingFields();
bool checkWinner(int recentMove, int player);
bool checkInput(char input[]);
void displayGraph(char recentPlayer);

char tictactoeField[9]; // Jedes Element zu 0 initiieren. 0 ist Leer, 1 ist O and 2 ist X, ? ist ein Error
/* Tictactoe Feld
    012
    345
    678
*/

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "Falsche Anzahl an Argumente (Programm Funktionsweise: ./tictactoe [player, easy, hard] [player, easy, hard])\n" << endl;
        return 1;
    }
    int (*getMove1_ptr)(int) = setDifficulty(argv[1]); // Function Pointer um Schwierigkeit einzustellen: Spieler 1
    int (*getMove2_ptr)(int) = setDifficulty(argv[2]); // Function Pointer um Schwierigkeit einzustellen: Spieler 2
    if ((getMove1_ptr == NULL) || (getMove2_ptr == NULL))
    {
        cout << "Nicht vorhandener Spielmodus (Programm Funktionsweise: ./tictactoe [player, easy, hard] [player, easy, hard])\n" << endl;
        return 2;
    }

    srand(time(NULL));

    for (int i = 0; i < 9; i++)
        tictactoeField[i] = emptyField;
    int movePlayer1, movePlayer2;
    char winner;

    int recentMove = -1;   // Auserhalb des Feldes
    char nextPlayer = 'O'; // X fängt immer an

    displayGraph(nextPlayer);
    while (true)
    {
        movePlayer1 = getMove1_ptr(recentMove);
        tictactoeField[movePlayer1] = 'O';
        recentMove = movePlayer1;
        nextPlayer = 'X';
        displayGraph(nextPlayer);
        if (checkWinner(movePlayer1, 'O'))
        {
            winner = 'O';
            break;
        }

        vector <int> remainingFields = getRemainingFields();
        const int amountRemainingFields = remainingFields.size();
        if (amountRemainingFields == 0) // Das Feld ist voll
        {
            displayGraph('-');
            winner = '-';
            break;
        }

        movePlayer2 = getMove2_ptr(recentMove);
        tictactoeField[movePlayer2] = 'X';
        recentMove = movePlayer2;
        nextPlayer = 'O';
        displayGraph(nextPlayer);
        if (checkWinner(movePlayer2, 'X'))
        {
            winner = 'X';
            break;
        }
    }
    cout << "\nWinner: " << winner << endl;
}

int (*setDifficulty(string difficulty))(int)
{
    if (difficulty == "easy")
    {
        return getMoveComEasy;
    }
    else if (difficulty == "hard")
    {
        return getMoveComHard;
    }
    else if (difficulty == "player")
    {
        return getMovePlayer;
    }
    return NULL;
}

int getMovePlayer(int recentMove)
{
    char input[3];

    do
    {
        printf("Koordinate eingeben (z.B. B3): ");
        cin >> input;
    } while (checkInput(input));

    input[0] = toupper(input[0]);
    return (input[0] - 'A') * 3 + input[1] - '1'; // hash spielzug z.B. B2 -> 5
}

int getMoveComEasy(int recentMove) // Feldauswahl durch reinen Zufall
{
    if (activateComDelay)
    {
        this_thread::sleep_for(chrono::milliseconds(1000));
    }

    vector <int> remainingFields = getRemainingFields();
    const int randomNumber = rand() % remainingFields.size(); // Zufällige Zahl zwischen: 0 und "remaining fields"
    const int move = remainingFields.at(randomNumber);
    return move;
}

int getMoveComHard(int recentMove) // Feldauswahl durch Minimax
{
    if (activateComDelay)
    {
        this_thread::sleep_for(chrono::milliseconds(1000));
    }

    moveStruct move = findBestMoveMinimax(recentMove, true);
    return move.move;
}

moveStruct findBestMoveMinimax(int recentMove, bool currentPlayer)
{
    vector <int> remainingFields = getRemainingFields();
    const int amountRemainingFields = remainingFields.size();
    moveStruct move;
    move.move = recentMove;

    // Testet ob Spiel vorbei ist
    const int result = checkFullFieldMinimax(recentMove);
    if (result != 0)
    {
        move.score = result * (amountRemainingFields + 1);
    }
    else if (amountRemainingFields == 0)
    {
        move.score = 0;
    }
    else // Wenn Spiel nicht vorbei ist
    {
        moveStruct scores[amountRemainingFields];
        for (int i = 0; i < amountRemainingFields; i++) // Scores von allen Feldern einsammeln
        {
            scores[i].move = remainingFields.at(i);
            int currentplayerField;
            (currentPlayer) ? (currentplayerField = 'X') : (currentplayerField = 'O');
            tictactoeField[scores[i].move] = currentplayerField; // Feld wird temporär belegt
            moveStruct tempMove = findBestMoveMinimax(scores[i].move, !currentPlayer);
            scores[i].score = tempMove.score;
            tictactoeField[scores[i].move] = emptyField; // Feld wird zurückgesetzt
        }
        move = minOrMaxMinimax(currentPlayer, scores, amountRemainingFields); // Move mit höchsten Score wird heraussortiert
    }

    return move;
}

moveStruct minOrMaxMinimax(bool currentPlayer, moveStruct scores[], int amountRemainingFields)
{
    if (currentPlayer) // Höchster Score wird gesucht
    {
        moveStruct max = scores[0];
        for (int i = 1; i < amountRemainingFields; i++)
        {
            if (scores[i].score > max.score)
            {
                max = scores[i];
            }
        }
        return max;
    }
    else // Niedrigster Score wird gesucht
    {
        moveStruct min = scores[0];
        for (int i = 1; i < amountRemainingFields; i++)
        {
            if (scores[i].score < min.score)
            {
                min = scores[i];
            }
        }
        return min;
    }
}

int checkFullFieldMinimax(int recentMove)
{
    if (checkWinner(recentMove, 'X'))
    {
        return 1;
    }
    else if (checkWinner(recentMove, 'O'))
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

vector<int> getRemainingFields()
{
    vector<int> remainingFields;
    for (int i = 0; i < 9; i++)
    {
        if (tictactoeField[i] == emptyField)
        {
            remainingFields.push_back(i);
        }
    }
    return remainingFields;
}

bool checkWinner(int recentMove, int player)
{
    const int collumn = recentMove / 3;
    const int row = recentMove % 3;
    int checkTictactoe[3][3] = {};

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            checkTictactoe[i][j] = tictactoeField[3 * i + j];
        }
    }

    if (checkTictactoe[0][row] == player && checkTictactoe[1][row] == player && checkTictactoe[2][row] == player) // reihen
    {
        return true;
    }
    if (checkTictactoe[collumn][0] == player && checkTictactoe[collumn][1] == player && checkTictactoe[collumn][2] == player) // spalten
    {
        return true;
    }
    if (checkTictactoe[0][0] == player && checkTictactoe[1][1] == player && checkTictactoe[2][2] == player) // 1. diagonale
    {
        return true;
    }
    if (checkTictactoe[2][0] == player && checkTictactoe[1][1] == player && checkTictactoe[0][2] == player) // 2. diagonale
    {
        return true;
    }

    return false;
}

bool checkInput(char input[])
{
    vector<int> remainingFields = getRemainingFields();
    const int amountRemainingFields = remainingFields.size();
    input[0] = toupper(input[0]);

    if (!isalpha(input[0]) || !isdigit(input[1]) || (input[1] > '3') || input[1] == '0' || input[0] > 'C')
    {
        cout << "Ungültiger Eingabe" << endl;
        return true;
    }

    const int inputHash = (input[0] - 'A') * 3 + input[1] - '1';
    bool fieldIsTaken = true;

    for (int i = 0; i < amountRemainingFields; i++) // Testet ob Feld belegt ist
    {
        if (inputHash == remainingFields.at(i))
        {
            fieldIsTaken = false;
        }
    }
    if (fieldIsTaken)
    {
        cout << "Belegtes Feld" << endl;
        return true;
    }

    return false; // Eingabe war erfolgreich
}

void displayGraph(char recentPlayer)
{
    system("cls");
    cout << "\n\n\tTic Tac Toe\n\n";

    cout << recentPlayer << " ist am Zug" << endl << endl;

    cout << "    1     2     3  " << endl;
    cout << "       |     |     " << endl;
    cout << "A " << "  " << tictactoeField[0] << "  |  " << tictactoeField[1] << "  |  " << tictactoeField[2] << endl;

    cout << "  _____|_____|_____" << endl;
    cout << "       |     |     " << endl;

    cout << "B " << "  " << tictactoeField[3] << "  |  " << tictactoeField[4] << "  |  " << tictactoeField[5] << endl;

    cout << "  _____|_____|_____" << endl;
    cout << "       |     |     " << endl;

    cout << "C " << "  " << tictactoeField[6] << "  |  " << tictactoeField[7] << "  |  " << tictactoeField[8] << endl;

    cout << "       |     |     " << endl << endl;
}