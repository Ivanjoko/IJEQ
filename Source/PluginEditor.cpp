#include "PluginEditor.h"
#include "Presets.h"

using EqMath::numBands;

static juce::String fmtHz (float f)
{
    if (f >= 1000.f) return juce::String (f / 1000.f, 3) + " kHz";
    if (f >= 100.f)  return juce::String (f, 1) + " Hz";
    return juce::String (f, 2) + " Hz";
}

static juce::String fmtTick (float f)
{
    const bool whole = std::abs (f - std::round (f)) < 1e-3f;
    if (f >= 1000.f)
    {
        const float k = f / 1000.f;
        return juce::String (k, std::abs (k - std::round (k)) < 1e-3f ? 0 : 1) + "k";
    }
    return juce::String (f, whole ? 0 : 1);
}

//==============================================================================
//  StereoMeter
//==============================================================================
void StereoMeter::setLevels (float rmsL, float rmsR, float pkL, float pkR)
{
    const float nr[2] = { rmsL, rmsR }, np[2] = { pkL, pkR };
    bool changed = false;
    for (int c = 0; c < 2; ++c)
        if (std::abs (nr[c] - rms[c]) > 0.2f || std::abs (np[c] - pk[c]) > 0.2f)
            changed = true;

    if (! changed)
        return;

    rms[0] = rmsL; rms[1] = rmsR; pk[0] = pkL; pk[1] = pkR;
    repaint();
}

void StereoMeter::paint (juce::Graphics& g)
{
    const auto& st = AppSettings::get();
    g.fillAll (Cyber::bg);

    auto b = getLocalBounds().toFloat().reduced (2.f);
    auto frame = Cyber::chamfer (b, 8.f);
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff1b1230), 0.f, b.getY(), juce::Colour (0xff0a0614), 0.f, b.getBottom(), false));
    g.fillPath (frame);
    g.setColour (Cyber::text.withAlpha (0.08f));
    g.drawLine (b.getX() + 8.f, b.getY() + 1.5f, b.getRight() - 2.f, b.getY() + 1.5f, 1.f);
    g.setColour (Cyber::cyan.withAlpha (0.14f));
    g.strokePath (frame, juce::PathStrokeType (5.f));
    g.setColour (Cyber::cyan.withAlpha (0.65f));
    g.strokePath (frame, juce::PathStrokeType (1.2f));

    auto inner = b.reduced (7.f, 9.f);
    g.setColour (Cyber::cyan);
    g.setFont (Cyber::font (13.f, true));
    g.drawText (tr (key), inner.removeFromTop (16.f).toNearestInt(), juce::Justification::centred);

    g.setColour (Cyber::textDim);
    g.setFont (Cyber::font (11.f));
    g.drawText ("L      R", inner.removeFromBottom (16.f).toNearestInt(), juce::Justification::centred);

    constexpr int nSeg = 40;
    const float minDb = -(float) st.meterRange, maxDb = 6.f, step = (maxDb - minDb) / (float) nSeg;
    const float gap = 24.f;
    const float barW = (inner.getWidth() - gap) * 0.5f;
    const float segH = inner.getHeight() / (float) nSeg;

    for (int c = 0; c < 2; ++c)
    {
        const float x = inner.getX() + (float) c * (barW + gap);
        for (int s = 0; s < nSeg; ++s)
        {
            const float segDb = minDb + step * (float) (s + 1);
            const float y = inner.getBottom() - (float) (s + 1) * segH;
            const juce::Colour col = segDb < -18.f ? Cyber::green
                                   : segDb < -6.f  ? Cyber::yellow
                                   : segDb < 0.f   ? Cyber::orange : Cyber::magenta;

            const bool lit = rms[c] >= segDb - step;
            const bool isPeak = st.peakHold && pk[c] >= segDb - step && pk[c] < segDb;

            g.setColour (isPeak ? Cyber::text : (lit ? col : col.withAlpha (0.10f)));
            g.fillRect (x, y + 1.f, barW, segH - 1.5f);
        }
    }

    g.setFont (Cyber::font (9.5f));
    g.setColour (Cyber::textDim);
    for (float d : { 0.f, -6.f, -12.f, -24.f, -48.f })
    {
        if (d < minDb) continue;
        const float y = inner.getBottom() - (d - minDb) / (maxDb - minDb) * inner.getHeight();
        g.drawText (juce::String ((int) d), (int) (inner.getCentreX() - 12.f), (int) y - 5, 24, 10, juce::Justification::centred);
    }
}

//==============================================================================
//  InfoOverlay
//==============================================================================
InfoOverlay::InfoOverlay()
{
    text.setMultiLine (true, true);
    text.setReadOnly (true);
    text.setCaretVisible (false);
    text.setScrollbarsShown (true);
    text.setColour (juce::TextEditor::backgroundColourId, juce::Colour (0xff0a0614));
    text.setColour (juce::TextEditor::textColourId, Cyber::text);
    text.setColour (juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
    text.setColour (juce::TextEditor::focusedOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible (text);

    closeBtn.setNeon (Cyber::magenta);
    closeBtn.onClick = [this] { if (onClose) onClose(); };
    addAndMakeVisible (closeBtn);
    refresh();
}

void InfoOverlay::refresh()
{
    text.setFont (juce::Font (Cyber::font (15.f)));
    text.setText (Loc::infoText());
    closeBtn.setButtonText (tr ("CLOSE"));
    repaint();
}

void InfoOverlay::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black.withAlpha (0.6f));
    Cyber::drawBevelPanel (g, getLocalBounds().toFloat().reduced (6.f), Cyber::magenta, 16.f);

    g.setColour (Cyber::cyan);
    g.setFont (Cyber::font (20.f, true));
    g.drawText ("// INFO", getLocalBounds().reduced (26, 14).removeFromTop (32), juce::Justification::centredLeft);
}

void InfoOverlay::resized()
{
    auto r = getLocalBounds().reduced (26, 16);
    auto head = r.removeFromTop (36);
    closeBtn.setBounds (head.removeFromRight (110).reduced (0, 2));
    r.removeFromTop (8);
    text.setBounds (r);
}

//==============================================================================
//  SettingsOverlay
//==============================================================================
SettingsOverlay::SettingsOverlay()
{
    static const char* tabKeys[5] = { "General", "Top bar", "Sides", "Bottom", "Center" };
    for (int i = 0; i < 5; ++i)
    {
        tabBtn[i].setClickingTogglesState (true);
        tabBtn[i].setRadioGroupId (201);
        tabBtn[i].setNeon (Cyber::cyan);
        tabBtn[i].onClick = [this, i] { buildTab (i); };
        addAndMakeVisible (tabBtn[i]);
        tabBtn[i].setButtonText (tr (tabKeys[i]));
    }

    closeBtn.setNeon (Cyber::magenta);
    closeBtn.onClick = [this] { if (onClose) onClose(); };
    addAndMakeVisible (closeBtn);

    resetBtn.setNeon (Cyber::orange);
    resetBtn.onClick = [this]
    {
        AppSettings::get().reset();
        changed();
        refreshTexts();
    };
    addAndMakeVisible (resetBtn);

    refreshTexts();
}

void SettingsOverlay::refreshTexts()
{
    static const char* tabKeys[5] = { "General", "Top bar", "Sides", "Bottom", "Center" };
    for (int i = 0; i < 5; ++i)
        tabBtn[i].setButtonText (tr (tabKeys[i]));
    closeBtn.setButtonText (tr ("CLOSE"));
    resetBtn.setButtonText (tr ("RESET"));
    buildTab (tab);
    repaint();
}

void SettingsOverlay::changed()
{
    AppSettings::get().save();
    if (onChanged) onChanged();
}

void SettingsOverlay::buildTab (int t)
{
    tab = t;
    for (int i = 0; i < 5; ++i)
        tabBtn[i].setToggleState (i == t, juce::dontSendNotification);

    labels.clear();
    controls.clear();
    auto& st = AppSettings::get();

    auto addLabel = [this] (const char* key)
    {
        auto* l = labels.add (new juce::Label());
        l->setText (tr (key), juce::dontSendNotification);
        l->setFont (juce::Font (Cyber::font (14.f)));
        l->setColour (juce::Label::textColourId, Cyber::text);
        l->setJustificationType (juce::Justification::centredLeft);
        addAndMakeVisible (l);
    };

    auto addToggle = [&] (const char* key, bool& ref)
    {
        addLabel (key);
        auto* b = new CyberButton();
        controls.add (b);
        b->setClickingTogglesState (true);
        b->setNeon (Cyber::green);
        b->setToggleState (ref, juce::dontSendNotification);
        b->setButtonText (tr (ref ? "ON" : "OFF"));
        bool* p = &ref;
        b->onClick = [this, b, p] { *p = b->getToggleState(); b->setButtonText (tr (*p ? "ON" : "OFF")); changed(); };
        addAndMakeVisible (b);
    };

    // lista de opciones: el valor guardado es values[índice]
    auto addChoice = [&] (const char* key, std::vector<juce::String> items, std::vector<int> values, int& ref, bool relabel)
    {
        addLabel (key);
        auto* c = new juce::ComboBox();
        controls.add (c);
        int sel = 0;
        for (size_t i = 0; i < items.size(); ++i)
        {
            c->addItem (items[i], (int) i + 1);
            if (values[i] == ref) sel = (int) i;
        }
        c->setSelectedId (sel + 1, juce::dontSendNotification);
        int* p = &ref;
        c->onChange = [this, c, p, values, relabel]
        {
            *p = values[(size_t) juce::jlimit (0, (int) values.size() - 1, c->getSelectedId() - 1)];
            changed();
            if (relabel)
            {
                juce::Component::SafePointer<SettingsOverlay> self (this);
                juce::MessageManager::callAsync ([self] { if (self != nullptr) self->refreshTexts(); });
            }
        };
        addAndMakeVisible (c);
    };

    auto addSlider = [&] (const char* key, double lo, double hi, double step, float& ref)
    {
        addLabel (key);
        auto* s = new juce::Slider();
        controls.add (s);
        s->setSliderStyle (juce::Slider::LinearHorizontal);
        s->setTextBoxStyle (juce::Slider::TextBoxRight, false, 52, 20);
        s->setRange (lo, hi, step);
        s->setValue ((double) ref, juce::dontSendNotification);
        s->setColour (juce::Slider::trackColourId, Cyber::accent());
        float* p = &ref;
        s->onValueChange = [this, s, p] { *p = (float) s->getValue(); changed(); };
        addAndMakeVisible (s);
    };

    auto pct = [] (const char* a, const char* b, const char* c) { return std::vector<juce::String> { tr (a), tr (b), tr (c) }; };

    switch (t)
    {
        case 0:
            addChoice ("Language", { "English", juce::String::fromUTF8 ("Español"), juce::String::fromUTF8 ("中文") }, { 0, 1, 2 }, st.language, true);
            addChoice ("Accent colour", { tr ("Cyan"), tr ("Magenta"), tr ("Green"), tr ("Orange"), tr ("Violet") }, { 0, 1, 2, 3, 4 }, st.accent, false);
            addToggle ("Animations", st.animations);
            addToggle ("Scanlines effect", st.scanlines);
            break;

        case 1:
            addToggle ("Start collapsed", st.topCollapsed);
            addToggle ("Show subtitle", st.showSubtitle);
            break;

        case 2:
            addToggle ("Start collapsed", st.sidesCollapsed);
            addChoice ("Meter range", { "-90 dB", "-72 dB", "-60 dB", "-48 dB", "-36 dB" }, { 90, 72, 60, 48, 36 }, st.meterRange, false);
            addToggle ("Peak hold", st.peakHold);
            addChoice ("Meter fall speed", pct ("Slow", "Normal", "Fast"), { 0, 1, 2 }, st.meterFall, false);
            break;

        case 3:
            addToggle ("Start collapsed", st.bottomCollapsed);
            addSlider ("Panel opacity", 0.3, 1.0, 0.01, st.panelOpacity);
            addToggle ("Show captions", st.showCaptions);
            break;

        default:
            addSlider ("Band size", 0.6, 1.8, 0.01, st.nodeSize);
            addSlider ("Band opacity", 0.3, 1.0, 0.01, st.nodeOpacity);
            addSlider ("Curve thickness", 1.0, 4.0, 0.1, st.curveWidth);
            addSlider ("Spectrum opacity", 0.0, 1.0, 0.01, st.spectrumOpacity);
            addChoice ("Spectrum smoothing", pct ("Fast", "Normal", "Smooth"), { 0, 1, 2 }, st.spectrumSmoothing, false);
            addChoice ("Analyzer resolution", pct ("Low", "Medium", "High"), { 0, 1, 2 }, st.analyzerRes, false);
            addSlider ("Grid opacity", 0.0, 1.5, 0.01, st.gridOpacity);
            addToggle ("Show cursor readout", st.cursorReadout);
            addToggle ("Show band numbers", st.showNumbers);
            addToggle ("Fill selected band", st.fillSelected);
            addToggle ("Invert swipe direction (Q)", st.invertSwipe);
            break;
    }
    resized();
}

