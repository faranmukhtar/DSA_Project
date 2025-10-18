#include "map.h"

int main() {
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

    // SFML 3: use size.x and size.y
    unsigned int windowWidth = static_cast<unsigned int>(desktop.size.x * 0.95f);
    unsigned int windowHeight = static_cast<unsigned int>(desktop.size.y * 0.89f);

    // SFML 3: VideoMode constructor takes a Vector2u
    sf::RenderWindow window(sf::VideoMode({windowWidth, windowHeight}), "SFML Window - % based size");
    window.setPosition({35,10});


    sf::RectangleShape rectangle({100.f, 200.f});
    rectangle.setFillColor(sf::Color::Blue);
    rectangle.setPosition({100.f, 200.f});

    while (window.isOpen()) {
        // SFML 3: pollEvent returns std::optional<Event>
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        window.clear(sf::Color::Black);

        window.draw(rectangle);
        window.display();

    }
    return 0;
}
