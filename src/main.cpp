#include <iostream>
#include <stdexcept>
#include <cmath>
#include <SDL.h>

inline double deg2rad(double deg) { return (deg * M_PI) / 180.0; }
inline double rad2deg(double rad) { return (rad * 180.0) / M_PI; }

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

int main(int argc, const char* const argv[])
{
  try
  {
    SDL sdl(SDL_INIT_VIDEO);
    std::cout << "360 degrees = " << deg2rad(360.0) << " radians." << std::endl;
    std::cout << "1 radian = " << rad2deg(1.0) << " degrees." << std::endl;
  }
  catch(const std::runtime_error& rte)
  {
    std::cerr << "Exeption caught: " << rte.what() << std::endl;
  }
  return 0;
}
