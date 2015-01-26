// -*- coding: utf-8 -*-
#include "vector2.h"

#include <SDL.h>
#include <SDL_main.h>
#include <SDL_image.h>

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

#include <math.h>
#include <stdio.h>
#include <string.h>

using namespace std;

constexpr unsigned screen_width = 480;
constexpr unsigned screen_height = 640;
constexpr size_t bricks_per_column = 16;
constexpr size_t bricks_per_line = 13;
constexpr float brick_width = 34;
constexpr float brick_height = 18;
constexpr unsigned hud_height = 64;
constexpr unsigned edge_width = 19;
constexpr unsigned gameplay_area_offset_x = edge_width;
constexpr unsigned gameplay_area_offset_y = hud_height + edge_width;
constexpr unsigned gameplay_area_width = bricks_per_line * brick_width;
constexpr unsigned gameplay_area_height = screen_height - gameplay_area_offset_y;
constexpr size_t max_num_balls = 3;
constexpr float ball_box_width = 10;
constexpr float ball_box_height = 10;
constexpr float ball_texture_width = 12;
constexpr float ball_texture_height = 12;
constexpr float ball_base_speed = 3;
constexpr float ball_max_speed = 6;
constexpr float ball_time_to_reach_max_speed = 20;
constexpr float paddle_y = 512;
constexpr float paddle_velocity = 6;
constexpr float base_paddle_width = 70;
constexpr float extended_paddle_width = 140;
constexpr float paddle_height = 18;
constexpr float bonus_duration = 20;
constexpr size_t num_bricks_types = 4;
constexpr unsigned lives_left_x = 32;
constexpr unsigned lives_left_y = screen_height - ball_texture_height - 8;
constexpr unsigned pickup_spawn_chance_inv = 8;
constexpr size_t max_num_pickups = 16;
constexpr unsigned pickup_width = 32;
constexpr unsigned pickup_height = 32;
constexpr float pickup_velocity = 3;
constexpr unsigned highscores_x = 150;
constexpr unsigned highscores_y = 200;
constexpr unsigned score_bonus = 100;
constexpr unsigned score_x = 190;
constexpr unsigned score_y = 25;

enum State {
    Game_STATE,
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
    SHOOTING_PADDLE_BONUS,
    NUM_BONUS_TYPES
};

struct Pickup {
    Vector2 position;
    BonusType type;
};

struct Resources {
    SDL_Texture* background_texture;
    SDL_Texture* ball_texture;
    SDL_Texture* paddle_texture;
    SDL_Texture* brick_textures[num_bricks_types];
    SDL_Texture* pickup_textures[NUM_BONUS_TYPES];
    SDL_Texture* gameover_texture;
    SDL_Texture* charset_texture;
};

struct Brick {
    int type; // -1, 0, 1, 2, 3, ...
    bool destroyed = false;
};

struct Ball {
    Vector2 position;
    Vector2 velocity;
    bool freezed = false;
};

struct Game {
    ifstream *levels_file = nullptr;
    std::string top_players[5];
    unsigned top_scores[5] = {0};
    bool give_name = false;
    unsigned top_score_index = 0;
    bool show_highscores = false;
    bool gameover = false;
    unsigned score = 0;
    Pickup pickups[max_num_pickups];
    unsigned pickups_count = 0;
    Vector2 bullets[max_num_pickups];
    unsigned bullets_count = 0;
    Brick bricks[bricks_per_column][bricks_per_line];
    Ball balls[max_num_balls];
    unsigned balls_count = 0;
    float balls_speed = 0;
    unsigned balls_acceleration_start_time = 0;
    float paddle_x;
    int paddle_moving_dir = 0; // -1, 0, 1
    float paddle_width;
    unsigned lives_left = 0;
    BonusType active_bonus_type;
    unsigned active_bonus_start_time;
};

void draw_string(SDL_Renderer *renderer, SDL_Texture *charset, unsigned x, unsigned y, const char *text) {
    SDL_Rect s, d;
    s.w = s.h = d.w = d.h = 8;
    d.x = x;
    d.y = y;
    while(*text) {
        if(*text == '\n') {
            d.y += 8;
            d.x = x;
        } else {
            s.x = (*text % 16) * 8;
            s.y = (*text / 16) * 8;
            SDL_RenderCopy(renderer, charset, &s, &d);
            d.x += 8;
        }
        ++text;
    }
}

