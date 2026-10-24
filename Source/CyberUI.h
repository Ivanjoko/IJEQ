#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "AppSettings.h"

//==============================================================================
//  Estilo cyberpunk con relieve. Los colores del tema siguen al color de acento
//  elegido en los ajustes (applyTheme()), de modo que cambia todo el plugin.
//==============================================================================
namespace Cyber
{
    // ---- colores del tema (se recalculan con applyTheme)
    inline juce::Colour cyan    { 0xff00f0ff };   // color primario (acento)
    inline juce::Colour magenta { 0xffff2bd6 };   // color secundario
    inline juce::Colour bg      { 0xff07050f };
    inline juce::Colour panel   { 0xff110a1f };
    inline juce::Colour dim     { 0xff2a1f44 };
    inline juce::Colour text    { 0xffcfe9ff };
    inline juce::Colour textDim { 0xff8a82b8 };
    inline float hue = 0.5f;                       // tono del tema (0..1)

    // ---- colores fijos
    inline const juce::Colour yellow { 0xfffcee0a };
    inline const juce::Colour green  { 0xff39ff14 };
    inline const juce::Colour orange { 0xffff7a00 };
    inline const juce::Colour violet { 0xffb026ff };

    // Tono del tema con el brillo y la saturación indicados (para fondos, degradados, etc.)
    inline juce::Colour tint (float brightness, float saturation = 0.5f, float alpha = 1.f)
    {
        return juce::Colour::fromHSV (hue, saturation, brightness, alpha);
    }

    // Paleta de acento: 10 colores + uno personalizado por tono
    inline juce::Colour accentColour (int idx, float customHue)
    {
        static const juce::uint32 p[10] = { 0xff00f0ff, 0xffff2bd6, 0xff39ff14, 0xffff7a00, 0xffb026ff,
                                            0xfffcee0a, 0xffff2a4d, 0xff2d6bff, 0xffff7ad9, 0xffd8f3ff };
        if (idx >= 0 && idx < 10)
            return juce::Colour (p[idx]);
        return juce::Colour::fromHSV (customHue, 0.85f, 1.f, 1.f);
    }

    inline void applyTheme()
    {
        const auto& st = AppSettings::get();
        const auto a = accentColour (st.accent, st.accentHue);
        hue = a.getHue();
        cyan    = a;
        magenta = juce::Colour::fromHSV (std::fmod (hue + 0.36f, 1.f), 0.85f, 1.f, 1.f);
        bg      = tint (0.055f, 0.55f);
        panel   = tint (0.11f, 0.50f);
        dim     = tint (0.22f, 0.45f);
        text    = juce::Colour::fromHSV (hue, 0.14f, 0.97f, 1.f);
        textDim = juce::Colour::fromHSV (hue, 0.30f, 0.64f, 1.f);
    }

    inline juce::Colour accent() { return cyan; }

    // Colores de las bandas (fijos: identifican cada banda)
    inline juce::Colour band (int b)
    {
        static const juce::uint32 c[6] = { 0xff00f0ff, 0xffff2bd6, 0xfffcee0a, 0xff39ff14, 0xffff7a00, 0xffb026ff };
        return juce::Colour (c[((b % 6) + 6) % 6]);
    }

    // Rectángulo con las esquinas superior izquierda e inferior derecha cortadas
    inline juce::Path chamfer (juce::Rectangle<float> r, float c)
    {
        juce::Path p;
        p.startNewSubPath (r.getX() + c, r.getY());
        p.lineTo (r.getRight(), r.getY());
        p.lineTo (r.getRight(), r.getBottom() - c);
        p.lineTo (r.getRight() - c, r.getBottom());
        p.lineTo (r.getX(), r.getBottom());
        p.lineTo (r.getX(), r.getY() + c);
        p.closeSubPath();
        return p;
    }

    // Tipografía: Futura en macOS (aspecto técnico / sci-fi), una fuente CJK para el chino.
    inline const juce::String& fontName()
    {
        static const juce::String latin =
           #if JUCE_MAC
            "Futura";
           #elif JUCE_WINDOWS
            "Segoe UI";
           #else
            juce::Font::getDefaultSansSerifFontName();
           #endif
        static const juce::String cjk =
           #if JUCE_MAC
            "PingFang SC";
           #elif JUCE_WINDOWS
            "Microsoft YaHei";
           #else
            juce::Font::getDefaultSansSerifFontName();
           #endif
        return AppSettings::get().language == 2 ? cjk : latin;
    }

    inline juce::FontOptions font (float h, bool bold = false)
    {
        return juce::FontOptions (fontName(), h, bold ? juce::Font::bold : juce::Font::plain);
    }

