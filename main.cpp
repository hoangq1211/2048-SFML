#include <bits/stdc++.h>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <deque>
#include <stdlib.h>
#include <iostream>
#include <fstream>

using namespace std;
int  score = 0;
int highScore;

int  Tile[4][4];

sf::Font font;
sf::Color windowBGColour(250, 248, 239, 255);
sf::Color gridBGColour(187, 173, 160, 255);
sf::Color textColour(67, 53, 40, 255);
map<int, sf::Color> numberColours;

float WindowWidth = 400.f;
float WindowHeight = 500.f;
float padding = 10.f;
float gridSize = (400.f - (padding * 2.f));
float tileSize = (gridSize - (5 * padding)) / 4;


bool showGameOver = false;
bool showYouWin = false;
bool Paused = false;

string filePath = "C:/Users/hoang/Desktop/2048/data/HighScore.txt";

// Load high score
void LoadHighScore(string file)
{
	ifstream fileInput(file);

	if (fileInput.fail())
	{
		cout << "Cannot open file at " << file << std::endl;
		return;
	}

	if (!fileInput.eof())
	{
		char line[255];
		fileInput.getline(line, 255);
		highScore = atoi(line);
		cout << highScore;
	}

}

//Save high Score
void SaveHighScore(string file)
{
	ofstream fileOutput(file);

	if (fileOutput.fail())
	{
		cout << "Cannot open file at " << file << endl;
		return;
	}

	fileOutput << highScore;
}

// Tim cac Tile trong
vector<sf::Vector2i> getEmptyTiles()
{
	vector<sf::Vector2i> empties;
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			if (Tile[x][y] == 0) {
				empties.push_back(sf::Vector2i(x, y));
			}
		}
	}
	return empties;
}

// Them Tile 2/4 vao cac Tile trong ngau nhien
void addNumber() {
	int newCellOptions[2] = { 2, 4 };
	vector<sf::Vector2i> empties = getEmptyTiles();
	if (empties.size() == 0) {
		return; // Khong con Tile trong, het ham.
	}
	else {
		int randEmpty = rand() % empties.size();
		Tile[empties.at(randEmpty).x][empties.at(randEmpty).y] = newCellOptions[rand() % 2];
	}
}

// Kiem tra xem co the di chuyen duoc cac Tile hay khong
bool CanMove() {

	int MovesLeft = 0;
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			if (Tile[x][y] > 0) {
				// Neu day la 1 tile co gia tri, xem xung quanh tile do co o trong hoac tile co cung gia tri hay khong, co thi van di chuyen duoc
				if (x + 1 < 4 && (Tile[x][y] == Tile[x + 1][y] || Tile[x + 1][y] == 0)) {
					MovesLeft++;
				}
				if (y + 1 < 4 && (Tile[x][y] == Tile[x][y + 1] || Tile[x][y + 1] == 0)) {
					MovesLeft++;
				}
			}
		}
	}
	return MovesLeft>0;
}

// Kiem tra dieu kien ket thuc game
bool isGameOver()
{
	if (getEmptyTiles().size() > 0 || CanMove()) {
		return false;
	}
	else {
		return true;
	}
}
bool areYouWin()
{
	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
			if (Tile[x][y] == 2048)
				return true;
	}
	return false;

}

// Reset game
void Reset()
{
	Paused = false;
	// Reset diem
	score = 0;

	// Reset lai cac Tile
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			Tile[x][y] = 0;
		}
	}

	// Tat bang game over / you win
	showGameOver = false;
	showYouWin = false;

	// Them 2 so ngau nhien khi bat dau game
	addNumber();
	addNumber();
}


// Gop cac o co cung so trong 1 hang/cot lien ke nhau.
void Combine(deque<int>& TileList)
{
	if (TileList.size() <= 1) {
		return;
	}
	int index = 0;
	while (index < TileList.size() - 1)
	{
		int cell1 = TileList.at(index);
		int cell2 = TileList.at(index + 1);

		if (cell1 == cell2) {
			TileList.at(index) = cell1 * 2;
			score += TileList.at(index);
			TileList.erase(TileList.begin() + index + 1);
		}

		index++;

	}


}