bool aabbs_overlap(Vector2 pos1, Vector2 box1,
                   Vector2 pos2, Vector2 box2)
{
    return  pos2.x < pos1.x+box1.x && pos2.x+box2.x > pos1.x &&
    pos2.y < pos1.y+box1.y && pos2.y+box2.y > pos1.y;
}

void init(SDL_Window **out_window, SDL_Renderer **out_renderer) {
    assert(SDL_Init(SDL_INIT_TIMER|SDL_INIT_VIDEO) == 0);
    assert(IMG_Init(IMG_INIT_PNG) != 0);
    SDL_Window *window = SDL_CreateWindow("Arkanoid", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          screen_width, screen_height, 0);
    assert(window);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    assert(renderer);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");
    SDL_RenderSetLogicalSize(renderer, screen_width, screen_height);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    *out_window = window;
    *out_renderer = renderer;
}

SDL_Texture *load_texture(SDL_Renderer *renderer, const char *filename) {
    SDL_Surface *surface = IMG_Load(filename);
    assert(surface);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    assert(texture);
    SDL_FreeSurface(surface);
    return texture;
}

void load_scores(Game *game) {
    ifstream f;
    f.open("scores.txt");
    for(int i = 0; i < 5; ++i) {
        f >> game->top_players[i];
        f >> game->top_scores[i];
    }
}

void save_scores(Game *game) {
    ofstream f;
    f.open("scores.txt");
    for(int i = 0; i < 5; ++i) {
        f << game->top_players[i] << ' ';
        f << game->top_scores[i] << ' ';
    }
}

void load_resources(SDL_Renderer *renderer, Resources &res) {
    res.background_texture = load_texture(renderer, "background.png");;
    res.ball_texture = load_texture(renderer, "ball.png");
    res.paddle_texture = load_texture(renderer, "paddle.png");
    {
        char buffer[] = "brick_A.png";
        for(unsigned i = 0; i < num_bricks_types; ++i) {
            buffer[sizeof(buffer)-sizeof(".png")-1] = 'A'+i;
            res.brick_textures[i] = load_texture(renderer, buffer);
        }
    }
    {
        char buffer[] = "bonus_1.png";
        for(unsigned i = 1; i < NUM_BONUS_TYPES; ++i) {
            buffer[sizeof(buffer)-sizeof(".png")-1] = '0'+i;
            res.pickup_textures[i] = load_texture(renderer, buffer);
        }
    }
    res.gameover_texture = load_texture(renderer, "gameover.png");
    res.charset_texture = load_texture(renderer, "cs8x8.bmp");
}

bool load_level(Game *game) {
    char c;
    unsigned y = 0, x = 0;
    ifstream &f = *game->levels_file;
    while(y < bricks_per_column && f >> c) {
        if(c == '.') {
            game->bricks[y][x].type = -1;
            game->bricks[y][x].destroyed = true;
        } else {
            unsigned type = c - 'A';
            assert(type < num_bricks_types);
            game->bricks[y][x].type = type;
            game->bricks[y][x].destroyed = false;
        }
        if(++x >= bricks_per_line)
            x = 0, ++y;
    }
    
    return (bool)f;
}

void handle_wall_collisions(Game *game)
{
    for(int i = 0; i < game->balls_count; ++i) {
        Ball &ball = game->balls[i];
        if(ball.position.x < 0)
            ball.velocity.x = abs(ball.velocity.x);
        if(ball.position.x + ball_box_width >= gameplay_area_width)
            ball.velocity.x = -abs(ball.velocity.x);
        if(ball.position.y < 0)
            ball.velocity.y = abs(ball.velocity.y);
//        if(ball.position.y + ball_box_height >= gameplay_area_height)
//            ball.velocity.y = -abs(ball.velocity.y);
    }
}

