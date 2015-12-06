#include <iostream>
#include <stdexcept>
#include <vector>
#include <cmath>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <SDL.h>

inline double deg2rad(double deg) { return (deg * M_PI) / 180.0; }
inline double rad2deg(double rad) { return (rad * 180.0) / M_PI; }

//
// Return the pixel value at (x, y)
// NOTE: The surface must be locked before calling this!
//
Uint32 getpixel(SDL_Surface* surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;

    case 2:
        return *(Uint16*)p;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
        return *(Uint32*)p;

    default:
        return 0;       // shouldn't happen, but avoids warnings
    }
}

//
// Set the pixel at (x, y) to the given value
// NOTE: The surface must be locked before calling this!
//
void putpixel(SDL_Surface* surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16*)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32*)p = pixel;
        break;
    }
}


namespace ublas = boost::numeric::ublas;

class SDL
{
  public:
    SDL();
    SDL(Uint32 flags);
    virtual ~SDL();
};

SDL::SDL()
{
  if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
  {
    throw std::runtime_error(SDL_GetError());
  }
}

SDL::SDL(Uint32 flags)
{
  if(SDL_Init(flags) != 0)
  {
    throw std::runtime_error(SDL_GetError());
  }
}

SDL::~SDL()
{
  SDL_Quit();
}

typedef boost::numeric::ublas::vector<double> vector_d;
typedef boost::numeric::ublas::vector<float> vector_f;
typedef boost::numeric::ublas::vector<int> vector_i;

SDL_Point vector2point(const vector_i& v, const int scale)
{
  SDL_Point point = {0, 0};
  if (v.size() < 3)
  {
    return point;
  }
  vector_i temp = (v * scale) / v[2];
  point.x = temp[0];
  point.y = temp[1];
  return point;
}

int main(int argc, char* argv[])
{
  int windowWidth = 640;
  int windowHeight = 480;
  Uint32 windowFlags = SDL_WINDOW_SHOWN;
  const char* windowTitle = "3DICA";
  const int SCALE = 256;
  boost::random::mt19937 gen;
  boost::random::uniform_int_distribution<> dist(-10, 10);
  boost::random::uniform_int_distribution<> speed(1, 6);

  try
  {
    SDL sdl(SDL_INIT_VIDEO);

    std::cout << "360 degrees = " << deg2rad(360.0) << " radians." << std::endl;
    std::cout << "1 radian = " << rad2deg(1.0) << " degrees." << std::endl;
    ublas::vector<double> v(3);
    for (unsigned i = 0; i < v.size (); ++ i)
        v (i) = i;
    std::cout << v << std::endl;
    ublas::vector<int> a(2), b(2);
    a[0] = 3; a[1] = 1;
    b[0] = 1; b[1] = -7;
    std::cout << a + b << std::endl;
    std::cout << a - b << std::endl;
    std::cout << ublas::inner_prod((a + b), (a - b)) << std::endl;

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    if (SDL_CreateWindowAndRenderer(windowWidth, windowHeight, windowFlags, &window, &renderer))
    {
      throw std::runtime_error(SDL_GetError());
    }

    SDL_Surface* icon = SDL_LoadBMP("resources/iconzilla.bmp");
    if (!icon)
    {
      throw std::runtime_error(SDL_GetError());
    }
    if (SDL_MUSTLOCK(icon))
    {
      if (SDL_LockSurface(icon) < 0)
      {
        throw std::runtime_error(SDL_GetError());
      }
    }
    Uint32 colorkey = getpixel(icon, 0, 0);
    if (SDL_SetColorKey(icon, SDL_TRUE, colorkey) < 0)
    {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to set color key for window icon: %s", SDL_GetError());
    }
    if (SDL_MUSTLOCK(icon))
    {
      SDL_UnlockSurface(icon);
    }
    SDL_SetWindowIcon(window, icon);
    SDL_FreeSurface(icon);

    SDL_SetWindowTitle(window, windowTitle);

    bool done = false;
    SDL_Event event;

    std::vector<vector_i> stars;

    for (int i = 0; i < 10; i++)
    {
      vector_i star(4);
      star[0] = dist(gen);
      star[1] = dist(gen);
      star[2] = 256;
      star[3] = speed(gen);
      std::cout << star[3] << std::endl;
      stars.push_back(star);
    }

    SDL_Color background = {0x00, 0x00, 0x00, 0xFF};
    SDL_Color star_color = {0xFF, 0xFF, 0xFF, 0xFF};

    SDL_Point camera = {windowWidth / 2, windowHeight / 2};

    while (!done)
    {
      SDL_PollEvent(&event);
      if (event.type == SDL_QUIT)
        done = true;
      if (SDL_SetRenderDrawColor(renderer, background.r, background.g, background.b, background.a) < 0)
      {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to set background draw color: %s", SDL_GetError());
      }
      SDL_RenderClear(renderer);
      if (SDL_SetRenderDrawColor(renderer, star_color.r, star_color.g, star_color.b, star_color.a) < 0)
      {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to set foreground draw color: %s", SDL_GetError());
      }
      for (std::vector<vector_i>::iterator star = stars.begin(); star != stars.end(); star++)
      {
        SDL_Point p = vector2point(*star, SCALE);
        p.x += camera.x;
        p.y += camera.y;
        if (SDL_RenderDrawPoint(renderer, p.x, p.y) < 0)
        {
          SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to draw a star: %s", SDL_GetError());
        }
        (*star)[2] -= (*star)[3];
        if ((*star)[2] <= 0)
        {
          (*star)[0] = dist(gen);
          (*star)[1] = dist(gen);
          (*star)[2] = 256;
          (*star)[3] = speed(gen);
        }
      }
      SDL_RenderPresent(renderer);
      SDL_Delay(10);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
  }
  catch(const std::runtime_error& rte)
  {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Exeption caught: %s", rte.what());
  }
  return 0;
}
