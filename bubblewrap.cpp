#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <time.h>

float getRandFloat(const float& min, const float& max){
    float uniform = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    float diff = max - min;
    return uniform * diff + min;
}

void setPoppedTexture(sf::Vertex* quad){
    quad[0].texCoords = sf::Vector2f(0, 0);
    quad[1].texCoords = sf::Vector2f(60, 0);
    quad[2].texCoords = sf::Vector2f(60, 60);
    quad[3].texCoords = sf::Vector2f(0, 60);
}

void setUnpoppedTexture(sf::Vertex* quad){
    quad[0].texCoords = sf::Vector2f(61, 0);
    quad[1].texCoords = sf::Vector2f(120, 0);
    quad[2].texCoords = sf::Vector2f(120, 60);
    quad[3].texCoords = sf::Vector2f(61, 60);
}

void initPoppedArray(bool* poppedArr, const int& size){
    // Populating the popped array
    for (unsigned int i = 0; i < size * size; ++i) {
        poppedArr[i] = false;
    }
}

void setBubbleTextures(sf::VertexArray* bubbles, const unsigned int& size, bool* poppedArr){
    for (unsigned int i = 0; i < size; i++){  // Row
        for (unsigned int j = 0; j < size; j++){  // Column
            int currentTile = i * size + j;
            sf::Vertex* quad = &(*bubbles)[currentTile * 4];
            if (poppedArr[currentTile]){
                // Popped Texture
                setPoppedTexture(quad);
            }else{
                // Non-Popped Texture
                setUnpoppedTexture(quad);
            }
        }
    }
}

int main(){
    // Seed Randomizer
    srand(time(nullptr));
    // Variables
    sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode(600,600),
                                                    "Bubble Wrap Simulator!");
    window->setFramerateLimit(30);
    sf::Event event;
    bool running = true;
    const int size = 10;
    int tileSize = 60;
    sf::Texture* bubbleTex = new sf::Texture();
    // Load The Texture
    if (!bubbleTex->loadFromFile("images/texture.png")){
        return 127;
    }
    // Load the Sound
    sf::SoundBuffer* popSound = new sf::SoundBuffer();
    if (!popSound->loadFromFile("sounds/pop.wav")){
        return 127;
    }
    sf::Sound* pop = new sf::Sound(*popSound);
    pop->setLoop(false);
    sf::VertexArray* bubbles = new sf::VertexArray(sf::PrimitiveType::Quads,
                                                   4 * size * size);
    bool * poppedArr = new bool[size * size];
    initPoppedArray(poppedArr, size);
    // Populating the vertex array
    for (unsigned int i = 0; i < size; i++){  // Row
        for (unsigned int j = 0; j < size; j++){  // Column
            int currentTile = i * size + j;
            sf::Vertex* quad = &(*bubbles)[currentTile * 4];
            quad[0].position = sf::Vector2f(j * tileSize, i * tileSize);
            quad[1].position = sf::Vector2f((j + 1) * tileSize, i * tileSize);
            quad[2].position = sf::Vector2f((j + 1) * tileSize, (i + 1) * tileSize);
            quad[3].position = sf::Vector2f(j * tileSize, (i + 1) * tileSize);
        }
    }
    setBubbleTextures(bubbles, size, poppedArr);
    while (running){
        // Event Polling
        while (window->pollEvent(event)){
            if (event.type == sf::Event::Closed){
                running = false;
            }
            if (event.type == sf::Event::MouseButtonReleased){
                if (event.mouseButton.button == sf::Mouse::Left){
                    int tileX = event.mouseButton.x / tileSize;
                    int tileY = event.mouseButton.y / tileSize;
                    int currentTile = tileY * size + tileX;
                    if (!poppedArr[currentTile]){
                        poppedArr[currentTile] = true;
                        sf::Vertex* quad = &(*bubbles)[currentTile * 4];
                        setPoppedTexture(quad);
                        pop->setPitch(getRandFloat(0.8f, 1.6f));
                        pop->play();
                    }
                }
            }
            if (event.type == sf::Event::KeyReleased){
                if (event.key.code == sf::Keyboard::R){
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
