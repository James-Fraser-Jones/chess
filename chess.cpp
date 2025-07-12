#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <format>

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture* imageTexture = NULL;

constexpr int SCREEN_WIDTH = 1920;
constexpr int SCREEN_HEIGHT = 1080;
constexpr int BOARD_LOGICAL_SIZE = 720;
constexpr int SQUARE_SIZE = BOARD_LOGICAL_SIZE / 8;

struct Color {
  uint8_t r, g, b;
};

constexpr Color WHITE = {255, 255, 255};
constexpr Color BLACK = {0, 0, 0};
constexpr Color RED = {255, 0, 0};
constexpr Color GREEN = {0, 255, 0};
constexpr Color BLUE = {0, 0, 255};
constexpr Color YELLOW = {255, 255, 0};
constexpr Color CYAN = {0, 255, 255};
constexpr Color MAGENTA = {255, 0, 255};
constexpr Color GRAY = {128, 128, 128};
constexpr Color DARK_GRAY = {64, 64, 64};
constexpr Color LIGHT_GRAY = {192, 192, 192};
constexpr Color OLIVE_GREEN = {119, 148, 83};
constexpr Color LIGHT_YELLOW = {237, 237, 207};

void setRenderDrawColor(SDL_Renderer* renderer, const Color& color, uint8_t alpha = SDL_ALPHA_OPAQUE) {
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, alpha);
}

char intToChar(int value) {
  return static_cast<char>('A' + value);
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
  SDL_SetAppMetadata("Chess", "1.0", "uk.co.fraser-jones.chess");
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  if (!SDL_CreateWindowAndRenderer("Chess", SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer)) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  if (!SDL_SetRenderLogicalPresentation(renderer, BOARD_LOGICAL_SIZE, BOARD_LOGICAL_SIZE, SDL_LOGICAL_PRESENTATION_LETTERBOX)) {
    SDL_Log("Couldn't set logical presentation: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  SDL_Surface* imageSurface = SDL_LoadBMP("chess_pieces/wp.bmp");
  if (!imageSurface) {
    SDL_Log("Unable to load BMP file! SDL Error: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  imageTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
  if (!imageTexture) {
    SDL_Log("Unable to create texture from surface! SDL Error: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  SDL_DestroySurface(imageSurface);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
  // setRenderDrawColor(renderer, BLACK, SDL_ALPHA_OPAQUE);
  // SDL_RenderClear(renderer);
  SDL_FRect rect = {0, 0, SQUARE_SIZE, SQUARE_SIZE};
  for (int row = 0; row < 8; ++row) {
    for (int col = 0; col < 8; ++col) {
      setRenderDrawColor(renderer, (row + col) % 2 == 0 ? LIGHT_YELLOW : OLIVE_GREEN, SDL_ALPHA_OPAQUE);
      rect.x = col * SQUARE_SIZE;
      rect.y = row * SQUARE_SIZE;
      SDL_RenderFillRect(renderer, &rect);
      setRenderDrawColor(renderer, (row + col) % 2 == 0 ? OLIVE_GREEN : LIGHT_YELLOW, SDL_ALPHA_OPAQUE);
      SDL_RenderDebugText(renderer, rect.x + 5, rect.y + 5, std::format("{}{}", intToChar(col), 8 - row).c_str());
      SDL_RenderTexture(renderer, imageTexture, NULL, &rect);
    }
  }
  SDL_RenderPresent(renderer);
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {}