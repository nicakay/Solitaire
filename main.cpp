#include "raylib.h"
#include <algorithm>

// Define custom colors
#define LIGHTBLUE \
    (Color) { 215, 241, 245, 255 }
#define DARKERGREEN \
    (Color) { 0, 74, 10, 255 }

// Define Card as it will show on the board
struct Card
{
    int rank;      // from 1 (Ace) tp 13 (King)
    int suit;      // 0 - Clubs, 1 - Diamonds, 2 - Hearts, 3 - Spades
    bool visible;  // face-up or face-down
    bool selected; // tells if the card is or is not selected by the user
};

// Define max numebr of cards that can be drawn from hand
#define MAX_DRAWN_CARDS 24

// Define properties of the drawn card
struct DrawnCard
{
    int rank;
    int suit;
    Vector2 position;
};

// Initiate an array of type of DrawnCard
DrawnCard drawnCards[MAX_DRAWN_CARDS];

// State to track the card seletion on the board
typedef enum
{
    CARD_NONE,     // No card selected
    CARD_SELECTED, // A card has been selected
    CARD_CONFIRMED // It's been one frame since the card was selected and no deleselection occurred
} CardState;

CardState cardState;

// Function that sets properties of each rectangle that I need to draw the card front textures with
void SetRectangleProperties(Rectangle &rect, int index, int texWidth, int texHeight, int rows, int cols)
{
    rect.width = texWidth / cols;
    rect.height = texHeight / rows;

    rect.x = (index % cols) * rect.width;
    rect.y = (index / cols) * rect.height;
}

// Finction that makes a rectangle of each card that is facing up on the board
Rectangle MakeThisRectangle(int cardWidth, int cardHeight, Vector2 cardPos)
{
    Rectangle thisCardRec;
    thisCardRec.x = cardPos.x;
    thisCardRec.y = cardPos.y;
    thisCardRec.width = cardWidth;
    thisCardRec.height = cardHeight;
    return thisCardRec;
}

void SelectCard(Card cardsOnTheBoard[][7], int suit, int rank, int row, int col, Card **currentlySelectedCard)
{

    printf("Current card state: %d\n", cardState);

    if (cardState == CARD_NONE || cardState == CARD_CONFIRMED)
    {
        printf("Transitioning to CARD_SELECTED...\n");
        cardState = CARD_SELECTED;
        // Select this card
        cardsOnTheBoard[row][col].selected = true;
        *currentlySelectedCard = &cardsOnTheBoard[row][col];
    }
    else if (cardState == CARD_SELECTED)
    {

        if (*currentlySelectedCard == &cardsOnTheBoard[row][col])
        {
            printf("Deselecting the card...\n");

            // Deselect this card if it's already selected
            cardsOnTheBoard[row][col].selected = false;
            cardState = CARD_NONE;
            *currentlySelectedCard = NULL; // Reset the pointe
        }
        // If this is the second selected card
        else
        {
            if ((((suit == 1 || suit == 2) && ((*currentlySelectedCard)->suit == 0 || (*currentlySelectedCard)->suit == 3)) || ((suit == 0 || suit == 3) && ((*currentlySelectedCard)->suit == 1 || (*currentlySelectedCard)->suit == 2))) && rank - (*currentlySelectedCard)->rank == 1)
            {
                printf("Valid move detected!\n");
                // If the suit of the previous card has oposite colour and if the rank is -1 of the current card - Move the previous card on to this card
            }

            // Deselect the previously selected card, if any was selected
            if (*currentlySelectedCard != NULL)
            {
                // Deselect the previous card
                printf("Deselecting the previously selected card...\n");
                (*currentlySelectedCard)->selected = false;
            }

            // Select the current card and update the pointer
            cardsOnTheBoard[row][col].selected = true;
            *currentlySelectedCard = &cardsOnTheBoard[row][col];
        }
    }
}

