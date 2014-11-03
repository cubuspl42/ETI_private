// -*- coding: utf-8 -*-
#include <SDL.h>
#include <SDL_main.h>

#include <cassert>
#include <initializer_list>
#include <vector>

#define _USE_MATH_DEFINES // MSVC <3
#include <math.h>
#include <stdio.h>
#include <string.h>

#define SCREEN_WIDTH	480
#define SCREEN_HEIGHT   640

class range {
    class iterator {
    public:
        explicit iterator(unsigned i_) : i(i_) {}
        inline void operator++() { ++i; }
        inline unsigned operator*() { return i; };
        inline bool operator!=(const iterator &it) { return i != it.i; }
    private:
        unsigned i;
    };
public:
    explicit range(unsigned n_) : n(n_) {}
    inline iterator begin() { return iterator(0); }
    inline iterator end() { return iterator(n); }
private:
    unsigned n;
};

struct Vector2D
{
    float x;
    float y;
    Vector2D(float x_=0, float y_=0)
    : x(x_), y(y_) {}
    Vector2D& set(int x_, int y_) {
        x = x_;
        y = y_;
        return (*this);
    }
    float& operator [](size_t i) {
        static_assert((offsetof(Vector2D, y)-offsetof(Vector2D, x))==sizeof(float), "Offset mismatch");
        return ((&x)[i]);
    }
    float operator [](size_t i) const {
        return ((&x)[i]);
    }
    Vector2D& operator +=(const Vector2D& v) {
        x += v.x;
        y += v.y;
        return (*this);
    }
    Vector2D& operator -=(const Vector2D& v) {
        x -= v.x;
        y -= v.y;
        return (*this);
    }
    Vector2D& operator *=(int t) {
        x *= t;
        y *= t;
        return (*this);
    }
    Vector2D operator -(void) const {
        return (Vector2D(-x, -y));
    }
    Vector2D operator +(const Vector2D& v) const {
        return (Vector2D(x+v.x, y+v.y));
    }
    Vector2D operator -(const Vector2D& v) const {
        return (Vector2D(x-v.x, y-v.y));
    }
    Vector2D operator *(int t) const {
        return (Vector2D(x*t, y*t));
    }
    Vector2D operator /(int t) const {
        return (Vector2D(x*1.0f/t, y*1.0f/t));
    }
    int operator *(const Vector2D& v) const {
        return (x*v.x+y*v.y);
    }
    bool operator ==(const Vector2D& v) const {
        return ((x == v.x) && (y == v.y));
    }
    bool operator !=(const Vector2D& v) const {
        return ((x != v.x) || (y != v.y));
    }
    int lengthsq() {
        return x*x+y*y;
    }
    double length() {
        return sqrt(lengthsq());
    }
    Vector2D normalized(void) {
        return (*this) / length();
    }
};

struct AABB {
    Vector2D position, box;
    inline float top() const {
        return position.y;
    }
    inline float bottom() const {
        return position.y+box.y;
    }
    inline float left() const {
        return position.x;
    }
    inline float right() const {
        return position.x+box.x;
    }
    SDL_Rect rect() {
        SDL_Rect r;
        r.x = position.x;
        r.y = position.y;
        r.w = box.x;
        r.h = box.y;
        return r;
    }
};

enum State {
    GAMEPLAY_STATE,
    PAUSED_STATE,
    GAME_OVER_STATE,
    LEVEL_END_STATE
};

enum BonusType {
    NO_BONUS,
    SLOW_MOTION_BONUS,
    STICKY_PADDLE_BONUS,
    EXTRA_LIFE_BONUS,
    WIDER_PADDLE_BONUS,
    THREE_BALLS_BONUS,
    SHOOTING_PADDLE_BONUS
};

constexpr unsigned gameplay_area_width = 480;
constexpr unsigned gameplay_area_height = 640;
constexpr size_t max_num_bricks = 256;
constexpr size_t max_num_balls = 3;
constexpr float ball_box_width = 16;
constexpr float ball_box_height = 16;
constexpr float bonus_duration = 20;