// Di chuyen cac Tile va kiem tra xem cac Tile da di chuyen chua
void MoveDown(bool &tilesMoved)
{

	tilesMoved = false;
	for (int x = 0; x < 4; x++)
	{
		deque<int> cellList;
		for (int y = 3; y >= 0; y--)
		{
			if (Tile[x][y] != 0)
			{
				cellList.push_back(Tile[x][y]);
			}
		}
		Combine(cellList);

		int tileBefore;
		for (int y = 3; y >= 0; y--)
		{
			tileBefore = Tile[x][y];

			if (cellList.size() > 0)
			{
				Tile[x][y] = cellList.front();
				cellList.pop_front();
			}
			else {
				Tile[x][y] = 0;
			}

			if (Tile[x][y] != tileBefore)
			{
				tilesMoved = true;
			}
		}
	}

}

void MoveUp(bool &tilesMoved)
{

	tilesMoved = false;
	for (int x = 0; x < 4; x++)
	{
		deque<int> TileList;
		for (int y = 0; y < 4; y++)
		{
			if (Tile[x][y] != 0)
			{
				TileList.push_back(Tile[x][y]);
			}
		}
		Combine(TileList);

		int  tileBefore;
		for (int y = 0; y < 4; y++)
		{
			tileBefore = Tile[x][y];

			if (TileList.size() > 0)
			{
				Tile[x][y] = TileList.front();
				TileList.pop_front();
			}
			else
			{
				Tile[x][y] = 0;
			}

			if (Tile[x][y] != tileBefore)
			{
				tilesMoved = true;
			}
		}
	}
}


void MoveLeft(bool &tilesMoved)
{
	tilesMoved = false;

	for (int y = 0; y < 4; y++)
	{
		deque<int> TileList;
		for (int x = 0; x < 4; x++)
		{
			if (Tile[x][y] != 0)
			{
				TileList.push_back(Tile[x][y]);
			}
		}
		Combine(TileList);

		int  tileBefore;
		for (int x = 0; x < 4; x++)
		{
			tileBefore = Tile[x][y];

			if (TileList.size() > 0)
			{
				Tile[x][y] = TileList.front();
				TileList.pop_front();
			}
			else
			{
				Tile[x][y] = 0;
			}

			if (Tile[x][y] != tileBefore)
			{
				tilesMoved = true;
			}
		}
	}
}

void MoveRight(bool &tilesMoved)
{
	tilesMoved = false;
	for (int y = 0; y < 4; y++)
	{
		deque<int> cellList;
		for (int x = 3; x >= 0; x--)
		{
			if (Tile[x][y] != 0)
			{
				cellList.push_back(Tile[x][y]);
			}
		}
		Combine(cellList);

		int  tileBefore;
		for (int x = 3; x >= 0; x--)
		{
			tileBefore = Tile[x][y];

			if (cellList.size() > 0)
			{
				Tile[x][y] = cellList.front();
				cellList.pop_front();
			}
			else
			{
				Tile[x][y] = 0;
			}

			if (Tile[x][y] != tileBefore)
			{
				tilesMoved = true;;
			}
		}
	}
}

