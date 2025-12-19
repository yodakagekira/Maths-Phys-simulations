#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <string>
#include <cmath>

#include "rng.hpp"
#include "fin.hpp"
#include "plot.hpp"

static float clampf(float x, float a, float b) { return (x < a) ? a : (x > b) ? b : x; }

// Simple slider with label on left, value on right
static bool SliderFloat(const char* label, Rectangle bounds, float* v, float vmin, float vmax) {
    // bounds: full row rectangle (label area + slider area)
    const float labelWidth = 80.0f;
    Rectangle labelRect = { bounds.x, bounds.y, labelWidth, bounds.height };
    Rectangle sliderRect = { bounds.x + labelWidth + 10, bounds.y, bounds.width - labelWidth - 100, bounds.height };

    DrawText(label, (int)labelRect.x + 4, (int)(labelRect.y + 6), 16, LIGHTGRAY);

    // Slider background
    DrawRectangleRec(sliderRect, (Color){40,40,40,255});
    DrawRectangleLinesEx(sliderRect, 1.0f, DARKGRAY);

    float t = (*v - vmin) / (vmax - vmin);
    t = clampf(t, 0.0f, 1.0f);
    float knobX = sliderRect.x + t * sliderRect.width;
    DrawCircle((int)knobX, (int)(sliderRect.y + sliderRect.height/2), 9, RAYWHITE);

    // Value text
    DrawText(TextFormat("%.4f", *v), (int)(sliderRect.x + sliderRect.width + 10), (int)(sliderRect.y + 6), 16, RAYWHITE);

    bool changed = false;
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        Vector2 m = GetMousePosition();
        if (CheckCollisionPointRec(m, sliderRect)) {
            float nt = (m.x - sliderRect.x) / sliderRect.width;
            nt = clampf(nt, 0.0f, 1.0f);
            float nv = vmin + nt * (vmax - vmin);
            if (std::fabs(nv - *v) > 1e-6f) { *v = nv; changed = true; }
        }
    }
    return changed;
}

static bool SliderInt(const char* label, Rectangle bounds, int* v, int vmin, int vmax) {
    float fv = (float)*v;
    bool changed = SliderFloat(label, bounds, &fv, (float)vmin, (float)vmax);
    int nv = (int)std::round(fv);
    nv = Clamp(nv, vmin, vmax);
    if (nv != *v) { *v = nv; changed = true; }
    return changed;
}

