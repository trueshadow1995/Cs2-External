#pragma once
#include <cstdint>
#include <iostream>

class c_color {
 public:
  float r, g, b, a;

  c_color(float _r, float _g, float _b, float _a) {
    r = _r;
    g = _g;
    b = _b;
    a = _a;
  }

  c_color(uint32_t color) {
    this->a = (color >> 24) & 0xff;
    this->r = (color >> 16) & 0xff;
    this->g = (color >> 8) & 0xff;
    this->b = (color & 0xff);
  }

  static c_color from_hsb(float flHue, float flSaturation, float flBrightness) {
    const float h = std::fmodf(flHue, 1.0f) / (60.0f / 360.0f);
    const int i = static_cast<int>(h);
    const float f = h - static_cast<float>(i);
    const float p = flBrightness * (1.0f - flSaturation);
    const float q = flBrightness * (1.0f - flSaturation * f);
    const float t = flBrightness * (1.0f - flSaturation * (1.0f - f));

    float r = 0.0f, g = 0.0f, b = 0.0f;

    switch (i) {
      case 0:
        r = flBrightness, g = t, b = p;
        break;
      case 1:
        r = q, g = flBrightness, b = p;
        break;
      case 2:
        r = p, g = flBrightness, b = t;
        break;
      case 3:
        r = p, g = q, b = flBrightness;
        break;
      case 4:
        r = t, g = p, b = flBrightness;
        break;
      case 5:
      default:
        r = flBrightness, g = p, b = q;
        break;
    }

    return c_color(r, g, b, 255);
  }

  void random_color(int iTick) { this->r = sin(3.f * iTick + 0.f) * 127 + 128; }
};