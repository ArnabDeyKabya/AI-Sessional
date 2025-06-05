#include <bits/stdc++.h>
using namespace std;

class Board;
void maxMove(Board &b, int m);
void minMove(Board &b, int m);
void print(int a[14]);
int DEPTH = 8;
int PLUS = 100000;
int MINUS = -100000;
int W1 = 1, W2 = 1, W3 = 1, W4 = 1;

class Board
{
public:
    int arr[14];
    int alpha = MINUS;
    int beta = PLUS;
    int val;
    bool mm;
    int maxaddmove = 0;
    int minaddmove = 0;
    int maxstone_cap = 0;
    int minstone_cap = 0;
    int move;
    Board()
    {
        for (int i = 1; i <= 13; i++)
        {
            arr[i] = 4;
        }
        arr[0] = 0;
        arr[7] = 0;
    }
    Board(Board *b)
    {
        alpha = b->alpha;
        beta = b->beta;
        maxaddmove = b->maxaddmove;
        maxstone_cap = b->maxstone_cap;
        minaddmove = b->minaddmove;
        minstone_cap = b->minstone_cap;
        for (int i = 0; i < 14; i++)
        {
            arr[i] = b->arr[i];
        }
    }
    void reset()
    {
        alpha = MINUS;
        beta = PLUS;
        maxaddmove = 0;
        maxstone_cap = 0;
        minaddmove = 0;
        minstone_cap = 0;
        move = 0;
    }
};

int h1(Board b)
{
    return b.arr[7] - b.arr[0];
}

int h2(Board b)
{
    int s1 = 0, s2 = 0;
    for (int i = 1; i < 7; i++)
        s1 += b.arr[i];
    for (int i = 8; i < 14; i++)
        s2 += b.arr[i];
    int r = h1(b);
    return W1 * r + W2 * (s1 - s2);
}

int h3(Board b)
{
    int r = h2(b);
    if (b.mm)
        return r + W3 * b.maxaddmove;
    else
        return r - W3 * b.minaddmove;
}

int h4(Board b)
{
    int r = h3(b);
    if (b.mm)
        return r + W4 * b.maxstone_cap;
    else
        return r - W4 * b.minstone_cap;
}

class Player
{
public:
    int (*func)(Board);

    Player(int (*f)(Board))
    {
        func = f;
    }
    Player() {}
    void setheuristic(int n)
    {
        if (n == 1)
            func = h1;
        else if (n == 2)
            func = h2;
        else if (n == 3)
            func = h3;
        else if (n == 4)
            func = h4;
        else
            exit(1);
    }
    void dfs(Board &b, int d)
    {
        if (d == DEPTH || (b.arr[0] + b.arr[7] == 48))
        {
            b.val = func(b);
        }
        else
        {
            if (b.mm)
            {
                for (int i = 1; i <= 6; i++)
                {
                    if (b.arr[i] != 0)
                    {
                        Board ch(&b);
                        maxMove(ch, i);
                        dfs(ch, d + 1);
                        if (ch.val > b.alpha)
                        {
                            b.alpha = ch.val;
                            b.move = i;
                        }
                        if (b.alpha >= b.beta)
                            break;
                    }
                }
                if (b.alpha == MINUS)
                    b.val = func(b);
                else
                    b.val = b.alpha;
            }
            else
            {
                for (int i = 1; i <= 6; i++)
                {
                    if (b.arr[i + 7] != 0)
                    {
                        Board ch(&b);
                        minMove(ch, i);
                        dfs(ch, d + 1);
                        if (ch.val < b.beta)
                        {
                            b.beta = ch.val;
                            b.move = i;
                        }
                        if (b.alpha >= b.beta)
                            break;
                    }
                }
                if (b.beta == PLUS)
                    b.val = func(b);
                else
                    b.val = b.beta;
            }
        }
    }
};

void print(int a[14])
{
    cout << endl;
    cout << "    ";
    for (int i = 13; i > 7; i--)
        cout << a[i] << " ";
    cout << endl;
    cout << a[0] << "                " << a[7] << endl;
    cout << "    ";
    for (int i = 1; i < 7; i++)
        cout << a[i] << " ";
    cout << endl;
}

