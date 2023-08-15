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

    // Initialize the deck
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

        Texture2D testTexture;
        switch (board[0][0].suit)
        {
        case 0:
            DrawTexture(clubsTextures[board[0][0].rank], 50, 50, WHITE);
            break;
        
        case 1:
            DrawTexture(diamondsTextures[board[0][0].rank], 50, 50, WHITE);
            break;

        case 2:
            DrawTexture(heartsTextures[board[0][0].rank], 50, 50, WHITE);
            break;

        case 3:
            DrawTexture(spadesTextures[board[0][0].rank], 50, 50, WHITE);
            break;
        
        default:
            break;
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