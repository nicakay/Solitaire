#include "raylib.h"
#include <algorithm>
#include <string>

// Define custom colors
#define LIGHTBLUE \
    (Color) { 215, 241, 245, 255 }
#define DARKERGREEN \
    (Color) { 0, 74, 10, 255 }

// Define Card as it will show on the board
struct Card
{
    int rank;      // from 1 (Ace) to 13 (King)
    int suit;      // 0 - Clubs, 1 - Diamonds, 2 - Hearts, 3 - Spades
    bool visible;  // face-up or face-down
    bool selected; // tells if the card is or is not selected by the user
    bool isEmpty;  // tells if the card exists or not
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
} CardState;

CardState cardState;

// Struct of the paramaters needed to load the cards graphics from the files - textures and rectangles
struct CardGraphics
{
    Texture2D texture;
    Rectangle rec[13];
};

// Array of 4 CardGraphics types, one for each suit
CardGraphics cardGraphics[4];

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

void DrawFoundation(CardGraphics cardGfx, int rank, Vector2 cardPos)
{
    DrawTextureRec(cardGfx.texture, cardGfx.rec[rank - 1], cardPos, WHITE);
}

// Function that handles cards selection
void SelectCard(Card cardsOnTheBoard[][20], int suit, int rank, int row, int col, Card **currentlySelectedCard, bool foundationPiles[4][14], Card foundation[4][14], CardGraphics cardGfx, Vector2 cardPos)
{
    // Variables that stores the [row] and [col] of the previously selected card
    static int oldRow{-1};
    static int oldCol{-1};

    if (cardState == CARD_NONE)
    {
        // Check if the card below this card is empty and is the card in the foundationPiles array that has the same suit and the rank lower -1 is set to 1 (in other words if the the card exists or if the current cart is Ace)
        if (cardsOnTheBoard[row][col + 1].isEmpty && foundationPiles[suit][rank - 1])
        {
            // Move this card to the Ace spot
            printf("Moving the card on the ACE PILE...\n");
            // Assign true to that card in the foundationPiles array, so that the next card in the rank can be compared
            foundationPiles[suit][rank] = true;
            // Flip the card that was above that card
            cardsOnTheBoard[row][col - 1].visible = true;
            // Copy the current card to the Foundation
            foundation[suit][rank] = cardsOnTheBoard[row][col];
            // Empty this card spot
            cardsOnTheBoard[row][col].isEmpty = true;
        }
        else // Else - select the card
        {
            printf("Transitioning to CARD_SELECTED...\n");
            cardState = CARD_SELECTED;
            // Select this card
            cardsOnTheBoard[row][col].selected = true;
            *currentlySelectedCard = &cardsOnTheBoard[row][col];

            // Update the row and col of the previously selected card
            oldRow = row;
            oldCol = col;
        }
    }
    else if (cardState == CARD_SELECTED)
    {
        if (*currentlySelectedCard == &cardsOnTheBoard[row][col])
        {
            printf("Deselecting the card...\n");

            // Deselect this card if it's already selected
            cardsOnTheBoard[row][col].selected = false;
            cardState = CARD_NONE;
            *currentlySelectedCard = NULL; // Reset the pointer
        }
        // If this is the second selected card
        else
        {
            // Code for a valid move
            bool validMove = ((((suit == 1 || suit == 2) && ((*currentlySelectedCard)->suit == 0 || (*currentlySelectedCard)->suit == 3)) || ((suit == 0 || suit == 3) && ((*currentlySelectedCard)->suit == 1 || (*currentlySelectedCard)->suit == 2))) && rank - (*currentlySelectedCard)->rank == 1) || ((*currentlySelectedCard)->rank == 13 && cardsOnTheBoard[row][col].isEmpty && col == 0);

            // If a valid move is detected
            if (validMove)
            {
                printf("Valid move detected!\n");
                // Make a copy of the previously selected card and place it below the current card
                int newCol = 0;
                while (!cardsOnTheBoard[row][newCol].isEmpty && newCol < 20)
                {
                    newCol++;
                }

                printf("Moving from [%d][%d] to [%d][%d]\n", oldRow, oldCol, row, newCol);

                if (newCol <= 20) // Ensure we are not out of bounds
                {
                    cardsOnTheBoard[row][newCol] = cardsOnTheBoard[oldRow][oldCol];
                    cardsOnTheBoard[row][newCol].isEmpty = false; // This slot is now occupied

                    // Mark the old card spot as empty
                    cardsOnTheBoard[oldRow][oldCol].isEmpty = true;
                    // Flip the card that was above that card
                    cardsOnTheBoard[oldRow][oldCol - 1].visible = true;
                }

                printf("Moved card to position: [%d][%d] with attributes - rank: %d, suit: %d, visible: %d, selected: %d, isEmpty: %d\n", row, newCol, cardsOnTheBoard[row][newCol].rank, cardsOnTheBoard[row][newCol].suit, cardsOnTheBoard[row][newCol].visible, cardsOnTheBoard[row][newCol].selected, cardsOnTheBoard[row][newCol].isEmpty);
            }

            // Deselect the previously selected card, if any was selected
            if (*currentlySelectedCard != NULL)
            {
                // Deselect the previous card
                printf("Deselecting the previously selected card...\n");
                (*currentlySelectedCard)->selected = false;
            }

            // Check if the card below this card is empty and is the card in the foundationPiles array that has the same suit and the rank lower -1 is set to 1 (in other words if the the card exists or if the current cart is Ace)
            if (cardsOnTheBoard[row][col + 1].isEmpty && foundationPiles[suit][rank - 1])
            {
                // Move this card to the Ace spot
                printf("Moving the card on the ACE PILE...\n");

                // Assign 1 to that card in the foundationPiles array, so that the next card in the rank can be compared
                foundationPiles[suit][rank] = true;
                // Flip the card that was above that card
                cardsOnTheBoard[row][col - 1].visible = true;
                // Copy the current card to the Foundation
                foundation[suit][rank] = cardsOnTheBoard[row][col];
                // Empty this card spot
                cardsOnTheBoard[row][col].isEmpty = true;
            }
            else
            {
                // Select the current card and update the pointer
                cardsOnTheBoard[row][col].selected = true;
                *currentlySelectedCard = &cardsOnTheBoard[row][col];

                // Update the row and col of the previously selected card
                oldRow = row;
                oldCol = col;
            }
        }
    }
}