void SettingsOverlay::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black.withAlpha (0.6f));
    Cyber::drawBevelPanel (g, getLocalBounds().toFloat().reduced (6.f), Cyber::cyan, 16.f);

    g.setColour (Cyber::magenta);
    g.setFont (Cyber::font (20.f, true));
    g.drawText ("// " + tr ("SETTINGS"), getLocalBounds().reduced (26, 14).removeFromTop (34), juce::Justification::centredLeft);
}

void SettingsOverlay::resized()
{
    auto r = getLocalBounds().reduced (26, 16);
    auto head = r.removeFromTop (38);
    closeBtn.setBounds (head.removeFromRight (110).reduced (0, 2));  head.removeFromRight (8);
    resetBtn.setBounds (head.removeFromRight (120).reduced (0, 2));

    r.removeFromTop (6);
    auto tabs = r.removeFromTop (34);
    const int tw = juce::jmin (150, tabs.getWidth() / 5);
    for (int i = 0; i < 5; ++i)
        tabBtn[i].setBounds (tabs.removeFromLeft (tw).reduced (3, 0));

    r.removeFromTop (14);
    const int n = labels.size(), rowH = 40;
    const int cols = (n * rowH > r.getHeight()) ? 2 : 1;
    const int perCol = juce::jmax (1, (n + cols - 1) / cols);
    const int colW = r.getWidth() / cols;

    for (int i = 0; i < n; ++i)
    {
        juce::Rectangle<int> row (r.getX() + (i / perCol) * colW, r.getY() + (i % perCol) * rowH, colW - 24, rowH - 6);
        labels[i]->setBounds (row.removeFromLeft (row.getWidth() * 55 / 100));
        controls[i]->setBounds (row.reduced (4, 2));
    }
}

//==============================================================================
//  EqGraph: utilidades
//==============================================================================
EqGraph::EqGraph (IJEQProcessor& p, std::function<void (int)> cb)
    : proc (p), onSelect (std::move (cb))
{
    setOpaque (true);
    setWantsKeyboardFocus (true);
    startTimerHz (30);
}

EqGraph::~EqGraph()
{
    stopTimer();
}

EqGraph::Band EqGraph::band (int b) const
{
    const auto& p = proc.pBand[(size_t) b];
    return { p.used->load() > 0.5f, p.on->load() > 0.5f, p.dyn->load() > 0.5f, (int) std::lround (p.type->load()),
             p.freq->load(), p.gain->load(), p.q->load() };
}

juce::Rectangle<float> EqGraph::plot() const
{
    return getLocalBounds().toFloat().reduced (14.f, 14.f).withTrimmedLeft (30.f).withTrimmedBottom (14.f);
}

float EqGraph::freqToX (float f) const
{
    auto r = plot();
    return (float) ((double) r.getX() + (double) r.getWidth() * (std::log ((double) f) - cur.lo) / (cur.hi - cur.lo));
}

float EqGraph::xToFreq (float x) const
{
    auto r = plot();
    return (float) std::exp (cur.lo + (double) (x - r.getX()) / (double) r.getWidth() * (cur.hi - cur.lo));
}

float EqGraph::gainToY (float g) const
{
    auto r = plot();
    return r.getBottom() - (float) (((double) g - (cur.center - cur.half)) / (2.0 * cur.half)) * r.getHeight();
}

float EqGraph::yToGain (float y) const
{
    auto r = plot();
    const double g = cur.center - cur.half + (double) (r.getBottom() - y) / (double) r.getHeight() * 2.0 * cur.half;
    return juce::jlimit (-24.f, 24.f, (float) g);
}

juce::Point<float> EqGraph::nodePos (int b) const
{
    const auto bd = band (b);
    const float g = (bd.type == EqMath::Notch) ? 0.f : bd.gain;
    auto pl = plot();
    return { freqToX (bd.freq), juce::jlimit (pl.getY(), pl.getBottom(), gainToY (g)) };
}

bool EqGraph::nodeVisible (juce::Point<float> p) const
{
    auto pl = plot();
    return p.x >= pl.getX() - 8.f && p.x <= pl.getRight() + 8.f;
}

int EqGraph::hitTest (juce::Point<float> p) const
{
    int best = -1;
    float bestD = 8.f + 7.f * AppSettings::get().nodeSize;
    for (int b = 0; b < numBands; ++b)
    {
        if (! bandUsed (b)) continue;
        const auto np = nodePos (b);
        if (! nodeVisible (np)) continue;
        const float d = np.getDistanceFrom (p);
        if (d < bestD) { bestD = d; best = b; }
    }
    return best;
}

void EqGraph::editParam (juce::RangedAudioParameter* p, float real)
{
    p->beginChangeGesture();
    setNorm (p, real);
    p->endChangeGesture();
}

juce::uint32 EqGraph::usedMask() const
{
    juce::uint32 m = 0;
    for (int b = 0; b < numBands; ++b)
        if (bandUsed (b)) m |= (1u << b);
    return m;
}

int EqGraph::firstUsed() const
{
    for (int b = 0; b < numBands; ++b)
        if (bandUsed (b)) return b;
    return -1;
}

int EqGraph::numUsed() const
{
    int n = 0;
    for (int b = 0; b < numBands; ++b)
        if (bandUsed (b)) ++n;
    return n;
}

int EqGraph::selectedCount() const
{
    int n = 0;
    for (int b = 0; b < numBands; ++b)
        if (isSel (b)) ++n;
    return n;
}

void EqGraph::setSelection (juce::uint32 mask, int prim)
{
    selMask = mask;
    if (mask == 0)
        prim = -1;
    else if (prim < 0 || ! isSel (prim))
        for (int b = 0; b < numBands; ++b)
            if (isSel (b)) { prim = b; break; }

    curveDirty = true;
    if (prim != selected)
    {
        selected = prim;
        onSelect (prim);
    }
    repaint();
}

void EqGraph::setSelected (int b)
{
    setSelection (b >= 0 ? (1u << b) : 0u, b);
}

void EqGraph::selectBandIndex (int b)
{
    if (! bandUsed (b)) return;
    ensureVisible (band (b).freq);
    setSelection (1u << b, b);
}

void EqGraph::refreshSelection()
{
    const auto um = usedMask();
    if ((selMask & ~um) != 0u)
        setSelection (selMask & um, selected);
}

void EqGraph::selectNeighbor (int dir)
{
    std::vector<std::pair<float, int>> list;
    for (int b = 0; b < numBands; ++b)
        if (bandUsed (b)) list.push_back ({ band (b).freq, b });
    if (list.empty()) return;

    std::sort (list.begin(), list.end());

    int idx = -1;
    for (size_t i = 0; i < list.size(); ++i)
        if (list[i].second == selected) idx = (int) i;

    const int n = (int) list.size();
    idx = (idx < 0) ? (dir > 0 ? 0 : n - 1) : ((idx + dir) % n + n) % n;
    selectBandIndex (list[(size_t) idx].second);
}

int EqGraph::addBand (float freq, float gainDb, int type, float q)
{
    proc.historyTouch();
    for (int b = 0; b < numBands; ++b)
    {
        if (bandUsed (b)) continue;

        const auto& bp = proc.bandParam[(size_t) b];
        editParam (bp.type, (float) type);
        editParam (bp.freq, freq);
        editParam (bp.gain, gainDb);
        editParam (bp.q, q);
        editParam (bp.dyn, 0.f);
        editParam (bp.on, 1.f);
        editParam (bp.used, 1.f);

        setSelection (1u << b, b);
        return b;
    }
    return -1;   // las 24 bandas están en uso
}

void EqGraph::deleteBand (int b)
{
    proc.historyTouch();
    editParam (proc.bandParam[(size_t) b].used, 0.f);
    setSelection (selMask & ~(1u << b), -1);
}

void EqGraph::deleteSelected()
{
    proc.historyTouch();
    for (int b = 0; b < numBands; ++b)
        if (isSel (b))
            editParam (proc.bandParam[(size_t) b].used, 0.f);
    setSelection (0u, -1);
}

void EqGraph::settingsChanged()
{
    gridImg = {};
    curveDirty = specDirty = true;
    ensureFft();
    repaint();
}

//==============================================================================
//  EqGraph: vista (zoom y desplazamiento)
//==============================================================================
void EqGraph::clampView (View& v) const
{
    const double sr = proc.currentSr.load();
    const double minLo = std::log (10.0), maxHi = std::log (juce::jmin (24000.0, sr * 0.5));
    const double span = juce::jlimit (std::log (2.0), maxHi - minLo, v.hi - v.lo);

    double lo = v.lo;
    if (lo < minLo) lo = minLo;
    if (lo + span > maxHi) lo = maxHi - span;
    v.lo = lo;
    v.hi = lo + span;
    v.half = juce::jlimit (1.5, 36.0, v.half);
    v.center = juce::jlimit (-30.0, 30.0, v.center);
}

void EqGraph::setRange (int db)
{
    tgt.half = db;
    tgt.center = 0.0;
    clampView (tgt);
    animating = true;
    startTimerHz (60);
}

void EqGraph::resetView()
{
    const double sr = proc.currentSr.load();
    tgt = { std::log (20.0), std::log (juce::jmin (20000.0, sr * 0.49)), 0.0, 18.0 };
    clampView (tgt);
    animating = true;
    startTimerHz (60);
}

void EqGraph::zoomFreq (float x, double factor)
{
    auto r = plot();
    const double t = juce::jlimit (0.0, 1.0, (double) ((x - r.getX()) / r.getWidth()));
    const double f0 = tgt.lo + t * (tgt.hi - tgt.lo);
    const double span = (tgt.hi - tgt.lo) * factor;
    tgt.lo = f0 - t * span;
    tgt.hi = tgt.lo + span;
    clampView (tgt);
    animating = true;
    startTimerHz (60);
}

void EqGraph::zoomGain (float y, double factor)
{
    auto r = plot();
    const double t = juce::jlimit (0.0, 1.0, (double) ((r.getBottom() - y) / r.getHeight()));
    const double g0 = tgt.center - tgt.half + t * 2.0 * tgt.half;
    tgt.half = juce::jlimit (1.5, 36.0, tgt.half * factor);
    tgt.center = g0 - (t - 0.5) * 2.0 * tgt.half;
    clampView (tgt);
    animating = true;
    startTimerHz (60);
}

