#include "audio.h"
#include <raylib.h>

int main() {
    PaStream* stream;
    AudioData data;
    start_audio_server(stream, &data);

    InitWindow(1200, 900, "visualizer");
    SetTargetFPS(60);

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLANK);
        DrawCircle(400, 450, data.left * 200, RED);
        DrawCircle(800, 450, data.right * 200, RED);
        EndDrawing();
    }
    close_audio_server(stream);
    return 0;
}