void maxMove(Board &b, int m)
{
    int k = 1, s = b.arr[m];
    b.arr[m] = 0;
    for (int i = 1; i <= s; i++, k++)
    {
        int j = (m + k) % 14;
        if (j != 0)
            (b.arr[j])++;
        else
        {
            k++;
            (b.arr[1])++;
        }
    }
    int j = (m + k - 1) % 14;
    if (j == 7)
    {
        b.mm = true; // max
        b.maxaddmove++;
    }
    else
        b.mm = false; // min
    if (j < 7 && b.arr[j] == 1)
    {
        b.arr[7] += b.arr[14 - j];
        b.maxstone_cap += b.arr[14 - j];
        b.arr[14 - j] = 0;
    }
    s = 0;
    for (int i = 1; i < 7; i++)
        s += b.arr[i];         
    if (s == 0)
    {
        for (int i = 8; i < 14; i++)
        {
            b.arr[0] += b.arr[i];
            b.arr[i] = 0;
        }
    }
}

void minMove(Board &b, int m)
{
    m += 7;
    int k = 1, s = b.arr[m];
    b.arr[m] = 0;
    for (int i = 1; i <= s; i++, k++)
    {
        int j = (m + k) % 14;
        if (j != 7)
            b.arr[j]++;
        else
        {
            k++;
            b.arr[8]++;
        }
    }
    int j = (m + k - 1) % 14;
    if (j == 0)
    {
        b.mm = false; // max
        b.minaddmove++;
    }
    else
        b.mm = true; // min
    if (j > 7 && j < 14 && b.arr[j] == 1)
    {
        b.arr[0] += b.arr[14 - j];
        b.minstone_cap += b.arr[14 - j];
        b.arr[14 - j] = 0;
    }
    s = 0;
    for (int i = 8; i < 14; i++)
        s += b.arr[i];
    if (s == 0)
    {
        for (int i = 1; i < 7; i++)
        {
            b.arr[7] += b.arr[i];
            b.arr[i] = 0;
        }
    }
}

int main()
{
    int games = 100;
    int total_combinations = 4; // There are 4 heuristics
    int player1_wins, player2_wins, draws;

    // Loop through all combinations of heuristics
    for (int h1 = 1; h1 <= total_combinations; h1++)
    {
        for (int h2 = 1; h2 <= total_combinations; h2++)
        {
            player1_wins = 0;
            player2_wins = 0;
            draws = 0;

            Player player1, player2;

            for (int game = 0; game < games; game++)
            {
                Board board;
                player1.setheuristic(h1); // Set heuristic for player 1
                player2.setheuristic(h2); // Set heuristic for player 2

                int turn = 1; // Start with player 1
                int x = 1;
                if (turn == 1)
                    board.mm = true;
                else
                    board.mm = false;

                while (x != 100 && board.arr[0] + board.arr[7] != 48)
                {
                    if (turn == 1)
                    {
                        player1.dfs(board, 0);
                        if (board.move == 0)
                            break;
                        maxMove(board, board.move);
                    }
                    else if (turn == 2)
                    {
                        player2.dfs(board, 0);
                        if (board.move == 0)
                            break;
                        minMove(board, board.move);
                    }
                    board.reset();

                    if (board.mm)
                        turn = 1;
                    else
                        turn = 2;

                    x++;
                }

                // Determine result of the game
                if (board.arr[7] > board.arr[0])
                    player1_wins++;
                else if (board.arr[7] < board.arr[0])
                    player2_wins++;
                else
                    draws++;
            }

            // Print results for the current heuristic combination
            cout << "Heuristic Player 1: h" << h1 << " vs. Heuristic Player 2: h" << h2 << endl;
            cout << "Player 1 Wins: " << player1_wins << endl;
            cout << "Player 2 Wins: " << player2_wins << endl;
            cout << "Draws: " << draws << endl;
            cout << "==============================" << endl;
        }
    }

    return 0;
}