void EqGraph::ensureVisible (float freq)
{
    const double lf = std::log ((double) freq);
    if (lf >= tgt.lo && lf <= tgt.hi)
        return;

    const double span = tgt.hi - tgt.lo;
    tgt.lo = lf - span * 0.5;
    tgt.hi = tgt.lo + span;
    clampView (tgt);
    animating = true;
    startTimerHz (60);
}

bool EqGraph::stepView()
{
    if (! animating)
        return false;

    const double k = AppSettings::get().animations ? 0.35 : 1.0;
    cur.lo     += (tgt.lo - cur.lo) * k;
    cur.hi     += (tgt.hi - cur.hi) * k;
    cur.center += (tgt.center - cur.center) * k;
    cur.half   += (tgt.half - cur.half) * k;

    auto near = [] (double a, double b, double eps) { return std::abs (a - b) < eps; };
    if (near (cur.lo, tgt.lo, 1e-4) && near (cur.hi, tgt.hi, 1e-4) && near (cur.center, tgt.center, 1e-3) && near (cur.half, tgt.half, 1e-3))
    {
        cur = tgt;
        animating = false;
        startTimerHz (30);
    }
    specDirty = true;
    curveDirty = true;
    return true;
}

//==============================================================================
//  EqGraph: temporizador, espectro y resonancias
//==============================================================================
juce::uint32 EqGraph::paramHash() const
{
    juce::uint32 h = 2166136261u;
    auto mix = [&h] (float v)
    {
        juce::uint32 u;
        std::memcpy (&u, &v, sizeof (u));
        h = (h ^ u) * 16777619u;
    };

    for (int b = 0; b < numBands; ++b)
    {
        const auto& p = proc.pBand[(size_t) b];
        mix (p.used->load());  mix (p.on->load());  mix (p.type->load());  mix (p.freq->load());
        mix (p.gain->load());  mix (p.q->load());   mix (p.dyn->load());
        mix (proc.dynShown[(size_t) b].load (std::memory_order_relaxed));
    }
    mix (proc.pHpfOn->load());  mix (proc.pHpfFreq->load());  mix (proc.pHpfSlope->load());
    mix (proc.pLpfOn->load());  mix (proc.pLpfFreq->load());  mix (proc.pLpfSlope->load());
    mix (proc.pBypass->load());
    mix ((float) selMask);
    return h;
}

void EqGraph::timerCallback()
{
    bool need = stepView();

    refreshSelection();

    if (! paused && (showSpectrum || showRes))
    {
        if (updateSpectrum())
        {
            if (showRes) findResonances();
            need = true;
        }
    }

    if (specDirty)
    {
        if (showSpectrum && fftSize > 0) { rebuildSpectrum(); need = true; }
        specDirty = false;
    }

    const auto h = paramHash();
    if (h != lastHash)
    {
        lastHash = h;
        curveDirty = true;
        need = true;
    }

    if (need)
        repaint();
}

void EqGraph::ensureFft()
{
    static const int sizes[3] = { 4096, 8192, 16384 };
    const int want = sizes[juce::jlimit (0, 2, AppSettings::get().analyzerRes)];
    if (fftSize == want)
        return;

    fftSize = want;
    int order = 0;
    while ((1 << order) < want) ++order;

    fft = std::make_unique<juce::dsp::FFT> (order);
    window = std::make_unique<juce::dsp::WindowingFunction<float>> ((size_t) want, juce::dsp::WindowingFunction<float>::hann);
    fftData.assign ((size_t) (2 * want), 0.f);
    spec.assign ((size_t) (want / 2), -120.f);
    specSlow.assign ((size_t) (want / 2), -120.f);
    lastSpecSum = 0.f;
    specDirty = true;
    res.clear();
}

bool EqGraph::updateSpectrum()
{
    ensureFft();

    const int mask = IJEQProcessor::scopeSize - 1;
    const int end = proc.scopePos.load (std::memory_order_acquire);

    for (int i = 0; i < fftSize; ++i)
        fftData[(size_t) i] = proc.scope[(size_t) ((end - fftSize + i) & mask)].load (std::memory_order_relaxed);
    std::fill (fftData.begin() + fftSize, fftData.end(), 0.f);

    window->multiplyWithWindowingTable (fftData.data(), (size_t) fftSize);
    fft->performFrequencyOnlyForwardTransform (fftData.data());

    static const float decay[3] = { 0.65f, 0.85f, 0.93f };
    const float dec = decay[juce::jlimit (0, 2, AppSettings::get().spectrumSmoothing)];

    float sum = 0.f;
    for (int k = 0; k < fftSize / 2; ++k)
    {
        const float db = juce::Decibels::gainToDecibels (fftData[(size_t) k] / ((float) fftSize * 0.25f), -120.f);
        auto& s = spec[(size_t) k];
        s = db > s ? db : s * dec + db * (1.f - dec);                       // subida rápida, caída suave
        specSlow[(size_t) k] = specSlow[(size_t) k] * 0.97f + db * 0.03f;    // promedio lento (~1 s)
        sum += s;
    }

    const bool changed = std::abs (sum - lastSpecSum) > 0.05f;
    lastSpecSum = sum;
    if (changed) specDirty = true;
    return changed;
}

void EqGraph::rebuildSpectrum()
{
    specFill.clear();
    specLine.clear();
    if (fftSize == 0) return;

    auto pl = plot();
    const double sr = proc.currentSr.load();
    const float binHz = (float) (sr / fftSize);
    const int half = fftSize / 2;
    bool started = false;

    for (float x = pl.getX(); x <= pl.getRight(); x += 2.f)
    {
        const float bLo = xToFreq (x - 1.f) / binHz, bHi = xToFreq (x + 1.f) / binHz;
        float db;
        if (bHi - bLo < 1.f)
        {
            const float bc = 0.5f * (bLo + bHi);
            const int k0 = juce::jlimit (0, half - 2, (int) bc);
            const float fr = bc - (float) k0;
            db = spec[(size_t) k0] * (1.f - fr) + spec[(size_t) k0 + 1] * fr;
        }
        else
        {
            const int k0 = juce::jlimit (1, half - 1, (int) bLo), k1 = juce::jlimit (1, half - 1, (int) bHi);
            db = -120.f;
            for (int k = k0; k <= k1; ++k) db = juce::jmax (db, spec[(size_t) k]);
        }
        const float y = juce::jmap (juce::jlimit (-90.f, 0.f, db), -90.f, 0.f, pl.getBottom(), pl.getY());
        if (! started) { specLine.startNewSubPath (x, y); started = true; }
        else           specLine.lineTo (x, y);
    }

    specFill = specLine;
    specFill.lineTo (pl.getRight(), pl.getBottom());
    specFill.lineTo (pl.getX(), pl.getBottom());
    specFill.closeSubPath();
}

// Detecta picos estrechos que sobresalen del nivel de su entorno (frecuencias molestas)
void EqGraph::findResonances()
{
    res.clear();
    if (fftSize == 0) return;

    const double sr = proc.currentSr.load();
    const float binHz = (float) (sr / fftSize);
    const int half = fftSize / 2;
    const int kMin = juce::jmax (6, (int) (120.f / binHz));
    const int kMax = juce::jmin (half - 8, (int) (12000.f / binHz));

    std::vector<Resonance> cand;
    for (int k = kMin; k <= kMax; ++k)
    {
        const float a = specSlow[(size_t) k - 1], v = specSlow[(size_t) k], c = specSlow[(size_t) k + 1];
        if (v < -75.f || v < a || v < c)
            continue;

        const int w = juce::jmax (5, k / 5);
        float sum = 0.f;
        int cnt = 0;
        for (int j = k - w; j <= k + w; ++j)
            if (std::abs (j - k) > 1) { sum += specSlow[(size_t) j]; ++cnt; }

        const float excess = v - sum / (float) cnt;
        if (excess > 6.f)
        {
            const float den = a - 2.f * v + c;                                   // interpolación parabólica: frecuencia más exacta
            const float delta = std::abs (den) > 1e-6f ? juce::jlimit (-0.5f, 0.5f, 0.5f * (a - c) / den) : 0.f;
            cand.push_back ({ ((float) k + delta) * binHz, excess });
        }
    }

    std::sort (cand.begin(), cand.end(), [] (const Resonance& a, const Resonance& b) { return a.excess > b.excess; });
    for (auto& c : cand)
    {
        bool near = false;
        for (auto& r : res)
            if (std::abs (std::log2 (c.freq / r.freq)) < 0.33f) { near = true; break; }
        if (! near) res.push_back (c);
        if (res.size() >= 6) break;
    }
}

double EqGraph::responseDbAt (double f) const
{
    const double w = 2.0 * EqMath::pi * f / cSr;
    const double cw = std::cos (w), c2 = std::cos (2.0 * w), sw = std::sin (w), s2 = std::sin (2.0 * w);
    double mag = 1.0;
    for (int k = 0; k < cNh; ++k) mag *= EqMath::magnitudeTrig (cHp[k], cw, c2, sw, s2);
    for (int k = 0; k < cNl; ++k) mag *= EqMath::magnitudeTrig (cLp[k], cw, c2, sw, s2);
    for (int b = 0; b < numBands; ++b)
        if (cAct[b]) mag *= EqMath::magnitudeTrig (cBand[b], cw, c2, sw, s2);
    return 20.0 * std::log10 (std::max (mag, 1e-9));
}

float EqGraph::levelAt (float f) const
{
    if (fftSize == 0 || spec.empty()) return -120.f;
    const float bin = f / (float) (proc.currentSr.load() / fftSize);
    const int half = fftSize / 2;
    const int k0 = juce::jlimit (0, half - 2, (int) bin);
    const float fr = juce::jlimit (0.f, 1.f, bin - (float) k0);
    return spec[(size_t) k0] * (1.f - fr) + spec[(size_t) k0 + 1] * fr;
}

//==============================================================================
//  EqGraph: dibujo
//==============================================================================
void EqGraph::resized()
{
    gridImg = {};
    curveDirty = specDirty = true;
}