int main()
{
    // Initialize the game Window
    const int windowWidth{1000};
    const int windowHeight{800};
    InitWindow(windowWidth, windowHeight, "Solitaire");

    // Create a Deck of 52 cards
    Card deck[52];

    // Initialise the deck
    for (int i = 0; i < 52; i++)
    {
        deck[i].rank = i % 13 + 1;
        deck[i].suit = i / 13;
        deck[i].visible = false;  // Initially, all cards are face-down
        deck[i].selected = false; // Initially, all cards are unselected
    }

    // Shuffle the deck using the Fisher-Yates shuffle algorithm

    for (int i = 51; i > 0; i--)
    {
        int j = rand() % (i + 1);
        std::swap(deck[i], deck[j]);
    }

    // Two-dimensional array to store the layout of the cards on the board
    Card cardsOnTheBoard[30][7];

    int cardIndex{};
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
    // Stores the selected card so that I can compare it to the second selected card | NULL means it's not selected
    Card *currentlySelectedCard = NULL;
    // Set the CardState enum to NONE - No card is selected at the beginning of the game
    cardState = CARD_NONE;
    // A tracker that will help me to track the number of the cards left in hand
    int cardsInHandTracker{24};

    // Copy the remaining cards in the deck array to the cardsInHand array
    for (int i = 0; i < 24; i++)
    {
        cardsInHand[i] = deck[i + 28];
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
    int colsNum{5};
    int rowsNum{3};

    Rectangle clubsRec[13];
    Rectangle diamondsRec[13];
    Rectangle heartsRec[13];
    Rectangle spadesRec[13];

    // Set the Rectangle values for each rectangle in all the 4 arrays by calling a fuction
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

    // Load the Deck texture
    Texture2D deckTex = LoadTexture("images/cards/Card_DeckA-88x140.png");
    Rectangle deckRec;
    deckRec.width = deckTex.width / 3;
    deckRec.height = deckTex.height;
    deckRec.x = 0.f;
    deckRec.y = 0.f;

    // Set the mouse pointer
    Vector2 mousePointer = {0.f, 0.f};

    // Set the variables for the position of some of the cards
    int handPosX{150}; // Cards in hand X
    int handPosY{22};  // Cards in hand Y
    int righHand{0};   // Tracking the number of cards drawn from hand and put on the right side of it

    // The Rectangle for the cards in hand deck
    Rectangle cardsInHandRec;
    cardsInHandRec.width = deckRec.width;
    cardsInHandRec.height = deckRec.height;
    cardsInHandRec.x = handPosX;
    cardsInHandRec.y = handPosY;
    // Tracks the number of cards that has been drawn so far. At the beginning of the game it's 0
    int numDrawnCards{};
    // Tracks the number of drawn cards that display on the screen from 0 to 2
    int oneTwoThree{};

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(DARKGREEN);

        // Update the mouse pointer Vector to the current mouse position
        mousePointer = GetMousePosition();

        int cardSpacingX{100};
        int cardSpacingY{15};

        // Draw the top part of the board
        DrawRectangle(0, 0, windowWidth, 170, DARKERGREEN);

        // Draw an empty space for cards in hand
        DrawRectangle(handPosX, handPosY, backRec.width, backRec.height, DARKGREEN);
        DrawText("E", 160, 32, 50, DARKERGREEN);

        Vector2 handPos{static_cast<float>(handPosX), static_cast<float>(handPosY)};

        if (cardsInHandTracker > 0)
        {
            // Draw the deck texture
            DrawTextureRec(deckTex, deckRec, handPos, WHITE);
        }

        // Check if the mouse cursor is above the deck texture and if there's left mouse click
        if (CheckCollisionPointRec(mousePointer, cardsInHandRec) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {

            // Check if there are ny cards in hand left, and if the max number of drawn cards hasn't been reached
            if (cardsInHandTracker > 0 && numDrawnCards < MAX_DRAWN_CARDS)
            {

                // Draw a card from the cards-in-hand deck and put it next to it
                cardsInHandTracker--;
                righHand += 1;                             // Cards to the right of the hand deck (Face-up)
                int rank = cardsInHand[righHand - 1].rank; // Every next card we draw from the deck in hand

                // Check if the number of drawn cards that are going to be display one next to another is greater than 2. If it is reset it to 0. This will change the vector position of the next card displayed as the calculation uses the oneTwoThree variable
                if (oneTwoThree > 2)
                {
                    oneTwoThree = 0;
                }

                // Set the vector position for each card drawn from hand
                Vector2 drawnCardPos = {static_cast<float>(handPosX) + 100 + (oneTwoThree * 20), static_cast<float>(handPosY)};

                // Assign parameters to the card drawn
                drawnCards[numDrawnCards].rank = rank;
                drawnCards[numDrawnCards].suit = cardsInHand[righHand - 1].suit;
                drawnCards[numDrawnCards].position = drawnCardPos;

                // Increment variables
                numDrawnCards++;
                oneTwoThree++;
            }
        }

        // Draw the empty spaces for Aces and each suit
        DrawRectangle(handPosX + 300, handPosY, backRec.width, backRec.height, DARKGREEN);
        DrawText("A", handPosX + 320, handPosY + 20, 50, DARKERGREEN);
        DrawRectangle(handPosX + 400, handPosY, backRec.width, backRec.height, DARKGREEN);
        DrawText("A", handPosX + 420, handPosY + 20, 50, DARKERGREEN);
        DrawRectangle(handPosX + 500, handPosY, backRec.width, backRec.height, DARKGREEN);
        DrawText("A", handPosX + 520, handPosY + 20, 50, DARKERGREEN);
        DrawRectangle(handPosX + 600, handPosY, backRec.width, backRec.height, DARKGREEN);
        DrawText("A", handPosX + 620, handPosY + 20, 50, DARKERGREEN);

        // Draw the cards on the board (the main solitaire layout)

        for (int row = 0; row < 7; row++)
        {
            float posX{150.f + row * cardSpacingX};
            float posY{170.f + cardSpacingY};

            for (int col = 0; col <= row; col++)
            {
                int suit = cardsOnTheBoard[row][col].suit;
                int rank = cardsOnTheBoard[row][col].rank;
                bool visible = cardsOnTheBoard[row][col].visible;
                bool isSelected = cardsOnTheBoard[row][col].selected;

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
                    Rectangle thisCardRec;

                    switch (suit) // If the suit is:
                    {
                    case 0: // Clubs
                        thisCardRec = MakeThisRectangle(clubsRec[rank - 1].width, clubsRec[rank - 1].height, cardPos);
                        // If the card is clicked
                        if (CheckCollisionPointRec(mousePointer, thisCardRec) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                        {
                            // Select the card | Deselect other card and select this one
                            SelectCard(cardsOnTheBoard, suit, rank, row, col, &currentlySelectedCard);
                        }
                        if (isSelected)
                        {
                            // LIGHTBLUE - colour tint for a selected card
                            DrawTextureRec(clubsTex, clubsRec[rank - 1], cardPos, LIGHTBLUE);
                        }
                        else
                        {
                            // If a card is not selected, draw it with no colour tint
                            DrawTextureRec(clubsTex, clubsRec[rank - 1], cardPos, WHITE);
                        }
                        break;

                    case 1: // Diamonds
                        thisCardRec = MakeThisRectangle(diamondsRec[rank - 1].width, diamondsRec[rank - 1].height, cardPos);
                        // If the card is clicked
                        if (CheckCollisionPointRec(mousePointer, thisCardRec) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                        {
                            // Select the card | Deselect other card and select this one
                            SelectCard(cardsOnTheBoard, suit, rank, row, col, &currentlySelectedCard);
                        }

                        if (isSelected)
                        {
                            // LIGHTBLUE - colour tint for a selected card
                            DrawTextureRec(diamondsTex, diamondsRec[rank - 1], cardPos, LIGHTBLUE);
                        }
                        else
                        {
                            // If a card is not selected, draw it with no colour tint
                            DrawTextureRec(diamondsTex, diamondsRec[rank - 1], cardPos, WHITE);
                        }
                        break;

                    case 2: // Hearts
                        thisCardRec = MakeThisRectangle(heartsRec[rank - 1].width, heartsRec[rank - 1].height, cardPos);
                        // If the card is clicked
                        if (CheckCollisionPointRec(mousePointer, thisCardRec) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                        {
                            // Select the card | Deselect other card and select this one
                            SelectCard(cardsOnTheBoard, suit, rank, row, col, &currentlySelectedCard);
                        }

                        if (isSelected)
                        {
                            // LIGHTBLUE - colour tint for a selected card
                            DrawTextureRec(heartsTex, heartsRec[rank - 1], cardPos, LIGHTBLUE);
                        }
                        else
                        {
                            // If a card is not selected, draw it with no colour tint
                            DrawTextureRec(heartsTex, heartsRec[rank - 1], cardPos, WHITE);
                        }
                        break;

                    default: // Spades
                        thisCardRec = MakeThisRectangle(spadesRec[rank - 1].width, spadesRec[rank - 1].height, cardPos);
                        // If the card is clicked
                        if (CheckCollisionPointRec(mousePointer, thisCardRec) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                        {
                            // Select the card | Deselect other card and select this one
                            SelectCard(cardsOnTheBoard, suit, rank, row, col, &currentlySelectedCard);
                        }

                        if (isSelected)
                        {
                            // LIGHTBLUE - colour tint for a selected card
                            DrawTextureRec(spadesTex, spadesRec[rank - 1], cardPos, LIGHTBLUE);
                        }
                        else
                        {
                            // If a card is not selected, draw it with no colour tint
                            DrawTextureRec(spadesTex, spadesRec[rank - 1], cardPos, WHITE);
                        }
                        break;
                    }
                }

                posY += 20; // Adjust the Y position for the cards in the same column
            }
        }

        // Draw textures for the cards drawn from the hand
        for (int i = 0; i < numDrawnCards; i++)
        {
            int rank = drawnCards[i].rank;
            Vector2 drawnCardPos = drawnCards[i].position;

            switch (drawnCards[i].suit)
            {
            case 0:
                DrawTextureRec(clubsTex, clubsRec[rank - 1], drawnCardPos, WHITE);
                break;
            case 1:
                DrawTextureRec(diamondsTex, diamondsRec[rank - 1], drawnCardPos, WHITE);
                break;
            case 2:
                DrawTextureRec(heartsTex, heartsRec[rank - 1], drawnCardPos, WHITE);
                break;
            default:
                DrawTextureRec(spadesTex, spadesRec[rank - 1], drawnCardPos, WHITE);
                break;
            }
        }
        /*
        if (cardState == CARD_SELECTED)
        {
            cardState = CARD_CONFIRMED;
        }
        */

        EndDrawing();
    }

    // Unload Textures from VRAM
    UnloadTexture(clubsTex);
    UnloadTexture(diamondsTex);
    UnloadTexture(heartsTex);
    UnloadTexture(spadesTex);
    UnloadTexture(backTex);
    UnloadTexture(deckTex);

    // Close the Window
    CloseWindow();
}
