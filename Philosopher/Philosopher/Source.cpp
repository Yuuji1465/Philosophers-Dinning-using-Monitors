#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <condition_variable>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

using namespace std;

enum Status { thinking, hungry, eating };

const int NUM_PHILOSOPHERS = 5;
std::mutex stateMutex;

// Cấu trúc Monitor trong Dining-Philosopher Problem
class DP {
public:
    // Khởi tạo trạng thái ban đầu của các triết gia
    DP()
    {
        for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
            state[i] = thinking;
        }
    }

    // Hàm lấy fork
    void Pickup(int i) {
        state[i] = hungry;
        test(i);

        if (state[i] != eating) {
            std::unique_lock<std::mutex> lock(mtx);
            self[i].wait(lock, [this, i] { return state[i] == eating || (state[(i + 1) % NUM_PHILOSOPHERS] != eating && state[(i + 4) % NUM_PHILOSOPHERS] != eating); });
        }
    }

    // Hàm đặt fork
    void Putdown(int i) {
        state[i] = thinking;
        test((i + 1) % NUM_PHILOSOPHERS);
        test((i + 4) % NUM_PHILOSOPHERS);
    }

    // Hàm đánh dấu triết gia đói
    void SetHungry(int i) {
        state[i] = hungry;
    }

    // Hàm Getter để lấy status của triết gia
    Status GetStatus(int i) {
        return state[i];
    }

private:
    Status state[NUM_PHILOSOPHERS];
    std::condition_variable self[NUM_PHILOSOPHERS];
    std::mutex mtx;

    void test(int i) {
        if (state[(i + 1) % NUM_PHILOSOPHERS] != eating &&
            state[(i + 4) % NUM_PHILOSOPHERS] != eating &&
            state[i] == hungry) {
            state[i] = eating;
            self[i].notify_one();
        }
    }
};

DP dp;

void philosopher(int i, SDL_Renderer* renderer, SDL_Rect* Squares) {
    while (true) {
        // ...
        this_thread::sleep_for(chrono::milliseconds(rand() % 5000));
        // Hungry
        printf("Philosopher %d wants to eat\n", i);
        dp.Pickup(i);

        // Eating
        printf("Philosopher %d is eating\n", i);
        

        {
            std::lock_guard<std::mutex> lock(stateMutex);
            // Cập nhật màu sắc của ô vuông của triết gia khi ăn
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Màu đỏ
            SDL_RenderFillRect(renderer, &Squares[i]);
            SDL_RenderPresent(renderer);
            //if (dp.GetStatus((i + 1) % NUM_PHILOSOPHERS) == hungry || dp.GetStatus((i + 4) % NUM_PHILOSOPHERS) == hungry) {
            //    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); // Màu xám
            //    SDL_RenderFillRect(renderer, &Squares[i]);
            //    SDL_RenderPresent(renderer);
            //}
            int leftNeighbor = (i + 1) % NUM_PHILOSOPHERS;
            int rightNeighbor = (i + NUM_PHILOSOPHERS - 1) % NUM_PHILOSOPHERS;

            if (dp.GetStatus(leftNeighbor) == hungry) {
                // Cập nhật màu sắc của triết gia bên trái
                SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); // Màu xám
                SDL_RenderFillRect(renderer, &Squares[leftNeighbor]);
            }

            if (dp.GetStatus(rightNeighbor) == hungry) {
                // Cập nhật màu sắc của triết gia bên phải
                SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); // Màu xám
                SDL_RenderFillRect(renderer, &Squares[rightNeighbor]);
            }
            SDL_RenderPresent(renderer);
        }



        // Done Eating
        printf("Philosopher %d is done eating\n", i);
        this_thread::sleep_for(chrono::milliseconds(rand() % 5000));
        dp.Putdown(i);

        {
            std::lock_guard<std::mutex> lock(stateMutex);
            // Cập nhật màu sắc của ô vuông của triết gia khi suy tư (thinking)
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Màu đen
            SDL_RenderFillRect(renderer, &Squares[i]);
            SDL_RenderPresent(renderer);
        }

        // ...
    }
}


//void startPhilosophers(thread philosophers[NUM_PHILOSOPHERS], bool& isRunning, bool& shouldExit) {
//    if (!isRunning) {
//        isRunning = true;
//        shouldExit = false;
//        for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
//            philosophers[i] = thread(philosopher, i, renderer, Squares, ref(shouldExit));
//        }
//    }
//    else {
//        isRunning = false;
//        shouldExit = true;
//        for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
//            philosophers[i].join();
//        }
//        // Reset philosopher states here (set them all to thinking, for example)
//        for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
//            state[i] = thinking;
//        }
//        // Update the graphics to reflect the thinking state
//        // ...
//    }
//}



