#include "CyberUI.h"

//==============================================================================
//  Panel con relieve
//==============================================================================
void Cyber::drawBevelPanel (juce::Graphics& g, juce::Rectangle<float> r, juce::Colour neon, float cut)
{
    auto p = chamfer (r, cut);

    for (int i = 4; i >= 1; --i)                       // sombra de relieve sin desenfoque (rápida)
    {
        g.setColour (juce::Colours::black.withAlpha (0.14f));
        g.strokePath (p, juce::PathStrokeType ((float) i * 2.4f), juce::AffineTransform::translation (0.f, 2.f + (float) i * 0.6f));
    }

    g.setGradientFill (juce::ColourGradient (tint (0.20f, 0.5f), 0.f, r.getY(), tint (0.075f, 0.55f), 0.f, r.getBottom(), false));
    g.fillPath (p);

    g.setColour (text.withAlpha (0.09f));
    g.drawLine (r.getX() + cut, r.getY() + 1.5f, r.getRight() - 2.f, r.getY() + 1.5f, 1.2f);
    g.setColour (juce::Colours::black.withAlpha (0.45f));
    g.drawLine (r.getX() + 2.f, r.getBottom() - 1.5f, r.getRight() - cut, r.getBottom() - 1.5f, 1.5f);

    g.setColour (neon.withAlpha (0.14f));
    g.strokePath (p, juce::PathStrokeType (5.f));
    g.setColour (neon.withAlpha (0.70f));
    g.strokePath (p, juce::PathStrokeType (1.2f));
}

//==============================================================================
//  Logotipo IJEQ: letras cursivas con esquinas biseladas, degradado y brillo,
//  sombra desplazada en el color secundario e icono de barras de ecualizador.
//==============================================================================
void Cyber::drawLogo (juce::Graphics& g, juce::Rectangle<float> area)
{
    const auto c1 = cyan, c2 = magenta;

    const float H = area.getHeight() * 0.74f;
    const float t = H * 0.21f, gap = H * 0.20f;
    const float y = area.getY() + (area.getHeight() - H) * 0.5f;

    // ---- icono: marco biselado con barras de ecualizador
    const auto icon = juce::Rectangle<float> (area.getX() + 2.f, y - H * 0.06f, H * 1.12f, H * 1.12f);
    {
        auto frame = chamfer (icon, 7.f);
        g.setColour (c1.withAlpha (0.20f));
        g.strokePath (frame, juce::PathStrokeType (5.f));
        g.setColour (c1);
        g.strokePath (frame, juce::PathStrokeType (1.6f));

        const float hs[5] = { 0.42f, 0.82f, 0.58f, 1.0f, 0.34f };
        const auto inner = icon.reduced (icon.getWidth() * 0.18f, icon.getHeight() * 0.18f);
        const float bw = inner.getWidth() / 9.f;
        for (int i = 0; i < 5; ++i)
        {
            const float bh = inner.getHeight() * hs[i];
            const auto bar = juce::Rectangle<float> (inner.getX() + (float) (i * 2) * bw, inner.getBottom() - bh, bw, bh);
            g.setGradientFill (juce::ColourGradient (c1, 0.f, bar.getY(), c2, 0.f, bar.getBottom(), false));
            g.fillRect (bar);
        }
    }

    // ---- letras
    float x = icon.getRight() + 12.f;
    juce::Path letters;

    auto add = [&letters] (const juce::Path& centre, float width)
    {
        juce::Path stroked;
        juce::PathStrokeType (width, juce::PathStrokeType::beveled, juce::PathStrokeType::butt).createStrokedPath (stroked, centre);
        letters.addPath (stroked);
    };

    {   // I
        juce::Path p;
        p.startNewSubPath (x + t * 0.5f, y);
        p.lineTo (x + t * 0.5f, y + H);
        add (p, t);
        x += t + gap;
    }
    {   // J
        const float w = H * 0.62f;
        juce::Path p;
        p.startNewSubPath (x + t * 0.5f, y + H * 0.66f);
        p.lineTo (x + t * 0.5f, y + H - t * 0.5f);
        p.lineTo (x + w - t * 0.5f, y + H - t * 0.5f);
        p.lineTo (x + w - t * 0.5f, y);
        add (p, t);
        x += w + gap;
    }
    {   // E
        const float w = H * 0.62f;
        juce::Path p;
        p.startNewSubPath (x + w, y + t * 0.5f);
        p.lineTo (x + t * 0.5f, y + t * 0.5f);
        p.lineTo (x + t * 0.5f, y + H - t * 0.5f);
        p.lineTo (x + w, y + H - t * 0.5f);
        p.startNewSubPath (x + t * 0.5f, y + H * 0.5f);
        p.lineTo (x + w * 0.84f, y + H * 0.5f);
        add (p, t);
        x += w + gap;
    }
    {   // Q (octógono con cola)
        const float w = H * 0.80f, c = t * 1.3f, h2 = t * 0.5f;
        juce::Path o;
        o.startNewSubPath (x + c, y + h2);
        o.lineTo (x + w - c, y + h2);
        o.lineTo (x + w - h2, y + c);
        o.lineTo (x + w - h2, y + H - c);
        o.lineTo (x + w - c, y + H - h2);
        o.lineTo (x + c, y + H - h2);
        o.lineTo (x + h2, y + H - c);
        o.lineTo (x + h2, y + c);
        o.closeSubPath();
        add (o, t);

        juce::Path tail;
        tail.startNewSubPath (x + w * 0.52f, y + H * 0.60f);
        tail.lineTo (x + w + t * 0.7f, y + H + t * 0.4f);
        juce::Path tailStroked;
        juce::PathStrokeType (t * 0.95f, juce::PathStrokeType::mitered, juce::PathStrokeType::butt).createStrokedPath (tailStroked, tail);
        letters.addPath (tailStroked);
    }

    // cursiva: la parte de arriba se desplaza a la derecha
    const float k = 0.20f;
    letters.applyTransform (juce::AffineTransform::shear (-k, 0.f).translated (k * (y + H), 0.f));

    // sombra desplazada (color secundario)
    {
        juce::Path ghost (letters);
        ghost.applyTransform (juce::AffineTransform::translation (2.6f, 2.2f));
        g.setColour (c2.withAlpha (0.60f));
        g.fillPath (ghost);
    }

    // brillo neón
    g.setColour (c1.withAlpha (0.14f));
    g.strokePath (letters, juce::PathStrokeType (8.f));
    g.setColour (c1.withAlpha (0.28f));
    g.strokePath (letters, juce::PathStrokeType (3.5f));

    // relleno con degradado y filo claro
    g.setGradientFill (juce::ColourGradient (c1.brighter (0.35f), 0.f, y, c1.interpolatedWith (c2, 0.55f), 0.f, y + H, false));
    g.fillPath (letters);
    g.setColour (text.withAlpha (0.55f));
    g.strokePath (letters, juce::PathStrokeType (0.8f));
}

