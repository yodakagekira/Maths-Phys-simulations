#pragma once
#include <vector>
#include <algorithm>
#include <raylib.h>

inline void draw_axes(
    Rectangle r,
    const char* xlabel,
    const char* ylabel,
    float xmin, float xmax,
    float ymin, float ymax,
    int xticks = 5,
    int yticks = 5
) {
    DrawRectangleLinesEx(r, 1.0f, DARKGRAY);

    // axes
    DrawLine((int)r.x, (int)(r.y + r.height), (int)(r.x + r.width), (int)(r.y + r.height), GRAY);
    DrawLine((int)r.x, (int)r.y, (int)r.x, (int)(r.y + r.height), GRAY);

    // x ticks
    for (int i = 0; i <= xticks; ++i) {
        float t = (float)i / xticks;
        float x = r.x + t * r.width;
        float v = xmin + t * (xmax - xmin);

        DrawLine((int)x, (int)(r.y + r.height),
                 (int)x, (int)(r.y + r.height + 6), GRAY);
        DrawText(TextFormat("%.2f", v),
                 (int)x - 12, (int)(r.y + r.height + 10),
                 12, LIGHTGRAY);
    }

    // y ticks
    for (int i = 0; i <= yticks; ++i) {
        float t = (float)i / yticks;
        float y = r.y + r.height * (1.0f - t);
        float v = ymin + t * (ymax - ymin);

        DrawLine((int)(r.x - 6), (int)y, (int)r.x, (int)y, GRAY);
        DrawText(TextFormat("%.1f", v), (int)(r.x - 42), (int)y - 6, 12, LIGHTGRAY);
    }

    // labels
    DrawText(xlabel, (int)(r.x + r.width / 2 - 30), (int)(r.y + r.height + 30), 14, RAYWHITE);

    DrawText(ylabel, (int)(r.x - 60), (int)(r.y - 24), 14, RAYWHITE);
}


inline void draw_line_series(const std::vector<float>& y, Rectangle r) {
    if (y.size() < 2) return;

    float ymin = *std::min_element(y.begin(), y.end());
    float ymax = *std::max_element(y.begin(), y.end());
    if (ymax - ymin < 1e-9f) { ymax = ymin + 1.0f; }

    auto X = [&](int i) {
        return r.x + (r.width * (float)i) / (float)(y.size() - 1);
    };
    auto Y = [&](float v) {
        float t = (v - ymin) / (ymax - ymin);
        return r.y + r.height * (1.0f - t);
    };

    for (int i = 0; i < (int)y.size() - 1; ++i) {
        DrawLineV({X(i), Y(y[i])}, {X(i + 1), Y(y[i + 1])}, RAYWHITE);
    }
}

inline void draw_paths_fan(const std::vector<std::vector<float>>& paths, Rectangle r) {
    if (paths.empty() || paths[0].size() < 2) return;

    // Determine global min/max for scaling
    float ymin = paths[0][0], ymax = paths[0][0];
    for (auto& p : paths) {
        for (float v : p) { ymin = std::min(ymin, v); ymax = std::max(ymax, v); }
    }
    if (ymax - ymin < 1e-6f) ymax = ymin + 1.0f;

    const int steps = (int)paths[0].size();

    auto X = [&](int i) {
        return r.x + (r.width * (float)i) / (float)(steps - 1);
    };
    auto Y = [&](float v) {
        float t = (v - ymin) / (ymax - ymin);
        return r.y + r.height * (1.0f - t);
    };

    for (auto& p : paths) {
        for (int i = 0; i < steps - 1; ++i) {
            DrawLineV({X(i), Y(p[i])}, {X(i + 1), Y(p[i + 1])}, (Color){200,200,200,80});
        }
    }
}
