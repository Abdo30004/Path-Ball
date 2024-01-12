#include <draw.h>

void getMatrixClick(SDL_Renderer *renderer, int clickX, int clickY, int n, int *i, int *j)
{
    if (clickX <= OFFSET || clickX >= OFFSET + GRID_SIZE || clickY <= OFFSET || clickY >= OFFSET + GRID_SIZE)
    {
        *i = -1;
        *j = -1;
        return;
    }

    *j = (clickX - OFFSET) * n / GRID_SIZE;
    *i = (clickY - OFFSET) * n / GRID_SIZE;

    // check if the click in the circle

    bool isOuterMatrix = *i == 0 || *i == n - 1 || *j == 0 || *j == n - 1;
    if (!isOuterMatrix)
    {
        *i = -1;
        *j = -1;
        return;
    }

    int cellSize = GRID_SIZE / n;
    int x = OFFSET + *j * cellSize + cellSize / 2 + THICKNESS / 2;
    int y = OFFSET + *i * cellSize + cellSize / 2 + THICKNESS / 2;
    int radius = cellSize / 3; // radius of the small circle

    if (clickX < x - radius || clickX > x + radius || clickY < y - radius || clickY > y + radius)
    {
        *i = -1;
        *j = -1;
        return;
    }
}

// void getClickCenter()
void drawButton(SDL_Renderer *renderer, button *Button)
{
    int x = Button->centerX;
    int y = Button->centerY;
    int w = Button->width;
    int h = Button->height;

    SDL_Rect rect = {x - w / 2, y - h / 2, w, h};
    SDL_SetRenderDrawColor(renderer, Button->outlineColor.r, Button->outlineColor.g, Button->outlineColor.b, Button->outlineColor.a);
    for (int i = 0; i < Button->outlineThickness; i++)
    {
        SDL_RenderDrawRect(renderer, &rect);
        rect.x++;
        rect.y++;
        rect.w -= 2;
        rect.h -= 2;
    }
    SDL_Color color = Button->color;
    if (Button->hoverColor.r != -1)
    {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        if (mouseX >= x - w / 2 && mouseX <= x + w / 2 && mouseY >= y - h / 2 && mouseY <= y + h / 2)
        {
            color = Button->hoverColor;
        }
    }

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
    // rounded corners

    color = Button->textColor;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    int fontSize = 24;
    int textWidth = mesureTextWidth(GAMEPAUSED_FONT, Button->label, fontSize);
    writeText(renderer, GAMEPAUSED_FONT, Button->label, x - textWidth / 2, y - fontSize / 2, fontSize, color.r, color.g, color.b, color.a);
}

