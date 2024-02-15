#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include<iostream>
#include <fstream>
#include<cmath>
#include"Collision.h"

using namespace std;
using namespace sf;

// Pixel Perfect Collision Testing
bool coll(Sprite schar1, Sprite schar2)
{
	bool k = 0;

	if (Collision::pixelPerfectTest(schar1, schar2))
	{
		k = true;
	}
	return k;
}

bool collBox(Sprite schar1, Sprite schar2)
{
	bool k = 0;

	if (Collision::boundingBoxTest(schar1, schar2))
	{
		k = true;
	}
	return k;
}

// Hero Class
class Hero {
private:
	int heroX, heroY, health = 100;
	bool hasKey = 0, dead = 0;
	Sprite sh;
	Texture tex;
	Texture textureArray[4][4];
public:
	int i = 0, j = 0;
	Hero(string name, float x, float y) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				Texture textemp;
				textemp.loadFromFile(name, IntRect(j * 32, i * 48, 32, 48));
				this->textureArray[i][j] = textemp;
			}
		}
		Sprite s(this->textureArray[3][2]);
		heroX = x;
		heroY = y;
		s.move(heroX, heroY);
		sh = s;
	}

	Sprite* getSprite() {
		return&sh;
	}

	void setHealth(int x) {
		this->health = x;
	}

	int getHealth() {
		return this->health;
	}

	void setHasKey() {
		hasKey = 1;
	}

	bool getHasKey() {
		return hasKey;
	}

	void setIsDead() {
		this->dead = 1;
	}

	bool getIsDead() {
		return this->dead;
	}


	void setTexture() {
		Sprite s(this->textureArray[i][j]);
		s.move(sh.getPosition());
		sh = s;
	}

	void kill(Sprite* enemySprite) {
		enemySprite->move(100000, 100000);
	}
};

// Enemy Class
class enemy {
private:
	bool hasKey = 0, move=0, player=0;
	Sprite se;
	Texture tex;
	Texture textureArray[4][4];
	int i = 0, j = 0;
public:
	enemy() {};
	enemy(string name) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				Texture textemp;
				textemp.loadFromFile(name, IntRect(j * 32, i * 48, 32, 48));
				this->textureArray[i][j] = textemp;
			}
		}
		Sprite s(this->textureArray[3][2]);
		se = s;
	}

	void setData(string name, int x, int y) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				Texture textemp;
				textemp.loadFromFile(name, IntRect(j * 32, i * 48, 32, 48));
				this->textureArray[i][j] = textemp;
			}
		}
		Sprite s(this->textureArray[3][2]);
		s.move(x, y);
		se = s;
	}

	Sprite* getSprite() {
		return &se;
	}

	void attack(Hero* h, int* time, Sound* punch, Sprite* punchSp1, Sprite* punchSp2)
	{
		if (*time >= 30) {
			punch->play();
			if (!this->player) {
				punchSp1->setPosition(h->getSprite()->getPosition().x - 15, h->getSprite()->getPosition().y - 5);
			}
			else {
				punchSp2->setPosition(h->getSprite()->getPosition().x - 15, h->getSprite()->getPosition().y - 5);
			}
			if (h->getHealth() != 0) {
				h->setHealth(h->getHealth() - 10);
				*time = 0;
			}
			if (h->getHealth() == 0) {
				h->setIsDead();
				h->getSprite()->move(1000000, 1000000);
			}
		}
	}

	void setHasKey() {
		this->hasKey = 1;
	}

	bool getHasKey() {
		return this->hasKey;
	}

	void setMove(bool player) {
		this->move = 1;
		this->player = player;
	}

	bool* getMove() {
		bool arr[2] = { this->move, this->player };
		return arr;
	}

	void setTexture() {
		Sprite s(this->textureArray[i][j]);
		s.move(se.getPosition());
		se = s;
	}

	void moveToHero(Sprite* hero, int* time) {
		Vector2f pos1 = hero->getPosition();
		pos1.x += 16;
		pos1.y += 24;
		Vector2f pos2 = se.getPosition();
		pos2.x += 16;
		pos2.y += 24;
		double distX = (double)pos1.x - pos2.x;
		double distY = (double)pos1.y - pos2.y;
		double hypot = sqrt((distX * distX) + (distY * distY));
		double sineAlpha = distX / hypot;
		double cosineAlpha = distY / hypot;
		double movementX = 2 * sineAlpha;
		double movementY = 2 * cosineAlpha;

		if (!coll(*hero, this->se)) {
			if (abs(movementY) > abs(movementX) && movementY > 0) {
				i = 0;
			}
			if (abs(movementY) > abs(movementX) && movementY < 0) {
				i = 3;
			}
			if (abs(movementY) < abs(movementX) && movementX > 0) {
				i = 2;
			}
			if (abs(movementY) < abs(movementX) && movementX < 0) {
				i = 1;
			}

			j++;
			j == 4 ? j = 0 : NULL;

			if (*time > 10) {
				setTexture();
			}

			se.move(movementX, movementY);
		}
	}
};