void handle_brick_collisions(Game *game)
{
    for(int i = 0; i < game->balls_count; ++i) {
        Ball &ball = game->balls[i];
        for(int y = 0; y < bricks_per_column; ++y) {
            for(int x = 0; x < bricks_per_line; ++x) {
                Brick &brick = game->bricks[y][x];
                if(!brick.destroyed) {
                    Vector2 brick_pos = {x*brick_width, y*brick_height};
                    if(aabbs_overlap(ball.position, {ball_box_width, ball_box_height},
                                     brick_pos, {brick_width, brick_height})) {
                        float penetration_right = brick_pos.x + brick_width - ball.position.x;
                        float penetration_left = ball.position.x + ball_box_width - brick_pos.x;
                        float penetration_down = brick_pos.y + brick_height - ball.position.y;
                        float penetration_up = ball.position.y + ball_box_height - brick_pos.y;
                        if(min(penetration_left, penetration_right) < min(penetration_up, penetration_down)) {
                            if(penetration_left < penetration_right)
                                ball.velocity.x = -abs(ball.velocity.x);
                            else ball.velocity.x = abs(ball.velocity.x);
                        } else {
                            if(penetration_up < penetration_down)
                                ball.velocity.y = -abs(ball.velocity.y);
                            else ball.velocity.y = abs(ball.velocity.y);
                        }
                        brick.destroyed = true;
                        game->score += score_bonus;
                        if(game->pickups_count < max_num_pickups && rand()%pickup_spawn_chance_inv == 0) {
                            Pickup &pickup = game->pickups[game->pickups_count++];
                            pickup.type = (BonusType)(rand()%(NUM_BONUS_TYPES-1)+1);
                            pickup.position.x = brick_pos.x + brick_width/2 - (pickup_width / 2);
                            pickup.position.y = brick_pos.y + brick_height/2 - (pickup_height / 2);
                        }
                    }
                }
            }
        }
    }
}

void reset_game(Game *game) {
    game->balls_count = 1;
    game->balls_speed = ball_base_speed;
    game->balls_acceleration_start_time = SDL_GetTicks();
    game->balls[0].position = {game->paddle_x+game->paddle_width/2-ball_box_width/2, paddle_y-ball_box_height};
    game->balls[0].freezed = true;
    game->balls[0].velocity = Vector2(1, 1).normalized() * ball_base_speed;
}

void deactivate_bonus(Game *game) {
    switch (game->active_bonus_type) {
        case SLOW_MOTION_BONUS:
            game->balls_speed = ball_base_speed;
            game->balls_acceleration_start_time = SDL_GetTicks();
            break;
        case WIDER_PADDLE_BONUS:
            game->paddle_x += (extended_paddle_width-base_paddle_width)/2;
            game->paddle_width = base_paddle_width;
            break;
        default:
            break;
    }
    game->active_bonus_type = NO_BONUS;
}

void new_level(Game *game) {
    deactivate_bonus(game);
    for(int y = 0; y < bricks_per_column; ++y) {
        for(int x = 0; x < bricks_per_line; ++x) {
            Brick &brick = game->bricks[y][x];
            if(brick.type >= 0)
                brick.destroyed = false;
        }
    }
    game->paddle_width = base_paddle_width;
    game->pickups_count = 0;
    game->paddle_x = gameplay_area_width/2-base_paddle_width/2;
    reset_game(game);
}

void new_game(Game *game) {
    game->score = 0;
    game->lives_left = 2;
    game->levels_file->clear();
    game->levels_file->seekg(0);
    load_level(game);
    new_level(game);
}

bool handle_events(Game *game) {
    SDL_Event event;
    if(game->give_name) {
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    return false;
                case SDL_KEYDOWN:
                    unsigned sym = event.key.keysym.sym;
                    if(sym >= 'a' && sym <= 'z') {
                        game->top_players[game->top_score_index] += sym;
                    } else if(sym == SDLK_RETURN) {
                        game->give_name = false;
                        game->show_highscores = true;
                    }
                    break;
            }
        }
    } else if(game->show_highscores) {
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    return false;
                case SDL_KEYDOWN:
                    game->show_highscores = false;
                    break;
            }
        }
    } else {
        game->paddle_moving_dir = 0;
        const Uint8 *state = SDL_GetKeyboardState(NULL);
        if(state[SDL_SCANCODE_LEFT])
            --game->paddle_moving_dir;
        if(state[SDL_SCANCODE_RIGHT])
            ++game->paddle_moving_dir;
        
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    return false;
                case SDL_KEYDOWN:
                    if(game->gameover) {
                        game->gameover = false;
                        new_game(game);
                    } else {
                        if(event.key.keysym.sym == SDLK_s) {
                            for(int i = 0; i < game->balls_count; ++i) {
                                game->balls[i].freezed = false;
                            }
                        }
                    }
                    break;
            }
        }
    }
    return true;
}