int main() {
	// Giup random cac cap so moi ngau nhien thay vi 1 cap so nhat dinh
	srand(time(NULL));

    //Load nhac nen
	sf::Music soundTrack;
    soundTrack.openFromFile("Spectre.wav");
    soundTrack.setLoop(true);
    soundTrack.setVolume(2);
    soundTrack.play();

	// Load high score tu file tao ra
	LoadHighScore(filePath);

	//Khoi tao cac file am thanh
	sf::SoundBuffer buffer1, buffer2, buffer3;
	buffer1.loadFromFile("data/sound/pop.flac");
	buffer2.loadFromFile("data/sound/blbl.flac");
	buffer3.loadFromFile("data/sound/win_sound.flac");
	sf::Sound addScoreSound, GameOverSound, WinSound;
	addScoreSound.setBuffer(buffer1);
	GameOverSound.setBuffer(buffer2);
	WinSound.setBuffer(buffer3);


	// Tai font, khong tim thay font thi bao loi
	if (!font.loadFromFile("ClearSans-Bold.ttf")) {
		printf("Font fail\r\n");
		return 1;
	}

	// Mau cua Tile
	numberColours[0] = sf::Color(204, 192, 179, 255);
	numberColours[2] = sf::Color(238, 228, 218, 255);
	numberColours[4] = sf::Color(237, 224, 200, 255);
	numberColours[8] = sf::Color(242, 177, 121, 255);
	numberColours[16] = sf::Color(245, 149, 99, 255);
	numberColours[32] = sf::Color(246, 124, 95, 255);
	numberColours[64] = sf::Color(246, 94, 59, 255);
	numberColours[128] = sf::Color(237, 207, 114, 255);
	numberColours[256] = sf::Color(237, 204, 97, 255);
	numberColours[512] = sf::Color(237, 200, 80, 255);
	numberColours[1024] = sf::Color(237, 197, 63, 255);
	numberColours[2048] = sf::Color(237, 194, 46, 255);

	// Tao cua so window
	sf::RenderWindow window(sf::VideoMode(WindowWidth, WindowHeight), "2048", sf::Style::Close | sf::Style::Titlebar);

	// Thiet lap nen
	sf::RectangleShape gridBackgroundRect(sf::Vector2f(gridSize, gridSize)); // Khoi tao nen
	gridBackgroundRect.setFillColor(gridBGColour);
	gridBackgroundRect.setPosition(sf::Vector2f(10, 110));

	// Game Over / Win layer
	sf::RectangleShape BlurBackground(sf::Vector2f(gridSize, gridSize));
	sf::Text gameOverText("Game Over!", font, 50);
	BlurBackground.setFillColor(sf::Color(250, 248, 239, 175));
	BlurBackground.setPosition(sf::Vector2f(10, 110));
	gameOverText.setStyle(sf::Text::Bold);
	gameOverText.setFillColor(textColour);
	gameOverText.setPosition(sf::Vector2f(60, 265));
	sf::Text WinText("You Win!", font, 50);
	WinText.setStyle(sf::Text::Bold);
	WinText.setFillColor(textColour);
	WinText.setPosition(sf::Vector2f(103, 265));

		// Tao bang diem
	sf::Text scoreText(to_string(score), font, 15);
	scoreText.setStyle(sf::Text::Regular);
	scoreText.setFillColor(textColour);
	sf::Text scoreDecoratorText("", font, 15);
	scoreDecoratorText.setFillColor(textColour);

	// Tao nut New Game
	sf::Text NewGameText("Restart", font, 20);
	NewGameText.setStyle(sf::Text::Regular);
	NewGameText.setFillColor(textColour);
	NewGameText.setPosition(sf::Vector2f(300,60));
	sf::RectangleShape NewGameObject(sf::Vector2f(tileSize + 20, 35));
	NewGameObject.setFillColor(sf::Color(255, 162, 0, 170));
	NewGameObject.setPosition(sf::Vector2f(284, 55));


	// Tao title game
	sf::Text titleText("2048!", font, 48);
	titleText.setStyle(sf::Text::Bold);
	titleText.setFillColor(textColour);
	titleText.setPosition(sf::Vector2f(140, 0));



	// Thiet lap cho cac Tile
	sf::RectangleShape gridTile(sf::Vector2f(tileSize, tileSize));
	sf::Text tileText;
	tileText.setFont(font);
	tileText.setCharacterSize(32);
	tileText.setStyle(sf::Text::Bold);
	tileText.setFillColor(textColour);


	// Bat dau game moi
	Reset();

	// Tao do tre khi them cac Tile ngau nhien
	bool MoveDone = false;
	bool ScheduledNumberAdd = false;
	sf::Clock clock;
	clock.restart();
	sf::Time addTimeout = sf::milliseconds(60);

	// Game loop
	while (window.isOpen())
	{
		sf::Event event;

		while (window.pollEvent(event))
		{
			// Tao nut Reset game
			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					if (event.mouseButton.x >= NewGameObject.getPosition().x
						&& event.mouseButton.x <= NewGameObject.getPosition().x + NewGameObject.getSize().x
						&& event.mouseButton.y >= NewGameObject.getPosition().y
						&& event.mouseButton.y <= NewGameObject.getPosition().y + NewGameObject.getSize().y)
						Reset();

				}
			}

			if (event.type == sf::Event::Closed)
			{
				window.close();
			}

			int tempScore = score;
			if (!Paused)
			{
				if (event.type == sf::Event::KeyPressed && !MoveDone && !ScheduledNumberAdd)
				{
					bool tilesMoved = false;
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
					{
						MoveUp(tilesMoved);
					}
					else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
					{
						MoveRight(tilesMoved);
					}
					else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
					{
						MoveDown(tilesMoved);
					}
					else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
					{
						MoveLeft(tilesMoved);
					}


					MoveDone = true;

					if (score > tempScore) {
						addScoreSound.play();
					}
					if (score > highScore) {
						highScore = score;
					}
					if (tilesMoved > 0) {
						ScheduledNumberAdd = true;
						clock.restart();
					}
				}
			}
			//Khi di chuyen xong, kiem tra xem da thang/thua hay chua
			if (event.type == sf::Event::KeyReleased && MoveDone)
			{
				if (isGameOver())
				{
					GameOverSound.play();
					showGameOver = true;

				}
				if (areYouWin())
				{
					WinSound.play();
					showYouWin = true;
				}

				MoveDone = false;
			}

		}

		if (ScheduledNumberAdd && clock.getElapsedTime() > addTimeout)
		{
			addNumber();
			ScheduledNumberAdd = false;
		}
		window.clear(windowBGColour);
		window.draw(gridBackgroundRect);

		// render cac Tile
		for (int x = 0; x < 4; x++)
			{
			for (int y = 0; y < 4; y++)
			{
					gridTile.setPosition(sf::Vector2f(20 + (x * (tileSize + padding)), 120 + (y * (tileSize + padding))));
					gridTile.setFillColor(numberColours.find(Tile[x][y])->second);
					window.draw(gridTile);

					if (Tile[x][y] > 0)
					{
						tileText.setString(to_string(Tile[x][y]));
						sf::Vector2f textPosition = (gridTile.getPosition() + (gridTile.getSize() / 2.f));
						textPosition.x -= tileText.getGlobalBounds().width / 2.f;
						textPosition.y -= (tileText.getGlobalBounds().height + padding) / 2.f;
						tileText.setPosition(textPosition);
						if (Tile[x][y] > 1000)
						{
							tileText.setCharacterSize(25);
						}

						window.draw(tileText);
					}
			}
		}
		// render bang diem
		scoreDecoratorText.setString("highscore: " + to_string(highScore));
		scoreDecoratorText.setPosition(sf::Vector2f(26, 72));
		window.draw(scoreDecoratorText);
		scoreDecoratorText.setString("score:        " + to_string(score));
		scoreDecoratorText.setPosition(sf::Vector2f(26 , 57));
		window.draw(scoreDecoratorText);

		// render game over
		if (showGameOver)
		{
			Paused = true;
			window.draw(BlurBackground);
			window.draw(gameOverText);
			scoreDecoratorText.setString("Hit enter to restart");
			scoreDecoratorText.setPosition(sf::Vector2f(gameOverText.getPosition().x + 75, gameOverText.getPosition().y + gameOverText.getLocalBounds().height + padding * 2));
			window.draw(scoreDecoratorText);

			// Bam enter de reset game
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
			{
				Reset();

			}
		}
		// render You Win
		if (showYouWin)
		{
			Paused = true;
			window.draw(BlurBackground);
			window.draw(WinText);
			scoreDecoratorText.setString("Hit enter to restart");
			scoreDecoratorText.setPosition(sf::Vector2f(gameOverText.getPosition().x + 75, gameOverText.getPosition().y + gameOverText.getLocalBounds().height + padding * 2));
						window.draw(scoreDecoratorText);

			// Bam enter de reset game
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
			{
				Reset();
			}
		}
		window.draw(titleText);
		window.draw(NewGameObject);
		window.draw(NewGameText);
		window.display();


	}
	// Luu lai diem cao truoc khi thoat cua so game
	SaveHighScore(filePath);
	return 0;
}