// Function that draws cards on the board and handles interactions with them
void DrawAndInteractWithCards(CardGraphics cardGfx, Card card, Vector2 cardPos, Vector2 mousePointer, Color highlightColor, Card cardsOnTheBoard[][20], int row, int col, Card **currentlySelectedCard, bool foundationPiles[4][14], Card foundation[4][14])
{
    // Do nothing with empty cards
    if (card.isEmpty)
        return;

    Rectangle thisCardRec = MakeThisRectangle(cardGfx.rec[card.rank - 1].width, cardGfx.rec[card.rank - 1].height, cardPos);
    // If the card is clicked
    if (CheckCollisionPointRec(mousePointer, thisCardRec) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        // Select the card | Deselect other card and select this one
        SelectCard(cardsOnTheBoard, card.suit, card.rank, row, col, currentlySelectedCard, foundationPiles, foundation, cardGfx, cardPos);
    }
    if (card.selected)
    {
        // LIGHTBLUE - colour tint for a selected card
        DrawTextureRec(cardGfx.texture, cardGfx.rec[card.rank - 1], cardPos, LIGHTBLUE);
    }
    else
    {
        // If a card is not selected, draw it with no colour tint
        DrawTextureRec(cardGfx.texture, cardGfx.rec[card.rank - 1], cardPos, WHITE);
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
        deck[i].isEmpty = false;  // This will be assigned later
    }

    // Shuffle the deck using the Fisher-Yates shuffle algorithm
    for (int i = 51; i > 0; i--)
    {
        int j = rand() % (i + 1);
        std::swap(deck[i], deck[j]);
    }

    // Two-dimensional array to store the layout of the cards on the board
    Card cardsOnTheBoard[7][20];

    int cardIndex{};
    // Copy the shuffled deck into the two dimentional array
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 20; j++)
        {
            if (j <= i)
            {
                cardsOnTheBoard[i][j] = deck[cardIndex];
                cardsOnTheBoard[i][j].isEmpty = false;
                cardIndex++;
            }
            else
            {
                cardsOnTheBoard[i][j].isEmpty = true;
            }
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

    // The four final decks of suits (from Aces to Kings). Are empty at the beginning of the game
    /* The foundationPiles[4][14] will track the cards put at the Ace spots. The number represent 4 suits and 13 ranks. The number for ranks is 14 however. It's becasue the rank property in the Card struct goes from 1 to 13. And what I will use here it's the extra 0 in order to place an Ace.*/
    bool foundationPiles[4][14];

    // Array of Card structs that will be placed on foundation by copying them from the cardsOnTheBoard array
    Card foundation[4][14];

    // Setting all the values in the foundationPiles array to 0 (no cards) excet the first four cards for each rank (rank 0 below Ace, see the explanation above)
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 14; j++)
        {
            // Set all the foundation cards to empty
            foundation[i][j].isEmpty = true;
            if (j == 0)
            {
                foundationPiles[i][j] = true;
            }
            else
            {
                foundationPiles[i][j] = false;
            }
        }
    }

    // ---- LOAD TEXTURES ----

    // Load Front textures

    // Number of the rows and columns in each sprite sheet, as it happens is the same for all
    int colsNum{5};
    int rowsNum{3};

    // String variables to use them inside the relative path in the LoadTexture() function
    const std::string suits[4] = {"Clubs", "Diamonds", "Hearts", "Spades"};

    // Read the graphics for all the 4 suits from the all the 4 graphics files at once with a loop
    for (int i = 0; i < 4; i++)
    {
        // Load texture for each suit
        cardGraphics[i].texture = LoadTexture(("images/cards/" + suits[i] + "-88x124.png").c_str());

        for (int j = 0; j < 13; j++)
        {
            // Define a rectangle inside each of the 4 textures (each png has 13 graphics of fronts of the cards)
            SetRectangleProperties(cardGraphics[i].rec[j], j, cardGraphics[i].texture.width, cardGraphics[i].texture.height, rowsNum, colsNum);
        }
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
        Vector2 finalPos{450, static_cast<float>(handPosY)};
        int letterPaddingXY = 20;
        for (int i = 0; i <= 300; i += 100)
        {
            DrawRectangle(finalPos.x + i, finalPos.y, backRec.width, backRec.height, DARKGREEN);
            DrawText("A", finalPos.x + letterPaddingXY + i, finalPos.y + letterPaddingXY, 50, DARKERGREEN);
        }

        // Draw the cards on the board (the main solitaire layout)
        for (int row = 0; row < 7; row++)
        {
            float posX{150.f + row * cardSpacingX};
            float posY{170.f + cardSpacingY};

            for (int col = 0; col <= 20; col++)
            {
                int suit = cardsOnTheBoard[row][col].suit;
                bool visible = cardsOnTheBoard[row][col].visible;
                bool isEmpty = cardsOnTheBoard[row][col].isEmpty;

                // Vector posision of the current card
                posY += cardSpacingY;
                Vector2 cardPos{posX, posY};

                // If the card is facing down
                if (!visible && !isEmpty)
                {
                    // Draw the back of the card
                    DrawTextureRec(backTex, backRec, cardPos, WHITE);
                }
                // If the card is facing up
                else if (visible && !isEmpty)
                {
                    DrawAndInteractWithCards(cardGraphics[suit], cardsOnTheBoard[row][col], cardPos, mousePointer, LIGHTBLUE, cardsOnTheBoard, row, col, &currentlySelectedCard, foundationPiles, foundation);
                }

                posY += 20; // Adjust the Y position for the cards in the same column
            }
        }

        // Draw textures for the cards drawn from the hand
        for (int i = 0; i < numDrawnCards; i++)
        {
            int rank = drawnCards[i].rank;
            int suit = drawnCards[i].suit;
            Vector2 drawnCardPos = drawnCards[i].position;

            DrawTextureRec(cardGraphics[suit].texture, cardGraphics[suit].rec[rank - 1], drawnCardPos, WHITE);
        }

        // Draw the Foundation
        Vector2 foundationPos{450, 22};
        for (int suit = 0; suit < 4; suit++)
        {
            for (int rank = 1; rank <= 13; rank++)
            {
                if (foundationPiles[suit][rank])
                {
                    DrawFoundation(cardGraphics[suit], rank, foundationPos);
                }
            }
            foundationPos.x += 100;
        }

        EndDrawing();
    }

    // Unload Textures from VRAM
    for (int i = 0; i < 4; i++)
    {
        UnloadTexture(cardGraphics[i].texture);
    }
    UnloadTexture(backTex);
    UnloadTexture(deckTex);

    // Close the Window
    CloseWindow();
}