struct Brick {
    Vector2D position;
    unsigned type;
};

struct Ball {
    Vector2D position;
    Vector2D velocity;
};

struct Gameplay {
    unsigned score;
    Brick bricks[max_num_bricks];
    size_t num_bricks;
    Ball balls[max_num_balls];
    size_t num_balls = 1;
    float paddle_x;
    float paddle_width;
    BonusType active_bonus_type;
    float active_bonus_time_left;
};

// When there's collision, puts the projection vector into *out_proj and returns true.
// If there's no collision, returns false.
bool sat_aabb(const AABB &a, const AABB& b, Vector2D *out_proj) {
    Vector2D proj;
    int minlen = INT_MAX;
    if(b.top() < a.bottom() && b.bottom() > a.top()) {
        float up = a.top() - b.bottom();
        float down = a.bottom() - b.top();
        for(float y : {up, down}) {
            if(abs(y) < minlen) {
                minlen = abs(y);
                proj = Vector2D{0, y};
            }
        }
    } else return false;
    if(b.left() < a.right() && b.right() > a.left()) {
        int left = a.left() - b.right();
        int right = a.right() - b.left();
        for(float x : {left, right}) {
            if(abs(x) < minlen) {
                minlen = abs(x);
                proj = Vector2D{x, 0};
            }
        }
    } else return false;
    *out_proj = proj;
    return true;
}

void resolve_collision(Vector2D proj, Vector2D *pos, Vector2D *velocity) {
    assert(proj.x*proj.y == 0 && proj.x+proj.y != 0);
    *pos += proj;
    if(proj.y == 0)
        velocity->x *= -1;
    else if(proj.x == 0)
        velocity->y *= -1;
}

void handle_potential_wall_collision(AABB *aabb, Vector2D *velocity) {
    if(aabb->left() < 0)
        resolve_collision(Vector2D{-aabb->left(),0}, &aabb->position, velocity);
    if(aabb->right() > gameplay_area_width)
        resolve_collision(Vector2D{gameplay_area_width - aabb->right(),0}, &aabb->position, velocity);
    if(aabb->top() < 0)
        resolve_collision(Vector2D{0,-aabb->top()}, &aabb->position, velocity);
    if(aabb->bottom() > gameplay_area_height)
        resolve_collision(Vector2D{0,gameplay_area_height - aabb->bottom()}, &aabb->position, velocity);
}


// narysowanie napisu txt na powierzchni screen, zaczynając od punktu (x, y)
// charset to bitmapa 128x128 zawierająca znaki
void DrawString(SDL_Surface *screen, int x, int y, const char *text,
                SDL_Surface *charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while(*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
    }
}


// narysowanie na ekranie screen powierzchni sprite w punkcie (x, y)
// (x, y) to punkt środka obrazka sprite na ekranie
void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
}


// rysowanie pojedynczego pixela
void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32 *)p = color;
}


// rysowanie linii o długości l w pionie (gdy dx = 0, dy = 1) 
// bądź poziomie (gdy dx = 1, dy = 0)
void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for(int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
    }
}


// rysowanie prostokąta o długości boków l i k
void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k,
                   Uint32 outlineColor, Uint32 fillColor) {
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for(int i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
}

void draw(SDL_Surface *screen, const Gameplay &game) {
//    unsigned czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
//    unsigned zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
    unsigned czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
    unsigned niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);
    for(size_t i=0;i<game.num_balls;++i) {
        const Ball &ball = game.balls[i];
        DrawRectangle(screen, ball.position.x, ball.position.y, ball_box_width, ball_box_height, czerwony, niebieski);
    }
}

void physics_step(Gameplay *game) {
    for(auto i : range(game->num_balls)) {
        Ball &ball = game->balls[i];
        ball.position += ball.velocity;
        AABB aabb;
        aabb.position = ball.position;
        aabb.box = {ball_box_width, ball_box_height};
        handle_potential_wall_collision(&aabb, &ball.velocity);
        ball.position = aabb.position;
    }
}

