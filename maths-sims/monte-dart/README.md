# Monte Carlo Dart Simulation 🎯

A simple, visual simulation of the **Monte Carlo method** to estimate the value of π (pi), using **C++** and **raylib**. Darts are randomly "thrown" into a square board, and the ratio of darts that land inside the circle vs the total is used to approximate π.

> Visual + educational = fun!

---

## 🔧 Features

- Uses the **Monte Carlo method** to approximate π
- Real-time graphical output using **raylib**
- Darts are drawn incrementally — watch π converge visually!
- Colored dart hits: 🟢 = inside circle, 🔴 = outside
- Progress bar, π estimate, error display
- Rendered using a persistent render texture for performance and visual clarity

---

## 📷 Preview

![preview](preview.jpg)

---

## 🧠 How it works

1. A circle is inscribed inside a square.
2. Random "dart" coordinates are generated within the square.
3. If the dart falls inside the circle, it's a "hit".
4. The ratio of hits to total darts approximates π:
