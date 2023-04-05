#include "calculation.hpp"

int mandelbrot(void);

using namespace sf;
void outputMndlbrtAvx (Image &image, float center_x, float center_y);
void outputMndlbrt (Image &image, float center_x, float center_y);

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

    sf::Image image;
    image.create(WINDOW_SIZE_X, WINDOW_SIZE_Y, Color::Black);

    sf::Texture texture;
    texture.loadFromImage(image);

    sf::Sprite sprite;
    sprite.setTexture(texture);

    Clock clock; // for FPS

    while (window.isOpen())
    {
        clock.restart();

        if (Keyboard::isKeyPressed(Keyboard::Left))
        {
            center_x -= 0.1*scale;
        } 
        if (Keyboard::isKeyPressed(Keyboard::Right))
        {
            center_x += 0.1*scale;
        }
        if (Keyboard::isKeyPressed(Keyboard::Up))
        {
            center_y += 0.1*scale;
        }
        if (Keyboard::isKeyPressed(Keyboard::Down))
        {
            center_y -= 0.1*scale;
        }   
        if (Keyboard::isKeyPressed(Keyboard::F1))
        {
            scale /= 1.25;
            BORDER = BORDER*0.8;
            BORDER = BORDER*0.8;
            dx = (2*BORDER)/(WINDOW_SIZE_X);
            dy = (2*BORDER)/(WINDOW_SIZE_Y);
        }
            
        if (Keyboard::isKeyPressed(Keyboard::F2))
        {
            scale *= 1.25;
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

        #ifdef AVX
            outputMndlbrtAvx(image, center_x, center_y);
        #else
            outputMndlbrt(image, center_x, center_y);
        #endif

        Time elapsedTime = clock.getElapsedTime();
        char fps[100] = {0};
        sprintf(fps, "FPS: %.0f\n", 1 / elapsedTime.asSeconds());
        text.setString(fps);
        window.draw(text);

        window.clear(Color::Black);

        #ifdef DRAW
        texture.update(image);

        window.draw (sprite);
        #endif 
        window.draw (text);

        window.display();
    }

    return 0;
}

void outputMndlbrtAvx (Image &image, float center_x, float center_y)
{
    __m256 max_rad_vec = _mm256_set1_ps(MAX_DISTANCE);
    __m256 shift_dx = _mm256_set_ps (7*dx, 6*dx, 5*dx, 4*dx, 3*dx, 2*dx, dx, 0);

    for (int y0_pos = 0; y0_pos < WINDOW_SIZE_Y; y0_pos++)
    {
        __m256 y0_vec = _mm256_set1_ps (((2*BORDER)/WINDOW_SIZE_Y)*y0_pos - BORDER + center_y);

        for (int x0_pos = 0; x0_pos < WINDOW_SIZE_X; x0_pos += 8)
        {
            __m256 x0_vec = _mm256_set1_ps (((2*BORDER)/WINDOW_SIZE_X)*x0_pos - BORDER + center_x);
            x0_vec = _mm256_add_ps (x0_vec, shift_dx);

            __m256i numIteration = _mm256_set1_epi32(0);
            int counter = 0;

            for (__m256 x = x0_vec, y = y0_vec; counter < MAX_ITERATION; counter++)
            {
                __m256 x2_vec = _mm256_mul_ps(x, x);
                __m256 y2_vec = _mm256_mul_ps(y, y);
                __m256 xy_vec = _mm256_mul_ps(x, y);

                __m256 radius_vec = _mm256_add_ps (x2_vec, y2_vec);

                __m256 cmp_res = _mm256_cmp_ps (max_rad_vec, radius_vec, _CMP_GT_OS); // comparing each distance with max len
                int comparison_mask = _mm256_movemask_ps(cmp_res);                   // moves the most significant bit of each float to integer bits

                if (!comparison_mask)                                                // if all points out of range then break
                {
                    break;
                }

                numIteration = _mm256_sub_epi32 (numIteration, _mm256_castps_si256 (cmp_res));

                x = _mm256_add_ps(_mm256_sub_ps(x2_vec, y2_vec), x0_vec); 
                y = _mm256_add_ps(_mm256_add_ps(xy_vec, xy_vec), y0_vec);
            }

            uint32_t * iterationArray = (uint32_t *) &numIteration;
            
            int x_screen_pos = x0_pos;
            int y_screen_pos = y0_pos;

            
            for (int cntr = 0; cntr < 8; cntr++, x_screen_pos++)
            {
                Color color;
                
                int n = iterationArray[cntr];

                color = Color(255, 242, 245);

                if (n < MAX_ITERATION)
                {
                    
                    color = Color((uint8_t)n * 30, (uint8_t)n * 5, 255 - (uint8_t)n);
                }
                #ifdef DRAW
                image.setPixel(x_screen_pos, y_screen_pos, color); 
                #endif

            }
            
        }
    }  
}

void outputMndlbrt (Image &image, float center_x, float center_y)
{
    for (int y0_pos = 0; y0_pos < WINDOW_SIZE_Y; y0_pos++)
    {
        float y0 = ((2*BORDER)/WINDOW_SIZE_Y)*y0_pos - BORDER + center_y;

        for (int x0_pos = 0; x0_pos < WINDOW_SIZE_X; x0_pos++)
        {
            float x0 = ((2*BORDER)/WINDOW_SIZE_X)*x0_pos - BORDER + center_x;
            
            int counter = 0;

            for (float x = x0, y = y0; counter < MAX_ITERATION; counter++)
            {
                float x2 = x*x;
                float y2 = y*y;
                float xy = x*y;

                float radius_vec = x2 + y2;

                if (radius_vec >= MAX_DISTANCE)
                {
                    break;
                }

                x = x2 - y2 + x0; 
                y = xy + xy + y0;
            }

            
            Color color;

            color = Color(255, 242, 245);
            if (counter < MAX_ITERATION)
            {
                
                color = Color((uint8_t)counter * 30, (uint8_t)counter * 5, 255 - (uint8_t)counter);
            }
            #ifdef DRAW
                image.setPixel(x0_pos, y0_pos, color); 
            #endif
        }
    }
}