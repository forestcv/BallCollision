#include "SFML/Graphics.hpp"
#include "MiddleAverageFilter.h"
#include "ball.h"
#include "balls_controller.h"

constexpr int WINDOW_X = 1024;
constexpr int WINDOW_Y = 768;
constexpr int MAX_BALLS = 300;
constexpr int MIN_BALLS = MAX_BALLS - 1;

Math::MiddleAverageFilter<float,100> fpscounter;

void draw_fps(sf::RenderWindow& window, float fps)
{
    char c[32];
    snprintf(c, 32, "FPS: %f", fps);
    std::string string(c);
    sf::String str(c);
    window.setTitle(str);
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_X, WINDOW_Y), "ball collision demo");
    srand(time(NULL));

    std::vector<Ball> balls;

    // randomly initialize balls
    balls.reserve(MAX_BALLS);
    for (int i = 0; i < (rand() % (MAX_BALLS - MIN_BALLS) + MIN_BALLS); i++)
    {
        balls.push_back(Ball(sf::Vector2f(rand() % WINDOW_X, rand() % WINDOW_Y),
            sf::Vector2f((-5 + (rand() % 10)) / 3., (-5 + (rand() % 10)) / 3.),
            5 + rand() % 5, 
            30 + rand() % 30));
    }
    balls.shrink_to_fit();

   // window.setFramerateLimit(60);

    sf::Clock clock;
    float lastime = clock.restart().asSeconds();

    BallsController controller{ std::shared_ptr<std::vector<Ball>>(&balls),
    WINDOW_X,
    WINDOW_Y };
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

        float current_time = clock.getElapsedTime().asSeconds();
        float deltaTime = current_time - lastime;
        fpscounter.push(1.0f / (current_time - lastime));
        lastime = current_time;

        controller.update(deltaTime);
        window.clear();
        for (auto &ball : balls)
        {
            ball.draw(window);
        }

		draw_fps(window, fpscounter.getAverage());
		window.display();
    }
    return 0;
}