void EqGraph::ensureGrid()
{
    const int w = getWidth(), h = getHeight();
    if (w <= 0 || h <= 0)
        return;

    if (gridImg.isValid() && gridImg.getWidth() == w && gridImg.getHeight() == h
        && gridView.lo == cur.lo && gridView.hi == cur.hi && gridView.center == cur.center && gridView.half == cur.half)
        return;

    const auto& st = AppSettings::get();
    const float go = st.gridOpacity;
    const auto acc = Cyber::accent();

    gridView = cur;
    gridImg = juce::Image (juce::Image::RGB, w, h, false);
    juce::Graphics g (gridImg);

    g.fillAll (Cyber::bg);
    auto area = juce::Rectangle<float> (0.f, 0.f, (float) w, (float) h).reduced (2.f);
    auto frame = Cyber::chamfer (area, 12.f);
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff170e2a), 0.f, area.getY(),
                                             juce::Colour (0xff07040e), 0.f, area.getBottom(), false));
    g.fillPath (frame);

    // sombra interior (relieve hundido)
    g.saveState();
    g.reduceClipRegion (frame);
    g.setColour (juce::Colours::black.withAlpha (0.65f));
    g.strokePath (frame, juce::PathStrokeType (16.f));
    g.restoreState();

    auto pl = plot();

    // ---- líneas de dB
    g.setFont (Cyber::font (11.f));
    const double span = 2.0 * cur.half;
    static const double steps[] = { 0.5, 1.0, 2.0, 3.0, 5.0, 6.0, 10.0, 12.0 };
    double step = 12.0;
    for (double s : steps)
        if (s >= span / 6.0) { step = s; break; }

    for (double k = std::ceil ((cur.center - cur.half) / step); k * step <= cur.center + cur.half + 1e-9; k += 1.0)
    {
        const float v = (float) (k * step);
        const float y = gainToY (v);
        g.setColour (std::abs (v) < 1e-6f ? acc.withAlpha (juce::jmin (1.f, 0.5f * go)) : Cyber::violet.withAlpha (juce::jmin (1.f, 0.16f * go)));
        g.drawHorizontalLine ((int) y, pl.getX(), pl.getRight());
        g.setColour (Cyber::textDim);
        g.drawText ((v > 0.f ? "+" : "") + juce::String (v, step < 1.0 ? 1 : 0), 0, (int) y - 7, 36, 14, juce::Justification::centredRight);
    }

    // ---- líneas de frecuencia (más divisiones al hacer zoom)
    const bool zoomed = (cur.hi - cur.lo) < std::log (6.0);
    std::vector<float> cand;
    for (float dec : { 10.f, 100.f, 1000.f, 10000.f })
        for (int m = 1; m <= 9; ++m)
        {
            cand.push_back ((float) m * dec);
            if (zoomed) cand.push_back (((float) m + 0.5f) * dec);
        }

    auto isMajor = [] (float f)
    {
        const float dec = std::pow (10.f, std::floor (std::log10 (f)));
        const float m = f / dec;
        return std::abs (m - 1.f) < 1e-3f || std::abs (m - 2.f) < 1e-3f || std::abs (m - 5.f) < 1e-3f;
    };

    std::vector<float> labelX;
    auto tryLabel = [&] (float f, float x)
    {
        for (float lx : labelX)
            if (std::abs (lx - x) < 46.f) return;
        labelX.push_back (x);
        g.setColour (Cyber::textDim);
        g.drawText (fmtTick (f), (int) x - 20, (int) pl.getBottom() + 1, 40, 14, juce::Justification::centred);
    };

    for (int pass = 0; pass < 2; ++pass)
        for (float f : cand)
        {
            const float x = freqToX (f);
            if (x < pl.getX() - 0.5f || x > pl.getRight() + 0.5f) continue;
            const bool major = isMajor (f);

            if (pass == 0)
            {
                g.setColour (Cyber::violet.withAlpha (juce::jmin (1.f, (major ? 0.22f : 0.09f) * go)));
                g.drawVerticalLine ((int) x, pl.getY(), pl.getBottom());
                if (major) tryLabel (f, x);
            }
            else if (! major)
            {
                tryLabel (f, x);
            }
        }

    // ---- borde neón
    g.setColour (acc.withAlpha (0.16f));
    g.strokePath (frame, juce::PathStrokeType (6.f));
    g.setColour (acc.withAlpha (0.65f));
    g.strokePath (frame, juce::PathStrokeType (1.3f));
}

void EqGraph::rebuildCurve()
{
    cSr = proc.currentSr.load();
    auto pl = plot();

    cNh = cNl = 0;
    if (proc.pHpfOn->load() > 0.5f) cNh = EqMath::designCut (true,  cSr, proc.pHpfFreq->load(), (int) std::lround (proc.pHpfSlope->load()), cHp);
    if (proc.pLpfOn->load() > 0.5f) cNl = EqMath::designCut (false, cSr, proc.pLpfFreq->load(), (int) std::lround (proc.pLpfSlope->load()), cLp);

    for (int b = 0; b < numBands; ++b)
    {
        const auto bd = band (b);
        cAct[b] = bd.used && bd.on;
        if (! cAct[b]) continue;

        float gain = bd.gain;
        if (bd.type != EqMath::Notch && bd.dyn)
            gain += proc.dynShown[(size_t) b].load (std::memory_order_relaxed);   // la curva sigue a la dinámica en vivo
        cBand[b] = EqMath::designBand (bd.type, cSr, bd.freq, gain, bd.q);
    }

    auto dbToY = [this] (double mag)
    {
        const double db = 20.0 * std::log10 (std::max (mag, 1e-9));
        return gainToY ((float) juce::jlimit (cur.center - cur.half * 1.3, cur.center + cur.half * 1.3, db));
    };

    curvePath.clear();
    selFill.clear();

    const bool selActive = selected >= 0 && cAct[selected];
    const float y0 = gainToY (0.f);
    if (selActive) selFill.startNewSubPath (pl.getX(), y0);

    // 1 punto por píxel: curva más precisa y suave
    const int n = juce::jmax (2, (int) std::ceil (pl.getWidth()) + 1);
    float xLast = pl.getX();
    for (int i = 0; i < n; ++i)
    {
        const float x = pl.getX() + (float) i;
        xLast = x;
        const double w = 2.0 * EqMath::pi * (double) xToFreq (x) / cSr;
        const double cw = std::cos (w), c2 = std::cos (2.0 * w), sw = std::sin (w), s2 = std::sin (2.0 * w);

        double mag = 1.0;
        for (int k = 0; k < cNh; ++k) mag *= EqMath::magnitudeTrig (cHp[k], cw, c2, sw, s2);
        for (int k = 0; k < cNl; ++k) mag *= EqMath::magnitudeTrig (cLp[k], cw, c2, sw, s2);
        for (int b = 0; b < numBands; ++b)
            if (cAct[b]) mag *= EqMath::magnitudeTrig (cBand[b], cw, c2, sw, s2);

        const float y = dbToY (mag);
        if (i == 0) curvePath.startNewSubPath (x, y); else curvePath.lineTo (x, y);
        if (selActive) selFill.lineTo (x, dbToY (EqMath::magnitudeTrig (cBand[selected], cw, c2, sw, s2)));
    }

    if (selActive)
    {
        selFill.lineTo (xLast, y0);
        selFill.closeSubPath();
    }
}

void EqGraph::paint (juce::Graphics& g)
{
    const auto& st = AppSettings::get();
    const auto acc = Cyber::accent();

    ensureGrid();
    if (gridImg.isValid())
        g.drawImageAt (gridImg, 0, 0);
    else
        g.fillAll (Cyber::bg);

    if (curveDirty)
    {
        rebuildCurve();
        curveDirty = false;
    }

    auto pl = plot();
    const bool bypassed = proc.pBypass->load() > 0.5f;

    g.saveState();
    g.reduceClipRegion (pl.toNearestInt());

    if (showSpectrum && st.spectrumOpacity > 0.01f)
    {
        g.setColour (Cyber::magenta.withAlpha (0.16f * st.spectrumOpacity));
        g.fillPath (specFill);
        g.setColour (Cyber::magenta.withAlpha (juce::jmin (1.f, 0.55f * st.spectrumOpacity)));
        g.strokePath (specLine, juce::PathStrokeType (1.2f));
    }

    if (selected >= 0 && ! bypassed && st.fillSelected)
    {
        g.setColour (Cyber::band (selected).withAlpha (0.20f));
        g.fillPath (selFill);
    }

    const auto lineCol = bypassed ? Cyber::textDim : acc;
    const float cw = st.curveWidth;
    g.setColour (lineCol.withAlpha (0.10f));
    g.strokePath (curvePath, juce::PathStrokeType (cw * 5.5f));
    g.setColour (lineCol.withAlpha (0.25f));
    g.strokePath (curvePath, juce::PathStrokeType (cw * 2.8f));
    g.setColour (lineCol.brighter (0.5f));
    g.strokePath (curvePath, juce::PathStrokeType (cw));

    g.restoreState();

    // ---- resonancias (frecuencias molestas)
    if (showRes)
    {
        const float dash[2] = { 4.f, 4.f };
        g.setFont (Cyber::font (12.f, true));
        for (auto& r : res)
        {
            const float x = freqToX (r.freq);
            if (x < pl.getX() || x > pl.getRight()) continue;

            g.setColour (Cyber::magenta.withAlpha (0.6f));
            g.drawDashedLine (juce::Line<float> (x, pl.getY() + 12.f, x, pl.getBottom()), dash, 2, 1.f);

            juce::Path tri;
            tri.addTriangle (x - 7.f, pl.getY(), x + 7.f, pl.getY(), x, pl.getY() + 12.f);
            g.setColour (Cyber::magenta.withAlpha (0.3f));
            g.strokePath (tri, juce::PathStrokeType (5.f));
            g.setColour (Cyber::magenta);
            g.fillPath (tri);

            const float tx = (x > pl.getRight() - 150.f) ? x - 148.f : x + 8.f;
            g.setColour (Cyber::text);
            g.drawText (fmtHz (r.freq) + " +" + juce::String ((int) std::lround (r.excess)) + "dB",
                        (int) tx, (int) pl.getY() + 1, 140, 14, juce::Justification::centredLeft);
        }
    }

    // ---- puntos de las bandas (esferas con relieve)
    const bool layered = st.nodeOpacity < 0.99f;
    if (layered) g.beginTransparencyLayer (st.nodeOpacity);

    const float R = 9.f * st.nodeSize;
    for (int b = 0; b < numBands; ++b)
    {
        const auto bd = band (b);
        if (! bd.used) continue;

        const auto c = nodePos (b);
        if (! nodeVisible (c)) continue;

        const bool active = bd.on;
        const auto r = juce::Rectangle<float> (R * 2.f, R * 2.f).withCentre (c);
        const auto col = Cyber::band (b);

        if (active)
        {
            g.setColour (col.withAlpha (0.25f));
            g.fillEllipse (r.expanded (R * 0.55f));
            g.setGradientFill (juce::ColourGradient (col.brighter (0.8f), c.x - R * 0.3f, c.y - R * 0.45f,
                                                     col.darker (0.5f), c.x + R * 0.8f, c.y + R * 0.9f, true));
            g.fillEllipse (r);
        }
        else
        {
            g.setColour (Cyber::bg);
            g.fillEllipse (r);
            g.setColour (col.withAlpha (0.6f));
            g.drawEllipse (r, 2.f);
        }

        if (bd.type != EqMath::Notch && bd.dyn)
        {
            g.setColour (Cyber::text.withAlpha (0.7f));
            g.drawEllipse (r.expanded (R * 0.33f), 1.f);

            const float off = proc.dynShown[(size_t) b].load (std::memory_order_relaxed);
            if (std::abs (off) > 0.1f)
            {
                const float gy = juce::jlimit (pl.getY(), pl.getBottom(), gainToY (bd.gain + off));
                g.setColour (col.withAlpha (0.8f));
                g.drawLine (c.x, c.y, c.x, gy, 1.5f);
                g.drawEllipse (juce::Rectangle<float> (R * 1.1f, R * 1.1f).withCentre ({ c.x, gy }), 1.5f);
            }
        }

        if (st.showNumbers)
        {
            g.setColour (active ? Cyber::bg : col);
            g.setFont (Cyber::font (juce::jmax (8.f, 11.5f * st.nodeSize), true));
            g.drawText (juce::String (b + 1), r.toNearestInt(), juce::Justification::centred);
        }

        if (isSel (b))
        {
            g.setColour (Cyber::text.withAlpha (b == selected ? 1.f : 0.6f));
            g.drawEllipse (r.expanded (R * 0.55f), b == selected ? 2.f : 1.4f);
        }
    }
    if (layered) g.endTransparencyLayer();

    // ---- rectángulo de selección
    if (drag == Drag::Marquee && ! marquee.isEmpty())
    {
        g.setColour (acc.withAlpha (0.12f));
        g.fillRect (marquee);
        g.setColour (acc.withAlpha (0.85f));
        g.drawRect (marquee, 1.f);
    }

    // ---- lectura bajo el cursor: frecuencia exacta, nota, ganancia del EQ y nivel del espectro
    if (st.cursorReadout && hasHover && drag == Drag::None && pl.contains (hoverPos))
    {
        const float x = hoverPos.x;
        g.setColour (acc.withAlpha (0.30f));
        g.drawVerticalLine ((int) x, pl.getY(), pl.getBottom());

        const float f = xToFreq (x);
        const double midi = 69.0 + 12.0 * std::log2 ((double) f / 440.0);
        const int nn = (int) std::lround (midi);
        const int cents = (int) std::lround ((midi - nn) * 100.0);
        static const char* names[12] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
        const juce::String note = juce::String (names[((nn % 12) + 12) % 12]) + juce::String (nn / 12 - 1)
                                  + (cents >= 0 ? " +" : " ") + juce::String (cents) + "c";
        const double eq = responseDbAt ((double) f);
        const float lvl = levelAt (f);

        const juce::String txt = fmtHz (f) + "    " + note + "    EQ " + (eq >= 0.0 ? "+" : "") + juce::String (eq, 1) + " dB"
                                 + (showSpectrum && lvl > -110.f ? "    " + juce::String (lvl, 1) + " dBFS" : juce::String());

        const float bw = 330.f, bh = 20.f;
        float bx = x + 12.f;
        if (bx + bw > pl.getRight()) bx = x - 12.f - bw;
        const auto box = juce::Rectangle<float> (juce::jmax (pl.getX(), bx), pl.getY() + 26.f, bw, bh);
        g.setColour (Cyber::bg.withAlpha (0.82f));
        g.fillRoundedRectangle (box, 4.f);
        g.setColour (acc.withAlpha (0.6f));
        g.drawRoundedRectangle (box, 4.f, 1.f);
        g.setColour (Cyber::text);
        g.setFont (Cyber::font (12.f, true));
        g.drawText (txt, box.toNearestInt(), juce::Justification::centred);
    }

    // ---- textos
    g.setFont (Cyber::font (12.5f, true));
    if (selected >= 0 && bandUsed (selected))
    {
        auto txt = [] (juce::RangedAudioParameter* p) { return p->getCurrentValueAsText(); };
        const auto& bp = proc.bandParam[(size_t) selected];
        g.setColour (Cyber::band (selected));
        const int n = selectedCount();
        g.drawText ((n > 1 ? juce::String (n) + " " + tr ("BANDS") + "  //  " + tr ("MAIN") + " " : tr ("BAND") + " ") + juce::String (selected + 1)
                    + "  //  " + txt (bp.freq) + "  //  " + txt (bp.gain) + "  //  Q " + txt (bp.q),
                    (int) pl.getX() + 8, (int) pl.getBottom() - 20, 560, 16, juce::Justification::centredLeft);
    }

    g.setColour (Cyber::textDim);
    g.drawText (tr ("BANDS") + " " + juce::String (numUsed()) + "/" + juce::String (numBands),
                (int) pl.getRight() - 150, (int) pl.getBottom() - 20, 142, 16, juce::Justification::centredRight);

    if (paused)
    {
        g.setColour (Cyber::yellow);
        g.drawText (tr ("ANALYZER PAUSED"), (int) pl.getRight() - 300, (int) pl.getY() + 4, 292, 16, juce::Justification::centredRight);
    }
}

