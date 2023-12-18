#include <raylib.h>

// Window dimensions
#define WINDOWS_WIDTH 512
#define WINDOWS_HEIGHT 300

// Estructura
struct AnimData
{
    Texture2D texture;
    Rectangle rec;
    Vector2 pos;
    int totalFrames;
    int frame;
    float updateTime;
    float runningTime;
};


// This functions checks if the player is on the ground
int checkHeightPosition(AnimData scarfyData, int windowsHeight) 
{
   bool isOnGround = scarfyData.pos.y >= windowsHeight - scarfyData.rec.height;         
   return isOnGround;
}


// This function animates each frame for the player and the nebulae
AnimData updateAnimData(AnimData data, float deltaTime, int totalFrames) {
    
    // Update running time
    data.runningTime += deltaTime;
    if (data.runningTime >= data.updateTime) 
    {
        data.runningTime = 0.0;            
        data.rec.x = data.frame * data.rec.width;
        data.frame++;

        // Si se sobrepasa el último frame de la animación, restablecerla a 0
        if (data.frame > totalFrames) {
            data.frame  = 0;
        }
    }

    return data;
}


// main function
int main() {

    // Acceleration due to gravity (pixels/sec) / sec
    const int gravity = 1'000;

    // Jump Velocity -600 pixeles/sec
    const int jumpVel = -600;

    // Init Window and set FPS
    InitWindow(WINDOWS_WIDTH, WINDOWS_HEIGHT, "Dasher Game");

    // Background
    Texture2D background1Texture = LoadTexture("textures/far-buildings.png");
    Texture2D background2Texture = LoadTexture("textures/far-buildings.png");
    Vector2 background1Pos{0.0, 0.0};
    float backgroundRotation = 0.0;
    float backgroundScale = 2.0;
    Color backgroundColor = WHITE;
    float backgroundVelocity = 20.0;

    // Back-Buildings
    Texture2D backBuildings1Texture = LoadTexture("textures/back-buildings.png");
    float backBuildings1PositionX = 0.0;
    Texture2D backBuildings2Texture = LoadTexture("textures/back-buildings.png");

    // ForeGrounds
    Texture2D foreground1Texture = LoadTexture("textures/foreground.png");
    float foreground1PositionX = 0.0;
    Texture2D foreground2Texture = LoadTexture("textures/foreground.png");

    // Scarfy's AnimData
    AnimData scarfyData;
    scarfyData.texture = LoadTexture("textures/scarfy.png");
    scarfyData.totalFrames = 6;
    scarfyData.rec.width = scarfyData.texture.width / scarfyData.totalFrames;
    scarfyData.rec.height = scarfyData.texture.height;
    scarfyData.rec.x = 0;
    scarfyData.rec.y = 0;
    scarfyData.pos.x = WINDOWS_WIDTH / 2 - scarfyData.rec.width / 2;
    scarfyData.pos.y = WINDOWS_HEIGHT - scarfyData.rec.height;
    scarfyData.frame = 0;
    scarfyData.runningTime = 0.0;
    scarfyData.updateTime = 1.0 / 12.0;

    // Loop Nebula's array
    const int totalNebulae = 10;
    AnimData nebulaList[totalNebulae] {};

    // Set the default Nebula data
    for (int i = 0; i < totalNebulae; i++)
    {
        nebulaList[i].texture = LoadTexture("textures/12_nebula_spritesheet.png");
        nebulaList[i].totalFrames = 8;
        nebulaList[i].rec.width = nebulaList[i].texture.width / nebulaList[i].totalFrames ;
        nebulaList[i].rec.height = nebulaList[i].texture.height / nebulaList[i].totalFrames;
        nebulaList[i].pos.x = WINDOWS_WIDTH + i * 300;
        nebulaList[i].pos.y = WINDOWS_HEIGHT - nebulaList[i].rec.height;
        nebulaList[i].rec.x = 0.0;
        nebulaList[i].rec.y = 0.0;
        nebulaList[i].frame = 0;
        nebulaList[i].runningTime = 0.0;
        nebulaList[i].updateTime = 1.0 / 16.0;        
    }

    // Set finish line
    float finishLine{nebulaList[totalNebulae - 1].pos.x + 300};
    
    // nebula X velocity (pixels/second)
    int nebulaVel = -200;

    // Init values
    bool isInAir = false;
    bool collision = false;
    float velocity = 0;

    while (!WindowShouldClose())
    {
        // Start Drawing
        BeginDrawing();
        ClearBackground(WHITE);        

        // Get current frame time
        const float  deltaTime = GetFrameTime();

        // Animate Background
        background1Pos.x -= backgroundVelocity * deltaTime;
        if (background1Pos.x <= -background1Texture.width * backgroundScale) {
            background1Pos.x = 0;
        }
        Vector2 background2Pos{background1Pos.x + background1Texture.width * backgroundScale, 0.0};
        
        // Animate foreground and back-buildings
        backBuildings1PositionX -= 40 * deltaTime; 
        foreground1PositionX -= 80 * deltaTime;

        if (backBuildings1PositionX <= -backBuildings1Texture.width * backgroundScale) {
            backBuildings1PositionX = 0;
        }

        if (foreground1PositionX <= -foreground1Texture.width * backgroundScale) {
            foreground1PositionX = 0;
        }

        Vector2 backgroundBuilding1Pos{backBuildings1PositionX, 0.0};
        Vector2 backgroundBuilding2Pos{backBuildings1PositionX + backBuildings1Texture.width * backgroundScale, 0.0};
        Vector2 foregroundBuilding1Pos{foreground1PositionX, 0.0};
        Vector2 foregroundBuilding2Pos{foreground1PositionX + foreground1Texture.width * backgroundScale, 0.0};

        // If there is no collision beetwen player and Nebulae
        DrawTextureEx(background1Texture, background1Pos, backgroundRotation, backgroundScale, backgroundColor);
        DrawTextureEx(background2Texture, background2Pos, backgroundRotation, backgroundScale, backgroundColor);
        DrawTextureEx(backBuildings1Texture, backgroundBuilding1Pos, backgroundRotation, backgroundScale, backgroundColor);
        DrawTextureEx(backBuildings2Texture, backgroundBuilding2Pos, backgroundRotation, backgroundScale, backgroundColor);
        DrawTextureEx(foreground1Texture, foregroundBuilding1Pos, backgroundRotation, backgroundScale, backgroundColor);
        DrawTextureEx(foreground2Texture, foregroundBuilding2Pos, backgroundRotation, backgroundScale, backgroundColor);

        // If player is on the ground
        const int isPlayerOnGround = checkHeightPosition(scarfyData, WINDOWS_HEIGHT) ;
        if (isPlayerOnGround) {
            
            // velocity is 0
            velocity = 0;

            // set air flag to false
            isInAir = false;
        }
        // If player is in the air
        else {
            
            // Apply Gravity
            velocity += gravity * deltaTime;

            // set air flag to true
            isInAir = true;
        }
        // if player jumps
        if (IsKeyPressed(KEY_SPACE) && !isInAir) {
            velocity += jumpVel;
        }

        // Update scarfy position and animation frame
        scarfyData.pos.y  += velocity * deltaTime;

        // If player is in the air
        if (!isInAir) {
           scarfyData = updateAnimData(scarfyData, deltaTime, scarfyData.totalFrames);
        }

        // Update each Nebula's Positon
        for (int i = 0; i < totalNebulae; i++)
        {
            nebulaList[i].pos.x += nebulaVel * deltaTime;
        }
        
        // Move finish line to the player
        finishLine += nebulaVel * deltaTime;
        
        // Check all current nebula frame
        for (int i = 0; i < totalNebulae; i++)
        {
            nebulaList[i] = updateAnimData(nebulaList[i], deltaTime, nebulaList[i].totalFrames);
        }

        for (AnimData nebula : nebulaList) {

            float padding = 50;
            Rectangle nebulaRec{
                nebula.pos.x + padding,
                nebula.pos.y + padding,
                nebula.rec.width - 2 * padding,
                nebula.rec.height - 2 * padding,
            };
            Rectangle scarfyRec{
                scarfyData.pos.x,
                scarfyData.pos.y,
                scarfyData.rec.width,
                scarfyData.rec.height,
            };

            if (CheckCollisionRecs(nebulaRec, scarfyRec)) {
                collision = true;
            }
        }    

        // If there is a collision beetwen player and Nebulae
        if (collision) {
            // Lose the Game
            DrawText("GAME OVER", WINDOWS_WIDTH / 3.5, WINDOWS_HEIGHT / 2, 40, RED);
        }
        // If player wins the game
        else if (scarfyData.pos.x >= finishLine) {

            DrawText("YOU WIN!", WINDOWS_WIDTH / 3, WINDOWS_HEIGHT / 2, 40, GREEN);
        }
        else 
        {
            // Draw each Nebula
            for (int i = 0; i < totalNebulae; i++)
            {
                DrawTextureRec(nebulaList[i].texture, nebulaList[i].rec, nebulaList[i].pos, WHITE);
            }

            // Draw scarfy
            DrawTextureRec(scarfyData.texture, scarfyData.rec, scarfyData.pos, WHITE);
        }

        // Stop Drawing
        EndDrawing();
    }

    // Unload Scarfy Texture at the end of the program
    UnloadTexture(scarfyData.texture);

    // Unload each Nebula's texture
    UnloadTexture(nebulaList->texture);

    // Unload the background textures
    UnloadTexture(background1Texture);
    UnloadTexture(background2Texture);
    UnloadTexture(backBuildings1Texture);
    UnloadTexture(foreground1Texture);
    UnloadTexture(backBuildings2Texture);
    UnloadTexture(foreground2Texture);

    CloseWindow();
        
    return 0;
}