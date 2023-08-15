#include "raylib.h"
#include <algorithm>

// Define Card
struct Card
{
    int rank;     // from 1 (Ace) tp 13 (King)
    int suit;     // 0 - Clubs, 1 - Diamonds, 2 - Hearts, 3 - Spades
    bool visible; // face-up or face-down
};

int main()
{
    // Initialize the game Window
    const int windowWidth = 1000;
    const int windowHeight = 600;
    InitWindow(windowWidth, windowHeight, "Solitaire");

    // Create a Deck of 52 cards
    Card deck[52];

    // Initialise the deck
    for (int i = 0; i < 52; i++)
    {
        deck[i].rank = i % 13 + 1;
        deck[i].suit = i / 13;
        deck[i].visible = false; // Initially, all cards are face-down
    }

    // Shuffle the deck using the Fisher-Yates shuffle algorithm

    for (int i = 51; i > 0; i--)
    {
        int j = rand() % (i + 1);
        std::swap(deck[i], deck[j]);
    }

    // Two-dimensional array to store the layout of the cards on the board
    Card board[7][7];

    int cardIndex = 0;

    // Copy the shuffled deck into the two dimentional array
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j <= i; j++)
        {
            board[i][j] = deck[cardIndex];
            cardIndex++;
        }
    }

    // Set the visibility of the last card of each column to true
    for (int i = 0; i < 7; i++)
    {
        board[i][i].visible = true;
    }

    // ---- LOAD TEXTURES ----

    // Load Front textures
    Texture2D clubsTextures[13];
    Texture2D diamondsTextures[13];
    Texture2D heartsTextures[13];
    Texture2D spadesTextures[13];
    for (int i = 0; i < 13; i++)
    {
        // buffer which stores a temporary string that will be the name of the file (the file path)
        char filename[32];

        // Clubs front textures
        snprintf(filename, sizeof(filename), "images/cards/c%d.png", i + 1);
        clubsTextures[i] = LoadTexture(filename);

        // Diamonds front textures
        snprintf(filename, sizeof(filename), "images/cards/d%d.png", i + 1);
        diamondsTextures[i] = LoadTexture(filename);

        // Hearts front textures
        snprintf(filename, sizeof(filename), "images/cards/h%d.png", i + 1);
        heartsTextures[i] = LoadTexture(filename);

        // Spades front textures
        snprintf(filename, sizeof(filename), "images/cards/s%d.png", i + 1);
        spadesTextures[i] = LoadTexture(filename);
    }

    // Load Cards Back texture
    Texture2D back = LoadTexture("images/cards/Back1.png");

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(DARKGREEN);

        int cardSpacingX = 80;
        int cardSpacingY = 100;

        for (int row = 0; row < 7; row++)
        {
            int posX = 150 + row * cardSpacingX;
            int posY = 10 + cardSpacingY;

            for (int col = 0; col <= row; col++)
            {
                int suit = board[row][col].suit;
                int rank = board[row][col].rank;
                bool visible = board[row][col].visible;

                if (!visible)
                {
                    DrawTexture(back, posX, posY, WHITE);
                }
                else
                {
                    switch (suit)
                    {
                    case 0:
                        DrawTexture(clubsTextures[rank - 1], posX, posY, WHITE);
                        break;

                    case 1:
                        DrawTexture(diamondsTextures[rank - 1], posX, posY, WHITE);
                        break;

                    case 2:
                        DrawTexture(heartsTextures[rank - 1], posX, posY, WHITE);
                        break;

                    case 3:
                        DrawTexture(spadesTextures[rank - 1], posX, posY, WHITE);
                        break;

                    default:
                        break;
                    }
                }

                posY += 20; // Adjust the Y position for the cards in the same column
            }
        }

        EndDrawing();
    }

    // Unload Textures from VRAM
    for (int i = 0; i < 13; i++)
    {
        UnloadTexture(clubsTextures[i]);
        UnloadTexture(diamondsTextures[i]);
        UnloadTexture(heartsTextures[i]);
        UnloadTexture(spadesTextures[i]);
    }
    UnloadTexture(back);

    // Close the Window
    CloseWindow();
}