//==============================================================================
//  EqGraph: ratón y trackpad
//==============================================================================
void EqGraph::showMenu (int b)
{
    const juce::uint32 mask = isSel (b) ? selMask : (1u << b);
    const auto bd = band (b);
    int count = 0;
    for (int i = 0; i < numBands; ++i) if ((mask >> i) & 1u) ++count;

    juce::PopupMenu m;
    m.addItem (1, count > 1 ? tr ("Delete selected bands") : tr ("Delete band"));
    m.addItem (2, bd.on ? tr ("Disable") : tr ("Enable"));
    m.addItem (3, bd.dyn ? tr ("Remove dynamics") : tr ("Enable dynamics"));

    juce::Component::SafePointer<EqGraph> self (this);
    m.showMenuAsync (juce::PopupMenu::Options(), [self, mask, bd] (int r)
    {
        if (self == nullptr || r == 0) return;
        self->proc.historyTouch();

        for (int i = 0; i < numBands; ++i)
        {
            if (! ((mask >> i) & 1u)) continue;
            const auto& bp = self->proc.bandParam[(size_t) i];
            if (r == 1)      self->editParam (bp.used, 0.f);
            else if (r == 2) self->editParam (bp.on,  bd.on  ? 0.f : 1.f);
            else if (r == 3) self->editParam (bp.dyn, bd.dyn ? 0.f : 1.f);
        }
        if (r == 1)
            self->setSelection (self->selMask & ~mask, -1);
    });
}

void EqGraph::mouseDown (const juce::MouseEvent& e)
{
    grabKeyboardFocus();
    proc.historyTouch();
    drag = Drag::None;
    dragBand = -1;

    const int b = hitTest (e.position);

    // mover la vista: Alt + arrastrar en vacío, o botón central
    if (e.mods.isMiddleButtonDown() || (e.mods.isAltDown() && b < 0))
    {
        drag = Drag::Pan;
        panStart = e.position;
        panStartView = tgt;
        return;
    }

    // clic en el triángulo de una resonancia: crea una banda que la reduce
    if (showRes)
        for (auto& r : res)
            if (std::abs (e.position.x - freqToX (r.freq)) < 10.f && e.position.y < plot().getY() + 24.f)
            {
                addBand (r.freq, -juce::jlimit (3.f, 12.f, r.excess * 0.7f), EqMath::Bell, 6.f);
                return;
            }

    if (b < 0)
    {
        // clic en vacío: selección rectangular
        drag = Drag::Marquee;
        marqueeStart = e.position;
        marquee = {};
        marqueeBase = (e.mods.isShiftDown() || e.mods.isCommandDown()) ? selMask : 0u;
        if (marqueeBase == 0u)
            setSelection (0u, -1);
        return;
    }

    if (e.mods.isPopupMenu())
    {
        if (! isSel (b)) setSelection (1u << b, b);
        showMenu (b);
        return;
    }

    if (e.mods.isShiftDown() || e.mods.isCommandDown())
    {
        const auto m = selMask ^ (1u << b);
        setSelection (m, (m >> b) & 1u ? b : -1);
        if (! isSel (b)) return;
    }
    else if (! isSel (b))
    {
        setSelection (1u << b, b);
    }
    else if (selected != b)
    {
        setSelection (selMask, b);
    }

    // preparar el arrastre del grupo
    drag = Drag::Node;
    dragBand = b;
    gestures.clear();
    for (int i = 0; i < numBands; ++i)
    {
        if (! isSel (i)) continue;
        const auto bd = band (i);
        startF[i] = bd.freq;
        startG[i] = bd.gain;
        const auto& bp = proc.bandParam[(size_t) i];
        bp.freq->beginChangeGesture();
        bp.gain->beginChangeGesture();
        gestures.push_back (bp.freq);
        gestures.push_back (bp.gain);
    }

    anchorMouse = e.position;
    anchorNode  = nodePos (b);
    lastFine    = e.mods.isAltDown();
}

void EqGraph::mouseDrag (const juce::MouseEvent& e)
{
    auto pl = plot();

    if (drag == Drag::Pan)
    {
        const auto d = e.position - panStart;
        const double perPx = (panStartView.hi - panStartView.lo) / (double) pl.getWidth();
        const double dbPerPx = 2.0 * panStartView.half / (double) pl.getHeight();
        tgt = panStartView;
        tgt.lo -= (double) d.x * perPx;
        tgt.hi -= (double) d.x * perPx;
        tgt.center += (double) d.y * dbPerPx;
        clampView (tgt);
        cur = tgt;
        specDirty = curveDirty = true;
        repaint();
        return;
    }

    if (drag == Drag::Marquee)
    {
        marquee = juce::Rectangle<float> (marqueeStart, e.position);
        juce::uint32 m = marqueeBase;
        for (int b = 0; b < numBands; ++b)
            if (bandUsed (b) && marquee.contains (nodePos (b)))
                m |= (1u << b);
        setSelection (m, -1);
        repaint();
        return;
    }

    if (drag != Drag::Node || dragBand < 0)
        return;

    // Alt = ajuste fino (10 veces más lento)
    const bool fine = e.mods.isAltDown();
    if (fine != lastFine)
    {
        anchorMouse = e.position;
        anchorNode  = nodePos (dragBand);
        lastFine    = fine;
    }
    const auto target = anchorNode + (e.position - anchorMouse) * (fine ? 0.1f : 1.0f);

    const double dOct = std::log2 ((double) xToFreq (target.x) / (double) startF[dragBand]);
    const float dG = yToGain (target.y) - startG[dragBand];

    for (int i = 0; i < numBands; ++i)
    {
        if (! isSel (i)) continue;
        const auto& bp = proc.bandParam[(size_t) i];

        if (proc.pBand[(size_t) i].on->load() < 0.5f)       // al mover una banda apagada, se enciende
            setNorm (bp.on, 1.f);

        setNorm (bp.freq, (float) juce::jlimit (20.0, 20000.0, (double) startF[i] * std::pow (2.0, dOct)));
        if ((int) std::lround (proc.pBand[(size_t) i].type->load()) != EqMath::Notch)
            setNorm (bp.gain, juce::jlimit (-24.f, 24.f, startG[i] + dG));
    }
}

void EqGraph::mouseUp (const juce::MouseEvent&)
{
    for (auto* p : gestures)
        p->endChangeGesture();
    gestures.clear();

    if (drag == Drag::Marquee)
    {
        marquee = {};
        repaint();
    }
    drag = Drag::None;
    dragBand = -1;
}

void EqGraph::mouseMove (const juce::MouseEvent& e)
{
    hoverPos = e.position;
    hasHover = true;
    if (AppSettings::get().cursorReadout)
        repaint();
}

void EqGraph::mouseExit (const juce::MouseEvent&)
{
    hasHover = false;
    repaint();
}

void EqGraph::mouseDoubleClick (const juce::MouseEvent& e)
{
    proc.historyTouch();
    const int b = hitTest (e.position);
    if (b >= 0)
    {
        editParam (proc.bandParam[(size_t) b].gain, 0.f);
        return;
    }

    // doble clic en un espacio vacío: nueva banda
    if (plot().contains (e.position))
        addBand (xToFreq (e.position.x), yToGain (e.position.y), EqMath::Bell, 1.f);
}

void EqGraph::mouseWheelMove (const juce::MouseEvent& e, const juce::MouseWheelDetails& w)
{
    proc.historyTouch();

    // Sentido físico de los dedos: positivo = hacia arriba, con o sin "desplazamiento natural"
    float up = w.isReversed ? -w.deltaY : w.deltaY;
    if (AppSettings::get().invertSwipe) up = -up;

    // Cmd/Ctrl + rueda: zoom en frecuencia · Alt + rueda: zoom vertical
    if (e.mods.isCommandDown()) { zoomFreq (e.position.x, std::exp (-(double) up * 1.2)); return; }
    if (e.mods.isAltDown())     { zoomGain (e.position.y, std::exp (-(double) up * 1.2)); return; }

    // Deslizar en horizontal: mueve la vista
    if (std::abs (w.deltaX) > std::abs (w.deltaY))
    {
        const double span = tgt.hi - tgt.lo;
        tgt.lo -= (double) w.deltaX * span * 0.5;
        tgt.hi = tgt.lo + span;
        clampView (tgt);
        animating = true;
        startTimerHz (60);
        return;
    }

    // Deslizar dos dedos arriba / abajo: sube / baja la Q (como en Pro-Q 3). No hace zoom.
    if (w.isInertial)
        return;                                   // la inercia del trackpad no debe seguir moviendo la Q

    const int hit = hitTest (e.position);
    juce::uint32 mask = 0;
    if (hit >= 0)            mask = isSel (hit) ? selMask : (1u << hit);
    else if (selected >= 0)  mask = selMask;
    if (mask == 0)
        return;

    const float k = std::exp (up * 2.0f);
    for (int i = 0; i < numBands; ++i)
        if ((mask >> i) & 1u)
            editParam (proc.bandParam[(size_t) i].q, proc.pBand[(size_t) i].q->load() * k);
}

