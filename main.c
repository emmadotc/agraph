#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <locale.h>
#include <ncurses.h>
#include <sys/ioctl.h>


double clamp(double num, double min, double max)
{
    return (num > max ? max : (num < min ? min : num));
}

int strln_nt(char *str)
{
    int len = 0;
    while(str[len])
        len++;

    return len;
}

int main()
{
    // set locale for ncurses
    setlocale(LC_CTYPE, "");

    // init ncurses
    initscr();
    raw();
    noecho();
    nodelay(stdscr, TRUE);
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);

    // get term size
    int HEIGHT, WIDTH;
    getmaxyx(stdscr, HEIGHT, WIDTH);
    HEIGHT -= 2;
    WIDTH  -= 2;

    // set animation properties
    double sinestep  = 5, sinephase  = 0,
           vsinestep = 1, vsinephase = 0,
           parastep  = 5, paraphase  = 1;

    // hide cursor
    curs_set(0);

    // draw border
    border('|', '|', '-', '-', '+', '+', '+', '+');

    while(1)
    {
        whilestart:
        if(getch() == 'q')
            goto end;

        sinephase += sinestep;
        if(sinephase > (20 * M_PI))
            sinephase = 0;

        vsinephase += vsinestep;
        if(vsinephase > (8 * M_PI))
            vsinephase = 0;

        paraphase += parastep;
        if(paraphase >= 360 || paraphase <= 1)
            parastep *= -1;

        // init graph
        char graph[HEIGHT][WIDTH];
        for(int y = 0; y < HEIGHT; ++y)
            for(int x = 0; x < WIDTH; ++x)
                graph[y][x] = ' ';

        // do the math
        /* sine wave */
        for(double x = 0; x < WIDTH; ++x)
            {
                int y = (int)round(clamp(HEIGHT / 4 * 3 + (sin((x - sinephase) / 10) * 5), 0, HEIGHT));
                graph[y][(int)x] = 'X';
            }

        /* cosine wave */
        for(double x = 0; x < WIDTH; ++x)
            {
                int y = (int)round(clamp(HEIGHT / 4 + (cos((x - sinephase) / 10) * 5), 0, HEIGHT));
                graph[y][(int)x] = 'X';
            }

        /* vertical sine wave */
        for(double y = 0; y < HEIGHT; ++y)
        {
            int x = (int)round(clamp(WIDTH / 2 + (sin((y - vsinephase) / 4) * 5), 0, WIDTH));
            graph[(int)y][x] = 'X';
        }

        /* hyperbola */
        for(double x = 0; x < WIDTH; ++x)
        {
            int y = (int)round(clamp(paraphase/x, 0, HEIGHT));
            graph[y][(int)x] = 'X';
        }

        //print the math correctly
        for(int y = HEIGHT - 1; y >= 0; --y)
        {
            while(is_term_resized(HEIGHT + 2, WIDTH + 2))
            {
                erase();
                refresh();
                usleep(300 * 1000);

                getmaxyx(stdscr, HEIGHT, WIDTH);
                WIDTH  -= 2;
                HEIGHT -= 2;
                border('|', '|', '-', '-', '+', '+', '+', '+');

                if(is_term_resized(HEIGHT + 2, WIDTH + 2))
                    goto whilestart;
            }

            for(int x = 0; x < WIDTH; ++x)
            {
                mvaddch(HEIGHT - y, x + 1, graph[y][x]);
            }
        }

        usleep(1000 / 30 * 1000);
    }

    // end
    end:
    curs_set(1);
    endwin();
    putchar(10);
    return 0;
}
