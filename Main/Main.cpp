#include <string>
#include <vector>
#include <Windows.h>
#include <CommCtrl.h>
#include "Define.h"
#include "Enum.h"
#include "Renderer.h"
#include "resource.h"
#include "SoundPlayer.h"
#include "Time.h"
#include "Data.h"
#include "Vector2.h"
#include "Enemy.h"
#include "Random.h"

#pragma comment(lib, "Comctl32.lib")

// Global variables
HINSTANCE hGlobalInstance;

wchar_t characterName[CHARACTER_MAX_NAME_LENGTH + 1];

bool characterMoveable;

ShowType showType = ShowType::Title;

std::vector<unsigned short> items;
std::vector<Enemy> enemies;

// Functions
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK CharacterInformationWindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK InventoryUseButtonProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR args, _In_ int cmdShow)
{
	// Variables
	HWND hWnd;
	MSG message;
	WNDCLASSEX wndClassEx;

	// Initialize
	Renderer::Initialize();
	SoundPlayer::Initialize();
	Time::Initialize();
	Data::Initialize();
	Random::Initialize();

	// Setup window class
	hGlobalInstance = hInstance;

	wndClassEx.cbClsExtra = 0;
	wndClassEx.cbSize = sizeof(WNDCLASSEX);
	wndClassEx.cbWndExtra = 0;
	wndClassEx.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wndClassEx.hCursor = LoadCursor(nullptr, IDC_CROSS);
	wndClassEx.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
	wndClassEx.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
	wndClassEx.hInstance = hInstance;
	wndClassEx.lpfnWndProc = WindowProcedure;
	wndClassEx.lpszClassName = WINDOW_CLASS_NAME;
	wndClassEx.lpszMenuName = nullptr;
	wndClassEx.style = CS_DBLCLKS | CS_PARENTDC;

	if (!RegisterClassEx(&wndClassEx))
	{
		return 0;
	}

	// Setup window
	hWnd = CreateWindow(WINDOW_CLASS_NAME, WINDOW_TITLE, WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH + WINDOW_WIDTH_OFFSET, WINDOW_HEIGHT + WINDOW_HEIGHT_OFFSET, HWND_DESKTOP, nullptr, hInstance, nullptr);

	if (hWnd == nullptr)
	{
		return 0;
	}

	ShowWindow(hWnd, cmdShow);

	// Setup character information window class
	wndClassEx.cbClsExtra = 0;
	wndClassEx.cbSize = sizeof(WNDCLASSEX);
	wndClassEx.cbWndExtra = 0;
	wndClassEx.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wndClassEx.hCursor = LoadCursor(nullptr, IDC_CROSS);
	wndClassEx.hIcon = LoadIcon(hGlobalInstance, MAKEINTRESOURCE(IDI_ICON));
	wndClassEx.hIconSm = LoadIcon(hGlobalInstance, MAKEINTRESOURCE(IDI_ICON));
	wndClassEx.hInstance = hGlobalInstance;
	wndClassEx.lpfnWndProc = CharacterInformationWindowProcedure;
	wndClassEx.lpszClassName = WINDOW_CHARACTER_INFORMATION_CLASS_NAME;
	wndClassEx.lpszMenuName = nullptr;
	wndClassEx.style = CS_DBLCLKS | CS_PARENTDC;

	if (!RegisterClassEx(&wndClassEx))
	{
		return 0;
	}

	// Message loop
	while (GetMessage(&message, nullptr, 0, 0))
	{
		TranslateMessage(&message);

		DispatchMessage(&message);
	}

	return message.wParam;
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Variables
	static double previousCharacterPosition;
	static unsigned char currentCharacterFrame;
	static bool characterOnDamaged = false;
	static unsigned char currentCharacterHealthBarSprite;

	static Vector2 characterPosition;

	static double enemySpawnTimer;
	static double scoreTimer;
	static double enemySpawnTick = 5;
	static double characterOnDamageTimer;

	static unsigned long long score = 0;

	static Direction characterMoveDirection;

	// Paint
	RECT clientRect;
	HDC hdc;
	HDC hMemoryDC;
	HDC hTempDC;
	PAINTSTRUCT paintStruct;
	HBITMAP hBitmap;
	HBITMAP hOldBitmap;

	// Buttons
	static HWND titleStartButton;
	static HWND gameoverTitleButton;
	static HWND inventoryItemUseButton;

	// Process messages
	switch (message)
	{
	// Start
	case WM_CREATE:
	{
		// Buttons
		titleStartButton = CreateWindow(L"button", L"시작", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 160, WINDOW_HEIGHT - 140, 138, 72, hWnd, reinterpret_cast<HMENU>(0), hGlobalInstance, nullptr);
		gameoverTitleButton = CreateWindow(L"button", L"타이틀 화면으로 돌아가기", WS_CHILD | BS_PUSHBUTTON, 25, WINDOW_HEIGHT - 200, 300, 160, hWnd, reinterpret_cast<HMENU>(1), hGlobalInstance, nullptr);
		inventoryItemUseButton = CreateWindow(L"button", L"사용하기", WS_CHILD | BS_PUSHBUTTON, 20, 340, 160, 60, hWnd, reinterpret_cast<HMENU>(2), hGlobalInstance, nullptr);

		SetWindowSubclass(inventoryItemUseButton, InventoryUseButtonProcedure, 0, 0);

		// Load datas
		Data::LoadData();

		wcscpy_s(::characterName, CHARACTER_MAX_NAME_LENGTH, Data::GetCharacterName());

		currentCharacterHealthBarSprite = Data::GetCharacterHealthBarSprite();
		characterPosition.Set(Data::GetCharacterPosition());
		
		unsigned short itemCount = 0;
		unsigned short* items = Data::GetItems(&itemCount);

		for (unsigned short i = 0; i < itemCount; i++)
		{
			::items.push_back(items[i]);
		}

		score = Data::GetScore();
		enemies = Data::GetEnemies();

		for (size_t i = 0; i < enemies.size(); i++)
		{
			enemies[i].SetTarget(&characterPosition);
		}

		// Sound, Update
		SoundPlayer::Play(TITLE_BGM);

		SetTimer(hWnd, 0, 1000 / GAME_FPS, nullptr);

		break;
	}

	// Update logics
	case WM_TIMER:
	{
		// Update
		Time::Update();

		ShowWindow(inventoryItemUseButton, showType == ShowType::Inventory ? SW_NORMAL : SW_HIDE);

		// Show type
		if (showType != ShowType::Game)
		{
			break;
		}

		double deltaTime = Time::GetDeltaTime();

		// Score
		scoreTimer += deltaTime;

		if (scoreTimer > 0.1)
		{
			score++;
			scoreTimer -= 0.1;
		}

		// Enemies
		enemySpawnTimer += deltaTime;

		if (enemySpawnTimer > enemySpawnTick)
		{
			// Get item
			int i = items.size();

			if (i <= 2)
			{
				::items.push_back(0);
			}

			// Spawn enemy
			enemies.push_back(Enemy(Vector2(Random::Int(120, WINDOW_WIDTH - 150), Random::Int(60, WINDOW_HEIGHT - 90)), &characterPosition, Random::Int(10, 60), Random::Int(3, 20)));

			enemySpawnTimer -= enemySpawnTick;

			enemySpawnTick -= 0.0001 * Random::Int(1, 1000);

			if (enemySpawnTick <= 0.2)
			{
				enemySpawnTick = 5;
			}
		}

		if (characterOnDamaged)
		{
			characterOnDamageTimer += deltaTime;

			if (characterOnDamageTimer > 0.5)
			{
				characterOnDamaged = false;
				characterOnDamageTimer -= 0.5;
			}
		}

		for (int i = 0; i < enemies.size(); i++)
		{
			if (!enemies[i].TrackingCharacter())
			{
				enemies.erase(enemies.begin() + i);

				continue;
			}

			// Collision
			Vector2* enemyPosition = enemies[i].GetPosition();

			if (!characterOnDamaged && characterPosition.x - 30 <= enemyPosition->x && enemyPosition->x <= characterPosition.x + CHARACTER_SIZE && characterPosition.y - 30 <= enemyPosition->y && enemyPosition->y <= characterPosition.y + CHARACTER_SIZE)
			{
				characterOnDamaged = true;
				currentCharacterHealthBarSprite++;

				if (currentCharacterHealthBarSprite >= 3)
				{
					showType = ShowType::GameOver;

					characterMoveable = false;
					score = 0;

					SoundPlayer::Stop();
					SoundPlayer::Play(GAME_OVER_BGM);

					enemies.clear();

					InvalidateRect(hWnd, nullptr, false);
					ShowWindow(gameoverTitleButton, SW_NORMAL);

					return 0;
				}
			}
		}

		// Move character
		GetClientRect(hWnd, &clientRect);

		switch (characterMoveDirection)
		{
		case Direction::Left:
			if (characterPosition.x - CHARACTER_SIZE < 0)
			{
				characterPosition.x = 0;
				characterMoveable = true;
				characterMoveDirection = Direction::None;

				break;
			}

			characterPosition.x -= CHARACTER_SIZE / CHARACTER_SPEED * deltaTime;

			if (previousCharacterPosition - characterPosition.x > CHARACTER_SIZE)
			{
				characterPosition.x = previousCharacterPosition - CHARACTER_SIZE;
				characterMoveable = true;
				characterMoveDirection = Direction::None;
			}

			break;
		case Direction::Up:
			if (characterPosition.y - CHARACTER_SIZE < 0)
			{
				characterPosition.y = 0;
				characterMoveable = true;
				characterMoveDirection = Direction::None;

				break;
			}

			characterPosition.y -= CHARACTER_SIZE / CHARACTER_SPEED * deltaTime;

			if (previousCharacterPosition - characterPosition.y > CHARACTER_SIZE)
			{
				characterPosition.y = previousCharacterPosition - CHARACTER_SIZE;
				characterMoveable = true;
				characterMoveDirection = Direction::None;
			}

			break;
		case Direction::Right:
			if (characterPosition.x + CHARACTER_SIZE + CHARACTER_SIZE >= clientRect.right)
			{
				characterPosition.x = clientRect.right - CHARACTER_SIZE;
				characterMoveable = true;
				characterMoveDirection = Direction::None;

				break;
			}

			characterPosition.x += CHARACTER_SIZE / CHARACTER_SPEED * deltaTime;

			if (characterPosition.x - previousCharacterPosition > CHARACTER_SIZE)
			{
				characterPosition.x = previousCharacterPosition + CHARACTER_SIZE;
				characterMoveable = true;
				characterMoveDirection = Direction::None;
			}

			break;
		case Direction::Down:
			if (characterPosition.y + CHARACTER_SIZE + CHARACTER_SIZE >= clientRect.bottom)
			{
				characterPosition.y = clientRect.bottom - CHARACTER_SIZE;
				characterMoveable = true;
				characterMoveDirection = Direction::None;

				break;
			}

			characterPosition.y += CHARACTER_SIZE / CHARACTER_SPEED * deltaTime;

			if (characterPosition.y - previousCharacterPosition > CHARACTER_SIZE)
			{
				characterPosition.y = previousCharacterPosition + CHARACTER_SIZE;
				characterMoveable = true;
				characterMoveDirection = Direction::None;
			}

			break;
		}

		InvalidateRect(hWnd, nullptr, false);

		break;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case 0:
			if (currentCharacterHealthBarSprite >= 3)
			{
				showType = ShowType::GameOver;

				characterMoveable = false;

				SoundPlayer::Stop();
				SoundPlayer::Play(GAME_OVER_BGM);

				InvalidateRect(hWnd, nullptr, false);
				ShowWindow(gameoverTitleButton, SW_NORMAL);

				return 0;
			}
			else
			{
				showType = ShowType::Game;

				characterMoveable = true;

				SoundPlayer::Stop();
				SoundPlayer::Play(GAME_BGM);
			}

			ShowWindow(titleStartButton, SW_HIDE);

			break;
		case 1:
			showType = ShowType::Title;

			currentCharacterHealthBarSprite = 0;

			SoundPlayer::Stop();
			SoundPlayer::Play(TITLE_BGM);

			InvalidateRect(hWnd, nullptr, false);
			ShowWindow(gameoverTitleButton, SW_HIDE);
			ShowWindow(titleStartButton, SW_NORMAL);

			break;

		// Item use events
		case 2:
			if (items.size() <= 0)
			{
				break;
			}

			switch (items[0])
			{
			case 0:
				if (currentCharacterHealthBarSprite > 0)
				{
					currentCharacterHealthBarSprite--;
				}

				break;
			}

			items.erase(items.begin());

			InvalidateRect(hWnd, nullptr, false);

			break;
		}

		break;

	// Get key down events
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'W':
		case 'w':
			if (!characterMoveable)
			{
				break;
			}

			previousCharacterPosition = characterPosition.y;
			characterMoveable = false;
			characterMoveDirection = Direction::Up;

			break;
		case 'A':
		case 'a':
			if (!characterMoveable)
			{
				break;
			}

			previousCharacterPosition = characterPosition.x;
			characterMoveable = false;
			characterMoveDirection = Direction::Left;

			break;
		case 'S':
		case 's':
			if (!characterMoveable)
			{
				break;
			}

			previousCharacterPosition = characterPosition.y;
			characterMoveable = false;
			characterMoveDirection = Direction::Down;

			break;
		case 'D':
		case 'd':
			if (!characterMoveable)
			{
				break;
			}

			previousCharacterPosition = characterPosition.x;
			characterMoveable = false;
			characterMoveDirection = Direction::Right;

			break;
		case 'I':
		case 'i':
			if (showType == ShowType::Title || showType == ShowType::GameOver)
			{
				break;
			}

			showType = (characterMoveable = !characterMoveable) ? ShowType::Game : ShowType::Inventory;

			InvalidateRect(hWnd, nullptr, false);

			SoundPlayer::Stop();
			SoundPlayer::Play(showType == ShowType::Inventory ? INVENTORY_BGM : GAME_BGM);

			break;
		case 'E':
		case 'e':
		{
			if (showType == ShowType::Title || showType == ShowType::GameOver)
			{
				break;
			}

			GetWindowRect(hWnd, &clientRect);

			CreateWindow(WINDOW_CHARACTER_INFORMATION_CLASS_NAME, L"캐릭터 정보", WS_VISIBLE | WS_SYSMENU, clientRect.left + WINDOW_WIDTH_OFFSET, clientRect.top + WINDOW_HEIGHT_OFFSET, WINDOW_WIDTH + WINDOW_WIDTH_OFFSET >> 1, WINDOW_HEIGHT + WINDOW_HEIGHT_OFFSET >> 1, hWnd, nullptr, hGlobalInstance, nullptr);

			break;
		}

		// If press to escape key, show quit the game popup.
		case 0x1b:
			int result = MessageBox(hWnd, L"게임을 나가시겠습니까?", L"정보", MB_YESNO | MB_ICONQUESTION);

			if (result == IDYES)
			{
				DestroyWindow(hWnd);
			}

			break;
		}

		break;
	case WM_PAINT:
		// Double buffering begin process
		GetClientRect(hWnd, &clientRect);

		hdc = BeginPaint(hWnd, &paintStruct);
		hMemoryDC = CreateCompatibleDC(hdc);
		hBitmap = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
		hOldBitmap = reinterpret_cast<HBITMAP>(SelectObject(hMemoryDC, hBitmap));

		PatBlt(hMemoryDC, 0, 0, clientRect.right, clientRect.bottom, BLACKNESS);

		hTempDC = hdc;
		hdc = hMemoryDC;
		hMemoryDC = hTempDC;

		// Draw
		switch (showType)
		{
		case ShowType::Title:
		{
			unsigned short fontSize = 50;

			Renderer::Image(hGlobalInstance, hWnd, hdc, IDB_TITLE_BACKGROUND, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
			Renderer::Text(hWnd, hdc, L"AROUND", 80, 40, fontSize, L"Noto Sans", Gdiplus::Color(27, 55, 83));

			fontSize = 16;

			wchar_t versionText[19];

			LoadString(hGlobalInstance, IDS_VERSION, versionText, sizeof(versionText));

			Renderer::Text(hWnd, hdc, versionText, 5, WINDOW_HEIGHT - 34, fontSize, L"Noto Sans", Gdiplus::Color(12, 12, 12));

			break;
		}
		case ShowType::Game:
		{
			unsigned short fontSize = 20;

			Renderer::Image(hGlobalInstance, hWnd, hdc, IDB_BACKGROUND, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
			Renderer::Image(hGlobalInstance, hWnd, hdc, IDB_CHARACTER_0 + currentCharacterFrame, characterPosition.x, characterPosition.y, CHARACTER_SIZE, CHARACTER_SIZE);
			Renderer::Image(hGlobalInstance, hWnd, hdc, IDB_CHARACTER_HEALTH_BAR_0 + currentCharacterHealthBarSprite, characterPosition.x - 15, characterPosition.y - 20, CHARACTER_SIZE + 30, 15);
			Renderer::Text(hWnd, hdc, characterName, characterPosition.x - 10 * (wcslen(characterName) >> 1), characterPosition.y - (CHARACTER_SIZE >> 1) - 28, fontSize, L"Noto Sans", Gdiplus::Color(122, 66, 221));

			// Enemies
			for (int i = 0; i < enemies.size(); i++)
			{
				Vector2* position = enemies[i].GetPosition();

				Renderer::Image(hGlobalInstance, hWnd, hdc, IDB_CHARACTER_HEALTH_BAR_0, position->x, position->y, 30, 30);
			}

			// UI
			fontSize = 30;

			std::wstring scoreText(L"SCORE : " + std::to_wstring(score));

			Renderer::Text(hWnd, hdc, scoreText.c_str(), 5, 0, fontSize, L"Noto Sans", Gdiplus::Color(217, 61, 234));

			currentCharacterFrame = (currentCharacterFrame + 1) % CHARACTER_FRAME_COUNT;

			break;
		}
		case ShowType::Inventory:
		{
			unsigned short fontSize = 60;

			Renderer::Image(hGlobalInstance, hWnd, hdc, IDB_INVENTORY_BACKGROUND, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
			Renderer::Text(hWnd, hdc, L"인벤토리", (WINDOW_WIDTH >> 1) - fontSize * 3, 60 - fontSize, fontSize, L"Noto Sans", Gdiplus::Color(162, 78, 77));

			// Items
			for (int i = 0; i < items.size(); i++)
			{
				Renderer::Image(hGlobalInstance, hWnd, hdc, IDB_ITEM_HEALTH_POTION + items[i], 20 + i * 180, 140, 160, 160);
			}

			break;
		}
		case ShowType::GameOver:
		{
			unsigned short fontSize = 90;

			Renderer::Text(hWnd, hdc, L"게임 오버!", 5, 50, fontSize, L"궁서", Gdiplus::Color(255, 255, 255));

			break;
		}
		}

		// Double buffering end process
		hTempDC = hdc;
		hdc = hMemoryDC;
		hMemoryDC = hTempDC;

		GetClientRect(hWnd, &clientRect);
		BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, hMemoryDC, 0, 0, SRCCOPY);
		SelectObject(hMemoryDC, hOldBitmap);
		DeleteObject(hOldBitmap);
		DeleteDC(hMemoryDC);
		EndPaint(hWnd, &paintStruct);

		break;

	// Release
	case WM_CLOSE:
	{
		int result = MessageBox(hWnd, L"게임을 나가시겠습니까?", L"정보", MB_YESNO | MB_ICONQUESTION);

		if (result == IDYES)
		{
			DestroyWindow(hWnd);
		}

		break;
	}
	case WM_DESTROY:
	{
		// Save data
		Data::SetCharacterName(characterName);
		Data::SetCharacterHealthBarSprite(currentCharacterHealthBarSprite);
		Data::SetCharacterPosition(characterPosition);

		unsigned short* items = new unsigned short[::items.size()];

		for (unsigned short i = 0; i < ::items.size(); i++)
		{
			items[i] = ::items[i];
		}

		Data::SetItems(::items.size(), items);
		Data::SetScore(score);
		Data::SetEnemies(enemies);

		// Release
		Renderer::Release();
		SoundPlayer::Release();
		Data::SaveData();

		PostQuitMessage(0);

		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

LRESULT CALLBACK CharacterInformationWindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Paint
	RECT clientRect;
	HDC hdc;
	HDC hMemoryDC;
	HDC hTempDC;
	PAINTSTRUCT paintStruct;
	HBITMAP hBitmap;
	HBITMAP hOldBitmap;

	// Input fields
	static HWND characterNameInputField;

	// Process messages
	switch (message)
	{
	// Start
	case WM_CREATE:
		characterNameInputField = CreateWindow(L"edit", nullptr, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, 50, 40, 160, 16, hWnd, nullptr, hGlobalInstance, nullptr);

		break;
	case WM_COMMAND:
		switch (HIWORD(wParam))
		{
		case EN_CHANGE:
			int inputLength = GetWindowTextLength(characterNameInputField);

			if (inputLength >= CHARACTER_MAX_NAME_LENGTH + 1)
			{
				break;
			}

			GetWindowText(characterNameInputField, characterName, inputLength);

			break;
		}

		break;
	case WM_PAINT:
	{
		// Double buffering begin process
		GetClientRect(hWnd, &clientRect);

		hdc = BeginPaint(hWnd, &paintStruct);
		hMemoryDC = CreateCompatibleDC(hdc);
		hBitmap = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
		hOldBitmap = reinterpret_cast<HBITMAP>(SelectObject(hMemoryDC, hBitmap));

		PatBlt(hMemoryDC, 0, 0, clientRect.right, clientRect.bottom, BLACKNESS);

		hTempDC = hdc;
		hdc = hMemoryDC;
		hMemoryDC = hTempDC;

		// Draw
		std::wstring characterName(L"캐릭터 이름 : ");

		characterName += ::characterName;

		Renderer::Text(hWnd, hdc, characterName.c_str(), 20, 5, 16, L"Noto Sans", Gdiplus::Color(232, 232, 232));

		// Double buffering end process
		hTempDC = hdc;
		hdc = hMemoryDC;
		hMemoryDC = hTempDC;

		GetClientRect(hWnd, &clientRect);
		BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, hMemoryDC, 0, 0, SRCCOPY);
		SelectObject(hMemoryDC, hOldBitmap);
		DeleteObject(hOldBitmap);
		DeleteDC(hMemoryDC);
		EndPaint(hWnd, &paintStruct);

		break;
	}

	// Release
	case WM_DESTROY:
		DestroyWindow(hWnd);

		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

LRESULT CALLBACK InventoryUseButtonProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	switch (message)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'I':
		case 'i':
			if (showType == ShowType::Title || showType == ShowType::GameOver)
			{
				break;
			}

			showType = (characterMoveable = !characterMoveable) ? ShowType::Game : ShowType::Inventory;

			SoundPlayer::Stop();
			SoundPlayer::Play(showType == ShowType::Inventory ? INVENTORY_BGM : GAME_BGM);

			break;

		// If press to escape key, show quit the game popup.
		case 0x1b:
			int result = MessageBox(hWnd, L"게임을 나가시겠습니까?", L"정보", MB_YESNO | MB_ICONQUESTION);

			if (result == IDYES)
			{
				DestroyWindow(hWnd);
			}

			break;
		}

		break;
	default:
		return DefSubclassProc(hWnd, message, wParam, lParam);
	}
}