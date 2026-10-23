#include <time.h>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Mouse.hpp>

float getRandFloat(const float& min, const float& max) {
    float uniform = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    float diff = max - min;
    return uniform * diff + min;
}

void setPoppedTexture(sf::Vertex* trie1, sf::Vertex* trie2) {
    trie1[0].texCoords = sf::Vector2f(0, 0);
    trie1[1].texCoords = sf::Vector2f(60, 0);
    trie1[2].texCoords = sf::Vector2f(0, 60);
    trie2[0].texCoords = sf::Vector2f(0, 60);
    trie2[1].texCoords = sf::Vector2f(60, 60);
    trie2[2].texCoords = sf::Vector2f(60, 0);
}

void setUnpoppedTexture(sf::Vertex* trie1, sf::Vertex* trie2) {
    trie1[0].texCoords = sf::Vector2f(61, 0);
    trie1[1].texCoords = sf::Vector2f(120, 0);
    trie1[2].texCoords = sf::Vector2f(61, 60);
    trie2[0].texCoords = sf::Vector2f(61, 60);
    trie2[1].texCoords = sf::Vector2f(120, 60);
    trie2[2].texCoords = sf::Vector2f(120, 0);
}

void initPoppedArray(bool* poppedArr, const int& size) {
    // Populating the popped array
    for (unsigned int i = 0; i < size * size; ++i) {
        poppedArr[i] = false;
    }
}

void setBubbleTextures(sf::VertexArray* bubbles,
                       const unsigned int& size,
                       bool* poppedArr) {
    for (unsigned int i = 0; i < size; i++) {      // Row
        for (unsigned int j = 0; j < size; j++) {  // Column
            int currentTile = i * size + j;
            sf::Vertex* trie1 = &(*bubbles)[currentTile * 6];
            sf::Vertex* trie2 = &(*bubbles)[currentTile * 6 + 3];
            if (poppedArr[currentTile]) {
                // Popped Texture
                setPoppedTexture(trie1, trie2);
            } else {
                // Non-Popped Texture
                setUnpoppedTexture(trie1, trie2);
            }
        }
    }
}

int main() {
    // Seed Randomizer
    srand(time(nullptr));
    // Variables
    sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode({600, 600}),
                                                    "Bubble Wrap Simulator!");
    window->setFramerateLimit(30);
    bool running = true;
    const int size = 10;
    int tileSize = 60;
    sf::Texture* bubbleTex = new sf::Texture();
    // Load The Texture
    if (!bubbleTex->loadFromFile("images/texture.png")) {
        return 127;
    }
    // Load the Sound
    sf::SoundBuffer* popSound = new sf::SoundBuffer();
    if (!popSound->loadFromFile("sounds/pop.wav")) {
        return 127;
    }
    sf::Sound* pop = new sf::Sound(*popSound);
    pop->setLooping(false);
    sf::VertexArray* bubbles =
        new sf::VertexArray(sf::PrimitiveType::Triangles, 6 * size * size);
    bool* poppedArr = new bool[size * size];
    initPoppedArray(poppedArr, size);
    // Populating the vertex array
    for (unsigned int i = 0; i < size; i++) {      // Row
        for (unsigned int j = 0; j < size; j++) {  // Column
            int currentTile = i * size + j;
            sf::Vertex* trie = &(*bubbles)[currentTile * 6];
            trie[0].position = sf::Vector2f(j * tileSize, i * tileSize);
            trie[1].position = sf::Vector2f((j + 1) * tileSize, i * tileSize);
            trie[2].position = sf::Vector2f(j * tileSize, (i + 1) * tileSize);
            trie[3].position = sf::Vector2f(j * tileSize, (i + 1) * tileSize);
            trie[4].position =
                sf::Vector2f((j + 1) * tileSize, (i + 1) * tileSize);
            trie[5].position = sf::Vector2f((j + 1) * tileSize, i * tileSize);
        }
    }
    setBubbleTextures(bubbles, size, poppedArr);
    while (running) {
        // Event Polling
        while (const std::optional event = window->pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                running = false;
            }
            if (const auto* mouseButton =
                    event->getIf<sf::Event::MouseButtonReleased>()) {
                if (mouseButton->button == sf::Mouse::Button::Left) {
                    int tileX = mouseButton->position.x / tileSize;
                    int tileY = mouseButton->position.y / tileSize;
                    int currentTile = tileY * size + tileX;
                    if (!poppedArr[currentTile]) {
                        poppedArr[currentTile] = true;
                        sf::Vertex* trie1 = &(*bubbles)[currentTile * 6];
                        sf::Vertex* trie2 = &(*bubbles)[currentTile * 6 + 3];
                        setPoppedTexture(trie1, trie2);
                        pop->setPitch(getRandFloat(0.8f, 1.6f));
                        pop->play();
                    }
                }
            }
            if (auto* key = event->getIf<sf::Event::KeyReleased>()) {
                if (key->code == sf::Keyboard::Key::R) {
                    // Reset the Board
                    initPoppedArray(poppedArr, size);
                    setBubbleTextures(bubbles, size, poppedArr);
                }
            }
        }
        // Window Drawing
        // Clearing
        window->clear();
        // Drawing
        sf::RenderStates states;
        states.texture = bubbleTex;
        window->draw(*bubbles, states);
        // Display
        window->display();
    }
    return 0;
}
