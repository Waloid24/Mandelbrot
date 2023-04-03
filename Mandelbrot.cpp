#include "calculation.hpp"

int mandelbrot(void);
void outputMndlbrtAvx (Texture * pixels, float center_x, float center_y);

using namespace sf;

const int SIZE_OF_TEXT = 15;



int main (void)
{
    mandelbrot();

    return 0;
}

int mandelbrot(void)
{
    RenderWindow window(VideoMode(WINDOW_SIZE_X, WINDOW_SIZE_Y), "Mandelbrot");
    window.setFramerateLimit(60);

    Font font;
    if (!font.loadFromFile("fonts/Disket-Mono-Regular.ttf"))
    {
        printf("\e[31m Unable to open the file with fonts\e[0m \n");
        return -1;
    }
    Text text;
    text.setFont(font);                  // select the font
    text.setCharacterSize(SIZE_OF_TEXT); // size in pixels
    text.setFillColor(Color::Red);

    float center_x = 0;
    float center_y = 0;
    float scale    = 1;

    Clock clock; // for FPS
    bool DRAW_MADL = 0;

    Texture pixels;
    pixels.create(WINDOW_SIZE_X, WINDOW_SIZE_Y);
    outputMndlbrtAvx (&pixels, center_x, center_y);

    while (window.isOpen())
    {
        clock.restart();

        if (Keyboard::isKeyPressed(Keyboard::Left))
        {
            DRAW_MADL = 1;
            center_x -= 0.1;
        } 
        if (Keyboard::isKeyPressed(Keyboard::Right))
        {
            DRAW_MADL = 1;
            center_x += 0.1;
        }
        if (Keyboard::isKeyPressed(Keyboard::Up))
        {
            DRAW_MADL = 1;
            center_y += 0.1;
        }
        if (Keyboard::isKeyPressed(Keyboard::Down))
        {
            DRAW_MADL = 1;
            center_y -= 0.1;
        }   
        if (Keyboard::isKeyPressed(Keyboard::F1))
        {
            DRAW_MADL = 1;
            BORDER = BORDER*0.8;
            BORDER = BORDER*0.8;
            dx = (2*BORDER)/(WINDOW_SIZE_X);
            dy = (2*BORDER)/(WINDOW_SIZE_Y);
        }
            
        if (Keyboard::isKeyPressed(Keyboard::F2))
        {
            DRAW_MADL = 1;
            BORDER = BORDER*1.25;
            BORDER = BORDER*1.25;
            dx = (2*BORDER)/(WINDOW_SIZE_X);
            dy = (2*BORDER)/(WINDOW_SIZE_Y);
        }
            

        sf::Event event;                // check all the window's events...
        while (window.pollEvent(event)) // that were triggered since the...
        {                               // last iteration of the loop
            if (event.type == sf::Event::Closed)
            {
                window.close(); // we close the window because "close requested" event has occurred
            }
        }

        #ifdef AVX_ON
            outputMndlbrtAvx(window, center_x, center_y);
        
        #else
            outputMndlbrt (window, center_x, center_y);

        #endif  

        sf::Time elapsedTime = clock.getElapsedTime();
        char fps[100] = {0};
        sprintf(fps, "FPS: %.0f\n", 1 / elapsedTime.asSeconds());
        text.setString(fps);
        window.draw(text);

        window.display();
        window.clear(Color::Black);
    }

    return 0;
}

sf::RectangleShape createRectangle(float width, float height, float x, float y)
{
    sf::RectangleShape rectangle(sf::Vector2f(120, 50));
    rectangle.setSize(Vector2f(width, height));
    rectangle.setFillColor(Color::Green);
    rectangle.setPosition(x, y);

    return rectangle;
}

void outputMndlbrt(RenderWindow &window, float center_x, float center_y)
{
    RectangleShape pixel = createRectangle(1, 1, 0, 0);

    float x_min = -BORDER - center_x, x_max = BORDER - center_x;
    float y_min = -BORDER - center_y, y_max = BORDER - center_y;

    float shift_x = BORDER + center_x;
    float shift_y = BORDER - center_y;

    for (float y0 = -BORDER; y0 <= BORDER; y0 += dy)
    {
        float y0_pos = (shift_y - y0) * WINDOW_SIZE_X/(2*BORDER); //((float)WINDOW_SIZE_Y/BORDER)

        for (float x0 = -BORDER; x0 <= BORDER; x0 += dx)
        {
            float x0_pos = (shift_x + x0) * WINDOW_SIZE_X/(2*BORDER); //((float)WINDOW_SIZE_X/BORDER)
            int numIteration = 0;

            for (float x = x0, y = y0; numIteration < MAX_ITERATION; numIteration++)
            {
                float x2 = x * x;
                float y2 = y * y;
                float xy = x * y;

                float distToCenter = x2 + y2;

                if (distToCenter >= MAX_DISTANCE)
                {
                    break;
                }
                x = x2 - y2 + x0;
                y = xy + xy + y0;
            }
            pixel.setPosition(float(x0_pos), float(y0_pos));
            pixel.setFillColor (Color(255, 242, 245));
            if (numIteration < MAX_ITERATION)
            {
                pixel.setFillColor(sf::Color((unsigned char)numIteration*30, (unsigned char)numIteration*5, 255 - (unsigned char)numIteration));
            }

            #ifdef DRAW
                window.draw(pixel);
            #endif
        }
    }
}

void outputMndlbrtAvx (Texture * pixels, float center_x, float center_y)
{
    assert (pixels);

    
}