void activate_bonus(Game *game, BonusType bonus) {
    deactivate_bonus(game);
    game->score += score_bonus;
    if(bonus == EXTRA_LIFE_BONUS) {
        ++game->lives_left;
    } else if(bonus == THREE_BALLS_BONUS) {
        if(game->balls_count == 1) {
            game->balls_count = 3;
            game->balls[1].position = game->balls[2].position = game->balls[0].position;
            Vector2 t {5, 1};
            Vector2 &v0 = game->balls[0].velocity;
            Vector2 &v1 = game->balls[1].velocity;
            v1 = {t.x * v0.x - t.y * v0.y, t.y * v0.x + t.x * v0.y};
            t.y = -1;
            Vector2 &v2 = game->balls[2].velocity;
            v2 = {t.x * v0.x - t.y * v0.y, t.y * v0.x + t.x * v0.y};
        }
    } else {
        game->active_bonus_type = bonus;
        game->active_bonus_start_time = SDL_GetTicks();
        switch (bonus) {
            case SLOW_MOTION_BONUS:
                game->balls_speed = ball_base_speed/2;
                break;
            case WIDER_PADDLE_BONUS:
                game->paddle_x -= (extended_paddle_width-base_paddle_width)/2;
                game->paddle_width = extended_paddle_width;
                break;
            default:
                break;
        }
    }
}

void update(Game *game) {
    if(game->gameover)
        return;
    unsigned non_destroyed_bricks = 0;
    for(int y = 0; y < bricks_per_column; ++y) {
        for(int x = 0; x < bricks_per_line; ++x) {
            Brick &brick = game->bricks[y][x];
            if(!brick.destroyed)
                ++non_destroyed_bricks;
        }
    }
    if(non_destroyed_bricks == 0) {
        game->score += score_bonus*10;

        if(!load_level(game)) {
            game->gameover = true;
        } else new_level(game);
    }
    
    unsigned t = SDL_GetTicks();
    if(game->active_bonus_type && (t - game->active_bonus_start_time) > bonus_duration*1000) {
        deactivate_bonus(game);
    }
    
    for(int i = 0; i < game->pickups_count; ++i) {
        Pickup &pickup = game->pickups[i];
        pickup.position.y += pickup_velocity;
        
        bool overlap = aabbs_overlap(pickup.position, {pickup_width, pickup_height},
                                     {game->paddle_x, paddle_y}, {game->paddle_width, paddle_height});
        if(pickup.position.y >= gameplay_area_height || overlap) {
            if(overlap)
                activate_bonus(game, pickup.type);
            pickup = game->pickups[game->pickups_count-1];
            --game->pickups_count;
        }
    }
    
    float prev_paddle_x = game->paddle_x;
    game->paddle_x += game->paddle_moving_dir * paddle_velocity;
    game->paddle_x = max(game->paddle_x, 0.0f);
    game->paddle_x = min(game->paddle_x, (float)gameplay_area_width-game->paddle_width);
    
    if(game->active_bonus_type != SLOW_MOTION_BONUS) {
        game->balls_speed = ball_base_speed + (t - game->balls_acceleration_start_time)/(ball_time_to_reach_max_speed*1000)*(ball_max_speed-ball_base_speed);
        game->balls_speed = min(game->balls_speed, ball_max_speed);
    }
    for(int i = 0; i < game->balls_count; ++i) {
        Ball &ball = game->balls[i];
        if(ball.freezed) {
            ball.position.x += game->paddle_x - prev_paddle_x;
        } else {
            ball.position += ball.velocity.normalized() * game->balls_speed;
            if(aabbs_overlap(ball.position, {ball_box_width, ball_box_height},
                             {game->paddle_x, paddle_y}, {game->paddle_width, paddle_height})) {
                float ratio = ball.position.x + ball_box_width/2 - (game->paddle_x + game->paddle_width/2);
                ratio /= game->paddle_width/2;
                float theta = ratio * 70 * 2*M_PI/360;
                ball.velocity = { sin(theta), -cos(theta) };
                if(game->active_bonus_type == STICKY_PADDLE_BONUS)
                    ball.freezed = true;
            } else if(ball.position.y >= gameplay_area_height) {
                ball = game->balls[game->balls_count-1];
                --game->balls_count;
            }
        }
    }
    
    handle_wall_collisions(game);
    handle_brick_collisions(game);
    
    if(game->balls_count == 0) {
        if(game->lives_left == 0) {
            game->gameover = true;
        } else {
            --game->lives_left;
            reset_game(game);
        }
    }
    
    if(game->gameover) {
        for(int i = 4; i >= 0; --i) {
            if(game->score > game->top_scores[i]) {
                game->give_name = true;
                game->top_score_index = i;
            }
        }
        if(!game->give_name)
            game->show_highscores = true;
        else {
            for(int j = 3; j >= (int)game->top_score_index; --j) {
                game->top_players[j+1] = game->top_players[j];
                game->top_scores[j+1] = game->top_scores[j];
            }
            game->top_players[game->top_score_index] = "";
            game->top_scores[game->top_score_index] = game->score;
        }
    }
}

