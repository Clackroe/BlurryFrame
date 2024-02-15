#include <SFML/include/SFML/Graphics.hpp>

int main() {
  // create the window
  sf::RenderWindow window(sf::VideoMode(800, 600), "My window");

  // run the program as long as the window is open
  while (window.isOpen()) {
    // check all the window's events that were triggered since the last
    // iteration of the loop
    sf::Event event;
    while (window.pollEvent(event)) {
      // "close requested" event: we close the window
      if (event.type == sf::Event::Closed)
        window.close();
    }

    sf::CircleShape shape(50);

    // set the shape color to green
    shape.setFillColor(sf::Color(100, 250, 50));

    // clear the window with black color
    window.clear(sf::Color::Blue);

    // draw everything here...
    // window.draw(...);

    window.draw(shape);

    // end the current frame
    window.display();
  }

  return 0;
}
