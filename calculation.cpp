#include "calculation.hpp"

// #define AVX_ON

// #define DRAW

using namespace sf;

int mandelbrot(void);
void outputFps(RenderWindow &window, sf::Clock *clock, sf::Text *text);
void outputMndlbrt(RenderWindow &window, float center_x, float center_y);
void outputMndlbrtAvx(RenderWindow &window, float center_x, float center_y);

const int SIZE_OF_TEXT = 15;

int main(void)
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


//                      (or 2*BORDER=6)
//          <------------1200--|----------> 
//          =============================== |
//       |  ||             |   |         || |         [   Xc = center_x   ]
//       |  ||             |   |         || |         [ relative to (0,0) ] new coordinate system
//       |  ||             |   | .(x0,y0)|| |         [   Yc = center_y   ]
// y_max |  ||             |   |         || |
//       |  ||        (0,0)|   |         || 1 (or 2*BORDER=6)
//       |  ||-------------|-------------|| 2 --->
//       |  ||             |   |         || 0
//   - - - -||- - - - - - -|- -.(Xc,Yc)- || 0 - ->
//          ||             |   |         || |   |
//          ||             |   |         || |   |y_min
//          ||             |   |         || |   |
//          =============================== |
//          <------600----->   |         |
//                             |         |
//          <------x_min------>|<-x_max->|
//


void outputMndlbrtAvx(RenderWindow &window, float center_x, float center_y)
{
    RectangleShape pixel = createRectangle(1, 1, 0, 0);

    __m256 density     = _mm256_set1_ps(WINDOW_SIZE_X/(2*BORDER));
    __m256 max_rad_vec = _mm256_set1_ps(MAX_DISTANCE);
    __m256 shift_dx    = _mm256_set_ps (0, dx, 2*dx, 3*dx, 4*dx, 5*dx, 6*dx, 7*dx);

    float x_min = -BORDER - center_x, x_max = BORDER - center_x;
    float y_min = -BORDER - center_y, y_max = BORDER - center_y;

    __m256 shift_x = _mm256_set1_ps (-x_min);
    __m256 shift_y = _mm256_set1_ps (y_max);

    for (float y0 = -BORDER; y0 <= BORDER; y0 += dy)
    {
        __m256 y0_vec = _mm256_set1_ps(y0);
        __m256 y0_vec_pos = _mm256_mul_ps(_mm256_sub_ps(shift_y, y0_vec), density);

        for (float x0 = -BORDER; x0 <= BORDER; x0 += 8*dx)
        {
            __m256 x0_vec = _mm256_add_ps(_mm256_set1_ps(x0), shift_dx);
            __m256 x0_vec_pos = _mm256_mul_ps(_mm256_add_ps(shift_x, x0_vec), density);

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
            float * xCoord = (float *) &x0_vec_pos;
            float * yCoord = (float *) &y0_vec_pos;

            for (int cntr = 0; cntr < 8; cntr++)
            {
                pixel.setPosition (xCoord[cntr], yCoord[cntr]);
                pixel.setFillColor (Color(255, 242, 245));
                int n = iterationArray[cntr];

                if (n < MAX_ITERATION)
                {
                    pixel.setFillColor(Color((uint8_t)n * 30, (uint8_t)n * 5, 255 - (uint8_t)n));
                }
                
                #ifdef DRAW
                    window.draw (pixel);
                #endif
            }
        }
    }
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