//Hàm in văn bản
void renderText(SDL_Renderer* renderer, int x, int y, const std::string& text, SDL_Color color, TTF_Font* font) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect textRect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &textRect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

//Hàm khởi tạo các thư viện SDL
int Init()
{
    //Khởi tạo SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cerr << "SDL initialization error: " << SDL_GetError() << endl;
        return 1;
    }
    if (TTF_Init() < 0) {
        std::cerr << "SDL_ttf initialization failed: " << TTF_GetError() << std::endl;
        return 1;
    }
    // Khởi tạo thư viện SDL_Image
    if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) < 0) {
        std::cerr << "SDL_Image initialization failed: " << IMG_GetError() << std::endl;
        return 1;
    }
    return 0;
}
int main(int argc, char* args[]) {

    Init();
    SDL_Window* window = SDL_CreateWindow("Dining Philosophers", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 800, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        cerr << "Window creation error: " << SDL_GetError() << endl;
        return 1;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        cerr << "Renderer creation error: " << SDL_GetError() << endl;
        return 1;
    }
    TTF_Font* font = TTF_OpenFont("arialbd.ttf", 12);
    TTF_Font* font1 = TTF_OpenFont("arialbd.ttf", 15);
    if (font == nullptr || font1 == nullptr) {
        std::cerr << "Không thể mở font" << std::endl;
        return 1;
    }


    SDL_SetRenderDrawColor(renderer, 76, 175, 80, 1);
    SDL_RenderClear(renderer);//Thiết lập màu cho background
    SDL_Color textColor = { 255, 255, 255, 255 };// Thiết lập màu chữ


    // Load ảnh bàn ăn của triết gia lên màn hình
    SDL_Surface* imageSurface = IMG_Load("table.png");
    if (imageSurface == nullptr) {
        std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
        return 1;
    }

    SDL_SetColorKey(imageSurface, SDL_TRUE, SDL_MapRGB(imageSurface->format, 255, 255, 255));
    SDL_Texture* imageTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    if (imageTexture == nullptr)
    {
        std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
        return 1;
    }

    SDL_FreeSurface(imageSurface); // Giải phóng bộ nhớ sau khi tạo texture

    SDL_Rect imageRect = { 250,300,300,300 };
    SDL_RenderCopy(renderer, imageTexture, NULL, &imageRect);

    //Chú thích cho các trạng thái của các triết gia

    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
    SDL_Rect Comment_block = { 700,100,50,40 };
    SDL_RenderFillRect(renderer, &Comment_block);
    renderText(renderer, 780, 110, "Hungry", textColor, font1);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect Comment_block1 = { 700,150,50,40 };
    SDL_RenderFillRect(renderer, &Comment_block1);
    renderText(renderer, 780, 160, "Eating", textColor, font1);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_Rect Comment_block2 = { 700,200,50,40 };
    SDL_RenderFillRect(renderer, &Comment_block2);
    renderText(renderer, 780, 210, "Thinking", textColor, font1);


    int circleX = 250 + 300 / 2;
    int circleY = 300 + 300 / 2;
    int circleRadius = 200; // Bán kính bàn hình tròn
    int i = 0;
    SDL_Rect Squares[5];
    for (int angle = -18; angle < 342; angle += 72) { // Vẽ 5 ô vuông xung quanh
        double radians = (angle * M_PI) / 180.0;//góc xoay
        int squareSize = 50;//kích thước ô vuông

        //Tọa độ x,y của từng ô vuông
        int squareX = circleX + circleRadius * std::cos(radians) - squareSize / 2;
        int squareY = circleY + circleRadius * std::sin(radians) - squareSize / 2;

        Squares[i] = { squareX, squareY, squareSize, squareSize };

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Màu đen
        SDL_RenderFillRect(renderer, &Squares[i]);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        renderText(renderer, squareX + 20, squareY + 20, to_string(i + 1), textColor, font);
        i++;
    }


    // Vẽ nút "Start"
    SDL_Rect startButton = { 100, 100, 100, 50 };
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Màu xanh lá cây
    SDL_RenderFillRect(renderer, &startButton);
    renderText(renderer, 120, 120, "Start", textColor, font);

    SDL_RenderPresent(renderer);

    bool running = true;
    srand(time(nullptr));

    thread philosophers[NUM_PHILOSOPHERS];//tạo 5 luồng chạy ( tượng trưng cho 5 triết gia)

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false; break;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);//Lấy tọa độ nhấp chuột trên màn hình
                if (mouseX >= startButton.x && mouseX <= startButton.x + startButton.w &&
                    mouseY >= startButton.y && mouseY <= startButton.y + startButton.h)
                {
                    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
                        philosophers[i] = thread(philosopher, i, renderer, Squares);
                    }
                }
                
            }
        }
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