void drawFilledCircle(SDL_Renderer *renderer, int centerX, int centerY, int radius)
{
    int x = radius - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    while (x >= y)
    {
        SDL_RenderDrawLine(renderer, centerX + x, centerY - y, centerX - x, centerY - y);
        SDL_RenderDrawLine(renderer, centerX + x, centerY + y, centerX - x, centerY + y);
        SDL_RenderDrawLine(renderer, centerX + y, centerY - x, centerX - y, centerY - x);
        SDL_RenderDrawLine(renderer, centerX + y, centerY + x, centerX - y, centerY + x);

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }
        if (err > 0)
        {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
}

void drawDiagonal(SDL_Renderer *renderer, int n, int direction, int centerX, int centerY)
{
    // Set the length of the diagonal line

    // Calculate the end points of the diagonal based on the direction
    int cellSize = GRID_SIZE / n;
    int of = cellSize / 4;    // OFFSET
    int bold = cellSize / 10; // boldness

    int Sx = centerX - of; // Start x

    int Ex = centerX + of; // End x

    int Sy = direction > 0 ? centerY + of : centerY - of; // Start y depending on the direction
    int Ey = direction > 0 ? centerY - of : centerY + of; // End y depending on the direction

    SDL_SetRenderDrawColor(renderer, DIAGONAL_COLOR); // Set the color of the diagonal
    if (direction < 0)
    {
        SDL_SetRenderDrawColor(renderer, DIAGONAL_COLOR);
    }
    for (int s = 0; s < bold; ++s)
    {
        SDL_RenderDrawLine(renderer, Sx, Sy + s, Ex, Ey + s);
        SDL_RenderDrawLine(renderer, Sx, Sy - s, Ex, Ey - s);
    }
}

void drawGrid(SDL_Renderer *renderer, game *Game)
{
    int n = Game->level + 5;
    int cellSize = GRID_SIZE / n;
    int(*matrix)[n] = Game->matrix;

    SDL_SetRenderDrawColor(renderer, BORDER_COLOR);

    SDL_Rect matrixRect = {OFFSET, OFFSET, GRID_SIZE, GRID_SIZE};
    SDL_RenderDrawRect(renderer, &matrixRect);
    // Draw vertical lines
    int x = n % 2;
    for (int i = 0; i < n - 1; i++)
    {

        int startX = (i + 1) * cellSize + OFFSET;
        int startY = cellSize + OFFSET;
        int endX = (i + 1) * cellSize + OFFSET;
        int endY = GRID_SIZE + OFFSET - cellSize;

        for (int s = 0; s < THICKNESS + x; s++)
        {

            SDL_RenderDrawLine(renderer, startX + s, startY, endX + s, endY);
            SDL_RenderDrawLine(renderer, startY, startX + s, endY, endX + s);
        }
    }

    for (int i = 1; i < n - 1; i++)
    {
        for (int j = 0; j < n; j++)
        {

            int x = OFFSET + j * cellSize + cellSize / 2 + THICKNESS / 2;
            int y = OFFSET + i * cellSize + cellSize / 2 + THICKNESS / 2;
            if (j > 0 && j < n - 1)
            {
                int current = matrix[i][j];

                if (current != 0 && Game->state != Selecting)
                {
                    drawDiagonal(renderer, n, current, x, y);
                }

                continue;
            }

            SDL_SetRenderDrawColor(renderer, BORDER_COLOR); // Color of the big circle

            drawFilledCircle(renderer, x, y, cellSize / 3); // Draw the big circle vertically

            drawFilledCircle(renderer, y, x, cellSize / 3); // Draw the big circle horizontally
            int startI = Game->solution->startI, startJ = Game->solution->startJ;

            int hoverX, hoverY;
            SDL_GetMouseState(&hoverX, &hoverY);

            int hoverI, hoverJ;
            getMatrixClick(renderer, hoverX, hoverY, n, &hoverI, &hoverJ);

            SDL_SetRenderDrawColor(renderer, SMALL_CIRCLE_COLOR); // Color of the small circle
            if (Game->state == Selecting)
            {
                if (startI == i && startJ == j)
                {
                    SDL_SetRenderDrawColor(renderer, START_CIRCLE_COLOR);
                }
                else if (hoverI == i && hoverJ == j)
                {
                    SDL_SetRenderDrawColor(renderer, HOVER_CIRCLE_COLOR);
                }
                else if (Game->helpers.selectedI == i && Game->helpers.selectedJ == j)
                {
                    SDL_SetRenderDrawColor(renderer, SELECTED_CIRCLE_COLOR);
                }
                else
                {
                    SDL_SetRenderDrawColor(renderer, SMALL_CIRCLE_COLOR);
                }
            }

            drawFilledCircle(renderer, x, y, cellSize / 4); // Draw the small circle vertically

            if (Game->state == Selecting)
            {
                if (startI == j && startJ == i)
                {
                    SDL_SetRenderDrawColor(renderer, START_CIRCLE_COLOR);
                }
                else if (hoverI == j && hoverJ == i)
                {
                    SDL_SetRenderDrawColor(renderer, HOVER_CIRCLE_COLOR);
                }
                else if (Game->helpers.selectedI == j && Game->helpers.selectedJ == i)
                {
                    SDL_SetRenderDrawColor(renderer, SELECTED_CIRCLE_COLOR);
                }
                else
                {
                    SDL_SetRenderDrawColor(renderer, SMALL_CIRCLE_COLOR);
                }
            }

            drawFilledCircle(renderer, y, x, cellSize / 4); // Draw the small circle horizontally
        }
    }

    if (strcmp(Game->player.name, "root") || Game->state != Selecting)
        return;

    SDL_SetRenderDrawColor(renderer, SELECTED_CIRCLE_COLOR);
    int endx = Game->solution->endJ * cellSize + OFFSET + cellSize / 2 + THICKNESS / 2;
    int endy = Game->solution->endI * cellSize + OFFSET + cellSize / 2 + THICKNESS / 2;
    drawFilledCircle(renderer, endx, endy, cellSize / 16);
}

void drawPath(SDL_Renderer *renderer, game *Game)
{
    int n = Game->level + 5;
    int cellSize = GRID_SIZE / n;
    path *sPath = Game->solution->path;

    while (sPath->next != NULL && sPath != NULL)
    {
        path *start = sPath;
        path *end = sPath->next;

        int startX = OFFSET + start->y * cellSize + cellSize / 2 + THICKNESS / 2;
        int startY = OFFSET + start->x * cellSize + cellSize / 2 + THICKNESS / 2;
        int endX = OFFSET + end->y * cellSize + cellSize / 2 + THICKNESS / 2;
        int endY = OFFSET + end->x * cellSize + cellSize / 2 + THICKNESS / 2;

        int circlesNumber = 2;
        int circleRadius = cellSize / 8;
        for (int i = 0; i < circlesNumber; i++)
        {

            if (i != circlesNumber && i != 0)
            {
                circleRadius = cellSize / 12;
            }
            else
            {
                circleRadius = cellSize / 8;
            }

            int c1 = startX + (endX - startX) * i / circlesNumber;
            int c2 = startY + (endY - startY) * i / circlesNumber;
            SDL_SetRenderDrawColor(renderer, PATH_COLOR);
            drawFilledCircle(renderer, c1, c2, circleRadius);
            drawSideBar(renderer, Game);
            SDL_RenderPresent(renderer);
            playSoundEffect("assets/sounds/step.wav");
            graycefulDelay(250);
        }

        // SDL_Delay(100);
        sPath = sPath->next;
    }
    if (sPath->next == NULL)
    {
        SDL_SetRenderDrawColor(renderer, END_CIRCLE_COLOR);
        int endx = sPath->y * cellSize + OFFSET + cellSize / 2 + THICKNESS / 2;
        int endy = sPath->x * cellSize + OFFSET + cellSize / 2 + THICKNESS / 2;
        drawFilledCircle(renderer, endx, endy, cellSize / 4);
        // playSoundEffect("assets/sounds/step.wav");
        drawSideBar(renderer, Game);
        SDL_RenderPresent(renderer);
    }

    graycefulDelay(1000);
}

void drawSideBar(SDL_Renderer *renderer, game *Game)
{
    // side bar
    int outline = 10;
    int sideBarWidth = WIDTH - GRID_SIZE - OFFSET * 2;

    SDL_Rect outerSideBar = {GRID_SIZE + OFFSET * 2, 0, sideBarWidth, HEIGHT};
    SDL_Rect innerSideBar = {GRID_SIZE + OFFSET * 2 + outline, outline, sideBarWidth - 2 * outline, HEIGHT - 2 * outline};

    SDL_SetRenderDrawColor(renderer, SIDE_BAR_OUTLINE_COLOR);
    SDL_RenderFillRect(renderer, &outerSideBar);

    SDL_SetRenderDrawColor(renderer, SIDE_BAR_COLOR);
    SDL_RenderFillRect(renderer, &innerSideBar);

    int centerX = outline + GRID_SIZE + OFFSET * 2 + sideBarWidth / 2;

    int centerY = outline + HEIGHT / 2;
    char text[100];
    int fontSize = 24;

    sprintf(text, "Score:%d", Game->player.score);
    int w = mesureTextWidth(GAMEPAUSED_FONT, text, fontSize);
    writeText(renderer, GAMEPAUSED_FONT, text, centerX - w / 2, centerY - 100, fontSize, FONT_COLOR);

    // write player name at the top of the side bar
    sprintf(text, "%s", Game->player.name);
    // set it in the top center of the side bar
    fontSize = 34;
    w = mesureTextWidth(GAMEPAUSED_FONT, text, fontSize);
    writeText(renderer, GAMEPAUSED_FONT, text, centerX - w / 2, outline + 10, fontSize, 255, 0, 0, 255);
    // line under the name
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawLine(renderer, centerX - w / 2, outline + 10 + fontSize, centerX + w / 2, outline + 10 + fontSize);

    // write time
    sprintf(text, "Time:%s", formatTime(SDL_GetTicks() - Game->helpers.gameStartTime));
    w = mesureTextWidth(GAMEPAUSED_FONT, text, fontSize);
    writeText(renderer, GAMEPAUSED_FONT, text, centerX - w / 2, centerY - 50, fontSize, FONT_COLOR);

    // write level
    sprintf(text, "Level:%d", Game->level);
    w = mesureTextWidth(GAMEPAUSED_FONT, text, fontSize);
    writeText(renderer, GAMEPAUSED_FONT, text, centerX - w / 2, centerY, fontSize, FONT_COLOR);

    // 3 displays
    // Name
    // Level
    // Score

    // 2 buttons
    // Pause
    // Save & Quit

    // pause and save & quit buttons are in the bottom of the side bar one under the other
    // pause button
    button pauseButton = {
        centerX, // center of the side bar
        centerY + 100,
        200,
        50,
        {0, 169, 157, 255},
        {255, 255, 0, 255},
        {255, 255, 255, 255},
        {0, 0, 0, 255},
        4,
        "Pause"};
    Game->buttons.pause = pauseButton;
    drawButton(renderer, &pauseButton);

    // save & quit button

    button saveAndQuitButton = {
        centerX,
        centerY + 200,
        200,
        50,
        {255, 0, 0, 255},
        {255, 255, 0, 255},
        {255, 255, 255, 255},
        {0, 0, 0, 255},
        4,
        "Save & Quit"};
    drawButton(renderer, &saveAndQuitButton);
}

void drawTextInput(SDL_Renderer *renderer, game *Game)
{

    char text[100];
    int fontSize = 24;
    sprintf(text, "Enter Your Name:");
    int w = mesureTextWidth(GAMEPAUSED_FONT, text, fontSize);

    writeText(renderer, GAMEPAUSED_FONT, text, WIDTH / 2 - w / 2, HEIGHT / 2 - 100, fontSize, FONT_COLOR);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    sprintf(text, "%s", Game->player.name);
    w = mesureTextWidth(GAMEPAUSED_FONT, text, fontSize);
    writeText(renderer, GAMEPAUSED_FONT, text, WIDTH / 2 - w / 2, HEIGHT / 2 - 50, fontSize, 255, 0, 0, 255);

    // draw wrting line
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    if (strlen(text))
        SDL_RenderDrawLine(renderer, WIDTH / 2 - w / 2, HEIGHT / 2 - 50 + fontSize, WIDTH / 2 + w / 2, HEIGHT / 2 - 50 + fontSize);

    sprintf(text, "Press Enter to Start");
    w = mesureTextWidth(GAMEPAUSED_FONT, text, fontSize);
    writeText(renderer, GAMEPAUSED_FONT, text, WIDTH / 2 - w / 2, HEIGHT / 2 + 50, fontSize, FONT_COLOR);

    drawSVG(renderer, "assets/images/enter.png", WIDTH / 2 - 50, HEIGHT / 2 + 100, 100, 100);
}

void drawGameOver(SDL_Renderer *renderer, game *Game)
{
    char text[100];
    int fontSize = 24;
    sprintf(text, "Game Over");
    int w = mesureTextWidth(GAMEPAUSED_FONT, text, fontSize);

    // you won if game->level = maxLevel or you lost if game->level = 0
    sprintf(text, "You %s", Game->level == 0 ? "Lost" : "Won");
    w = mesureTextWidth(GAMEPAUSED_FONT, text, fontSize);
    SDL_Color color = Game->level == 0 ? (SDL_Color){255, 0, 0, 255} : (SDL_Color){0, 255, 0, 255};
    writeText(renderer, GAMEPAUSED_FONT, text, WIDTH / 2 - w / 2, HEIGHT / 2 - 100, fontSize, color.r, color.g, color.b, color.a);

    sprintf(text, "Your Score:%d", Game->player.score);
    w = mesureTextWidth(GAMEPAUSED_FONT, text, fontSize);
    writeText(renderer, GAMEPAUSED_FONT, text, WIDTH / 2 - w / 2, HEIGHT / 2 - 50, fontSize, FONT_COLOR);
}

void drawPause(SDL_Renderer *renderer, game *Game)
{
    char text[100];
    int fontSize = 24;
    sprintf(text, "Game Paused");
    int w = mesureTextWidth(GAMEPAUSED_FONT, text, fontSize);

    writeText(renderer, GAMEPAUSED_FONT, text, WIDTH / 2 - w / 2, HEIGHT / 2 - 100, fontSize, FONT_COLOR);

    // resume button
    button resumeButton = {
        WIDTH / 2,
        HEIGHT / 2,
        200,
        50,
        {0, 169, 157, 255},
        {255, 255, 0, 255},
        {255, 255, 255, 255},
        {0, 0, 0, 255},
        4,
        "Resume"};
    Game->buttons.pause = resumeButton;
    drawButton(renderer, &resumeButton);
}

Mix_Music *
playMusic(char *path)
{
    Mix_Music *music = Mix_LoadMUS(path);
    if (!music)
    {
        printf("Mix_LoadMUS Error: %s\n", Mix_GetError());
        return NULL;
    }
    Mix_PlayMusic(music, 0);

    return music;
}

void playSoundEffect(char *path)
{
    Mix_Chunk *soundEffect = Mix_LoadWAV(path);
    if (!soundEffect)
    {
        printf("Mix_LoadWAV Error: %s\n", Mix_GetError());
        return;
    }
    Mix_PlayChannel(-1, soundEffect, 0);
}

void writeText(SDL_Renderer *renderer, char *fontPath, char *text, int x, int y, int size, int r, int g, int b, int a)
{
    if (!text || text[0] == '\0')
    {
        return; // Don't render anything if the text is empty
    }
    TTF_Font *font = TTF_OpenFont(fontPath, size);
    if (!font)
    {
        printf("TTF_OpenFont Error: %s\n", TTF_GetError());
        return;
    }

    // TTF_SetFontWrappedAlign(font, TTF_WRAPPED_ALIGN_CENTER);

    SDL_Color color = {r, g, b, a};
    // TTF_SetFontStyle(font, TTF_STYLE_BOLD);
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &rect);

    TTF_CloseFont(font);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

int mesureTextWidth(char *fontPath, char *text, int size)
{
    TTF_Font *font = TTF_OpenFont(fontPath, size);
    if (!font)
    {
        printf("TTF_OpenFont Error: %s\n", TTF_GetError());
        return 0;
    }

    int w, h;
    TTF_SizeText(font, text, &w, &h);

    TTF_CloseFont(font);
    return w;
}

void drawSVG(SDL_Renderer *renderer, char *path, int x, int y, int w, int h)
{
    SDL_Surface *surface = IMG_Load(path);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

SDL_Color getPixelColor(SDL_Renderer *renderer, int pixel_X, int pixel_Y)
{
    SDL_Surface *getPixel_Surface = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, getPixel_Surface->pixels, getPixel_Surface->pitch);
    const Uint8 getPixel_bpp = getPixel_Surface->format->BytesPerPixel;

    SDL_Color pixelColor;
    Uint8 *pPixel = (Uint8 *)getPixel_Surface->pixels + pixel_Y * getPixel_Surface->pitch + pixel_X * getPixel_bpp;

    Uint32 pixelData;

    switch (getPixel_bpp)
    {
    case 1:
        pixelData = *pPixel;
        break;
    case 2:
        pixelData = *(Uint16 *)pPixel;
        break;
    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            pixelData = pPixel[0] << 16 | pPixel[1] << 8 | pPixel[2];
        else
            pixelData = pPixel[0] | pPixel[1] << 8 | pPixel[2] << 16;
        break;
    case 4:
        pixelData = *(Uint32 *)pPixel;
        break;
    }

    SDL_GetRGBA(pixelData, getPixel_Surface->format, &pixelColor.r, &pixelColor.g, &pixelColor.b, &pixelColor.a);

    return pixelColor;
}

bool compareColor(SDL_Color color, int r, int g, int b, int a)
{
    return (r == color.r) && (g == color.g) && (b == color.b) && (a = color.a);
}

void machineModeMemorize(SDL_Renderer *renderer, int n, int matrix[n][n])
{
    int cellSize = GRID_SIZE / n;
    initializeMatrix(n, matrix);

    for (int i = 1; i < n - 1; i++)
    {
        for (int j = 1; j < n - 1; j++)
        {
            int x = OFFSET + j * cellSize + cellSize / 2 + THICKNESS / 2;
            int y = OFFSET + i * cellSize + cellSize / 2 + THICKNESS / 2;

            if (j <= 0 || j >= n - 1 || i <= 0 || i >= n - 1)
            {
                continue;
            }

            SDL_Color color = getPixelColor(renderer, x, y);
            // printf("color: %d %d %d %d\n at i=%d,j=%d", color.r, color.g, color.b, color.a, i, j);
            bool isDiagonal = compareColor(color, DIAGONAL_COLOR);
            if (!isDiagonal)
            {
                matrix[i][j] = 0;
                continue;
            }

            int of = cellSize / 4;
            color = getPixelColor(renderer, x - of, y + of);
            bool isUp = compareColor(color, DIAGONAL_COLOR); // check if the diagonal is up or down
            if (isUp)
            {
                matrix[i][j] = 1;
            }
            else
            {
                matrix[i][j] = -1;
            }
        }
    }
}

void machineModeSelecting(SDL_Renderer *renderer, int n, int matrix[n][n])
{
    int cellSize = GRID_SIZE / n;
    int startI = 0, startJ = 0;

    for (int i = 1; i < n - 1; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (j > 0 && j < n - 1)
                continue;
            int x = OFFSET + j * cellSize + cellSize / 2 + THICKNESS / 2;
            int y = OFFSET + i * cellSize + cellSize / 2 + THICKNESS / 2;

            SDL_Color color1 = getPixelColor(renderer, x, y);
            SDL_Color color2 = getPixelColor(renderer, y, x);
            bool isStart1 = compareColor(color1, START_CIRCLE_COLOR);
            bool isStart2 = compareColor(color2, START_CIRCLE_COLOR);
            if (isStart1 || isStart2)
            {
                startI = isStart1 ? i : j;
                startJ = isStart1 ? j : i;
                goto afterloop;
            }
        }
    }
afterloop:;

    solution *s = solveMatrix(matrix[startI][startJ], 0, n, matrix); // obs = 0
    // free Game->
    freePath(s->path);

    int endX = s->endJ * cellSize + OFFSET + cellSize / 2 + THICKNESS / 2;
    int endY = s->endI * cellSize + OFFSET + cellSize / 2 + THICKNESS / 2;
    free(s);

    graycefulDelay(1000);
    SDL_SetRenderDrawColor(renderer, SELECTED_CIRCLE_COLOR);
    drawFilledCircle(renderer, endX, endY, cellSize / 4);
    SDL_RenderPresent(renderer);
    graycefulDelay(1000);

    // custom event
    SDL_Event e;
    e.type = SDL_USEREVENT;
    e.user.code = 139;
    // set x and y coordinates
    e.user.data1 = NULL;
    e.user.data2 = NULL;

    e.button.x = endX;
    e.button.y = endY;

    SDL_PushEvent(&e);
}