void draw(SDL_Renderer *renderer, const Game &game, const Resources &res)
{
    SDL_RenderClear(renderer);
    
    if(game.give_name) {
        draw_string(renderer, res.charset_texture, highscores_x, highscores_y, "Highscore!\nGive your name:");
        draw_string(renderer, res.charset_texture, highscores_x, highscores_y+20, game.top_players[game.top_score_index].c_str());
    } else if(game.show_highscores) {
        char buff[] = "1.";
        for(int i = 0; i < 5; ++i) {
            draw_string(renderer, res.charset_texture, highscores_x-20, highscores_y+i*10, buff);
            draw_string(renderer, res.charset_texture, highscores_x, highscores_y+i*10, game.top_players[i].c_str());
            stringstream ss;
            ss << game.top_scores[i];
            draw_string(renderer, res.charset_texture, highscores_x+64, highscores_y+i*10, ss.str().c_str());
            ++buff[0];
        }
    } else {
        constexpr unsigned X = gameplay_area_offset_x;
        constexpr unsigned Y = gameplay_area_offset_y;
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, res.background_texture, nullptr, nullptr);
        SDL_Rect r;
        
        r.w = brick_width;
        r.h = brick_height;
        for(int y = 0; y < bricks_per_column; ++y) {
            for(int x = 0; x < bricks_per_line; ++x) {
                const Brick &brick = game.bricks[y][x];
                if(!brick.destroyed) {
                    r.x = X + x * brick_width;
                    r.y = Y + y * brick_height;
                    SDL_RenderCopy(renderer, res.brick_textures[brick.type], nullptr, &r);
                }
            }
        }
        
        for(int i = 0; i < game.balls_count; ++i) {
            const Ball &ball = game.balls[i];
            r.x = X + ball.position.x - (ball_texture_width-ball_box_width)/2;
            r.y = Y + ball.position.y - (ball_texture_height-ball_box_height)/2;;
            r.w = ball_texture_width;
            r.h = ball_texture_height;
            SDL_RenderCopy(renderer, res.ball_texture, nullptr, &r);
        }
        
        r.x = X + game.paddle_x;
        r.y = Y + paddle_y;
        r.w = game.paddle_width;
        r.h = paddle_height;
        SDL_RenderCopy(renderer, res.paddle_texture, nullptr, &r);
        
        r.w = ball_texture_width;
        r.h = ball_texture_height;
        r.y = lives_left_y;
        for(int i = 0; i < game.lives_left; ++i) {
            r.x = lives_left_x + i * (ball_texture_width + 4);
            SDL_RenderCopy(renderer, res.ball_texture, nullptr, &r);
        }
        
        for(int i = 0; i < game.pickups_count; ++i) {
            const Pickup &pickup = game.pickups[i];
            r.x = X + pickup.position.x;
            r.y = Y + pickup.position.y;
            r.w = pickup_width;
            r.h = pickup_height;
            SDL_RenderCopy(renderer, res.pickup_textures[pickup.type], nullptr, &r);
        }
        
        stringstream ss;
        ss << game.score;
        draw_string(renderer, res.charset_texture, score_x, score_y, ss.str().c_str()
                    );
        
        if(game.gameover)
            SDL_RenderCopy(renderer, res.gameover_texture, nullptr, nullptr);
    }

    SDL_RenderPresent(renderer);
}

int main(int argc, char **argv) {
    srand(time(nullptr));
    SDL_Window *window;
    SDL_Renderer *renderer;
    init(&window, &renderer);
    
    Game game;
    load_scores(&game);
    ifstream f;
    f.open("levels.txt");
    assert(f);
    game.levels_file = &f;
    Resources res;
    load_resources(renderer, res);
    new_game(&game);

	while(true) {
        if(!handle_events(&game))
            break;
        update(&game);
        draw(renderer, game, res);
        SDL_Delay(16);
    }
    
    save_scores(&game);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