//==============================================================================
//  Fondo decorativo (rejilla hexagonal, circuitos, resplandores y franjas)
//==============================================================================
void Cyber::drawBackdrop (juce::Graphics& g, int w, int h)
{
    const float W = (float) w, H = (float) h;

    g.setGradientFill (juce::ColourGradient (tint (0.13f, 0.65f), 0.f, 0.f, tint (0.03f, 0.65f), 0.f, H, false));
    g.fillAll();

    auto glow = [&g] (float cx, float cy, float r, juce::Colour c, float a)
    {
        g.setGradientFill (juce::ColourGradient (c.withAlpha (a), cx, cy, c.withAlpha (0.f), cx + r, cy, true));
        g.fillEllipse (cx - r, cy - r, r * 2.f, r * 2.f);
    };
    glow (0.f, 0.f, H * 0.95f, magenta, 0.24f);
    glow (W, H, H * 0.95f, cyan, 0.24f);
    glow (W * 0.5f, 0.f, H * 0.55f, cyan, 0.10f);

    // rejilla hexagonal (3 aristas por celda: sin duplicados)
    {
        const float s = 20.f, dx = s * 1.5f, dy = s * 0.8660254f;
        const float a60 = juce::MathConstants<float>::pi / 3.f;
        juce::Path hex;
        for (int col = -1; (float) col * dx < W + s; ++col)
            for (int row = -1; (float) row * dy * 2.f < H + s * 2.f; ++row)
            {
                const float cx = (float) col * dx;
                const float cy = (float) row * dy * 2.f + ((col & 1) != 0 ? dy : 0.f);
                for (int k = 0; k < 3; ++k)
                {
                    const float a0 = a60 * (float) k, a1 = a0 + a60;
                    hex.startNewSubPath (cx + s * std::cos (a0), cy + s * std::sin (a0));
                    hex.lineTo (cx + s * std::cos (a1), cy + s * std::sin (a1));
                }
            }
        g.setColour (cyan.withAlpha (0.08f));
        g.strokePath (hex, juce::PathStrokeType (1.f));
    }

    // trazos de circuito con nodos
    {
        juce::Random rng (20240519);
        juce::Path traces, nodes, dots;
        for (int i = 0; i < 24; ++i)
        {
            float x = rng.nextFloat() * W, y = rng.nextFloat() * H;
            traces.startNewSubPath (x, y);
            bool horizontal = rng.nextBool();
            const int segs = 3 + rng.nextInt (4);
            for (int s = 0; s < segs; ++s)
            {
                const float len = 40.f + rng.nextFloat() * 170.f;
                const float dir = rng.nextBool() ? 1.f : -1.f;
                if (horizontal) x += dir * len; else y += dir * len;
                traces.lineTo (x, y);
                horizontal = ! horizontal;
            }
            nodes.addEllipse (x - 4.f, y - 4.f, 8.f, 8.f);
            dots.addEllipse (x - 1.6f, y - 1.6f, 3.2f, 3.2f);
        }
        g.setColour (cyan.withAlpha (0.15f));
        g.strokePath (traces, juce::PathStrokeType (1.5f));
        g.setColour (magenta.withAlpha (0.40f));
        g.strokePath (nodes, juce::PathStrokeType (1.4f));
        g.setColour (magenta.withAlpha (0.65f));
        g.fillPath (dots);
    }

    // franjas diagonales en el borde inferior
    {
        juce::Path stripes;
        for (float x = -H; x < W + H; x += 28.f)
        {
            stripes.startNewSubPath (x, H);
            stripes.lineTo (x + 13.f, H);
            stripes.lineTo (x + 23.f, H - 9.f);
            stripes.lineTo (x + 10.f, H - 9.f);
            stripes.closeSubPath();
        }
        g.setColour (cyan.withAlpha (0.13f));
        g.fillPath (stripes);
    }

    // esquinas con marcas
    {
        const float L = 34.f, m = 3.f;
        juce::Path br;
        br.startNewSubPath (m, L);         br.lineTo (m, m);             br.lineTo (L, m);
        br.startNewSubPath (W - L, m);     br.lineTo (W - m, m);         br.lineTo (W - m, L);
        br.startNewSubPath (m, H - L);     br.lineTo (m, H - m);         br.lineTo (L, H - m);
        br.startNewSubPath (W - L, H - m); br.lineTo (W - m, H - m);     br.lineTo (W - m, H - L);
        g.setColour (magenta.withAlpha (0.55f));
        g.strokePath (br, juce::PathStrokeType (2.f));
    }
}

