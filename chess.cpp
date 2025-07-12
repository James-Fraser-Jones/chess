#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <cctype>
#include <format>

constexpr int SCREEN_WIDTH = 1920;
constexpr int SCREEN_HEIGHT = 1080;
constexpr int BOARD_LOGICAL_SIZE = 720;
constexpr int SQUARE_SIZE = BOARD_LOGICAL_SIZE / 8;

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;

enum class ChessPieceType : char {
  PAWN = 0,
  ROOK = 1,
  KNIGHT = 2,
  BISHOP = 3,
  QUEEN = 4,
  KING = 5,
  COUNT
};
std::string chessPieceTypeToString(ChessPieceType type) {
  switch (type) {
  case ChessPieceType::PAWN:
    return "Pawn";
  case ChessPieceType::ROOK:
    return "Rook";
  case ChessPieceType::KNIGHT:
    return "Knight";
  case ChessPieceType::BISHOP:
    return "Bishop";
  case ChessPieceType::QUEEN:
    return "Queen";
  case ChessPieceType::KING:
    return "King";
  default:
    return "Unknown";
  }
}

enum class PlayerType : char {
  BLACK = 0,
  WHITE = 1,
  COUNT
};
std::string playerTypeToString(PlayerType type) {
  switch (type) {
  case PlayerType::BLACK:
    return "Black";
  case PlayerType::WHITE:
    return "White";
  default:
    return "Unknown";
  }
}

char toLower(char c) {
  return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
}

static SDL_Texture* chessPieceTextures[static_cast<int>(PlayerType::COUNT)][static_cast<int>(ChessPieceType::COUNT)] = {};

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

constexpr Color OLIVE_GREEN = {119, 148, 83};
constexpr Color LIGHT_YELLOW = {237, 237, 207};

void setRenderDrawColor(SDL_Renderer* renderer, const Color& color, uint8_t alpha = SDL_ALPHA_OPAQUE) {
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, alpha);
}

char intToChar(int value) {
  return static_cast<char>('A' + value);
}

SDL_Texture* getTexture(const char* filename) {
  SDL_Surface* imageSurface = IMG_Load(filename);
  if (!imageSurface) {
    SDL_Log("Unable to load PNG file! SDL Error: %s", SDL_GetError());
    return NULL;
  }
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, imageSurface);
  if (!texture) {
    SDL_Log("Unable to create texture from surface! SDL Error: %s", SDL_GetError());
  }
  SDL_DestroySurface(imageSurface);
  return texture;
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

  for (int i = 0; i < static_cast<int>(PlayerType::COUNT); ++i) {
    for (int j = 0; j < static_cast<int>(ChessPieceType::COUNT); ++j) {
      std::string filename = SDL_GetBasePath(); // goes into the build directory
      filename += "\\..\\chess_pieces\\";
      std::string playerType = playerTypeToString(static_cast<PlayerType>(i));
      filename += toLower(playerType[0]);
      std::string pieceType = chessPieceTypeToString(static_cast<ChessPieceType>(j));
      filename += toLower(pieceType[0]);
      filename += ".png";
      chessPieceTextures[i][j] = getTexture(filename.c_str());
      if (!chessPieceTextures[i][j]) {
        SDL_Log("Failed to load texture: %s", filename.c_str());
        return SDL_APP_FAILURE;
      }
    }
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
  SDL_ConvertEventToRenderCoordinates(renderer, event);
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  } else if (event->type == SDL_EVENT_KEY_DOWN) {
  } else if (event->type == SDL_EVENT_KEY_UP) {
    SDL_KeyboardEvent* keyEvent = &event->key;
    if (keyEvent->key == SDLK_ESCAPE) {
      return SDL_APP_SUCCESS;
    }
  } else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
    SDL_MouseButtonEvent* mouseButtonEvent = &event->button;
    if (mouseButtonEvent->button == SDL_BUTTON_LEFT) {
      SDL_Log("Left mouse button clicked at (%f, %f)", mouseButtonEvent->x, mouseButtonEvent->y);
    }
  } else if (event->type == SDL_EVENT_MOUSE_MOTION) {
    SDL_MouseMotionEvent* mouseMotionEvent = &event->motion;
    SDL_Log("Mouse moved at (%f, %f)", mouseMotionEvent->x, mouseMotionEvent->y);
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
      SDL_RenderTexture(renderer, chessPieceTextures[static_cast<int>(PlayerType::BLACK)][static_cast<int>(ChessPieceType::BISHOP)], NULL, &rect);
    }
  }
  SDL_RenderPresent(renderer);
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}