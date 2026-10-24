#pragma once
#include "PluginProcessor.h"
#include "CyberUI.h"
#include "Localization.h"

using Loc::tr;

//==============================================================================
//  Vúmetro estéreo de LEDs (L y R)
//==============================================================================
class StereoMeter : public juce::Component
{
public:
    explicit StereoMeter (const char* titleKey) : key (titleKey) { setOpaque (true); }

    void setLevels (float rmsL, float rmsR, float pkL, float pkR);
    void paint (juce::Graphics&) override;

private:
    const char* key;
    float rms[2] { -100.f, -100.f }, pk[2] { -100.f, -100.f };
};

//==============================================================================
//  Manual (botón INFO)
//==============================================================================
class InfoOverlay : public juce::Component
{
public:
    InfoOverlay();
    void refresh();                      // vuelve a escribir el texto (cambio de idioma)
    void paint (juce::Graphics&) override;
    void resized() override;
    std::function<void()> onClose;

private:
    juce::TextEditor text;
    CyberButton closeBtn;
};

//==============================================================================
//  Ajustes: General · Barra superior · Laterales · Inferior · Centro
//  Todos los controles se crean una sola vez; al cambiar de pestaña solo se muestran u ocultan.
//==============================================================================
class SettingRow;

class SettingsOverlay : public juce::Component
{
public:
    SettingsOverlay();
    ~SettingsOverlay() override;
    void paint (juce::Graphics&) override;
    void resized() override;
    void refreshAll();                       // vuelve a escribir textos y valores
    std::function<void()> onClose, onChanged;

private:
    template <typename T, typename... Args> T* addRow (Args&&... args);
    void showTab (int t);
    void layoutRows();
    void changed();

    int tab = 0;
    CyberButton tabBtn[5], closeBtn, resetBtn;
    juce::Viewport viewport;
    juce::Component page;
    std::vector<std::unique_ptr<SettingRow>> rows;
};

//==============================================================================
//  Pantalla interactiva
//==============================================================================
class EqGraph : public juce::Component, private juce::Timer
{
public:
    EqGraph (IJEQProcessor&, std::function<void (int)> onSelectBand);
    ~EqGraph() override;

    void setRange (int db);
    void resetView();
    void setSpectrum (bool on)   { showSpectrum = on; specDirty = true; repaint(); }
    void setResonances (bool on) { showRes = on; repaint(); }
    void setPaused (bool p)      { paused = p; repaint(); }
    void settingsChanged();

    void setSelected (int b);
    void selectBandIndex (int b);
    void refreshSelection();
    void selectNeighbor (int dir);
    int  addBand (float freq, float gainDb, int type, float q);
    void deleteBand (int b);
    void deleteSelected();
    int  firstUsed() const;
    int  numUsed() const;
    bool bandUsed (int b) const { return b >= 0 && b < EqMath::numBands && proc.pBand[(size_t) b].used->load() > 0.5f; }
    int  primary() const { return selected; }
    int  selectedCount() const;

    void paint (juce::Graphics&) override;
    void resized() override;
    void mouseDown (const juce::MouseEvent&) override;
    void mouseDrag (const juce::MouseEvent&) override;
    void mouseUp (const juce::MouseEvent&) override;
    void mouseMove (const juce::MouseEvent&) override;
    void mouseExit (const juce::MouseEvent&) override;
    void mouseDoubleClick (const juce::MouseEvent&) override;
    void mouseWheelMove (const juce::MouseEvent&, const juce::MouseWheelDetails&) override;
    void mouseMagnify (const juce::MouseEvent&, float scaleFactor) override;

private:
    struct View { double lo, hi, center, half; };
    struct Resonance { float freq, excess; };
    struct Band { bool used, on, dyn; int type; float freq, gain, q; };
    enum class Drag { None, Node, Marquee, Pan };

    void timerCallback() override;
    bool stepView();
    void clampView (View&) const;
    void zoomFreq (float x, double factor);
    void zoomGain (float y, double factor);
    void ensureVisible (float freq);

    void ensureFft();
    bool updateSpectrum();
    void rebuildSpectrum();
    void rebuildCurve();
    void ensureGrid (float ps);
    void findResonances();
    double responseDbAt (double f) const;
    float levelAt (float f) const;
    juce::uint32 paramHash() const;

    void showMenu (int band);
    void setSelection (juce::uint32 mask, int prim);
    juce::uint32 usedMask() const;

    Band band (int b) const;
    juce::Rectangle<float> plot() const;
    float freqToX (float f) const;
    float xToFreq (float x) const;
    float gainToY (float g) const;
    float yToGain (float y) const;
    juce::Point<float> nodePos (int b) const;
    bool nodeVisible (juce::Point<float> p) const;
    int  hitTest (juce::Point<float>) const;
    bool isSel (int b) const { return (selMask >> b) & 1u; }

    void setNorm (juce::RangedAudioParameter* p, float real) { p->setValueNotifyingHost (p->convertTo0to1 (real)); }
    void editParam (juce::RangedAudioParameter* p, float real);

    IJEQProcessor& proc;
    std::function<void (int)> onSelect;

    View cur { std::log (20.0), std::log (20000.0), 0.0, 18.0 }, tgt = cur;
    int selected = -1;
    juce::uint32 selMask = 0;
    bool showSpectrum = true, showRes = false, paused = false, specDirty = true, curveDirty = true;
    juce::uint32 lastHash = 0;