//==============================================================================
//  CyberButton
//==============================================================================
void CyberButton::buttonStateChanged()
{
    if (! AppSettings::get().animations)
    {
        press = isDown() ? 1.f : 0.f;
        hover = isOver() ? 1.f : 0.f;
        repaint();
        return;
    }
    if (! isTimerRunning())
        startTimerHz (60);
}

void CyberButton::timerCallback()
{
    const float tp = isDown() ? 1.f : 0.f, th = isOver() ? 1.f : 0.f;
    press += (tp - press) * 0.45f;
    hover += (th - hover) * 0.30f;
    if (std::abs (tp - press) < 0.01f) press = tp;
    if (std::abs (th - hover) < 0.01f) hover = th;
    repaint();
    if (press == tp && hover == th)
        stopTimer();
}

void CyberButton::paintButton (juce::Graphics& g, bool, bool)
{
    const auto neonC = currentNeon();
    auto b = getLocalBounds().toFloat().reduced (1.5f);
    const bool on = getToggleState();
    const bool en = isEnabled();

    const float depth = 2.5f * (1.f - press);                         // altura del relieve
    auto face = b.withTrimmedBottom (depth).translated (0.f, press * 2.f);   // al pulsar se hunde

    if (depth > 0.2f)                                                 // zócalo oscuro bajo el botón
    {
        g.setColour (juce::Colours::black.withAlpha (0.7f));
        g.fillPath (Cyber::chamfer (b.withTrimmedTop (depth), 6.f));
    }

    auto path = Cyber::chamfer (face, 6.f);
    const float lum = 1.f + 0.20f * hover - 0.35f * press;

    juce::Colour top, bottom;
    if (on)
    {
        top    = neonC.withMultipliedBrightness (0.65f * lum);
        bottom = neonC.withMultipliedBrightness (0.28f * lum);
    }
    else
    {
        top    = Cyber::tint (0.30f, 0.45f).withMultipliedBrightness (lum);
        bottom = Cyber::tint (0.12f, 0.55f).withMultipliedBrightness (lum);
    }
    g.setGradientFill (juce::ColourGradient (top, 0.f, face.getY(), bottom, 0.f, face.getBottom(), false));
    g.fillPath (path);

    g.setColour (Cyber::text.withAlpha (0.22f * (1.f - press)));       // luz del borde superior
    g.drawLine (face.getX() + 8.f, face.getY() + 1.2f, face.getRight() - 3.f, face.getY() + 1.2f, 1.f);

    if (on)
    {
        g.setColour (neonC.withAlpha (0.22f));
        g.strokePath (path, juce::PathStrokeType (5.f));
    }
    g.setColour (on ? neonC : neonC.withAlpha (0.40f + 0.40f * hover));
    g.strokePath (path, juce::PathStrokeType (1.3f));

    if (gear)
    {
        const auto c = face.getCentre();
        const float r = juce::jmin (face.getWidth(), face.getHeight()) * 0.36f;
        g.setColour (neonC);
        for (int i = 0; i < 8; ++i)
        {
            juce::Path tooth;
            tooth.addRectangle (-r * 0.15f, -r, r * 0.30f, r * 0.34f);
            tooth.applyTransform (juce::AffineTransform::rotation (juce::MathConstants<float>::twoPi * (float) i / 8.f)
                                      .translated (c.x, c.y));
            g.fillPath (tooth);
        }
        g.drawEllipse (juce::Rectangle<float> (r * 1.28f, r * 1.28f).withCentre (c), 2.2f);
        g.fillEllipse (juce::Rectangle<float> (r * 0.44f, r * 0.44f).withCentre (c));
    }
    else if (hasArrow)
    {
        juce::Path tri;
        tri.addTriangle (-0.55f, -0.8f, -0.55f, 0.8f, 0.95f, 0.f);
        const float s = juce::jmin (face.getWidth(), face.getHeight()) * 0.30f;
        tri.applyTransform (juce::AffineTransform::rotation (arrowDir * juce::MathConstants<float>::twoPi)
                                .scaled (s, s).translated (face.getCentreX(), face.getCentreY()));
        g.setColour (on ? Cyber::text : neonC);
        g.fillPath (tri);
    }
    else
    {
        g.setColour (en ? Cyber::text : Cyber::textDim);
        g.setFont (Cyber::font (small ? 11.5f : 13.f, true));
        g.drawText (getButtonText(), face.toNearestInt().reduced (4, 0), juce::Justification::centred);
    }
}

