#include "raylib.h"
#include <algorithm>

#define LIGHTBLUE (Color) {215, 241, 245, 255}

// Define Card
struct Card
{
    int rank;     // from 1 (Ace) tp 13 (King)
    int suit;     // 0 - Clubs, 1 - Diamonds, 2 - Hearts, 3 - Spades
    bool visible; // face-up or face-down
};

// Function that sets properties of each rectangle that I need to draw the card front textures with
void SetRectangleProperties(Rectangle &rect, int index, int texWidth, int texHeight, int rows, int cols)
{
    rect.width = texWidth / cols;
    rect.height = texHeight / rows;

    rect.x = (index % cols) * rect.width;
    rect.y = (index / cols) * rect.height;
}

Rectangle MakeThisRectangle(int cardWidth, int cardHeight, Vector2 cardPos)
{
    Rectangle thisCard;
    thisCard.x = cardPos.x;
    thisCard.y = cardPos.y;
    thisCard.width = cardWidth;
    thisCard.height = cardHeight;
    return thisCard;
}

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
    Card cardsOnTheBoard[7][20];

    int cardIndex = 0;

    // Copy the shuffled deck into the two dimentional array
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j <= i; j++)
        {
            cardsOnTheBoard[i][j] = deck[cardIndex];
            cardIndex++;
        }
    }

    // At the beginning of the game there are 28 cards on the board, the remaining 24 is in hand
    Card cardsInHand[24];

    // Copy the remaining cards in the deck array to the cardsInHand array
    for (int i = 28; i < 52; i++)
    {
        cardsInHand[i] = deck[i];
    }

    // Set the visibility of the last card of each column on the board to true
    for (int i = 0; i < 7; i++)
    {
        cardsOnTheBoard[i][i].visible = true;
    }

    // ---- LOAD TEXTURES ----

    // Load Front textures

    // Loading a texture sprite for each suit
    Texture2D clubsTex = LoadTexture("images/cards/Clubs-88x124.png");
    Texture2D diamondsTex = LoadTexture("images/cards/Diamonds-88x124.png");
    Texture2D heartsTex = LoadTexture("images/cards/Hearts-88x124.png");
    Texture2D spadesTex = LoadTexture("images/cards/Spades-88x124.png");

    // Number of the rows and columns in each sprite sheet, as it happens is the same for all
    int colsNum = 5;
    int rowsNum = 3;

    Rectangle clubsRec[13];
    Rectangle diamondsRec[13];
    Rectangle heartsRec[13];
    Rectangle spadesRec[13];

    // Set the Rectangle values for each rectangle in all the 4 arrays by callin a fuction
    for (int i = 0; i < 13; i++)
    {
        SetRectangleProperties(clubsRec[i], i, clubsTex.width, clubsTex.height, rowsNum, colsNum);
        SetRectangleProperties(diamondsRec[i], i, diamondsTex.width, diamondsTex.height, rowsNum, colsNum);
        SetRectangleProperties(heartsRec[i], i, heartsTex.width, heartsTex.height, rowsNum, colsNum);
        SetRectangleProperties(spadesRec[i], i, spadesTex.width, spadesTex.height, rowsNum, colsNum);
    }

    // Load Cards Back texture
    Texture2D backTex = LoadTexture("images/cards/Card_Back-88x124.png");
    Rectangle backRec;
    backRec.width = backTex.width / 2;
    backRec.height = backTex.height;
    backRec.x = 0.f + backTex.width / 2;
    backRec.y = 0.f;

    // Set the mouse pointer
    Vector2 mousePointer = {0.f, 0.f};

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(DARKGREEN);

        // Update the mouse pointer Vector to the current mouse position
        mousePointer = GetMousePosition();

        int cardSpacingX = 100;
        int cardSpacingY = 20;

        // Draw the cards on the board

        for (int row = 0; row < 7; row++)
        {
            float posX = 150.f + row * cardSpacingX;
            float posY = 10.f + cardSpacingY;

            for (int col = 0; col <= row; col++)
            {
                int suit = cardsOnTheBoard[row][col].suit;
                int rank = cardsOnTheBoard[row][col].rank;
                bool visible = cardsOnTheBoard[row][col].visible;
                // Vector posision of the current card
                posY += cardSpacingY;
                Vector2 cardPos{posX, posY};

                // If the card is facing down
                if (!visible)
                {
                    // Draw the back of the card
                    DrawTextureRec(backTex, backRec, cardPos, WHITE);
                }
                // If the card is facing up
                else
                {
                    Rectangle thisCard;
                    switch (suit) // If the suit is
                    {
                    case 0: // Clubs
                        thisCard = MakeThisRectangle(clubsRec[rank - 1].width, clubsRec[rank - 1].height, cardPos);
                        if (CheckCollisionPointRec(mousePointer, thisCard))
                        {
                            DrawTextureRec(clubsTex, clubsRec[rank - 1], cardPos, LIGHTBLUE);
                        }
                        else
                        {
                            DrawTextureRec(clubsTex, clubsRec[rank - 1], cardPos, WHITE);
                        }
                        break;

                    case 1: // Diamonds
                        thisCard = MakeThisRectangle(diamondsRec[rank - 1].width, diamondsRec[rank - 1].height, cardPos);
                        if (CheckCollisionPointRec(mousePointer, thisCard))
                        {
                            DrawTextureRec(diamondsTex, diamondsRec[rank - 1], cardPos, LIGHTBLUE);
                        }
                        else
                        {
                            DrawTextureRec(diamondsTex, diamondsRec[rank - 1], cardPos, WHITE);
                        }
                        break;

                    case 2: // Hearts
                        thisCard = MakeThisRectangle(heartsRec[rank - 1].width, heartsRec[rank - 1].height, cardPos);
                        if (CheckCollisionPointRec(mousePointer, thisCard))
                        {
                            DrawTextureRec(heartsTex, heartsRec[rank - 1], cardPos, LIGHTBLUE);
                        }
                        else
                        {
                            DrawTextureRec(heartsTex, heartsRec[rank - 1], cardPos, WHITE);
                        }
                        break;

                    default: // Spades
                        thisCard = MakeThisRectangle(spadesRec[rank - 1].width, spadesRec[rank - 1].height, cardPos);
                        if (CheckCollisionPointRec(mousePointer, thisCard))
                        {
                            DrawTextureRec(spadesTex, spadesRec[rank - 1], cardPos, LIGHTBLUE);
                        }
                        else
                        {
                            DrawTextureRec(spadesTex, spadesRec[rank - 1], cardPos, WHITE);
                        }
                        break;
                    }
                }

                posY += 20; // Adjust the Y position for the cards in the same column
            }
        }

        EndDrawing();
    }

    // Unload Textures from VRAM
    UnloadTexture(clubsTex);
    UnloadTexture(diamondsTex);
    UnloadTexture(heartsTex);
    UnloadTexture(spadesTex);
    UnloadTexture(backTex);

    // Close the Window
    CloseWindow();
}