    // interacción
    Drag drag = Drag::None;
    int dragBand = -1;
    bool lastFine = false, animating = false, hasHover = false;
    juce::Point<float> anchorMouse, anchorNode, panStart, marqueeStart, hoverPos;
    View panStartView { 0, 0, 0, 0 };
    juce::Rectangle<float> marquee;
    juce::uint32 marqueeBase = 0;
    float startF[EqMath::numBands] {}, startG[EqMath::numBands] {};
    std::vector<juce::RangedAudioParameter*> gestures;

    // gráficos en caché
    juce::Image gridImg;
    View gridView { 0, 0, 0, 0 };
    float gridScale = 0.f;
    juce::Path curvePath, selFill, specFill, specLine;

    // última respuesta calculada (para la lectura bajo el cursor)
    EqMath::Coef cHp[EqMath::maxStages], cLp[EqMath::maxStages], cBand[EqMath::numBands];
    int cNh = 0, cNl = 0;
    bool cAct[EqMath::numBands] {};
    double cSr = 44100.0;

    // análisis de espectro (se crea al empezar a dibujar, no al abrir la ventana)
    int fftSize = 0;
    std::unique_ptr<juce::dsp::FFT> fft;
    std::unique_ptr<juce::dsp::WindowingFunction<float>> window;
    std::vector<float> fftData, spec, specSlow;
    float lastSpecSum = 0.f;
    std::vector<Resonance> res;
};

//==============================================================================
class IJEQEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit IJEQEditor (IJEQProcessor&);
    ~IJEQEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    bool keyPressed (const juce::KeyPress&) override;

private:
    struct FnTimer : juce::Timer
    {
        std::function<void()> fn;
        void timerCallback() override { if (fn) fn(); }
    };

    void timerCallback() override;
    void layoutMain();
    void layoutTop();
    void layoutBottom();
    void placeOverlays();
    void stepPanels();
    void togglePanel (int which);
    void rebuildBackground (float ps);
    void applyTexts();
    void settingsChanged();
    void showInfo();
    void showSettings();

    void selectBand (int b);
    void updateEnabled();
    void knob (juce::Slider&, bool primary);
    void label (juce::Label&, const char* key);
    void runProgrammatic (std::function<void()> change);
    void setPresetName (const juce::String&);
    void showPresetMenu();
    void askSavePreset();
    void loadPresetFile (const juce::File&);

    using SA = juce::AudioProcessorValueTreeState::SliderAttachment;
    using BA = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using CA = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    static constexpr int topFull = 96, botFull = 270, meterFull = 84;
    static constexpr float baseWidth = 1260.f;

    CyberLookAndFeel laf;
    IJEQProcessor& proc;
    Canvas content;                                  // todo el contenido; se escala con la ventana
    EqGraph graph;
    StereoMeter meterIn { "IN" }, meterOut { "OUT" };
    std::unique_ptr<InfoOverlay> info;               // se crean al usarlos: la ventana abre más rápido
    std::unique_ptr<SettingsOverlay> settingsPanel;

    Region topRegion, bottomRegion, leftRegion, rightRegion;
    Region filtersPanel { true }, bandPanel { true };

    juce::Label lblIn, lblOut, lblFreq, lblGain, lblQ, lblRange, lblThr, lblAtk, lblRel, lblHpf, lblLpf;
    juce::Slider inSl, outSl, hpfFreq, lpfFreq, bFreq, bGain, bQ, dRange, dThr, dAtk, dRel;
    CyberButton bypassBtn, polBtn, analyzerBtn, resBtn, infoBtn, hpfOn, lpfOn, bandOn, dynBtn, addBtn, delBtn;
    CyberButton presetBtn, saveBtn, clearBtn, centerBtn, pauseBtn, undoBtn, redoBtn, settingsBtn;
    CyberButton handle[4];
    juce::ComboBox hpfSlope, lpfSlope, bType, rangeBox;
    juce::HyperlinkButton link { "Instagram  @Ivanjoko_",
        juce::URL ("https://www.instagram.com/ivanjoko_?exln=MWpsYXhmODV0enQ1bA%3D%3D&utm_source=qr") };

    std::unique_ptr<SA> aIn, aOut, aHpfFreq, aLpfFreq, aBFreq, aBGain, aBQ, aDRange, aDThr, aDAtk, aDRel;
    std::unique_ptr<BA> aBypass, aPol, aHpfOn, aLpfOn, aBandOn, aDyn;
    std::unique_ptr<CA> aHpfSlope, aLpfSlope, aBType;

    float vis[4] { 1.f, 1.f, 1.f, 1.f }, visTarget[4] { 1.f, 1.f, 1.f, 1.f };
    FnTimer animTimer, bgTimer;
    EdgeResizer resizeR, resizeB, resizeC;

    int selected = -1;
    float inR[2] {}, inP[2] {}, outR[2] {}, outP[2] {};
    juce::Rectangle<int> capFilter, capDyn, footer;
    juce::Image bgCache;
    float bgScale = 0.f;
    int lastAccent = -1, lastLanguage = -1;
    float lastHue = -1.f;
    bool lastScanlines = true;
    juce::String currentPreset, digitBuf;
    juce::uint32 digitTime = 0;
    std::unique_ptr<juce::FileChooser> fileChooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IJEQEditor)
};