//==============================================================================
//  Region: el panel en relieve se guarda en una imagen a la resolución real
//==============================================================================
void Region::paint (juce::Graphics& g)
{
    const int w = getWidth(), h = getHeight();
    if (framed && w > 0 && h > 0)
    {
        const float ps = Cyber::pixelScale (g);
        const int pw = juce::jmax (1, (int) std::ceil ((float) w * ps)), ph = juce::jmax (1, (int) std::ceil ((float) h * ps));

        if (! cache.isValid() || cache.getWidth() != pw || cache.getHeight() != ph || std::abs (ps - cacheScale) > 0.01f)
        {
            cacheScale = ps;
            cache = juce::Image (juce::Image::ARGB, pw, ph, true);
            juce::Graphics ig (cache);
            ig.addTransform (juce::AffineTransform::scale ((float) pw / (float) w, (float) ph / (float) h));
            Cyber::drawBevelPanel (ig, juce::Rectangle<float> (0.f, 0.f, (float) w, (float) h).reduced (5.f), Cyber::cyan, 12.f);
        }

        g.setOpacity (opacity);
        g.drawImage (cache, 0, 0, w, h, 0, 0, pw, ph);
        g.setOpacity (1.f);
    }
    if (paintOver)
        paintOver (g);
}

//==============================================================================
//  EdgeResizer
//==============================================================================
EdgeResizer::EdgeResizer (juce::AudioProcessorEditor& e, Kind k, int minWidth, int minHeight, int maxWidth, int maxHeight)
    : editor (e), kind (k), minW (minWidth), minH (minHeight), maxW (maxWidth), maxH (maxHeight)
{
    setMouseCursor (k == Kind::Right  ? juce::MouseCursor::LeftRightResizeCursor
                  : k == Kind::Bottom ? juce::MouseCursor::UpDownResizeCursor
                                      : juce::MouseCursor::BottomRightCornerResizeCursor);
}