    inline juce::String u8 (const char* s) { return juce::String::fromUTF8 (s); }

    // Escala real de píxeles (pantalla retina + zoom de la ventana) para dibujar cachés nítidas
    inline float pixelScale (juce::Graphics& g)
    {
        return juce::jmax (1.f, (float) g.getInternalContext().getPhysicalPixelScaleFactor());
    }

    void drawBevelPanel (juce::Graphics&, juce::Rectangle<float>, juce::Colour neon, float cut = 12.f);
    void drawLogo (juce::Graphics&, juce::Rectangle<float> area);          // logotipo IJEQ
    void drawBackdrop (juce::Graphics&, int w, int h);                     // fondo decorativo
}

//==============================================================================
//  Botón con relieve y animación de pulsación (se hunde al apretarlo)
//==============================================================================
class CyberButton : public juce::Button, private juce::Timer
{
public:
    enum class Role { Fixed, Primary, Secondary };

    explicit CyberButton (const juce::String& name = {}) : juce::Button (name) {}

    void setNeon (juce::Colour c)  { neon = c; role = Role::Fixed; repaint(); }
    void setRole (Role r)          { role = r; repaint(); }                       // sigue al color del tema
    void setArrow (float dir)      { arrowDir = dir; hasArrow = true; repaint(); }   // 0 = derecha, .25 = abajo, .5 = izquierda, .75 = arriba
    void setGear()                 { gear = true; repaint(); }
    void setSmall (bool s)         { small = s; }

    void paintButton (juce::Graphics&, bool over, bool down) override;
    void buttonStateChanged() override;

private:
    juce::Colour currentNeon() const
    {
        return role == Role::Primary ? Cyber::cyan : role == Role::Secondary ? Cyber::magenta : neon;
    }
    void timerCallback() override;

    juce::Colour neon { 0xff00f0ff };
    Role role = Role::Primary;
    float arrowDir = 0.f, press = 0.f, hover = 0.f;
    bool hasArrow = false, gear = false, small = false;
};

//==============================================================================
//  Contenedor con panel en relieve (se dibuja en una imagen a la resolución real)
//==============================================================================
class Region : public juce::Component
{
public:
    explicit Region (bool framedPanel = false) : framed (framedPanel) {}

    void setFullSize (int w, int h) { if (getWidth() != w || getHeight() != h) setSize (w, h); }
    void setPanelOpacity (float a)  { opacity = a; repaint(); }
    void refreshTheme()             { cache = {}; repaint(); }
    void paint (juce::Graphics&) override;
    std::function<void (juce::Graphics&)> paintOver;

private:
    bool framed;
    juce::Image cache;
    float cacheScale = 0.f, opacity = 1.f;
};

//==============================================================================
//  Lienzo con pintado personalizado (contenedor del contenido escalable del editor)
//==============================================================================
class Canvas : public juce::Component
{
public:
    void paint (juce::Graphics& g) override { if (onPaint) onPaint (g); }
    std::function<void (juce::Graphics&)> onPaint;
};

//==============================================================================
//  Bordes para cambiar el tamaño de la ventana (derecho, inferior y esquina)
//==============================================================================
class EdgeResizer : public juce::Component
{
public:
    enum class Kind { Right, Bottom, Corner };

    EdgeResizer (juce::AudioProcessorEditor& e, Kind k, int minWidth, int minHeight, int maxWidth, int maxHeight);

    void paint (juce::Graphics&) override;
    void mouseDown (const juce::MouseEvent&) override;
    void mouseDrag (const juce::MouseEvent&) override;
    void mouseEnter (const juce::MouseEvent&) override { hover = true; repaint(); }
    void mouseExit (const juce::MouseEvent&) override  { hover = false; repaint(); }

private:
    juce::AudioProcessorEditor& editor;
    Kind kind;
    int minW, minH, maxW, maxH, startW = 0, startH = 0;
    juce::Point<int> startMouse;
    bool hover = false;
};

//==============================================================================
class CyberLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CyberLookAndFeel();
    void refreshFont();          // tras cambiar de idioma
    void refreshColours();       // tras cambiar de tema

    void drawRotarySlider (juce::Graphics&, int x, int y, int w, int h, float pos, float a0, float a1, juce::Slider&) override;
    void drawLinearSlider (juce::Graphics&, int x, int y, int w, int h, float sliderPos, float minPos, float maxPos,
                           juce::Slider::SliderStyle, juce::Slider&) override;
    void drawComboBox (juce::Graphics&, int w, int h, bool, int, int, int, int, juce::ComboBox&) override;
};
