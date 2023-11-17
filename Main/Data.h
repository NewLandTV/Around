#ifndef __DATA_H__
#define __DATA_H__

#include <vector>
#include <string>
#include "Define.h"
#include "Vector2.h"
#include "Enemy.h"

static class Data
{
private:
	static std::wstring characterName;

	static int characterHealthBarSprite;

	static Vector2 characterPosition;

	static unsigned short itemCount;
	static unsigned short items[MAX_INVENTORY_SIZE];

	static unsigned long long score;

	static std::vector<Enemy> enemies;

	static void EncryptData(std::wstring* out);
	static void DecryptData(std::wstring* out);
	static void ReverseData(std::wstring* out);

public:
	static void Initialize();

	static void SaveData();
	static void LoadData();

	// Getters
	static const wchar_t* GetCharacterName();
	static const int GetCharacterHealthBarSprite();
	static Vector2& GetCharacterPosition();
	static unsigned short* GetItems(unsigned short* countOut);
	static unsigned long long GetScore();
	static std::vector<Enemy> GetEnemies();

	// Setters
	static void SetCharacterName(const wchar_t* value);
	static void SetCharacterHealthBarSprite(const int value);
	static void SetCharacterPosition(Vector2& value);
	static void SetItems(unsigned short count, unsigned short* value);
	static void SetScore(unsigned long long score);
	static void SetEnemies(std::vector<Enemy> value);
};

#endif