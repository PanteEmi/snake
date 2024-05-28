#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include "serial.h"
#include "udp.h"


typedef Vector2 cell;

#define TIMEOUT_MS 100 // Timeout for select in milliseconds


int main(int argc, char *argv[])
{
    const int grid_height = 24;
    const int grid_width  = 40;  

    const int initial_snake_size = 4;
    const int cell_size = 20;
    const int border_size = 4;

    const int screen_width = grid_width * cell_size;
    const int screen_height = grid_height * cell_size;

    const Vector2 cell_dimensions = {20-border_size, 20-border_size};

    Vector2 player_position = (Vector2){screen_width/2, screen_height/2};


    InitWindow(screen_width, screen_height, "Snek");
    SetTargetFPS(10);

    // init snake
    struct{
        cell cells[100];
        int size;
    }snake = {.size = initial_snake_size};

    snake.cells[0] = player_position;

    
    for(int i=1;i<snake.size;i++){
        snake.cells[i] = (Vector2){-100, -100};
    }

    // apple
    Vector2 apple;
    apple.x = (rand()%grid_width)*cell_size;
    apple.y = (rand()%grid_height)*cell_size;

    int end_game = 0;

    Vector2 last_action;
    last_action.x = cell_size;




    //int fd = serial_begin(argv[1], B115200);

    struct UDP_client client = create_socket(8080, "192.168.208.75");


    // loop
    while (!WindowShouldClose())
    {

        char instructions[256];

        /*
        if (serial_wait_for_data(fd, TIMEOUT_MS) > 0) {
            serial_read(fd, instructions);
        }
        */

        if(socket_wait_for_data(client.sockfd, TIMEOUT_MS) > 0){
            receive_from_socket(client, instructions);
        }

        if(!end_game){
            if(instructions[0] == '2' && last_action.x != -cell_size){
                last_action.x = cell_size;
                last_action.y = 0;
            }
            else if(instructions[0] == '4' && last_action.x != cell_size){
                last_action.x = -cell_size;
                last_action.y = 0;
            }
            else if(instructions[0] == '1' && last_action.y != cell_size){
                last_action.x = 0;
                last_action.y = -cell_size;
            }
            else if(instructions[0] == '3' && last_action.y != -cell_size){
                last_action.x = 0;
                last_action.y = cell_size;
            }
        }

        /*
        if(!end_game){
            if(IsKeyDown(KEY_RIGHT) && last_action.x != -cell_size){
                last_action.x = cell_size;
                last_action.y = 0;
            }
            else if(IsKeyDown(KEY_LEFT) && last_action.x != cell_size){
                last_action.x = -cell_size;
                last_action.y = 0;
            }
            else if(IsKeyDown(KEY_UP) && last_action.y != cell_size){
                last_action.x = 0;
                last_action.y = -cell_size;
            }
            else if(IsKeyDown(KEY_DOWN) && last_action.y != -cell_size){
                last_action.x = 0;
                last_action.y = cell_size;
            }
        }
        */
        else{
            last_action.x = 0;
            last_action.y = 0;
            if(IsKeyDown(KEY_ENTER)){
                end_game = 0;
                snake.cells[0] = player_position;
                for(int i=1;i<snake.size;i++){
                    snake.cells[i] = (Vector2){-100, -100};
                }
                last_action.x = cell_size;
                snake.size = initial_snake_size;
            }
        }

        snake.cells[0].x += last_action.x;
        snake.cells[0].y += last_action.y;
        if(snake.cells[0].x>screen_width){
            snake.cells[0].x = 0;
        }
        else if(snake.cells[0].x<0){
            snake.cells[0].x = screen_width;
        }
        if(snake.cells[0].y>screen_height){
            snake.cells[0].y = 0;
        }
        else if(snake.cells[0].y<0){
            snake.cells[0].y = screen_height;
        }


        if(snake.cells[0].x == apple.x && snake.cells[0].y == apple.y){
            apple.x = (rand()%grid_width)*cell_size;
            apple.y = (rand()%grid_height)*cell_size;
            snake.size++;
        }
        char scor[20];
        sprintf(scor, "Score: %d", snake.size-initial_snake_size);

        BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawRectangleV(apple, cell_dimensions, GREEN);
            for(int i=0;i<snake.size;i++){
                DrawRectangleV(snake.cells[i], cell_dimensions, RED);
            }

            for(int i=1;i<snake.size;i++){
                if(snake.cells[0].x == snake.cells[i].x && snake.cells[0].y == snake.cells[i].y){
                    for(int i=snake.size-1; i>=0; i--){
                        DrawRectangleV(snake.cells[i], cell_dimensions, BLACK);
                    }
                    end_game = 1;
                    break;
                }
            }
            DrawText(scor, 20, 20, 20, BLACK);  

            if(end_game){
                DrawText("Game Over", screen_width/2-100, screen_height/2, 40, GRAY);  
            }

        EndDrawing();
        for(int i=snake.size-1;i>0 && !end_game;i--){
            snake.cells[i] = snake.cells[i-1];
        }
    }

    CloseWindow();

    return 0;
}