void EdgeResizer::paint (juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();
    if (kind == Kind::Corner)
    {
        g.setColour (Cyber::cyan.withAlpha (hover ? 0.95f : 0.55f));
        for (int i = 0; i < 3; ++i)
        {
            const float o = 3.f + (float) i * 4.f;
            g.drawLine (b.getRight() - o, b.getBottom() - 1.5f, b.getRight() - 1.5f, b.getBottom() - o, 1.6f);
        }
    }
    else if (hover)
    {
        g.setColour (Cyber::cyan.withAlpha (0.35f));
        g.fillRect (b);
        g.setColour (Cyber::cyan);
        if (kind == Kind::Right) g.fillRect (b.getRight() - 2.f, b.getY(), 2.f, b.getHeight());
        else                     g.fillRect (b.getX(), b.getBottom() - 2.f, b.getWidth(), 2.f);
    }
}

void EdgeResizer::mouseDown (const juce::MouseEvent& e)
{
    startW = editor.getWidth();
    startH = editor.getHeight();
    startMouse = e.getScreenPosition();
}

void EdgeResizer::mouseDrag (const juce::MouseEvent& e)
{
    const auto d = e.getScreenPosition() - startMouse;
    const int w = kind == Kind::Bottom ? startW : juce::jlimit (minW, maxW, startW + d.x);
    const int h = kind == Kind::Right  ? startH : juce::jlimit (minH, maxH, startH + d.y);
    editor.setSize (w, h);
}

//==============================================================================
//  Look and feel
//==============================================================================
CyberLookAndFeel::CyberLookAndFeel()
{
    refreshFont();
    refreshColours();
}

void CyberLookAndFeel::refreshFont()
{
    setDefaultSansSerifTypefaceName (Cyber::fontName());
}

void CyberLookAndFeel::refreshColours()
{
    setColourScheme (juce::LookAndFeel_V4::getMidnightColourScheme());

    setColour (juce::ComboBox::textColourId, Cyber::text);
    setColour (juce::PopupMenu::backgroundColourId, Cyber::panel);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, Cyber::magenta.withAlpha (0.35f));
    setColour (juce::PopupMenu::textColourId, Cyber::text);
    setColour (juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
    setColour (juce::Slider::textBoxTextColourId, Cyber::text);
    setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    setColour (juce::Slider::textBoxHighlightColourId, Cyber::magenta.withAlpha (0.4f));
    setColour (juce::Label::textColourId, Cyber::text);
    setColour (juce::TextEditor::textColourId, Cyber::text);
    setColour (juce::ScrollBar::thumbColourId, Cyber::cyan.withAlpha (0.55f));
    setColour (juce::AlertWindow::textColourId, Cyber::text);
    setColour (juce::AlertWindow::backgroundColourId, Cyber::panel);
    setColour (juce::AlertWindow::outlineColourId, Cyber::cyan);
}

void CyberLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h, float pos,
                                         float a0, float a1, juce::Slider& s)
{
    auto bounds = juce::Rectangle<float> ((float) x, (float) y, (float) w, (float) h).reduced (4.f);
    const float radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
    const auto c = bounds.getCentre();
    const float aTo = a0 + pos * (a1 - a0);
    const bool en = s.isEnabled();

    // los diales del tema siguen al color de acento; los de banda usan el color de su banda
    auto neon = s.getProperties().getWithDefault ("primary", false) ? Cyber::cyan
                                                                     : s.findColour (juce::Slider::rotarySliderFillColourId);
    if (! en) neon = neon.withAlpha (0.35f);

    const auto outer = juce::Rectangle<float> (radius * 2.f, radius * 2.f).withCentre (c);
    g.setColour (juce::Colours::black.withAlpha (0.55f));
    g.fillEllipse (outer.translated (0.f, 2.5f));
    g.setGradientFill (juce::ColourGradient (Cyber::tint (0.42f, 0.40f), outer.getX(), outer.getY(),
                                             Cyber::tint (0.07f, 0.55f), outer.getRight(), outer.getBottom(), false));
    g.fillEllipse (outer);

    const float arcR = radius - 4.f;
    juce::Path track, val;
    track.addCentredArc (c.x, c.y, arcR, arcR, 0.f, a0, a1, true);
    val.addCentredArc   (c.x, c.y, arcR, arcR, 0.f, a0, aTo, true);
    g.setColour (Cyber::tint (0.05f, 0.6f));
    g.strokePath (track, juce::PathStrokeType (5.f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    g.setColour (neon.withAlpha (0.22f));
    g.strokePath (val, juce::PathStrokeType (9.f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    g.setColour (neon);
    g.strokePath (val, juce::PathStrokeType (3.f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    const auto body = outer.reduced (radius * 0.36f);
    g.setColour (juce::Colours::black.withAlpha (0.6f));
    g.fillEllipse (body.translated (0.f, 1.5f));
    g.setGradientFill (juce::ColourGradient (Cyber::tint (0.50f, 0.35f), body.getX(), body.getY(),
                                             Cyber::tint (0.12f, 0.50f), body.getX(), body.getBottom(), false));
    g.fillEllipse (body);
    g.setColour (Cyber::text.withAlpha (0.12f));
    g.drawEllipse (body.reduced (0.5f), 1.f);

    const auto p1 = c.getPointOnCircumference (radius * 0.56f, aTo);
    const auto p2 = c.getPointOnCircumference (radius * 0.20f, aTo);
    g.setColour (Cyber::text.withAlpha (en ? 1.f : 0.4f));
    g.drawLine (juce::Line<float> (p2, p1), 2.2f);
}

void CyberLookAndFeel::drawLinearSlider (juce::Graphics& g, int x, int y, int w, int h, float sliderPos,
                                         float minPos, float maxPos, juce::Slider::SliderStyle style, juce::Slider& s)
{
    if (style != juce::Slider::LinearHorizontal)
    {
        LookAndFeel_V4::drawLinearSlider (g, x, y, w, h, sliderPos, minPos, maxPos, style, s);
        return;
    }

    const float cy = (float) y + (float) h * 0.5f;
    const auto neon = s.getProperties().getWithDefault ("primary", false) ? Cyber::cyan
                                                                           : s.findColour (juce::Slider::trackColourId);

    g.setColour (Cyber::tint (0.04f, 0.6f));
    g.fillRoundedRectangle ((float) x, cy - 3.f, (float) w, 6.f, 3.f);
    g.setColour (Cyber::text.withAlpha (0.08f));
    g.drawLine ((float) x + 2.f, cy + 3.5f, (float) (x + w) - 2.f, cy + 3.5f, 1.f);

    g.setColour (neon.withAlpha (0.25f));
    g.fillRoundedRectangle ((float) x, cy - 6.f, sliderPos - (float) x, 12.f, 5.f);
    g.setColour (neon);
    g.fillRoundedRectangle ((float) x, cy - 2.5f, sliderPos - (float) x, 5.f, 2.5f);

    auto thumb = juce::Rectangle<float> (10.f, 18.f).withCentre ({ sliderPos, cy });
    g.setColour (juce::Colours::black.withAlpha (0.6f));
    g.fillRoundedRectangle (thumb.translated (0.f, 1.5f), 3.f);
    g.setGradientFill (juce::ColourGradient (Cyber::text, 0.f, thumb.getY(), Cyber::tint (0.55f, 0.35f), 0.f, thumb.getBottom(), false));
    g.fillRoundedRectangle (thumb, 3.f);
}

void CyberLookAndFeel::drawComboBox (juce::Graphics& g, int w, int h, bool, int, int, int, int, juce::ComboBox&)
{
    auto r = juce::Rectangle<float> (0.f, 0.f, (float) w, (float) h).reduced (1.5f);
    auto p = Cyber::chamfer (r, 5.f);

    g.setColour (juce::Colours::black.withAlpha (0.55f));
    g.fillPath (Cyber::chamfer (r.translated (0.f, 1.5f), 5.f));
    g.setGradientFill (juce::ColourGradient (Cyber::tint (0.24f, 0.45f), 0.f, r.getY(), Cyber::tint (0.10f, 0.55f), 0.f, r.getBottom(), false));
    g.fillPath (p);
    g.setColour (Cyber::text.withAlpha (0.12f));
    g.drawLine (r.getX() + 7.f, r.getY() + 1.2f, r.getRight() - 2.f, r.getY() + 1.2f, 1.f);
    g.setColour (Cyber::cyan.withAlpha (0.65f));
    g.strokePath (p, juce::PathStrokeType (1.2f));

    juce::Path arrow;
    const float ax = (float) w - 22.f, ay = (float) h * 0.5f - 3.f;
    arrow.addTriangle (ax, ay, ax + 10.f, ay, ax + 5.f, ay + 6.f);
    g.setColour (Cyber::cyan);
    g.fillPath (arrow);
}
