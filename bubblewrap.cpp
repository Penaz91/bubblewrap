#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <time.h>

/**
 * @brief Returns a random number between two numbers
 *
 * @param min Lower limit
 * @param max Upper limit
 * @return A random number between min and max
 */
float getRandFloat(const float& min, const float& max){
    float uniform = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    float diff = max - min;
    return uniform * diff + min;
}

/**
 * @brief Edits a quad by side effect, settings the popped texture
 *
 * This function edits quads using side effects on a pointer, thus
 * particular care needs to be put on how the quad is passed to this function.
 * Here we set the quad's "spritesheet coordinates" to the "popped texture".
 *
 * HACK: This function actually does an uncontrolled memory read, by indexing into the VertexArray
 *     ^ past where it's intended to be indexed, given the Vertex passed as argument.
 *     ^ This is a "controlled situation" though, and it makes it easier to move each Vertex of the quad
 *     ^ without having other superstructures.
 *
 * @param quad The first vertex of the quad to be edited
 */
void setPoppedTexture(sf::Vertex* quad){
    quad[0].texCoords = sf::Vector2f(0, 0);
    quad[1].texCoords = sf::Vector2f(60, 0);
    quad[2].texCoords = sf::Vector2f(60, 60);
    quad[3].texCoords = sf::Vector2f(0, 60);
}

/**
 * @brief Edits a quad by side effect, settings the unpopped texture
 *
 * This function edits quads using side effects on a pointer, thus
 * particular care needs to be put on how the quad is passed to this function.
 * Here we set the quad's "spritesheet coordinates" to the "unpopped texture".
 *
 * HACK: This function actually does an uncontrolled memory read, by indexing into the VertexArray
 *     ^ past where it's intended to be indexed, given the Vertex passed as argument.
 *     ^ This is a "controlled situation" though, and it makes it easier to move each Vertex of the quad
 *     ^ without having other superstructures.
 *
 * @param quad The first vertex of the quad to be edited
 */
void setUnpoppedTexture(sf::Vertex* quad){
    quad[0].texCoords = sf::Vector2f(61, 0);
    quad[1].texCoords = sf::Vector2f(120, 0);
    quad[2].texCoords = sf::Vector2f(120, 60);
    quad[3].texCoords = sf::Vector2f(61, 60);
}

/**
 * @brief Sets the array of popped bubbles to all false.
 *
 * This function works by side effect on the "poppedArr" array, care should be
 * used when passing arguments to this function.
 *
 * @param poppedArr Array of booleans to initialize to false
 * @param size Size of one edge of the board (the board is considered square)
 */
void initPoppedArray(bool* poppedArr, const int& size){
    // Populating the popped array
    for (unsigned int i = 0; i < size * size; ++i) {
        poppedArr[i] = false;
    }
}

/**
 * @brief Sets the texture of the bubbles in the VertexArray, according to an array of booleans
 *
 * This function edits the bubbles VertexArray by side effect and makes use of functions that have
 * side effects of their own, care should be taken when using this function.
 *
 * @param bubbles The VertexArray to edit
 * @param size Size of one edge of the board (the board is assumed square)
 * @param poppedArr Array of booleans that is used to set the state of the bubbles
 */
void setBubbleTextures(sf::VertexArray* bubbles, const unsigned int& size, bool* poppedArr){
    for (unsigned int i = 0; i < size; i++){  // Select Row
        for (unsigned int j = 0; j < size; j++){  // Select Column
            // Treating a multi-dimensional structure as a 1D one.
            // We calculate the index of the tile at row i, column j
            int currentTile = i * size + j;
            // HACK: VertexArray contains an array of vertices, thus we take the first vertex
            //     ^ of the quad we're interested in and pass it to the rest of the functions.
            sf::Vertex* quad = &(*bubbles)[currentTile * 4];
            // If the current tile status is "popped"
            if (poppedArr[currentTile]){
                // Set Popped Texture
                setPoppedTexture(quad);
            }else{
                // Set Non-Popped Texture
                setUnpoppedTexture(quad);
            }
        }
    }
}

