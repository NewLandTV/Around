#include <locale>
#include <direct.h>
#include <cstdio>
#include <Windows.h>
#include "Data.h"

std::wstring Data::characterName;

int Data::characterHealthBarSprite;

Vector2 Data::characterPosition;

unsigned short Data::itemCount;
unsigned short Data::items[MAX_INVENTORY_SIZE];

unsigned long long Data::score;

std::vector<Enemy> Data::enemies;

void Data::EncryptData(std::wstring* out)
{
	ReverseData(out);

	for (int i = 0; i < out->size(); i++)
	{
		out->at(i) ^= -13;
	}
}

void Data::DecryptData(std::wstring* out)
{
	for (int i = 0; i < out->size(); i++)
	{
		out->at(i) ^= -13;
	}

	ReverseData(out);
}

void Data::ReverseData(std::wstring* out)
{
	std::reverse(out->begin(), out->end());
}

void Data::Initialize()
{
	setlocale(LC_ALL, "ko-KR");

	SetCharacterName(CHARACTER_NAME);
}

void Data::SaveData()
{
	// Check data path
	char buffer[256];

	strcpy_s(buffer, 255, DATA_PATH);

	buffer[255] = 0;

	for (char* directory = buffer; *directory; directory++)
	{
		if (*directory == '\\')
		{
			*directory = '\0';

			_mkdir(buffer);

			*directory = '\\';
		}
	}

	FILE* fp;

	if (fopen_s(&fp, DATA_FILE_PATH("Data.adat"), "w,ccs=UTF-8"))
	{
		return;
	}

	std::wstring characterHealthBarSpriteString(std::to_wstring(characterHealthBarSprite));
	std::wstring characterPositionXString(std::to_wstring(static_cast<int>(characterPosition.x)));
	std::wstring characterPositionYString(std::to_wstring(static_cast<int>(characterPosition.y)));
	std::wstring itemCount(std::to_wstring(Data::itemCount));
	std::wstring items(L" ");
	std::wstring score(std::to_wstring(Data::score));
	std::wstring enemies(L" ");

	for (unsigned short i = 0; i < Data::itemCount; i++)
	{
		items += L"," + std::to_wstring(Data::items[i]);
	}

	for (unsigned short i = 0; i < Data::enemies.size(); i++)
	{
		Vector2 position = *Data::enemies[i].GetPosition();

		enemies += L"," + std::to_wstring(position.x) + L"_" + std::to_wstring(position.y) + L"_" + std::to_wstring(Data::enemies[i].GetSpeed()) + L"_" + std::to_wstring(Data::enemies[i].GetLifeSpan());
	}

	EncryptData(&characterName);
	EncryptData(&characterHealthBarSpriteString);
	EncryptData(&characterPositionXString);
	EncryptData(&characterPositionYString);
	EncryptData(&itemCount);
	EncryptData(&items);
	EncryptData(&score);
	EncryptData(&enemies);
	fwprintf(fp, L"%s\n", characterName.c_str());
	fwprintf(fp, L"%s\n", characterHealthBarSpriteString.c_str());
	fwprintf(fp, L"%s\n", characterPositionXString.c_str());
	fwprintf(fp, L"%s\n", characterPositionYString.c_str());
	fwprintf(fp, L"%s\n", itemCount.c_str());
	fwprintf(fp, L"%s\n", items.c_str());
	fwprintf(fp, L"%s\n", score.c_str());
	fwprintf(fp, L"%s", enemies.c_str());
	fclose(fp);
}

void Data::LoadData()
{
	FILE* fp;

	if (fopen_s(&fp, DATA_FILE_PATH("Data.adat"), "r,ccs=UTF-8"))
	{
		return;
	}

	wchar_t line[256];

	// Character name
	ZeroMemory(&line, sizeof(line));

	fgetws(line, CHARACTER_MAX_NAME_LENGTH, fp);

	line[wcslen(line) - 1] = 0;

	std::wstring out(line);

	DecryptData(&out);
	SetCharacterName(out.c_str());

	// Character health bar sprite
	ZeroMemory(&line, sizeof(line));

	fgetws(line, CHARACTER_MAX_NAME_LENGTH, fp);

	line[wcslen(line) - 1] = 0;

	out = line;

	DecryptData(&out);
	SetCharacterHealthBarSprite(std::stoi(out));

	// Character position
	ZeroMemory(&line, sizeof(line));

	fgetws(line, CHARACTER_MAX_NAME_LENGTH, fp);
	
	line[wcslen(line) - 1] = 0;

	out = line;

	DecryptData(&out);

	float x = std::stoi(out);
	
	ZeroMemory(&line, sizeof(line));

	fgetws(line, CHARACTER_MAX_NAME_LENGTH, fp);

	line[wcslen(line) - 1] = 0;

	out = line;

	DecryptData(&out);

	float y = std::stoi(out);

	Vector2 position(x, y);

	SetCharacterPosition(position);

	// Items
	ZeroMemory(&line, sizeof(line));

	fgetws(line, CHARACTER_MAX_NAME_LENGTH, fp);

	line[wcslen(line) - 1] = 0;

	out = line;

	DecryptData(&out);

	unsigned short itemCount = std::stoi(out);

	ZeroMemory(&line, sizeof(line));

	fgetws(line, CHARACTER_MAX_NAME_LENGTH, fp);

	line[wcslen(line) - 1] = 0;

	if (wcslen(line) > 2)
	{
		out = line;

		DecryptData(&out);
		wcscpy_s(line, CHARACTER_MAX_NAME_LENGTH, out.c_str());

		wchar_t* context = nullptr;
		wchar_t* item = wcstok_s(line, L",", &context);
		unsigned short* items = new unsigned short[itemCount];

		for (unsigned short i = 0; i < itemCount; i++)
		{
			item = wcstok_s(nullptr, L",", &context);
			items[i] = std::stoi(item);
		}

		SetItems(itemCount, items);

		delete[] items;
	}

	// Score
	ZeroMemory(&line, sizeof(line));

	fgetws(line, CHARACTER_MAX_NAME_LENGTH, fp);

	line[wcslen(line) - 1] = 0;

	out = line;

	DecryptData(&out);

	unsigned long long score = std::stoull(out);

	SetScore(score);

	// Enemies
	ZeroMemory(&line, sizeof(line));

	fgetws(line, sizeof(line) >> 1, fp);

	if (wcslen(line) > 2)
	{
		out = line;

		DecryptData(&out);
		wcscpy_s(line, sizeof(line) >> 1, out.c_str());

		wchar_t* context = nullptr;
		wchar_t* position = wcstok_s(line, L",", &context);

		std::vector<Enemy> enemies;

		while (position = wcstok_s(nullptr, L",", &context))
		{
			wchar_t* context = nullptr;
			wchar_t* datas = wcstok_s(position, L"_", &context);
			float x = std::stod(datas);

			datas = wcstok_s(nullptr, L"_", &context);

			float y = std::stod(datas);

			datas = wcstok_s(nullptr, L"_", &context);

			float speed = std::stod(datas);

			datas = wcstok_s(nullptr, L"_", &context);

			float lifeSpan = std::stod(datas);

			enemies.push_back(Enemy(Vector2(x, y), nullptr, speed, lifeSpan));
		}

		SetEnemies(enemies);
	}

	// Close
	fclose(fp);
}

const wchar_t* Data::GetCharacterName()
{
	return characterName.c_str();
}

const int Data::GetCharacterHealthBarSprite()
{
	return characterHealthBarSprite;
}

Vector2& Data::GetCharacterPosition()
{
	return characterPosition;
}

unsigned short* Data::GetItems(unsigned short* countOut)
{
	*countOut = itemCount;

	return items;
}

unsigned long long Data::GetScore()
{
	return score;
}

std::vector<Enemy> Data::GetEnemies()
{
	return enemies;
}

void Data::SetCharacterName(const wchar_t* value)
{
	characterName = value;
}

void Data::SetCharacterHealthBarSprite(const int value)
{
	characterHealthBarSprite = value;
}

void Data::SetCharacterPosition(Vector2& value)
{
	characterPosition.Set(value);
}

void Data::SetItems(unsigned short count, unsigned short* value)
{
	itemCount = count;

	for (unsigned short i = 0; i < count; i++)
	{
		items[i] = value[i];
	}
}

void Data::SetScore(unsigned long long score)
{
	Data::score = score;
}

void Data::SetEnemies(std::vector<Enemy> value)
{
	enemies = value;
}