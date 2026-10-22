#pragma once
#include <juce_core/juce_core.h>

//==============================================================================
//  Ajustes del usuario (se guardan en un archivo y se comparten entre ventanas)
//==============================================================================
struct AppSettings
{
    // General
    int  language = 0;                 // 0 English, 1 Español, 2 中文
    bool animations = true, scanlines = true;
    int  accent = 0;                   // 0 cian, 1 magenta, 2 verde, 3 naranja, 4 violeta

    // Barra superior
    bool topCollapsed = false, showSubtitle = true;

    // Laterales (vúmetros)
    bool sidesCollapsed = false, peakHold = true;
    int  meterRange = 60;              // dB visibles
    int  meterFall = 1;                // 0 lenta, 1 normal, 2 rápida

    // Panel inferior
    bool bottomCollapsed = false, showCaptions = true;
    float panelOpacity = 1.f;

    // Pantalla central
    float nodeSize = 1.f, nodeOpacity = 1.f, curveWidth = 1.8f, spectrumOpacity = 1.f, gridOpacity = 1.f;
    int  spectrumSmoothing = 1;        // 0 rápido, 1 normal, 2 suave
    int  analyzerRes = 2;              // 0 = 4096, 1 = 8192, 2 = 16384 puntos
    bool cursorReadout = true, showNumbers = true, fillSelected = true, invertSwipe = false;

    static juce::File file()
    {
        return juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
                   .getChildFile ("IJEQ").getChildFile ("settings.xml");
    }

    static AppSettings& get()
    {
        static AppSettings s = load();
        return s;
    }

    static AppSettings load()
    {
        AppSettings s;
        if (auto xml = juce::parseXML (file()))
            s.fromXml (*xml);
        return s;
    }

    void save() const
    {
        auto f = file();
        f.getParentDirectory().createDirectory();
        juce::XmlElement xml ("IJEQSettings");
        toXml (xml);
        xml.writeTo (f);
    }

    void reset() { *this = AppSettings(); }

private:
    template <typename F>
    void visit (F&& f)
    {
        f ("language", language);  f ("animations", animations);  f ("scanlines", scanlines);  f ("accent", accent);
        f ("topCollapsed", topCollapsed);  f ("showSubtitle", showSubtitle);
        f ("sidesCollapsed", sidesCollapsed);  f ("peakHold", peakHold);  f ("meterRange", meterRange);  f ("meterFall", meterFall);
        f ("bottomCollapsed", bottomCollapsed);  f ("showCaptions", showCaptions);  f ("panelOpacity", panelOpacity);
        f ("nodeSize", nodeSize);  f ("nodeOpacity", nodeOpacity);  f ("curveWidth", curveWidth);
        f ("spectrumOpacity", spectrumOpacity);  f ("gridOpacity", gridOpacity);
        f ("spectrumSmoothing", spectrumSmoothing);  f ("analyzerRes", analyzerRes);
        f ("cursorReadout", cursorReadout);  f ("showNumbers", showNumbers);  f ("fillSelected", fillSelected);  f ("invertSwipe", invertSwipe);
    }

    void fromXml (const juce::XmlElement& xml)
    {
        visit ([&xml] (const char* name, auto& v)
        {
            using T = std::decay_t<decltype (v)>;
            if (! xml.hasAttribute (name)) return;
            if constexpr (std::is_same_v<T, bool>)       v = xml.getBoolAttribute (name);
            else if constexpr (std::is_same_v<T, int>)   v = xml.getIntAttribute (name);
            else                                         v = (float) xml.getDoubleAttribute (name);
        });
        language = juce::jlimit (0, 2, language);
        accent = juce::jlimit (0, 4, accent);
        meterFall = juce::jlimit (0, 2, meterFall);
        spectrumSmoothing = juce::jlimit (0, 2, spectrumSmoothing);
        analyzerRes = juce::jlimit (0, 2, analyzerRes);
        meterRange = juce::jlimit (30, 90, meterRange);
    }

    void toXml (juce::XmlElement& xml) const
    {
        const_cast<AppSettings*> (this)->visit ([&xml] (const char* name, auto& v)
        {
            using T = std::decay_t<decltype (v)>;
            if constexpr (std::is_same_v<T, bool>)       xml.setAttribute (name, v);
            else if constexpr (std::is_same_v<T, int>)   xml.setAttribute (name, v);
            else                                         xml.setAttribute (name, (double) v);
        });
    }
};