int main() {

	ifstream openfile("main.txt");
	Texture texture;
	Sprite sp;
	int map[15][20] = { 0 };
	if (openfile.is_open()) {
		int x = 0, y = 0;
		while (!openfile.eof()) {
			int c1;
			openfile >> c1;
			map[x][y] = c1;
			if (y == 20) {
				x++;
				y = 1;
			}
			else {
				y++;
			}
		}

	}

	//Window rendering
	RenderWindow window(VideoMode(800, 600), "Escape The Dungeon");
	window.setFramerateLimit(60);

	////The game icon 
	//Image myicon;
	//myicon.loadFromFile("gameicon2.png");
	//window.setIcon(51, 50, myicon.getPixelsPtr());
	 
	//the bar
	RectangleShape endScreen(Vector2f(800, 600));
	endScreen.setFillColor(Color(0, 0, 0));

	Texture texture1, texture2, texture3, texture4, texture5, texture6, texture7;
	texture1.loadFromFile("images/character1.png");
	texture2.loadFromFile("images/character2.png");
	texture3.loadFromFile("images/bar1.png");
	texture4.loadFromFile("images/bar2.png");
	texture5.loadFromFile("images/gamekey.png");
	texture7.loadFromFile("images/Pow.png");
	Sprite sprite, sprite2, sprite3, sprite4, sprite5, sprite6, sprite7, sprite8;

	sprite.setTexture(texture1);
	sprite2.setTexture(texture2);
	sprite3.setTexture(texture3);
	sprite4.setTexture(texture4);
	sprite5.setTexture(texture5);
	sprite6.setTexture(texture5);
	sprite7.setTexture(texture7);
	sprite8.setTexture(texture7);

	sprite.setPosition(Vector2f(-20, 0));
	sprite2.setPosition(Vector2f(-20, 50));
	sprite3.setPosition(Vector2f(35, 40));
	sprite4.setPosition(Vector2f(35, 90));
	sprite5.setPosition(Vector2f(160, 25));
	sprite6.setPosition(Vector2f(160, 75));

	//Fonts & Text
	Font font;
	font.loadFromFile("font.ttf");
	Text text;
	text.setFont(font);
	text.setString("100%");
	text.setCharacterSize(20);
	text.setFillColor(Color::Green);
	text.setPosition(Vector2f(85, 38));
	Text text2;
	text2.setFont(font);
	text2.setString("100%");
	text2.setCharacterSize(20);
	text2.setFillColor(Color::Green);
	text2.setPosition(Vector2f(85, 88));
	Text text3;
	text3.setFont(font);
	text3.setString("Score:");
	text3.setCharacterSize(20);
	text3.setFillColor(Color::Red);
	text3.setPosition(Vector2f(700, 50));
	Text winOrLoseText;
	winOrLoseText.setFont(font);
	winOrLoseText.setCharacterSize(40);
	winOrLoseText.setFillColor(Color(255, 0, 0));
	winOrLoseText.setPosition(300, 220);
	Text eventText;
	eventText.setFont(font);
	eventText.setCharacterSize(40);
	eventText.setFillColor(Color(255, 0, 0));
	eventText.setPosition(170, 270);

	//main sound effect
	SoundBuffer buffer;
	buffer.loadFromFile("mainsoundeffect.wav");
	Sound sound;
	sound.setBuffer(buffer);
	sound.setVolume(10);
	sound.play();

	// sword sound effect
	SoundBuffer buffer2;
	buffer2.loadFromFile("sword.wav");
	Sound sound2;
	sound2.setBuffer(buffer2);
	sound2.setVolume(50);

	// win sound when getting the key
	SoundBuffer buffer3;
	buffer3.loadFromFile("tookthekey.wav");
	Sound sound3;
	sound3.setBuffer(buffer3);
	sound.setVolume(60);

	// Enemy hits
	SoundBuffer buffer4;
	buffer4.loadFromFile("enemyhits.wav");
	Sound sound4;
	sound4.setBuffer(buffer4);
	sound.setVolume(70);

	//Hero objects Initialization
	Texture hero2Texture;
	Texture hero1Texture;
	Texture enemyTexture;
	string textureName1 = "images/58.png", textureName2 = "images/83.png", textureName3 = "images/86.png";
	string enemyTexturesNames[5] = { "images/24.png","images/59.png", "images/66.png", "images/82.png", "images/86.png" };
	Hero sp1(textureName1, 400, 500);
	Hero sp2(textureName2, 300, 500);

	//Enemy objects Initialization
	enemy enemies[10];
	srand(time(0));
	int random1, random2;
	random1 = rand() % 10;
	random2 = rand() % 10;
	RectangleShape zones[11];
	string str = "images/walls/transparent.png";
	for (int i = 0; i < 10; i++) {
		RectangleShape tempRect(Vector2f(120, 148));
		int randomXLocation = ((rand() % 18) * 40) + 40;
		int randomYLocation = ((rand() % 13) * 40) + 40;
		enemies[i].setData(enemyTexturesNames[rand() % 5], randomXLocation, randomYLocation);
		if (i == random1) {
			enemies[i].setHasKey();
		}
		if (i == random2) {
			enemies[i].setHasKey();
		}
		tempRect.setPosition(randomXLocation - 45, randomYLocation - 55);
		zones[i] = tempRect;
	}

	//Global Variables
	bool upkey = 0, downkey = 0, leftkey = 0, rightkey = 0, w = 0, a = 0, s = 0, d = 0, rightShift = 0, leftShift = 0;
	int score = 0, xAxis = 0, yAxis = 0, xAxis2 = 0, yAxis2 = 0, time = 0, timeAttack = 0, timeAttackHero1 = 0, timeAnimation1 = 0, timeAnimation2 = 0, timeAttackHero2 = 0, choiceArray[10];
	Texture t1, t2, t3, t4;
	Sprite s1, s2, s3, s4;

	for (int i = 0; i < 10; i++) {
		choiceArray[i] = rand() % 2;
	}

	//Main Game Loop
	while (window.isOpen()) {
		Event e;
		while (window.pollEvent(e)) {
			if (e.type == Event::Closed) {
				window.close();
			}
			if (e.type == Event::KeyPressed) {
				//Hero 1//
				if (e.key.code == Keyboard::RShift) { rightShift = true; }
				if (e.key.code == Keyboard::Up) { upkey = true; }
				if (e.key.code == Keyboard::Down) { downkey = true; }
				if (e.key.code == Keyboard::Left) { leftkey = true; sp1.i = 1; }
				if (e.key.code == Keyboard::Right) { rightkey = true; sp1.i = 2; }

				//Hero 2//
				if (e.key.code == Keyboard::LShift) { leftShift = true; }
				if (e.key.code == Keyboard::W) { w = true; }
				if (e.key.code == Keyboard::S) { s = true; }
				if (e.key.code == Keyboard::A) { a = true; }
				if (e.key.code == Keyboard::D) { d = true; }

			}

			if (e.type == Event::KeyReleased) {
				//Hero 1//
				if (e.key.code == Keyboard::RShift) { rightShift = false; }
				if (e.key.code == Keyboard::Up) { upkey = false; }
				if (e.key.code == Keyboard::Down) { downkey = false; }
				if (e.key.code == Keyboard::Left) { leftkey = false; }
				if (e.key.code == Keyboard::Right) { rightkey = false; }

				//Hero 2//
				if (e.key.code == Keyboard::LShift) { leftShift = false; }
				if (e.key.code == Keyboard::W) { w = false; }
				if (e.key.code == Keyboard::S) { s = false; }
				if (e.key.code == Keyboard::A) { a = false; }
				if (e.key.code == Keyboard::D) { d = false; }

			}
		}

		///////////////// Hero 1 Movement /////////////////////
		if (upkey == true) {
			yAxis = -3;
			sp1.i = 3;
			sp1.j++;
			sp1.j == 4 ? sp1.j = 0 : NULL;
		}
		if (downkey == true) {
			yAxis = 3;
			sp1.i = 0;
			sp1.j++;
			sp1.j == 4 ? sp1.j = 0 : NULL;
		};
		if (leftkey == true) {
			xAxis = -3;
			sp1.i = 1;
			sp1.j++;
			sp1.j == 4 ? sp1.j = 0 : NULL;
		};
		if (rightkey == true) {
			xAxis = 3;
			sp1.i = 2;
			sp1.j++;
			sp1.j == 4 ? sp1.j = 0 : NULL;
		};

		if (rightkey == true && leftkey == true)xAxis = 0;
		if (rightkey == false && leftkey == false)xAxis = 0;
		if (upkey == true && downkey == true)yAxis = 0;
		if (upkey == false && downkey == false)yAxis = 0;

		//////////////// Hero 2 Movement //////////////////////
		if (w == true) {
			yAxis2 = -3;
			sp2.i = 3;
			sp2.j++;
			sp2.j == 4 ? sp2.j = 0 : NULL;
		}
		if (s == true) {
			yAxis2 = 3;
			sp2.i = 0;
			sp2.j++;
			sp2.j == 4 ? sp2.j = 0 : NULL;
		}
		if (a == true) {
			xAxis2 = -3;
			sp2.i = 1;
			sp2.j++;
			sp2.j == 4 ? sp2.j = 0 : NULL;
		}
		if (d == true) {
			xAxis2 = 3;
			sp2.i = 2;
			sp2.j++;
			sp2.j == 4 ? sp2.j = 0 : NULL;
		}

		if (d == true && a == true)xAxis2 = 0;
		if (d == false && a == false)xAxis2 = 0;
		if (w == true && s == true)yAxis2 = 0;
		if (w == false && s == false)yAxis2 = 0;
		if (time > 10) {
			sp1.setTexture();
			sp2.setTexture();
			time = 0;
		}
		time++;

		int xh1 = sp1.getSprite()->getPosition().x, yh1 = sp1.getSprite()->getPosition().y;
		int xh2 = sp2.getSprite()->getPosition().x, yh2 = sp2.getSprite()->getPosition().y;

		if (xh1 > 40 && xh1 < 730 && yh1 > 40 && yh1 < 511)
			sp1.getSprite()->move(xAxis, yAxis);
		else
		{
			if (yh1 <= 40)
			{
				yAxis += 3;
			}
			else if (yh1 >= 511)
			{
				yAxis -= 3;
			}
			else if (xh1 <= 40)
			{
				xAxis += 3;
			}
			else if (xh1 >= 730)
			{
				xAxis -= 3;
			}
			sp1.getSprite()->move(xAxis, yAxis);
		}
		if (xh2 > 40 && xh2 < 730 && yh2 > 40 && yh2 < 511) {
			sp2.getSprite()->move(xAxis2, yAxis2);
		}
		else
		{
			if (yh2 <= 40)
			{
				sp2.getSprite()->move(xAxis2, yAxis2+3);
			}
			else if (yh2 >= 511)
			{
				sp2.getSprite()->move(xAxis2, yAxis2-3);
			}
			else if (xh2 <= 40)
			{
				sp2.getSprite()->move(xAxis2+3, yAxis2);
			}
			else if (xh2 >= 730)
			{
				sp2.getSprite()->move(xAxis2-3, yAxis2);
			}
		}


		window.clear(Color::Black);

		Collision::createTextureAndBitmask(t3, "images/walls/doorClosed.png");
		s3.setTexture(t3);
		Collision::createTextureAndBitmask(t1, "images/walls/wall2up.png");
		s1.setTexture(t1);
		Collision::createTextureAndBitmask(t2, "images/floor.jpg");
		s2.setTexture(t2);

		for (int j = 0; j < 15; j++) {
			for (int i = 0; i < 20; i++) {
				if (map[j][i] == 4) {
					s1.setPosition(i * 40, j * 40);
					window.draw(s1);
				}
				else if (map[j][i] == 1) {
					s2.setPosition(i * 40, j * 40);
					window.draw(s2);
				}
				else if (map[j][i] == 2) {
					s3.setPosition(i * 40, j * 40);
					window.draw(s1);
					window.draw(s3);
				}
				else {
					// enemy
					s4.setPosition(i * 40, j * 40);
					window.draw(s2);
				}

			}
		}

		window.draw(*sp1.getSprite());
		window.draw(*sp2.getSprite());

		for (int i = 0; i < 10; i++)
		{
			if (rightShift && coll(*sp1.getSprite(), *enemies[i].getSprite()) && timeAttackHero1 > 30) {
				if (enemies[i].getHasKey()) {
					sp1.setHasKey();
					sound3.play();
				}
				sp1.kill(enemies[i].getSprite());
				sound2.play();
				score += 34;
				timeAttackHero1 = 0;
			}
			if (leftShift && coll(*sp2.getSprite(), *enemies[i].getSprite()) && timeAttackHero2 > 30) {
				if (enemies[i].getHasKey()) {
					sp2.setHasKey();
					sound3.play();
				}
				sp2.kill(enemies[i].getSprite());
				sound2.play();
				score += 34;
				timeAttackHero2 = 0;
			}
		}
		timeAttackHero1++;
		timeAttackHero2++;

		for (int i = 0; i < 10; i++)
		{
			if (coll(*sp1.getSprite(), *enemies[i].getSprite())) {
				enemies[i].attack(&sp1, &timeAttack, &sound4, &sprite7, &sprite8);
			}

			if (coll(*sp2.getSprite(), *enemies[i].getSprite())) {
				enemies[i].attack(&sp2, &timeAttack, &sound4, &sprite7, &sprite8);
			}
		}

		timeAttack++;

		for (int i = 0; i < 10; i++) {
			if (sp1.getSprite()->getGlobalBounds().intersects(zones[i].getGlobalBounds())) {
				enemies[i].setMove(0);
			}
			else if (sp2.getSprite()->getGlobalBounds().intersects(zones[i].getGlobalBounds())) {
				enemies[i].setMove(1);
			}
			if (enemies[i].getMove()[0] && enemies[i].getMove()[1]) {
				if (!sp2.getIsDead()) {
					enemies[i].moveToHero(sp2.getSprite(), &time);
				}
				else {
					if (!sp1.getIsDead()) {
						enemies[i].moveToHero(sp1.getSprite(), &time);
					}
				}

			}
			if (enemies[i].getMove()[0] && !enemies[i].getMove()[1]) {
				if (!sp1.getIsDead()) {
					enemies[i].moveToHero(sp1.getSprite(), &time);
				}
				else {
					if (!sp2.getIsDead()) {
						enemies[i].moveToHero(sp2.getSprite(), &time);
					}
				}
			}
		}

		if (timeAnimation1 >= 25) {
			sprite7.move(100000000, 100000000);
			timeAnimation1 = 0;
		}

		if (timeAnimation2 >= 25) {
			sprite8.move(100000000, 100000000);
			timeAnimation2 = 0;
		}

		timeAnimation1++;
		timeAnimation2++;

		for (int i = 0; i < 10; i++) {
			window.draw(*enemies[i].getSprite());
		}

		text.setString(to_string(sp1.getHealth()) + "%");
		text2.setString(to_string(sp2.getHealth()) + "%");
		window.draw(sprite);
		window.draw(sprite2);
		window.draw(sprite3);
		window.draw(sprite4);
		window.draw(text);
		window.draw(text2);
		if (sp1.getHasKey()) {
			window.draw(sprite5);
		}
		if (sp2.getHasKey()) {
			window.draw(sprite6);
		}
		text3.setString("Score: " + to_string(score));
		window.draw(sprite7);
		window.draw(sprite8);
		window.draw(text3);
		if (sp1.getHasKey() && sp2.getHasKey()) {
			sound.pause();
			sound2.pause();
			sound3.pause();
			sound4.pause();
			window.draw(endScreen);
			winOrLoseText.setString("You Win!");
			eventText.setString("You both escaped the dungeon!");
			window.draw(winOrLoseText);
			window.draw(eventText);
		}
		if (sp1.getIsDead() || sp2.getIsDead()) {
			sound.pause();
			sound2.pause();
			sound3.pause();
			sound4.pause();
			window.draw(endScreen);
			winOrLoseText.setString("Game Over!");
			eventText.setString("One of you died & couldnt't escape!");
			window.draw(winOrLoseText);
			window.draw(eventText);
		}
		window.display();
	}
	return 0;
}