void EqGraph::mouseMagnify (const juce::MouseEvent& e, float scaleFactor)
{
    // Abrir / cerrar dos dedos: zoom en frecuencia
    if (scaleFactor > 0.f)
        zoomFreq (e.position.x, 1.0 / (double) scaleFactor);
}

//==============================================================================
//  IJEQEditor
//==============================================================================
IJEQEditor::IJEQEditor (IJEQProcessor& p)
    : AudioProcessorEditor (&p), proc (p),
      graph (p, [this] (int b) { selectBand (b); })
{
    const auto& st = AppSettings::get();

    setLookAndFeel (&laf);
    setWantsKeyboardFocus (true);
    proc.uiOpen.store (true);

    // ---- estructura
    addAndMakeVisible (graph);
    addAndMakeVisible (topRegion);
    addAndMakeVisible (bottomRegion);
    addAndMakeVisible (leftRegion);
    addAndMakeVisible (rightRegion);
    leftRegion.addAndMakeVisible (meterIn);
    rightRegion.addAndMakeVisible (meterOut);
    bottomRegion.addAndMakeVisible (filtersPanel);
    bottomRegion.addAndMakeVisible (bandPanel);

    // ---- estilo de botones
    auto toggle = [] (CyberButton& b, juce::Colour neon)
    {
        b.setClickingTogglesState (true);
        b.setNeon (neon);
    };

    toggle (bypassBtn, Cyber::orange);
    toggle (polBtn, Cyber::magenta);
    toggle (analyzerBtn, Cyber::cyan);
    toggle (resBtn, Cyber::magenta);
    toggle (pauseBtn, Cyber::yellow);
    toggle (hpfOn, Cyber::green);
    toggle (lpfOn, Cyber::green);
    toggle (bandOn, Cyber::green);
    toggle (dynBtn, Cyber::yellow);
    infoBtn.setNeon (Cyber::yellow);
    presetBtn.setNeon (Cyber::cyan);
    saveBtn.setNeon (Cyber::green);
    clearBtn.setNeon (Cyber::magenta);
    centerBtn.setNeon (Cyber::cyan);
    addBtn.setNeon (Cyber::cyan);
    delBtn.setNeon (Cyber::magenta);
    hpfOn.setButtonText ("ON");
    lpfOn.setButtonText ("ON");

    undoBtn.setNeon (Cyber::yellow);  undoBtn.setArrow (0.5f);
    redoBtn.setNeon (Cyber::yellow);  redoBtn.setArrow (0.f);
    settingsBtn.setNeon (Cyber::cyan);  settingsBtn.setGear();
    analyzerBtn.setToggleState (true, juce::dontSendNotification);

    // ---- barra superior
    for (auto* c : std::initializer_list<juce::Component*> { &analyzerBtn, &resBtn, &rangeBox, &infoBtn, &bypassBtn, &polBtn,
                                                              &lblIn, &inSl, &lblOut, &outSl, &undoBtn, &redoBtn,
                                                              &presetBtn, &saveBtn, &clearBtn, &centerBtn, &pauseBtn })
        topRegion.addAndMakeVisible (*c);

    for (auto* s : { &inSl, &outSl })
    {
        s->setSliderStyle (juce::Slider::LinearHorizontal);
        s->setTextBoxStyle (juce::Slider::TextBoxRight, false, 78, 20);
        s->setColour (juce::Slider::trackColourId, Cyber::accent());
    }

    rangeBox.onChange = [this] { graph.setRange (rangeBox.getSelectedId() * 6); };

    analyzerBtn.onClick = [this] { graph.setSpectrum (analyzerBtn.getToggleState()); };
    resBtn.onClick      = [this] { graph.setResonances (resBtn.getToggleState()); };
    pauseBtn.onClick    = [this] { graph.setPaused (pauseBtn.getToggleState()); };
    centerBtn.onClick   = [this] { graph.resetView(); rangeBox.setSelectedId (3, juce::dontSendNotification); };
    infoBtn.onClick     = [this] { showInfo(); };
    settingsBtn.onClick = [this] { showSettings(); };

    undoBtn.onClick = [this] { proc.undo(); graph.refreshSelection(); };
    redoBtn.onClick = [this] { proc.redo(); graph.refreshSelection(); };
    presetBtn.onClick = [this] { showPresetMenu(); };
    saveBtn.onClick   = [this] { askSavePreset(); };
    clearBtn.onClick  = [this]
    {
        runProgrammatic ([this] { Presets::clearEq (proc.apvts, true); });
        setPresetName ({});
    };

    // ---- panel de filtros de corte
    for (auto* c : std::initializer_list<juce::Component*> { &lblHpf, &hpfOn, &hpfFreq, &hpfSlope, &lblLpf, &lpfOn, &lpfFreq, &lpfSlope })
        filtersPanel.addAndMakeVisible (*c);
    knob (hpfFreq);  knob (lpfFreq);
    for (auto* c : { &hpfSlope, &lpfSlope })
        c->addItemList ({ "12 dB/oct", "24 dB/oct", "36 dB/oct", "48 dB/oct" }, 1);

    // ---- panel de banda
    for (auto* c : std::initializer_list<juce::Component*> { &bType, &bandOn, &dynBtn, &addBtn, &delBtn,
                                                              &bFreq, &bGain, &bQ, &dRange, &dThr, &dAtk, &dRel,
                                                              &lblFreq, &lblGain, &lblQ, &lblRange, &lblThr, &lblAtk, &lblRel })
        bandPanel.addAndMakeVisible (*c);
    for (auto* s : { &bFreq, &bGain, &bQ, &dRange, &dThr, &dAtk, &dRel })
        knob (*s);

    addBtn.onClick = [this] { graph.addBand (1000.f, 0.f, EqMath::Bell, 1.f); };
    delBtn.onClick = [this] { graph.deleteSelected(); };

    // ---- títulos y textos de los paneles
    topRegion.paintOver = [] (juce::Graphics& g)
    {
        const auto acc = Cyber::accent();
        g.setFont (Cyber::font (32.f, true));
        g.setColour (Cyber::magenta.withAlpha (0.35f));
        g.drawText ("IJEQ", juce::Rectangle<int> (2, 1, 230, 38), juce::Justification::centredLeft);
        g.setColour (acc);
        g.drawText ("IJEQ", juce::Rectangle<int> (0, 0, 230, 38), juce::Justification::centredLeft);
        if (AppSettings::get().showSubtitle)
        {
            g.setColour (Cyber::textDim);
            g.setFont (Cyber::font (10.f, true));
            g.drawText (tr ("PARAMETRIC EQUALIZER"), juce::Rectangle<int> (2, 36, 230, 14), juce::Justification::centredLeft);
        }
    };

    filtersPanel.paintOver = [] (juce::Graphics& g)
    {
        g.setColour (Cyber::magenta);
        g.setFont (Cyber::font (13.f, true));
        g.drawText (tr ("// CUT FILTERS"), 20, 14, 250, 18, juce::Justification::centredLeft);
    };

    bandPanel.paintOver = [this] (juce::Graphics& g)
    {
        g.setColour (Cyber::magenta);
        g.setFont (Cyber::font (13.f, true));

        juce::String title = tr ("// NO BAND SELECTED");
        if (selected >= 0)
        {
            const int n = graph.selectedCount();
            title = tr ("// BAND") + " " + juce::String (selected + 1) + "  -  " + bType.getText().toUpperCase();
            if (n > 1) title += "   (" + juce::String (n) + " " + tr ("bands move together") + ")";
        }
        g.drawText (title, 20, 14, 620, 18, juce::Justification::centredLeft);

        if (AppSettings::get().showCaptions)
        {
            g.setFont (Cyber::font (11.f, true));
            g.setColour (Cyber::textDim);
            g.drawText (tr ("FILTER"), capFilter, juce::Justification::centred);
            g.drawText (tr ("DYNAMICS"), capDyn, juce::Justification::centred);
        }
    };

    // ---- asas para minimizar zonas
    static const float openDir[4] = { 0.75f, 0.25f, 0.5f, 0.f };
    const bool startCollapsed[4] = { st.topCollapsed, st.bottomCollapsed, st.sidesCollapsed, st.sidesCollapsed };
    for (int i = 0; i < 4; ++i)
    {
        vis[i] = visTarget[i] = startCollapsed[i] ? 0.f : 1.f;
        handle[i].setNeon (Cyber::cyan);
        handle[i].setArrow (startCollapsed[i] ? std::fmod (openDir[i] + 0.5f, 1.f) : openDir[i]);
        handle[i].onClick = [this, i] { togglePanel (i); };
        addAndMakeVisible (handle[i]);
    }

    addAndMakeVisible (settingsBtn);

    // ---- enlace a Instagram
    link.setFont (juce::Font (Cyber::font (11.5f)), false, juce::Justification::centredRight);
    link.setColour (juce::HyperlinkButton::textColourId, Cyber::cyan.withAlpha (0.8f));
    addAndMakeVisible (link);

    animTimer.fn = [this] { stepPanels(); };

    filtersPanel.setPanelOpacity (st.panelOpacity);
    bandPanel.setPanelOpacity (st.panelOpacity);
    applyTexts();

    // ---- conexión con los parámetros
    auto& a = proc.apvts;
    aIn = std::make_unique<SA> (a, "inGain", inSl);
    aOut = std::make_unique<SA> (a, "outGain", outSl);
    aBypass = std::make_unique<BA> (a, "bypass", bypassBtn);
    aPol = std::make_unique<BA> (a, "polarity", polBtn);
    aHpfOn = std::make_unique<BA> (a, "hpfOn", hpfOn);
    aHpfFreq = std::make_unique<SA> (a, "hpfFreq", hpfFreq);
    aHpfSlope = std::make_unique<CA> (a, "hpfSlope", hpfSlope);
    aLpfOn = std::make_unique<BA> (a, "lpfOn", lpfOn);
    aLpfFreq = std::make_unique<SA> (a, "lpfFreq", lpfFreq);
    aLpfSlope = std::make_unique<CA> (a, "lpfSlope", lpfSlope);

    selectBand (-1);
    setResizable (true, true);
    setResizeLimits (1260, 460, 2600, 1800);
    setSize (1260, 840);
    startTimerHz (30);
}

IJEQEditor::~IJEQEditor()
{
    animTimer.stopTimer();
    proc.uiOpen.store (false);
    setLookAndFeel (nullptr);
}

void IJEQEditor::knob (juce::Slider& s)
{
    s.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 92, 18);
    s.setColour (juce::Slider::rotarySliderFillColourId, Cyber::cyan);
}

void IJEQEditor::label (juce::Label& l, const char* key)
{
    l.setText (tr (key), juce::dontSendNotification);
    l.setJustificationType (juce::Justification::centred);
    l.setColour (juce::Label::textColourId, Cyber::cyan.withAlpha (0.9f));
    l.setFont (juce::Font (Cyber::font (11.5f, true)));
}

