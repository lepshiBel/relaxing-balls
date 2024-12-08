#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <cstdlib> // Для rand() и srand()
#include <ctime>   // Для time()
#include <windows.h>


const int WINDOW_WIDTH = GetSystemMetrics(SM_CXSCREEN);
const int WINDOW_HEIGHT = GetSystemMetrics(SM_CYSCREEN);
const float BALL_RADIUS = 50.f;

struct Ball 
{
	sf::CircleShape shape;
	sf::Vector2f velocity;

	Ball(float x, float y, float vx, float vy, sf::Color color) 
	{
		shape.setRadius(BALL_RADIUS);
		shape.setOrigin(BALL_RADIUS, BALL_RADIUS);
		shape.setPosition(x, y);
		shape.setFillColor(color);
		velocity = sf::Vector2f(vx, vy);
	}

	void update() 
	{
		shape.move(velocity);

		// Проверка на столкновение со стенами
		if (shape.getPosition().x + BALL_RADIUS > WINDOW_WIDTH || shape.getPosition().x - BALL_RADIUS < 0) 
		{
			velocity.x = -velocity.x; // Отражение по оси X
		}
		if (shape.getPosition().y + BALL_RADIUS > WINDOW_HEIGHT || shape.getPosition().y - BALL_RADIUS < 0) 
		{
			velocity.y = -velocity.y; // Отражение по оси Y
		}
	}

	void draw(sf::RenderWindow& window) 
	{
		window.draw(shape);
	}
};

void handleCollision(Ball& ball1, Ball& ball2) 
{
	sf::Vector2f delta = ball1.shape.getPosition() - ball2.shape.getPosition();
	float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);

	if (distance < 2 * BALL_RADIUS) 
	{
		// Нормализуем вектор расстояния
		delta /= distance;

		// Обновляем скорости
		float v1 = (ball1.velocity.x * delta.x + ball1.velocity.y * delta.y);
		float v2 = (ball2.velocity.x * delta.x + ball2.velocity.y * delta.y);

		ball1.velocity -= v1 * delta;
		ball2.velocity -= v2 * delta;

		// Переместим шары, чтобы избежать залипания
		float overlap = 2 * BALL_RADIUS - distance;
		ball1.shape.move(delta * (overlap / 2));
		ball2.shape.move(-delta * (overlap / 2));
	}
}

int main() 
{
	srand(static_cast<unsigned int>(time(0))); // Инициализация генератора случайных чисел

	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Balls Simulation");

	Ball ball1(WINDOW_WIDTH / 4, WINDOW_HEIGHT / 2, rand() % 10 + 1, rand() % 10 + 1, sf::Color(255, 0, 255));
	Ball ball2(WINDOW_WIDTH / 4 * 3, WINDOW_HEIGHT / 2, rand() % 10 + 1, rand() % 10 + 1, sf::Color(255, 255, 0));

	while (window.isOpen()) 
	{
		sf::Event event;
		while (window.pollEvent(event)) 
		{
			if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) 
			{
				window.close();
			}
		}

		ball1.update();
		ball2.update();

		handleCollision(ball1, ball2);

		window.clear();

		ball1.draw(window);
		ball2.draw(window);

		window.display();

		sf::sleep(sf::milliseconds(1000 / 144)); // Ограничение FPS до 144
	}

	return 0;
}