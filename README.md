# Mandelbrot
As part of this project, I wrote code that shows the Mandelbrot factorial set. You can press:
- <kbd>\2191</kbd> to raise the picture;
- <kbd>&#8593;</kbd> to lower the picture;
- <kbd>&#8594;<kbd> to move the picture to the right;
- `&#8592;` to move the picture to the left;
- <kbd>F1</kbd> to zoom in;
- <kbd>F2</kbd> to zoom out.

On your device, you can reproduce the picture below:
![Mandelbrot](./img/Mandelbrot.png)

To do this, you need to compile the project by typing make in the console when you clone the repository to your computer. If you want to remove the __optimization flags__, then remove the corresponding flag in the `makefile`; to disable __rendering__ remove the -DDRAW flag; to disable __AVX2__ optimization, remove the -DAVX_ON flag. Initially, the optimization, rendering and AVX2 flags are __enabled__.

##AVX or not AVX...

| Version     | Compilation flags | FPS            |
| ------      | :---------------: | :------------: | 
| No AVX      | none              | 4              |  
| No AVX      | -О3               | 10             |  
| No AVX      | -Оfast            | 10             | 
| AVX         | none              | 9              |  
| AVX         | -О3               | 15             | 
| AVX         | -Ofast            | 15             | 

The tests were carried out at the same fractal position. It can be seen that thanks to AVX optimization, the average FPS grows by 1.5-2 times.
