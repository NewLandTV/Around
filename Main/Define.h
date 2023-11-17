#ifndef __DEFINE_H__
#define __DEFINE_H__

// Window
#define WINDOW_WIDTH 720
#define WINDOW_WIDTH_OFFSET 16
#define WINDOW_HEIGHT 540
#define WINDOW_HEIGHT_OFFSET 40
#define WINDOW_TITLE L"Around"
#define WINDOW_CLASS_NAME L"AroundWindowApp"
#define WINDOW_CHARACTER_INFORMATION_CLASS_NAME L"CharacterInformationWindow"

// Game
#define GAME_FPS 60

// Data
#define DATA_PATH "C:\\NewLand\\Around\\"
#define DATA_FILE_PATH(file) DATA_PATH##file

// Character
#define CHARACTER_SPEED 0.125
#define CHARACTER_FRAME_COUNT 18
#define CHARACTER_SIZE 60
#define CHARACTER_MAX_NAME_LENGTH 11
#define CHARACTER_NAME L"Character"

#define MAX_INVENTORY_SIZE 30

// Sounds
#define TITLE_BGM L"Resources\\Back Seat.mp3"
#define GAME_BGM L"Resources\\Sand Around.mp3"
#define INVENTORY_BGM L"Resources\\Ice On Normal.mp3"
#define GAME_OVER_BGM L"Resources\\Small Light And Rotate.mp3"

#endif