// Escribe todos los textos en el idioma elegido
void IJEQEditor::applyTexts()
{
    laf.refreshFont();

    analyzerBtn.setButtonText (tr ("ANALYZER"));
    resBtn.setButtonText (tr ("RESONANCES"));
    pauseBtn.setButtonText (tr ("PAUSE"));
    infoBtn.setButtonText (tr ("INFO"));
    bypassBtn.setButtonText (tr ("BYPASS"));
    polBtn.setButtonText (tr ("Ø POL"));
    saveBtn.setButtonText (tr ("SAVE"));
    clearBtn.setButtonText (tr ("CLEAR ALL"));
    centerBtn.setButtonText (tr ("CENTER VIEW"));
    bandOn.setButtonText (tr ("ACTIVE"));
    dynBtn.setButtonText (tr ("DYNAMIC"));
    addBtn.setButtonText (tr ("+ BAND"));
    delBtn.setButtonText (tr ("DELETE"));
    setPresetName (currentPreset);

    label (lblIn, "IN");           label (lblOut, "OUT");
    label (lblFreq, "FREQUENCY");  label (lblGain, "GAIN");        label (lblQ, "Q");
    label (lblRange, "RANGE");     label (lblThr, "THRESHOLD");    label (lblAtk, "ATTACK");   label (lblRel, "RELEASE");
    label (lblHpf, "HPF");         label (lblLpf, "LPF");

    const int rs = rangeBox.getSelectedId();
    rangeBox.clear (juce::dontSendNotification);
    for (int i = 1; i <= 4; ++i)
        rangeBox.addItem (tr ("Range") + juce::String::fromUTF8 (" ±") + juce::String (i * 6) + " dB", i);
    rangeBox.setSelectedId (rs > 0 ? rs : 3, juce::dontSendNotification);

    const int ts = bType.getSelectedId();
    bType.clear (juce::dontSendNotification);
    bType.addItem (tr ("Bell"), 1);
    bType.addItem (tr ("Low shelf"), 2);
    bType.addItem (tr ("High shelf"), 3);
    bType.addItem (tr ("Notch"), 4);
    if (ts > 0) bType.setSelectedId (ts, juce::dontSendNotification);

    topRegion.repaint();
    bandPanel.repaint();
    filtersPanel.repaint();
    repaint();
}

void IJEQEditor::settingsChanged()
{
    const auto& st = AppSettings::get();
    applyTexts();
    rebuildBackground();
    filtersPanel.setPanelOpacity (st.panelOpacity);
    bandPanel.setPanelOpacity (st.panelOpacity);
    for (auto* s : { &inSl, &outSl })
        s->setColour (juce::Slider::trackColourId, Cyber::accent());
    graph.settingsChanged();
    meterIn.repaint();
    meterOut.repaint();
    if (info) info->refresh();
    repaint();
}

void IJEQEditor::showInfo()
{
    if (! info)
    {
        info = std::make_unique<InfoOverlay>();
        info->onClose = [this] { info->setVisible (false); };
        addChildComponent (*info);
    }
    info->setBounds (getLocalBounds().reduced (60, 40));
    info->refresh();
    info->setVisible (true);
    info->toFront (true);
}

void IJEQEditor::showSettings()
{
    if (! settingsPanel)
    {
        settingsPanel = std::make_unique<SettingsOverlay>();
        settingsPanel->onClose = [this] { settingsPanel->setVisible (false); };
        settingsPanel->onChanged = [this] { settingsChanged(); };
        addChildComponent (*settingsPanel);
    }
    settingsPanel->setBounds (getLocalBounds().reduced (60, 40));
    settingsPanel->setVisible (true);
    settingsPanel->toFront (true);
}

void IJEQEditor::selectBand (int b)
{
    selected = b;

    aBandOn.reset(); aDyn.reset(); aBType.reset(); aBFreq.reset(); aBGain.reset(); aBQ.reset();
    aDRange.reset(); aDThr.reset(); aDAtk.reset(); aDRel.reset();

    if (b >= 0)
    {
        auto& a = proc.apvts;
        aBandOn = std::make_unique<BA> (a, bandId (b, "on"), bandOn);
        aDyn    = std::make_unique<BA> (a, bandId (b, "dyn"), dynBtn);
        aBType  = std::make_unique<CA> (a, bandId (b, "type"), bType);
        aBFreq  = std::make_unique<SA> (a, bandId (b, "freq"), bFreq);
        aBGain  = std::make_unique<SA> (a, bandId (b, "gain"), bGain);
        aBQ     = std::make_unique<SA> (a, bandId (b, "q"), bQ);
        aDRange = std::make_unique<SA> (a, bandId (b, "dRange"), dRange);
        aDThr   = std::make_unique<SA> (a, bandId (b, "dThr"), dThr);
        aDAtk   = std::make_unique<SA> (a, bandId (b, "dAtk"), dAtk);
        aDRel   = std::make_unique<SA> (a, bandId (b, "dRel"), dRel);

        for (auto* s : { &bFreq, &bGain, &bQ, &dRange, &dThr, &dAtk, &dRel })
            s->setColour (juce::Slider::rotarySliderFillColourId, Cyber::band (b));
    }

    updateEnabled();
    bandPanel.repaint();
}

void IJEQEditor::updateEnabled()
{
    const bool has = selected >= 0;
    const bool notch = has && (bType.getSelectedId() - 1) == EqMath::Notch;
    const bool dynOn = has && ! notch && dynBtn.getToggleState();

    auto en = [] (juce::Component& c, bool on)
    {
        if (c.isEnabled() != on) c.setEnabled (on);
        c.setAlpha (on ? 1.f : 0.35f);
    };
    en (bandOn, has);  en (bType, has);  en (bFreq, has);  en (bQ, has);  en (delBtn, has);
    en (bGain, has && ! notch);  en (dynBtn, has && ! notch);
    en (dRange, dynOn);  en (dThr, dynOn);  en (dAtk, dynOn);  en (dRel, dynOn);
}

void IJEQEditor::timerCallback()
{
    const auto& st = AppSettings::get();
    auto db = [] (float amp) { return juce::Decibels::gainToDecibels (amp, -100.f); };

    static const float fallRms[3] = { 0.95f, 0.90f, 0.80f }, fallPk[3] = { 0.985f, 0.97f, 0.93f };
    const float fr = fallRms[st.meterFall], fp = fallPk[st.meterFall];

    for (size_t c = 0; c < 2; ++c)
    {
        inR[c]  = juce::jmax (std::sqrt (2.f * proc.inMs[c].load()),  inR[c]  * fr);
        outR[c] = juce::jmax (std::sqrt (2.f * proc.outMs[c].load()), outR[c] * fr);
        inP[c]  = juce::jmax (proc.inPk[c].load(),  inP[c]  * fp);
        outP[c] = juce::jmax (proc.outPk[c].load(), outP[c] * fp);
    }
    if (vis[2] > 0.01f) meterIn.setLevels  (db (inR[0]),  db (inR[1]),  db (inP[0]),  db (inP[1]));
    if (vis[3] > 0.01f) meterOut.setLevels (db (outR[0]), db (outR[1]), db (outP[0]), db (outP[1]));

    updateEnabled();

    // Historial de deshacer / rehacer: solo se anotan los cambios hechos por el usuario
    const bool raw = juce::ModifierKeys::getCurrentModifiersRealtime().isAnyMouseButtonDown()
                  || dynamic_cast<juce::TextEditor*> (juce::Component::getCurrentlyFocusedComponent()) != nullptr;
    proc.historyTick (raw);

    const bool cu = proc.canUndo(), cr = proc.canRedo();
    undoBtn.setEnabled (cu);  undoBtn.setAlpha (cu ? 1.f : 0.3f);
    redoBtn.setEnabled (cr);  redoBtn.setAlpha (cr ? 1.f : 0.3f);
}

//==============================================================================
//  Maquetación, zonas minimizables y animación
//==============================================================================
void IJEQEditor::togglePanel (int i)
{
    static const float openDir[4] = { 0.75f, 0.25f, 0.5f, 0.f };

    visTarget[i] = visTarget[i] > 0.5f ? 0.f : 1.f;
    const bool open = visTarget[i] > 0.5f;
    handle[i].setArrow (open ? openDir[i] : std::fmod (openDir[i] + 0.5f, 1.f));
    animTimer.startTimerHz (60);
}

void IJEQEditor::stepPanels()
{
    const float k = AppSettings::get().animations ? 0.28f : 1.f;
    bool moving = false;
    for (int i = 0; i < 4; ++i)
    {
        if (vis[i] == visTarget[i]) continue;
        vis[i] += (visTarget[i] - vis[i]) * k;
        if (std::abs (vis[i] - visTarget[i]) < 0.004f) vis[i] = visTarget[i];
        moving = true;
    }
    layoutMain();
    if (! moving)
        animTimer.stopTimer();
}

void IJEQEditor::layoutMain()
{
    auto r = getLocalBounds().reduced (12, 10);
    footer = r.removeFromBottom (20);
    settingsBtn.setBounds (footer.getX(), footer.getY(), 24, 20);
    link.setBounds (footer.removeFromRight (200));

    const int topH   = (int) std::lround (vis[0] * topFull);
    const int botH   = (int) std::lround (vis[1] * botFull);
    const int leftW  = (int) std::lround (vis[2] * meterFull);
    const int rightW = (int) std::lround (vis[3] * meterFull);

    topRegion.setBounds (r.removeFromTop (topH));
    r.removeFromTop ((int) std::lround (vis[0] * 6.f));
    bottomRegion.setBounds (r.removeFromBottom (botH));
    r.removeFromBottom ((int) std::lround (vis[1] * 10.f));
    leftRegion.setBounds (r.removeFromLeft (leftW));
    r.removeFromLeft ((int) std::lround (vis[2] * 8.f));
    rightRegion.setBounds (r.removeFromRight (rightW));
    r.removeFromRight ((int) std::lround (vis[3] * 8.f));
    graph.setBounds (r);

    topRegion.setVisible (topH > 0);
    bottomRegion.setVisible (botH > 0);
    leftRegion.setVisible (leftW > 0);
    rightRegion.setVisible (rightW > 0);

    meterIn.setBounds (0, 0, meterFull, leftRegion.getHeight());
    meterOut.setBounds (0, 0, meterFull, rightRegion.getHeight());
    if (topH > 0) layoutTop();
    if (botH > 0) layoutBottom();

    const auto gb = graph.getBounds();
    handle[0].setBounds (gb.getCentreX() - 20, gb.getY() + 4, 40, 16);
    handle[1].setBounds (gb.getCentreX() - 20, gb.getBottom() - 20, 40, 16);
    handle[2].setBounds (gb.getX() + 4, gb.getCentreY() - 20, 16, 40);
    handle[3].setBounds (gb.getRight() - 20, gb.getCentreY() - 20, 16, 40);
    for (auto& h : handle) h.toFront (false);
}

