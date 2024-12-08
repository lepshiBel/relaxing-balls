#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>   // Для std::sqrt
#include <cstdlib> // Для rand() и srand()
#include <ctime>   // Для time()
#include <windows.h>


const int WINDOW_WIDTH = GetSystemMetrics(SM_CXSCREEN);
const int WINDOW_HEIGHT = GetSystemMetrics(SM_CYSCREEN);
const float RESTITUTION_COEFFICIENT = 0.8f; // Коэффициент восстановления

struct Ball 
{
    sf::CircleShape shape;
    sf::Vector2f velocity;

    Ball(float x, float y, float radius, sf::Color color)
    {
        shape.setRadius(radius);
        shape.setOrigin(radius, radius);
        shape.setPosition(x, y);
        shape.setFillColor(color);
        velocity = sf::Vector2f((rand() % 5 + 1) * (rand() % 2 == 0 ? 1 : -1), (rand() % 5 + 1) * (rand() % 2 == 0 ? 1 : -1));
    }

    void update()
    {
        shape.move(velocity);

        // Проверка на столкновение со стенами
        if (shape.getPosition().x + shape.getRadius() > WINDOW_WIDTH || shape.getPosition().x - shape.getRadius() < 0) 
        {
            velocity.x = -velocity.x;
        }
        if (shape.getPosition().y + shape.getRadius() > WINDOW_HEIGHT || shape.getPosition().y - shape.getRadius() < 0) 
        {
            velocity.y = -velocity.y;
        }
    }

    void draw(sf::RenderWindow& window)
    {
        window.draw(shape);
    }

    // Метод для обработки столкновения с другим шаром
    void handleCollision(Ball& other)
    {
        sf::Vector2f delta = shape.getPosition() - other.shape.getPosition();
        float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);

        // Проверка на столкновение
        if (distance < shape.getRadius() + other.shape.getRadius())
        {
            // Нормализуем вектор расстояния
            delta /= distance;

            // Обновляем скорости
            float v1 = (velocity.x * delta.x + velocity.y * delta.y);
            float v2 = (other.velocity.x * delta.x + other.velocity.y * delta.y);

            // Обновление скоростей с учетом упругого столкновения
            float newVxA = velocity.x - (v1 - v2) * RESTITUTION_COEFFICIENT * delta.x;
            float newVyA = velocity.y - (v1 - v2) * RESTITUTION_COEFFICIENT * delta.y;

            float newVxB = other.velocity.x + (v1 - v2) * RESTITUTION_COEFFICIENT * delta.x;
            float newVyB = other.velocity.y + (v1 - v2) * RESTITUTION_COEFFICIENT * delta.y;

            velocity.x = newVxA;
            velocity.y = newVyA;
            other.velocity.x = newVxB;
            other.velocity.y = newVyB;

            // Переместим шары, чтобы избежать залипания
            float overlap = (shape.getRadius() + other.shape.getRadius()) - distance;
            shape.move(delta * (overlap / 2));
            other.shape.move(-delta * (overlap / 2));
        }
    }
};

int main() {
    srand(static_cast<unsigned int>(time(0))); // Инициализация генератора случайных чисел

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Balls Simulation");

    int numberOfBalls = rand() % 14 + 2; // Генерация количества шаров

    std::vector<Ball> balls;

    for (int i = 0; i < numberOfBalls; ++i)
    {
        float radius = rand() % 30 + 20; // Случайный радиус от 20 до 50
        float x = rand() % (WINDOW_WIDTH - static_cast<int>(radius * 2)) + radius; // Случайная позиция X
        float y = rand() % (WINDOW_HEIGHT - static_cast<int>(radius * 2)) + radius; // Случайная позиция Y
        sf::Color color(rand() % 256, rand() % 256, rand() % 256); // Случайный цвет

        balls.emplace_back(x, y, radius, color); // Создание нового шара
    }

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        for (auto& ball : balls)
        {
            ball.update();
        }

        for (size_t i = 0; i < balls.size(); i++)
        {
            for (size_t j = i + 1; j < balls.size(); j++)
            {
                balls[i].handleCollision(balls[j]);
            }
        }

        window.clear();

        for (auto& ball : balls)
        {
            ball.draw(window);
        }

        window.display();

        sf::sleep(sf::milliseconds(1000 / 144)); // Ограничение FPS до 144
    }

    return 0;
}