int main(int argc, char **argv) {
	int t1, t2, quit, frames;
	double delta, worldTime, fpsTimer, fps, distance, etiSpeed;
	SDL_Event event;
	SDL_Surface *screen, *charset;
	SDL_Surface *eti;
	SDL_Texture *scrtex;
	SDL_Window *window;
	SDL_Renderer *renderer;

	if(SDL_Init(SDL_INIT_TIMER|SDL_INIT_VIDEO) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
    }

//  tryb pełnoekranowy
//	rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP,
//	                                 &window, &renderer);
	window = SDL_CreateWindow("Arkanoid", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1
                                  //,0
                                  , SDL_RENDERER_PRESENTVSYNC
                                  );
	if(!renderer || !window) {
		SDL_Quit();
		printf("Error: %s\n", SDL_GetError());
		return 1;
    }
	
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);


	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
	                              0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
	                           SDL_TEXTUREACCESS_STREAMING,
	                           SCREEN_WIDTH, SCREEN_HEIGHT);


	// wyłączenie widoczności kursora myszy
	SDL_ShowCursor(SDL_DISABLE);

	// wczytanie obrazka cs8x8.bmp
	charset = SDL_LoadBMP("./cs8x8.bmp");
	if(charset == NULL) {
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
    }
	SDL_SetColorKey(charset, true, 0x000000);

	eti = SDL_LoadBMP("./eti.bmp");
	if(eti == NULL) {
		printf("SDL_LoadBMP(eti.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
    }

	char text[128];
	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
//	int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);

	t1 = SDL_GetTicks();

	frames = 0;
	fpsTimer = 0;
	fps = 0;
	quit = 0;
	worldTime = 0;
	distance = 0;
	etiSpeed = 1;
    
    Gameplay game;
    game.num_balls = 2;
    game.balls[0].position = {16, 16};
    game.balls[0].velocity = {2, 2};
    game.balls[1].position = {256, 256};
    game.balls[1].velocity = {8, 8};

	while(!quit) {
		t2 = SDL_GetTicks();

		// w tym momencie t2-t1 to czas w milisekundach,
		// jaki uplynał od ostatniego narysowania ekranu
		// delta to ten sam czas w sekundach
		delta = (t2 - t1) * 0.001;
		t1 = t2;

		worldTime += delta;

		distance += etiSpeed * delta;

		SDL_FillRect(screen, NULL, czarny);

		DrawSurface(screen, eti,
		            SCREEN_WIDTH / 2 + sin(distance) * SCREEN_HEIGHT / 3,
			    SCREEN_HEIGHT / 2 + cos(distance) * SCREEN_HEIGHT / 3);


//		DrawScreen(screen, plane, ship, charset, worldTime, delta, vertSpeed);

		// naniesienie wyniku rysowania na rzeczywisty ekran
//		SDL_Flip(screen);

		fpsTimer += delta;
		if(fpsTimer > 0.5) {
			fps = frames * 2;
			frames = 0;
			fpsTimer -= 0.5;
        }

		// tekst informacyjny
		DrawRectangle(screen, 4, 4, SCREEN_WIDTH - 8, 36, czerwony, niebieski);
		sprintf(text, "Szablon drugiego zadania, czas trwania = %.1lf s  %.0lf klatek / s", worldTime, fps);
		DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 10, text, charset);
		sprintf(text, "Esc - wyjscie, \030 - przyspieszenie, \031 - zwolnienie");
		DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 26, text, charset);
        
        physics_step(&game);
        draw(screen, game);

		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
//		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);

		// obsługa zdarzeń (o ile jakieś zaszły)
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_KEYDOWN:
					if(event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
					else if(event.key.keysym.sym == SDLK_UP) etiSpeed = 2.0;
					else if(event.key.keysym.sym == SDLK_DOWN) etiSpeed = 0.3;
					break;
				case SDL_KEYUP:
					etiSpeed = 1.0;
					break;
				case SDL_QUIT:
					quit = 1;
					break;
            }
        }
		frames++;
    }

	// zwolnienie powierzchni
	SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
}