void IJEQEditor::layoutTop()
{
    const int W = topRegion.getWidth();

    auto r1 = juce::Rectangle<int> (0, 0, W, 50);
    r1.removeFromLeft (236);
    analyzerBtn.setBounds (r1.removeFromLeft (112).reduced (0, 10));  r1.removeFromLeft (6);
    resBtn.setBounds      (r1.removeFromLeft (124).reduced (0, 10));  r1.removeFromLeft (6);
    rangeBox.setBounds    (r1.removeFromLeft (118).reduced (0, 10));  r1.removeFromLeft (6);
    infoBtn.setBounds     (r1.removeFromLeft (64).reduced (0, 10));

    bypassBtn.setBounds (r1.removeFromRight (84).reduced (0, 10));  r1.removeFromRight (6);
    polBtn.setBounds    (r1.removeFromRight (84).reduced (0, 10));  r1.removeFromRight (10);

    auto placeGain = [] (juce::Label& l, juce::Slider& s, juce::Rectangle<int> b)
    {
        l.setBounds (b.removeFromLeft (40));
        s.setBounds (b);
    };
    placeGain (lblOut, outSl, r1.removeFromRight (190));  r1.removeFromRight (8);
    placeGain (lblIn,  inSl,  r1.removeFromRight (190));

    auto r2 = juce::Rectangle<int> (0, 56, W, 36);
    undoBtn.setBounds (r2.removeFromLeft (36).reduced (0, 2));  r2.removeFromLeft (6);
    redoBtn.setBounds (r2.removeFromLeft (36).reduced (0, 2));  r2.removeFromLeft (14);
    presetBtn.setBounds (r2.removeFromLeft (290).reduced (0, 2));  r2.removeFromLeft (6);
    saveBtn.setBounds   (r2.removeFromLeft (100).reduced (0, 2));  r2.removeFromLeft (14);
    clearBtn.setBounds  (r2.removeFromLeft (130).reduced (0, 2));

    pauseBtn.setBounds  (r2.removeFromRight (90).reduced (0, 2));   r2.removeFromRight (6);
    centerBtn.setBounds (r2.removeFromRight (150).reduced (0, 2));
}

void IJEQEditor::layoutBottom()
{
    const int W = bottomRegion.getWidth();

    filtersPanel.setFullSize (290, botFull);
    filtersPanel.setTopLeftPosition (0, 0);
    bandPanel.setFullSize (juce::jmax (420, W - 300), botFull);
    bandPanel.setTopLeftPosition (300, 0);

    {
        auto fp = juce::Rectangle<int> (0, 0, 290, botFull).reduced (16, 14);
        fp.removeFromTop (26);
        const int half = fp.getWidth() / 2;

        auto layoutCut = [] (juce::Rectangle<int> c, juce::Label& l, CyberButton& on, juce::Slider& f, juce::ComboBox& sl)
        {
            auto head = c.removeFromTop (28);
            l.setBounds (head.removeFromLeft (40));
            on.setBounds (head.reduced (4, 0));
            sl.setBounds (c.removeFromBottom (28).reduced (4, 0));
            c.removeFromBottom (6);
            f.setBounds (c);
        };
        layoutCut (fp.removeFromLeft (half), lblHpf, hpfOn, hpfFreq, hpfSlope);
        layoutCut (fp, lblLpf, lpfOn, lpfFreq, lpfSlope);
    }

    {
        auto bp = juce::Rectangle<int> (0, 0, bandPanel.getWidth(), botFull).reduced (16, 14);
        bp.removeFromTop (26);

        auto left = bp.removeFromLeft (170);
        bType.setBounds  (left.removeFromTop (30));  left.removeFromTop (8);
        bandOn.setBounds (left.removeFromTop (30));  left.removeFromTop (8);
        dynBtn.setBounds (left.removeFromTop (30));  left.removeFromTop (8);
        addBtn.setBounds (left.removeFromTop (30));  left.removeFromTop (8);
        delBtn.setBounds (left.removeFromTop (30));
        bp.removeFromLeft (12);

        const int kw = bp.getWidth() / 7;
        auto caps = bp.removeFromTop (16);
        capFilter = caps.removeFromLeft (kw * 3);
        capDyn = caps;

        auto place = [&] (juce::Label& l, juce::Slider& s)
        {
            auto col = bp.removeFromLeft (kw);
            l.setBounds (col.removeFromTop (18));
            s.setBounds (col);
        };
        place (lblFreq, bFreq);  place (lblGain, bGain);  place (lblQ, bQ);
        place (lblRange, dRange); place (lblThr, dThr);   place (lblAtk, dAtk);  place (lblRel, dRel);
    }
}

void IJEQEditor::rebuildBackground()
{
    const int w = juce::jmax (1, getWidth()), h = juce::jmax (1, getHeight());
    bgCache = juce::Image (juce::Image::RGB, w, h, false);
    juce::Graphics g (bgCache);
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff0e0919), 0.f, 0.f, juce::Colour (0xff04020a), 0.f, (float) h, false));
    g.fillAll();
    if (AppSettings::get().scanlines)
    {
        g.setColour (juce::Colours::black.withAlpha (0.20f));
        for (int y = 0; y < h; y += 3)
            g.drawHorizontalLine (y, 0.f, (float) w);
    }
}

void IJEQEditor::resized()
{
    rebuildBackground();
    if (info) info->setBounds (getLocalBounds().reduced (60, 40));
    if (settingsPanel) settingsPanel->setBounds (getLocalBounds().reduced (60, 40));
    layoutMain();
}

void IJEQEditor::paint (juce::Graphics& g)
{
    if (bgCache.isValid())
        g.drawImageAt (bgCache, 0, 0);
    else
        g.fillAll (Cyber::bg);

    g.setColour (Cyber::textDim);
    g.setFont (Cyber::font (11.f, true));
    g.drawText ("IJEQ", footer.withTrimmedLeft (34).withTrimmedRight (210), juce::Justification::centredLeft);
}

//==============================================================================
//  Teclado
//   ◀ ▶ cambian de banda · 1-9 saltan a esa banda · Cmd/Ctrl+Z deshace · Cmd/Ctrl+Shift+Z rehace
//==============================================================================
bool IJEQEditor::keyPressed (const juce::KeyPress& k)
{
    using juce::KeyPress;
    using juce::ModifierKeys;

    if (k == KeyPress ('z', ModifierKeys (ModifierKeys::commandModifier | ModifierKeys::shiftModifier), 0)
        || k == KeyPress ('y', ModifierKeys (ModifierKeys::commandModifier), 0))
    {
        proc.redo();
        graph.refreshSelection();
        return true;
    }
    if (k == KeyPress ('z', ModifierKeys (ModifierKeys::commandModifier), 0))
    {
        proc.undo();
        graph.refreshSelection();
        return true;
    }
    if (k == KeyPress (KeyPress::leftKey))  { graph.selectNeighbor (-1); return true; }
    if (k == KeyPress (KeyPress::rightKey)) { graph.selectNeighbor (1);  return true; }
    if (k == KeyPress (KeyPress::deleteKey) || k == KeyPress (KeyPress::backspaceKey))
    {
        graph.deleteSelected();
        return true;
    }

    // Teclas numéricas: 1–9, o dos dígitos seguidos (por ejemplo 1 y 2 = banda 12)
    const auto ch = k.getTextCharacter();
    if (ch >= '0' && ch <= '9' && ! k.getModifiers().isCommandDown() && ! k.getModifiers().isCtrlDown())
    {
        const auto now = juce::Time::getMillisecondCounter();
        if (now - digitTime > 700u) digitBuf.clear();
        digitTime = now;

        digitBuf += juce::String::charToString (ch);
        int n = digitBuf.getIntValue();
        if (n < 1 || n > numBands)
        {
            digitBuf = juce::String::charToString (ch);
            n = digitBuf.getIntValue();
        }
        if (n >= 1 && n <= numBands && graph.bandUsed (n - 1))
        {
            graph.selectBandIndex (n - 1);
            return true;
        }
        return false;
    }
    return false;
}

//==============================================================================
//  Presets y borrar todo
//==============================================================================
void IJEQEditor::runProgrammatic (std::function<void()> change)
{
    proc.historyCheckpoint();
    change();
    proc.historyAfterProgrammatic();
    graph.refreshSelection();
}

void IJEQEditor::setPresetName (const juce::String& n)
{
    currentPreset = n;
    presetBtn.setButtonText (n.isEmpty() ? tr ("PRESETS") : tr ("PRESET: ") + n);
}

void IJEQEditor::showPresetMenu()
{
    juce::PopupMenu menu;
    const auto& list = Presets::factory();

    juce::PopupMenu sub;
    juce::String cat;
    for (size_t i = 0; i < list.size(); ++i)
    {
        const auto c = tr (list[i].category);
        if (c != cat)
        {
            if (cat.isNotEmpty()) menu.addSubMenu (cat, sub);
            sub = juce::PopupMenu();
            cat = c;
        }
        sub.addItem (1000 + (int) i, tr (list[i].name));
    }
    if (cat.isNotEmpty()) menu.addSubMenu (cat, sub);

    menu.addSeparator();

    const auto files = Presets::userFiles();
    juce::PopupMenu mine;
    if (files.empty())
        mine.addItem (9, tr ("(empty)"), false, false);
    for (size_t i = 0; i < files.size(); ++i)
        mine.addItem (2000 + (int) i, files[i].getFileNameWithoutExtension());
    menu.addSubMenu (tr ("My presets"), mine);

    menu.addSeparator();
    menu.addItem (1, tr ("Save current preset..."));
    menu.addItem (2, tr ("Load from file..."));
    menu.addItem (3, tr ("Open presets folder"));

    juce::Component::SafePointer<IJEQEditor> self (this);
    menu.showMenuAsync (juce::PopupMenu::Options().withTargetComponent (&presetBtn), [self, files] (int r)
    {
        if (self == nullptr || r == 0)
            return;

        if (r >= 2000)
        {
            self->loadPresetFile (files[(size_t) (r - 2000)]);
        }
        else if (r >= 1000)
        {
            const auto& p = Presets::factory()[(size_t) (r - 1000)];
            self->runProgrammatic ([self, &p] { Presets::apply (self->proc.apvts, p); });
            self->setPresetName (tr (p.name));
        }
        else if (r == 1)
        {
            self->askSavePreset();
        }
        else if (r == 2)
        {
            self->fileChooser = std::make_unique<juce::FileChooser> (tr ("Choose a preset"), Presets::userDir(), "*.paraeq");
            self->fileChooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                                            [self] (const juce::FileChooser& fc)
                                            {
                                                const auto f = fc.getResult();
                                                if (self != nullptr && f.existsAsFile())
                                                    self->loadPresetFile (f);
                                            });
        }
        else if (r == 3)
        {
            Presets::userDir().createDirectory();
            Presets::userDir().revealToUser();
        }
    });
}

void IJEQEditor::loadPresetFile (const juce::File& f)
{
    if (auto xml = juce::parseXML (f))
    {
        runProgrammatic ([this, &xml] { Presets::applyXml (proc.apvts, *xml); });
        setPresetName (f.getFileNameWithoutExtension());
    }
}

void IJEQEditor::askSavePreset()
{
    auto* w = new juce::AlertWindow (tr ("Save preset"), tr ("Enter a name for your preset:"),
                                     juce::MessageBoxIconType::NoIcon, this);
    w->addTextEditor ("name", currentPreset.isEmpty() ? tr ("My preset") : currentPreset, juce::String());
    w->addButton (tr ("Save"), 1, juce::KeyPress (juce::KeyPress::returnKey));
    w->addButton (tr ("Cancel"), 0, juce::KeyPress (juce::KeyPress::escapeKey));

    juce::Component::SafePointer<IJEQEditor> self (this);
    w->enterModalState (true, juce::ModalCallbackFunction::create ([self, w] (int result)
    {
        if (result != 1 || self == nullptr)
            return;

        const auto name = w->getTextEditorContents ("name").trim();
        if (name.isNotEmpty() && Presets::saveUser (self->proc.apvts, name))
            self->setPresetName (name);
    }), true);
}