int main() {
    const int SCREEN_W = 1600;
    const int SCREEN_H = 900;
    InitWindow(SCREEN_W, SCREEN_H, "Monte Carlo Option Pricing (GBM) - raylib");
    SetTargetFPS(120);

    RNG rng(1234567);

    // Parameters
    float S_0 = 100.0f;
    float K  = 100.0f;
    float r  = 0.05f;
    float sigma = 0.20f;
    float T = 1.0f;

    int steps = 252;
    int paths = 200000;
    int fan_paths = 80;

    bool antithetic = true;
    OptionType type = OptionType::Call;

    // Convergence history
    std::vector<float> conv;

    auto resimulate_convergence = [&](int points) {
        conv.clear();
        conv.reserve(points);
        int N = 100;
        while ((int)conv.size() < points && N <= paths) {
            auto rr = price_european_mc(type, S_0, K, r, sigma, T, steps, N, antithetic, rng);
            conv.push_back((float)rr.price);
            N *= 2;
        }
        // Ensure at least 2 points for plotting
        while (conv.size() < 2) conv.push_back(conv.empty() ? 0.0f : conv.back());
    };

    auto make_fan = [&]() {
        std::vector<std::vector<float>> fan(fan_paths, std::vector<float>(steps + 1));
        const double dt = (double)T / steps;
        for (int p = 0; p < fan_paths; ++p) {
            double S = S_0;
            fan[p][0] = (float)S;
            for (int i = 1; i <= steps; ++i) {
                S = gbm_step_exact(S, r, sigma, dt, rng.Z());
                fan[p][i] = (float)S;
            }
        }
        return fan;
    };

    bool dirty = true;
    MCResult last{};
    std::vector<std::vector<float>> fan;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground((Color){15, 15, 20, 255});

        const float LEFT_PANEL_X = 30;
        const float LEFT_PANEL_W = 380;
        const float ROW_H = 36;
        float y = 60;

        // Title
        DrawText("Monte Carlo European Option Pricing (GBM)", (int)LEFT_PANEL_X, 20, 24, RAYWHITE);

        // === Black-Scholes Parameters ===
        DrawText("Model Parameters", (int)LEFT_PANEL_X, (int)y, 20, (Color){180,180,220,255});
        y += 30;
        DrawLine((int)LEFT_PANEL_X, (int)y, (int)(LEFT_PANEL_X + LEFT_PANEL_W), (int)y, (Color){60,60,80,255});
        y += 20;

        Rectangle row = { LEFT_PANEL_X, y, LEFT_PANEL_W, ROW_H };
        dirty |= SliderFloat("S₀", row, &S_0, 1.0f, 500.0f); y += ROW_H + 10;
        row.y = y;
        dirty |= SliderFloat("K", row, &K, 1.0f, 500.0f); y += ROW_H + 10;
        row.y = y;
        dirty |= SliderFloat("r", row, &r, 0.0f, 0.30f); y += ROW_H + 10;
        row.y = y;
        dirty |= SliderFloat("σ", row, &sigma, 0.01f, 1.0f); y += ROW_H + 10;
        row.y = y;
        dirty |= SliderFloat("T", row, &T, 0.05f, 10.0f); y += ROW_H + 20;

        // === Simulation Parameters ===
        DrawText("Simulation Settings", (int)LEFT_PANEL_X, (int)y, 20, (Color){180,180,220,255});
        y += 30;
        DrawLine((int)LEFT_PANEL_X, (int)y, (int)(LEFT_PANEL_X + LEFT_PANEL_W), (int)y, (Color){60,60,80,255});
        y += 20;

        row.y = y;
        dirty |= SliderInt("Steps", row, &steps, 10, 2000); y += ROW_H + 10;
        row.y = y;
        dirty |= SliderInt("Paths", row, &paths, 1000, 1000000); y += ROW_H + 10;
        row.y = y;
        dirty |= SliderInt("Fan Paths", row, &fan_paths, 10, 300); y += ROW_H + 30;

        // === Options & Buttons ===
        // Antithetic toggle
        Rectangle btnAnti = { LEFT_PANEL_X, y, 170, 40 };
        Color antiColor = antithetic ? (Color){70, 140, 70, 255} : (Color){60, 60, 70, 255};
        DrawRectangleRec(btnAnti, antiColor);
        DrawRectangleLinesEx(btnAnti, 2, antithetic ? (Color){100,200,100,255} : DARKGRAY);
        DrawText("Antithetic", (int)(btnAnti.x + 20), (int)(btnAnti.y + 12), 18, RAYWHITE);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), btnAnti)) {
            antithetic = !antithetic;
            dirty = true;
        }

        // Call/Put toggle
        Rectangle btnType = { LEFT_PANEL_X + 190, y, 170, 40 };
        DrawRectangleRec(btnType, (Color){70, 70, 120, 255});
        DrawRectangleLinesEx(btnType, 2, (Color){100,100,200,255});
        DrawText(type == OptionType::Call ? "Type: Call" : "Type: Put",
                 (int)(btnType.x + 25), (int)(btnType.y + 12), 18, RAYWHITE);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), btnType)) {
            type = (type == OptionType::Call) ? OptionType::Put : OptionType::Call;
            dirty = true;
        }

        y += 60;

        // Resimulate button
        Rectangle btnResim = { LEFT_PANEL_X, y, LEFT_PANEL_W, 50 };
        DrawRectangleRec(btnResim, (Color){90, 90, 160, 255});
        DrawRectangleLinesEx(btnResim, 2, (Color){130,130,220,255});
        DrawText("Resimulate", (int)(btnResim.x + 110), (int)(btnResim.y + 15), 20, RAYWHITE);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), btnResim)) {
            dirty = true;
        }

        // === Results Panel ===
        y += 80;
        Rectangle resultBox = { LEFT_PANEL_X, y, LEFT_PANEL_W, 140 };
        DrawRectangleRec(resultBox, (Color){30,30,45,255});
        DrawRectangleLinesEx(resultBox, 2, (Color){80,80,120,255});

        DrawText("Monte Carlo Result", (int)(resultBox.x + 10), (int)(resultBox.y + 12), 20, (Color){200,200,255,255});

        float ry = resultBox.y + 50;
        DrawText(TextFormat("Price:      %.6f", last.price), (int)(resultBox.x + 20), (int)ry, 18, RAYWHITE); ry += 28;
        DrawText(TextFormat("Std Err:    %.6f", last.std_err), (int)(resultBox.x + 20), (int)ry, 18, LIGHTGRAY); ry += 28;
        DrawText(TextFormat("95%% CI:     [%.6f, %.6f]", last.ci_lo, last.ci_hi),
                 (int)(resultBox.x + 20), (int)ry, 18, LIGHTGRAY);

        // === Plots ===
        const float PLOT_X = 440;
        const float PLOT_W = SCREEN_W - PLOT_X - 40;
        const float FAN_H = 380;
        const float CONV_H = 320;

        Rectangle plotFan = { PLOT_X, 80, PLOT_W, FAN_H };
        Rectangle plotConv = { PLOT_X, 80 + FAN_H + 50, PLOT_W, CONV_H };

        if (dirty) {
            last = price_european_mc(type, S_0, K, r, sigma, T, steps, paths, antithetic, rng);
            fan = make_fan();
            resimulate_convergence(20);
            dirty = false;
        }

        // Fan chart
        DrawText("GBM Paths (risk-neutral measure)", (int)plotFan.x + 100, (int)plotFan.y - 30, 20, (Color){200,220,200,255});
        float ymin = S_0, ymax = S_0;
        for (const auto& p : fan)
            for (float v : p) {
                ymin = std::min(ymin, v);
                ymax = std::max(ymax, v);
            }
        ymin *= 0.95f; ymax *= 1.05f; // padding
        draw_axes(plotFan, "Time (years)", "Asset Price S(t)", 0.0f, T, ymin, ymax, 6, 6);
        draw_paths_fan(fan, plotFan);

        // Convergence plot
        DrawText("Convergence of Option", (int)plotConv.x + 50, (int)plotConv.y - 30, 20, (Color){200,220,200,255});
        if (!conv.empty()) {
            float cmin = *std::min_element(conv.begin(), conv.end());
            float cmax = *std::max_element(conv.begin(), conv.end());
            float pad = (cmax - cmin) * 0.1f;
            if (pad < 1e-6f) pad = 0.01f;
            draw_axes(plotConv, "Number of Paths (N, log scale approx.)", "Estimated Price",
                      100.0f, (float)paths, cmin - pad, cmax + pad, 6, 6);
            draw_line_series(conv, plotConv);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}