int main(){
    // We set the random number generator seed, by seeding it with the system time.
    // This will be used to change the pop cound pitch.
    srand(time(nullptr));
    // We create our rendering window and set the frame limit to 30 fps using SFML's utilities.
    sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode(600,600),
                                                    "Bubble Wrap Simulator!");
    window->setFramerateLimit(30);
    // We prepare an event handler for later
    sf::Event event;
    bool running = true;  // The game is running
    const int size = 10;  // Size of the board, in this case it's 10x10 bubbles
    int tileSize = 60;    // Size of a tile, in pixels. In this case it's 60x60
    sf::Texture* bubbleTex = new sf::Texture();  // Preparing the texture handler
    /* Load The Texture
     * The texture is a 120x60 texture like follows:
     * +---------+---------+
     * | POP     | Non-Pop |
     * +---------+---------+
     */
    if (!bubbleTex->loadFromFile("images/texture.png")){
        // In case the load fails, exit with a code 127 (generic error)
        return 127;
    }
    // Load the Sound into the soundBuffer
    sf::SoundBuffer* popSound = new sf::SoundBuffer();
    if (!popSound->loadFromFile("sounds/pop.wav")){
        // In case the load fails, exit with a code 127 (generic error)
        return 127;
    }
    // Prepare the sound to be played
    sf::Sound* pop = new sf::Sound(*popSound);
    // We don't want the sound to loop
    pop->setLoop(false);
    /*
     * Prepare a VertexArray of Quads. VertexArray will contain all the vertices, thus we need
     * 4 times (4 vertices in a quad) the size of an edge of the board, squared.
     */
    sf::VertexArray* bubbles = new sf::VertexArray(sf::PrimitiveType::Quads,
                                                   4 * size * size);
    // Create an array of booleans, one per bubble
    bool * poppedArr = new bool[size * size];
    initPoppedArray(poppedArr, size);
    // Populate the vertex array
    for (unsigned int i = 0; i < size; i++){  // Select Row (y)
        for (unsigned int j = 0; j < size; j++){  // Select Column (x)
            // Treating a multi-dimensional structure as a 1D one.
            // We calculate the index of the tile at row i, column j
            int currentTile = i * size + j;
            // HACK: VertexArray contains an array of vertices, thus we take the first vertex
            //     ^ of the quad we're interested in and do a somewhat-risky (but controlled) memory read.
            sf::Vertex* quad = &(*bubbles)[currentTile * 4];
            /*
             * Quads have their vertices arranged as follows:
             * 0----1
             * |    |
             * 3----2
             */
            quad[0].position = sf::Vector2f(j * tileSize, i * tileSize);
            quad[1].position = sf::Vector2f((j + 1) * tileSize, i * tileSize);
            quad[2].position = sf::Vector2f((j + 1) * tileSize, (i + 1) * tileSize);
            quad[3].position = sf::Vector2f(j * tileSize, (i + 1) * tileSize);
        }
    }
    // Set all bubbles textures according to the boolean array (all unpopped)
    setBubbleTextures(bubbles, size, poppedArr);
    // Game Loop
    while (running){
        // Event Polling
        while (window->pollEvent(event)){
            // If I'm closing the game, set the running variable to false
            if (event.type == sf::Event::Closed){
                running = false;
            }
            // If I clicked in the window...
            if (event.type == sf::Event::MouseButtonReleased){
                // ...and it's the left mouse button
                if (event.mouseButton.button == sf::Mouse::Left){
                    // Calculate the tile indices
                    int tileX = event.mouseButton.x / tileSize;
                    int tileY = event.mouseButton.y / tileSize;
                    // Treating a multi-dimensional structure as a 1D one.
                    // We calculate the index of the tile at row i, column j in poppedArr
                    int currentTile = tileY * size + tileX;
                    // If the current tile is not popped yet
                    if (!poppedArr[currentTile]){
                        // Set it as popped
                        poppedArr[currentTile] = true;
                        // Get the first vertex of the quad of our tile
                        sf::Vertex* quad = &(*bubbles)[currentTile * 4];
                        // And set the popped texture
                        setPoppedTexture(quad);
                        // Randomize the pop sound pitch between 80% and 160%
                        pop->setPitch(getRandFloat(0.8f, 1.6f));
                        // Play a popping sound.
                        pop->play();
                    }
                }
            }
            // If we pressed a keyboard button...
            if (event.type == sf::Event::KeyReleased){
                // And it's the R Button.
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
