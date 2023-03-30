#include "calculation.hpp"

using namespace sf;

int mandelbrot (void);
void outputFps (RenderWindow &window, sf::Clock * clock, sf::Text * text);
void outputMndlbrt (RenderWindow &window);

const int SIZE_OF_TEXT = 15;


int main (void)
{
    mandelbrot ();
    return 0;
}

int mandelbrot (void)
{
    RenderWindow window (VideoMode(WINDOW_SIZE_X, WINDOW_SIZE_Y), "Mandelbrot");
    window.setFramerateLimit(60); 

    Font font;
    if (!font.loadFromFile("fonts/Disket-Mono-Regular.ttf"))
    {
        printf ("\e[31m Unable to open the file with fonts\e[0m \n");
        return -1;
    }
    Text text;
    text.setFont (font);                    //select the font
    text.setCharacterSize (SIZE_OF_TEXT);   //size in pixels
    text.setFillColor (Color::Blue);
    
    Clock clock;                            // for FPS

    while (window.isOpen())
    {

        clock.restart ();

        sf::Event event;                    // check all the window's events... 
        while (window.pollEvent(event))     // that were triggered since the...
        {                                   // last iteration of the loop
            if (event.type == sf::Event::Closed) 
            {
                window.close();     //we close the window because "close requested" event has occurred
            }
        }
        
        outputMndlbrt (window);
        
        sf::Time elapsedTime = clock.getElapsedTime();
        char fps[100] = {0};
        sprintf (fps, "FPS: %.0f\n", 1/elapsedTime.asSeconds()); 
        printf (fps);                 
        text.setString(fps);
        window.draw(text);

        window.display();
        window.clear();
    }

	
    return 0;
}

sf::RectangleShape createRectangle (float width, float height, float x, float y)
{
    sf::RectangleShape rectangle (sf::Vector2f(120, 50));
    rectangle.setSize (Vector2f(width, height));
    rectangle.setFillColor(sf::Color::Green);
    rectangle.setPosition (x, y);

    return rectangle;
}

void outputMndlbrt (RenderWindow &window)
{
    RectangleShape pixel = createRectangle (1,1,0,0);    

    for (float y0 = -2; y0 <= 2; y0 += dy)
    {
        float y0_pos = (2.0 - y0) * 250;

        for (float x0 = -2.0; x0 <= 2.0; x0 += dx)
        {
            float x0_pos = (2.0 + x0) * 250;

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
            pixel.setPosition (float (x0_pos), float (y0_pos));
            pixel.setFillColor (sf::Color((unsigned char)numIteration, 25, (unsigned char) numIteration/2));

            if (numIteration < MAX_ITERATION)
            {
                pixel.setFillColor (sf::Color::Yellow);
            }

            window.draw (pixel);
        }
    }
}