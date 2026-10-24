#pragma once
#include <juce_core/juce_core.h>
#include <unordered_map>
#include <array>
#include <string>
#include "AppSettings.h"

//==============================================================================
//  Idiomas: English (por defecto), Español, 中文
//  Todo el texto del plugin está escrito en inglés y se traduce con tr().
//==============================================================================
namespace Loc
{
    using Entry = std::array<const char*, 2>;      // { español, chino }

    inline const std::unordered_map<std::string, Entry>& table()
    {
        static const std::unordered_map<std::string, Entry> t = {
            // barra superior
            { "ANALYZER",        { "ANALIZADOR", "分析器" } },
            { "RESONANCES",      { "RESONANCIAS", "共振" } },
            { "INFO",            { "INFO", "说明" } },
            { "BYPASS",          { "BYPASS", "旁通" } },
            { "Ø POL",           { "Ø POL", "Ø 极性" } },
            { "PAUSE",           { "PAUSA", "暂停" } },
            { "PRESETS",         { "PRESETS", "预设" } },
            { "SAVE",            { "GUARDAR", "保存" } },
            { "CLEAR ALL",       { "BORRAR TODO", "全部清除" } },
            { "CENTER VIEW",     { "CENTRAR VISTA", "居中视图" } },
            { "Range",           { "Rango", "范围" } },
            { "IN",              { "IN", "输入" } },
            { "OUT",             { "OUT", "输出" } },
            { "PARAMETRIC EQUALIZER", { "ECUALIZADOR PARAMÉTRICO", "参数均衡器" } },

            // presets y diálogos
            { "PRESET: ",        { "PRESET: ", "预设: " } },
            { "My presets",      { "Mis presets", "我的预设" } },
            { "(empty)",         { "(vacío)", "(空)" } },
            { "Save current preset...",  { "Guardar preset actual...", "保存当前预设..." } },
            { "Load from file...",       { "Cargar desde archivo...", "从文件加载..." } },
            { "Open presets folder",     { "Abrir carpeta de presets", "打开预设文件夹" } },
            { "Choose a preset",         { "Elige un preset", "选择预设" } },
            { "Save preset",             { "Guardar preset", "保存预设" } },
            { "Enter a name for your preset:", { "Escribe un nombre para tu preset:", "请输入预设名称:" } },
            { "Save",            { "Guardar", "保存" } },
            { "Cancel",          { "Cancelar", "取消" } },
            { "My preset",       { "Mi preset", "我的预设" } },

            // pantalla central
            { "BAND",            { "BANDA", "频段" } },
            { "BANDS",           { "BANDAS", "频段" } },
            { "MAIN",            { "PRINCIPAL", "主频段" } },
            { "ANALYZER PAUSED", { "ANALIZADOR EN PAUSA", "分析器已暂停" } },
            { "Delete band",     { "Eliminar banda", "删除频段" } },
            { "Delete selected bands", { "Eliminar bandas seleccionadas", "删除所选频段" } },
            { "Disable",         { "Desactivar", "停用" } },
            { "Enable",          { "Activar", "启用" } },
            { "Enable dynamics", { "Activar dinámica", "启用动态" } },
            { "Remove dynamics", { "Quitar dinámica", "关闭动态" } },

            // panel de banda y filtros
            { "// CUT FILTERS",  { "// FILTROS DE CORTE", "// 截止滤波器" } },
            { "// BAND",         { "// BANDA", "// 频段" } },
            { "// NO BAND SELECTED", { "// SIN BANDA SELECCIONADA", "// 未选择频段" } },
            { "bands move together", { "bandas se mueven juntas", "个频段一起移动" } },
            { "Bell",            { "Campana", "钟形" } },
            { "Low shelf",       { "Shelf graves", "低架" } },
            { "High shelf",      { "Shelf agudos", "高架" } },
            { "Notch",           { "Notch", "陷波" } },
            { "ACTIVE",          { "ACTIVA", "启用" } },
            { "DYNAMIC",         { "DINÁMICA", "动态" } },
            { "+ BAND",          { "+ BANDA", "+ 频段" } },
            { "DELETE",          { "ELIMINAR", "删除" } },
            { "FREQUENCY",       { "FRECUENCIA", "频率" } },
            { "GAIN",            { "GANANCIA", "增益" } },
            { "RANGE",           { "RANGO", "范围" } },
            { "THRESHOLD",       { "UMBRAL", "阈值" } },
            { "ATTACK",          { "ATAQUE", "启动" } },
            { "RELEASE",         { "RELEASE", "释放" } },
            { "FILTER",          { "FILTRO", "滤波" } },
            { "DYNAMICS",        { "DINÁMICA", "动态" } },

            // ajustes
            { "SETTINGS",        { "AJUSTES", "设置" } },
            { "General",         { "General", "常规" } },
            { "Top bar",         { "Barra superior", "顶部栏" } },
            { "Sides",           { "Laterales", "两侧" } },
            { "Bottom",          { "Inferior", "底部" } },
            { "Center",          { "Centro", "中央" } },
            { "CLOSE",           { "CERRAR", "关闭" } },
            { "RESET",           { "RESTABLECER", "重置" } },
            { "Language",        { "Idioma", "语言" } },
            { "Animations",      { "Animaciones", "动画" } },
            { "Scanlines effect",{ "Efecto de líneas de escaneo", "扫描线效果" } },
            { "Accent colour",   { "Color de acento", "强调色" } },
            { "Cyan",            { "Cian", "青色" } },
            { "Magenta",         { "Magenta", "洋红" } },
            { "Green",           { "Verde", "绿色" } },
            { "Orange",          { "Naranja", "橙色" } },
            { "Violet",          { "Violeta", "紫色" } },
            { "Start collapsed", { "Empezar minimizado", "启动时收起" } },
            { "Show subtitle",   { "Mostrar subtítulo", "显示副标题" } },
            { "Meter range",     { "Rango del vúmetro", "电平表范围" } },
            { "Peak hold",       { "Retención de picos", "峰值保持" } },
            { "Meter fall speed",{ "Velocidad de caída", "下落速度" } },
            { "Slow",            { "Lenta", "慢" } },
            { "Normal",          { "Normal", "正常" } },
            { "Fast",            { "Rápida", "快" } },
            { "Panel opacity",   { "Opacidad de los paneles", "面板不透明度" } },
            { "Show captions",   { "Mostrar rótulos de grupo", "显示分组标题" } },
            { "Band size",       { "Tamaño de las bandas", "频段大小" } },
            { "Band opacity",    { "Opacidad de las bandas", "频段不透明度" } },
            { "Curve thickness", { "Grosor de la curva", "曲线粗细" } },
            { "Spectrum opacity",{ "Opacidad del espectro", "频谱不透明度" } },
            { "Spectrum smoothing", { "Suavizado del espectro", "频谱平滑" } },
            { "Smooth",          { "Suave", "平滑" } },
            { "Analyzer resolution", { "Resolución del analizador", "分析器分辨率" } },
            { "Low",             { "Baja", "低" } },
            { "Medium",          { "Media", "中" } },
            { "High",            { "Alta", "高" } },
            { "Grid opacity",    { "Opacidad de la rejilla", "网格不透明度" } },
            { "Show cursor readout", { "Mostrar lectura bajo el cursor", "显示光标读数" } },
            { "Show band numbers",   { "Mostrar números de las bandas", "显示频段编号" } },
            { "Fill selected band",  { "Rellenar la banda seleccionada", "填充所选频段" } },
            { "Invert swipe direction (Q)", { "Invertir sentido del deslizamiento (Q)", "反转滑动方向 (Q)" } },
            { "Size",            { "Tamaño", "大小" } },
            { "Custom colour",   { "Color personalizado", "自定义颜色" } },
            { "Settings opacity",{ "Opacidad de los ajustes", "设置面板不透明度" } },
            { "ON",              { "ON", "开" } },
            { "OFF",             { "OFF", "关" } },

            // categorías de presets
            { "Filters",         { "Filtros", "滤波器" } },
            { "Effects",         { "Efectos", "效果" } },
            { "Voice",           { "Voz", "人声" } },
            { "Drums",           { "Batería", "鼓" } },
            { "Strings & keys",  { "Cuerdas y teclas", "弦乐与键盘" } },
            { "Bass & synths",   { "Bajo y sintes", "贝斯与合成器" } },
            { "Wind",            { "Viento", "管乐" } },
            { "Music",           { "Música", "音乐" } },
            { "Correction",      { "Corrección", "修复" } },

            // presets
            { "HPF 80 Hz - clean low end",     { "HPF 80 Hz - limpiar graves", "高通 80 Hz - 清理低频" } },
            { "HPF 30 Hz - remove subsonics",  { "HPF 30 Hz - quitar subsónicos", "高通 30 Hz - 去除次声" } },
            { "LPF 16 kHz - soften highs",     { "LPF 16 kHz - suavizar agudos", "低通 16 kHz - 柔化高频" } },
            { "LPF 8 kHz - muffled sound",     { "LPF 8 kHz - sonido apagado", "低通 8 kHz - 闷音" } },
            { "Telephone call",    { "Llamada de teléfono", "电话通话" } },
            { "AM radio",          { "Radio AM", "AM 收音机" } },
            { "Megaphone",         { "Megáfono", "扩音喇叭" } },
            { "Lo-fi / vinyl",     { "Lo-fi / vinilo", "Lo-fi / 黑胶" } },
            { "Underwater",        { "Bajo el agua", "水下声" } },
            { "Clear voice",       { "Voz clara", "清晰人声" } },
            { "Male voice",        { "Voz masculina", "男声" } },
            { "Female voice",      { "Voz femenina", "女声" } },
            { "Podcast / voice-over", { "Podcast / locución", "播客 / 旁白" } },
            { "Vocal presence",    { "Presencia vocal", "人声临场感" } },
            { "De-esser (dynamic)",{ "De-esser (dinámico)", "去齿音 (动态)" } },
            { "Remove mud",        { "Quitar barro", "去除浑浊" } },
            { "Kick drum",         { "Bombo (kick)", "底鼓" } },
            { "Snare",             { "Caja (snare)", "军鼓" } },
            { "Hi-hat & cymbals",  { "Hi-hat y platillos", "踩镲与镲片" } },
            { "Drum overheads",    { "Batería (overheads)", "鼓组顶麦" } },
            { "Electric guitar",   { "Guitarra eléctrica", "电吉他" } },
            { "Acoustic guitar",   { "Guitarra acústica", "木吉他" } },
            { "Piano",             { "Piano", "钢琴" } },
            { "Strings (violin)",  { "Cuerdas (violín)", "弦乐 (小提琴)" } },
            { "Electric bass",     { "Bajo eléctrico", "电贝斯" } },
            { "808 bass",          { "Bajo 808", "808 贝斯" } },
            { "Synth / pad",       { "Sintetizador / pad", "合成器 / 铺底" } },
            { "Sax / flute",       { "Saxo / flauta", "萨克斯 / 长笛" } },
            { "Trumpet / brass",   { "Trompeta / metales", "小号 / 铜管" } },
            { "More bass",         { "Más graves", "增强低频" } },
            { "Brightness & air",  { "Brillo y aire", "明亮与空气感" } },
            { "Gentle mastering",  { "Mastering suave", "轻柔母带" } },
            { "Remove 50 Hz hum",  { "Quitar zumbido 50 Hz", "去除 50 Hz 嗡嗡声" } },
            { "Remove 60 Hz hum",  { "Quitar zumbido 60 Hz", "去除 60 Hz 嗡嗡声" } },
        };
        return t;
    }

    inline juce::String tr (const char* en)
    {
        const int l = AppSettings::get().language;
        if (l > 0)
        {
            const auto& t = table();
            const auto it = t.find (en);
            if (it != t.end())
                return juce::String::fromUTF8 (it->second[(size_t) (l - 1)]);
        }
        return juce::String::fromUTF8 (en);
    }

    //==========================================================================
    inline juce::String infoEnglish()
    {
        return juce::String::fromUTF8 (R"TXT(
IJEQ · QUICK GUIDE
══════════════════

THE INTERACTIVE SCREEN
• Double-click on an empty spot: adds a new band right there (up to 24). The plugin starts with none.
• Drag a point: left and right change the frequency; up and down change the gain.
• Hold ALT (Option on Mac) while dragging a point: 10× finer control.
• SELECTING SEVERAL BANDS: drag a rectangle over an empty area, or click points while holding SHIFT
  (or Cmd/Ctrl). When you move one of the selected bands, all of them move together.
• Click on an empty spot: clears the selection.
• Two-finger swipe UP / DOWN on the trackpad (or mouse wheel): raises / lowers the Q of the selected band
  (or of the band under the cursor). Q is how narrow or wide the band is.
• Double-click on a point: sets its gain to 0 dB.
• Right-click on a point: menu to delete, disable or enable dynamics.

ZOOM AND MOVING THE VIEW
• Pinch (open / close two fingers) on the trackpad: zoom in frequency.
• Cmd/Ctrl + wheel: zoom in frequency. ALT + wheel: vertical zoom (more or fewer dB).
• Two-finger swipe left / right: moves the view sideways.
• ALT + drag on an empty spot (or middle mouse button): moves the view horizontally and vertically.
• CENTER VIEW: back to the full view (20 Hz – 20 kHz).
• Move the cursor over the screen: a readout shows the exact frequency, the nearest musical note,
  the EQ gain and the spectrum level at that point.

KEYBOARD
• Arrows ◀ ▶: previous / next band (ordered from low to high frequency).
• Number keys 1–9 (or two digits quickly, e.g. 1 then 2 = band 12): jump to that band.
• Cmd + Z (Ctrl + Z on Windows): undo. Cmd + Shift + Z (Ctrl + Shift + Z or Ctrl + Y): redo.
• Delete / Backspace: removes the selected bands.
If your DAW keeps the keys, click once on the plugin screen first.

TOP BAR
• ANALYZER: shows or hides the spectrum. PAUSE: freezes the spectrum so you can study it calmly.
• RESONANCES: marks the "annoying frequencies" with triangles: peaks that stick out from the rest of the
  sound (hums, whistles, notes that sting). Click a triangle to create a narrow band that reduces it.
• RANGE: how many dB are shown on screen. INFO: this guide.
• IN / OUT: input volume (before the EQ) and output volume (after it), in dB.
• Ø POL: inverts the polarity. BYPASS: turns everything off so you can compare.

MINIMIZING AREAS
• The small arrows on the edges of the interactive screen hide or show the top bar, the bottom panel and
  each meter. You can keep only the interactive screen.

UNDO, PRESETS AND CLEAR ALL
• Arrows ◀ ▶ on the second row: undo and redo (up to 100 steps).
• CLEAR ALL: removes all bands and filters. If you regret it, undo.
• PRESETS: ready-made settings by category: filters, effects (telephone, radio, megaphone…), voice,
  drums, strings & keys, bass & synths, wind, music and hum removal.
• SAVE: saves your current setting with the name you like; it appears under "My presets".

METERS
• IN (left): input level, channels L and R. OUT (right): output level, channels L and R.
• The bar shows the average level and the white mark the peak. In magenta you are at 0 dB: beware of clipping.

BAND PANEL (bottom right)
• Type: BELL raises or lowers an area; LOW / HIGH SHELF raises or lowers everything below / above that
  frequency; NOTCH cuts one very specific frequency (it has no gain).
• ACTIVE: turns the band on or off without deleting it. + BAND / DELETE: add or remove bands.
• FREQUENCY, GAIN and Q: type exact values with a double-click, for example "2.5k".

DYNAMIC EQ
A dynamic band only acts when that frequency is loud. Turn on the DYNAMIC button.
• RANGE: how much the gain changes above the threshold (negative = reduces, like a gentle de-esser).
• THRESHOLD: level from which it acts. ATTACK / RELEASE: how fast it reacts and returns to normal.

CUT FILTERS
• HPF cuts the lows below the chosen frequency. LPF cuts the highs above it.
• The slope (12, 24, 36 or 48 dB/oct) is how steep the cut is.

WINDOW SIZE
• Drag the right edge, the bottom edge or the bottom-right corner of the plugin to make it smaller or larger.
  Everything scales with the window.
• In SETTINGS you can also change the size of the top bar, the side meters and the bottom panel.
  The small minimize arrows always stay in place.

SETTINGS (small gear, bottom left)
• Language (English, Español, 中文), accent colour, animations, and options for each area:
  top bar, sides, bottom panel and the central screen (band size and opacity, curve thickness,
  spectrum opacity, analyzer resolution, grid opacity…). Your choices are saved automatically.
)TXT");
    }

    inline juce::String infoSpanish()
    {
        return juce::String::fromUTF8 (R"TXT(
IJEQ · GUÍA RÁPIDA
══════════════════

LA PANTALLA INTERACTIVA
• Doble clic en un espacio vacío: añade una banda nueva justo ahí (hasta 24). El plugin empieza sin ninguna.
• Arrastra un punto: izquierda y derecha cambian la frecuencia; arriba y abajo, la ganancia.
• Mantén ALT (Option en Mac) mientras arrastras un punto: ajuste 10 veces más fino.
• SELECCIONAR VARIAS BANDAS: arrastra un rectángulo sobre un espacio vacío, o haz clic en los puntos
  manteniendo SHIFT (o Cmd/Ctrl). Al mover una de las seleccionadas, se mueven todas juntas.
• Clic en un espacio vacío: quita la selección.
• Deslizar dos dedos ARRIBA / ABAJO en el trackpad (o rueda del ratón): sube / baja la Q de la banda
  seleccionada (o de la que está bajo el cursor). La Q es lo estrecha o ancha que es la banda.
• Doble clic sobre un punto: pone su ganancia a 0 dB.
• Clic derecho sobre un punto: menú para eliminar, desactivar o activar la dinámica.

ZOOM Y DESPLAZAMIENTO
• Pellizco (abrir / cerrar dos dedos) en el trackpad: zoom en frecuencia.
• Cmd/Ctrl + rueda: zoom en frecuencia. ALT + rueda: zoom vertical (más o menos dB).
• Deslizar dos dedos a izquierda / derecha: mueve la vista hacia los lados.
• ALT + arrastrar en un espacio vacío (o botón central del ratón): mueve la vista en horizontal y vertical.
• CENTRAR VISTA: vuelve a la vista completa (20 Hz – 20 kHz).
• Al pasar el cursor por la pantalla aparece una lectura con la frecuencia exacta, la nota musical más
  cercana, la ganancia del EQ y el nivel del espectro en ese punto.

TECLADO
• Flechas ◀ ▶: banda anterior / siguiente (ordenadas de graves a agudos).
• Teclas numéricas 1–9 (o dos dígitos seguidos, por ejemplo 1 y 2 = banda 12): saltan a esa banda.
• Cmd + Z (Ctrl + Z en Windows): deshacer. Cmd + Shift + Z (Ctrl + Shift + Z o Ctrl + Y): rehacer.
• Suprimir / Retroceso: elimina las bandas seleccionadas.
Si tu programa de música se queda con las teclas, haz clic antes sobre la pantalla del plugin.

BARRA SUPERIOR
• ANALIZADOR: muestra u oculta el espectro. PAUSA: congela el espectro para estudiarlo con calma.
• RESONANCIAS: marca con triángulos las «frecuencias molestas»: picos que sobresalen del resto del sonido
  (zumbidos, silbidos, notas que «pinchan»). Haz clic en un triángulo y se crea una banda estrecha que la reduce.
• RANGO: cuántos dB se ven en pantalla. INFO: esta guía.
• IN / OUT: volumen de entrada (antes del ecualizador) y de salida (después), en dB.
• Ø POL: invierte la polaridad. BYPASS: lo desactiva todo para comparar.

MINIMIZAR ZONAS
• Las pequeñas flechas en los bordes de la pantalla interactiva ocultan o muestran la barra superior,
  el panel inferior y cada vúmetro. Puedes dejar solo la pantalla interactiva.

DESHACER, PRESETS Y BORRAR TODO
• Flechas ◀ ▶ de la segunda fila: deshacer y rehacer (hasta 100 pasos).
• BORRAR TODO: quita todas las bandas y filtros. Si te arrepientes, deshaz.
• PRESETS: ajustes listos por categorías: filtros, efectos (teléfono, radio, megáfono…), voz, batería,
  cuerdas y teclas, bajo y sintetizadores, viento, música y corrección de zumbidos.
• GUARDAR: guarda tu ajuste con el nombre que quieras; aparece en «Mis presets».

VÚMETROS
• IN (izquierda): nivel de entrada, canales L y R. OUT (derecha): nivel de salida, canales L y R.
• La barra muestra el nivel medio y la marca blanca, el pico. En magenta estás en 0 dB: cuidado con saturar.

PANEL DE BANDA (abajo a la derecha)
• Tipo: CAMPANA sube o baja una zona; SHELF GRAVES / AGUDOS sube o baja todo lo que queda por debajo / por
  encima de esa frecuencia; NOTCH corta una frecuencia muy concreta (no tiene ganancia).
• ACTIVA: enciende o apaga la banda sin borrarla. + BANDA / ELIMINAR: añade o borra bandas.
• FRECUENCIA, GANANCIA y Q: puedes escribir valores exactos con doble clic, por ejemplo «2.5k».

ECUALIZACIÓN DINÁMICA
Una banda dinámica solo actúa cuando esa frecuencia suena fuerte. Activa el botón DINÁMICA.
• RANGO: cuánto cambia la ganancia al superar el umbral (negativo = reduce, como un de-esser suave).
• UMBRAL: nivel a partir del cual actúa. ATAQUE y RELEASE: rapidez al actuar y al volver a la normalidad.

FILTROS DE CORTE
• HPF corta los graves por debajo de la frecuencia elegida. LPF corta los agudos por encima.
• La pendiente (12, 24, 36 o 48 dB/oct) indica lo brusco que es el corte.

TAMAÑO DE LA VENTANA
• Arrastra el borde derecho, el borde inferior o la esquina inferior derecha del plugin para hacerlo más pequeño
  o más grande. Todo se escala con la ventana.
• En AJUSTES también puedes cambiar el tamaño de la barra superior, de los vúmetros laterales y del panel inferior.
  Las pequeñas flechas de minimizar siempre se quedan en su sitio.

AJUSTES (pequeño engranaje, abajo a la izquierda)
• Idioma (English, Español, 中文), color de acento, animaciones y opciones de cada zona: barra superior,
  laterales, panel inferior y pantalla central (tamaño y opacidad de las bandas, grosor de la curva,
  opacidad del espectro, resolución del analizador, opacidad de la rejilla…). Se guardan solos.
)TXT");
    }

    inline juce::String infoChinese()
    {
        return juce::String::fromUTF8 (R"TXT(
IJEQ · 快速指南
════════════════

互动屏幕
• 在空白处双击：在该位置添加新频段（最多 24 个）。插件启动时没有任何频段。
• 拖动圆点：左右改变频率，上下改变增益。
• 拖动时按住 ALT（Mac 上为 Option）：精细调节，速度降为十分之一。
• 选择多个频段：在空白处拖出矩形，或按住 SHIFT（或 Cmd/Ctrl）逐个点击圆点。
  移动其中一个已选频段时，所有已选频段会一起移动。
• 在空白处单击：取消选择。
• 在触控板上用两指向上 / 向下滑动（或使用鼠标滚轮）：提高 / 降低所选频段（或光标下频段）的 Q 值。
  Q 值决定频段的宽窄。
• 双击圆点：将其增益设为 0 dB。
• 右键单击圆点：弹出菜单，可删除、停用频段或开启动态。

缩放与移动视图
• 在触控板上双指捏合 / 张开：在频率方向缩放。
• Cmd/Ctrl + 滚轮：频率缩放。ALT + 滚轮：纵向缩放（显示更多或更少 dB）。
• 两指左右滑动：水平移动视图。
• 在空白处按住 ALT 拖动（或按住鼠标中键）：水平和垂直移动视图。
• 居中视图：回到完整视图（20 Hz – 20 kHz）。
• 光标移到屏幕上时，会显示该点的精确频率、最接近的音符、均衡增益和频谱电平。

键盘
• 方向键 ◀ ▶：切换到上一个 / 下一个频段（按频率从低到高排序）。
• 数字键 1–9（或快速输入两位数，例如先按 1 再按 2 = 第 12 个频段）：直接跳到该频段。
• Cmd + Z（Windows 为 Ctrl + Z）：撤销。Cmd + Shift + Z（Ctrl + Shift + Z 或 Ctrl + Y）：重做。
• Delete / 退格键：删除所选频段。
如果宿主软件占用了按键，请先在插件界面上单击一次。

顶部栏
• 分析器：显示或隐藏频谱。暂停：冻结频谱，方便仔细观察。
• 共振：用三角形标出“刺耳频率”，即突出于其余声音的峰值（嗡嗡声、啸叫、刺耳的音符）。
  单击三角形会自动创建一个窄频段来削弱它。
• 范围：屏幕上显示的 dB 范围。说明：打开本指南。
• 输入 / 输出：均衡器之前 / 之后的音量（dB）。
• Ø 极性：反转极性。旁通：关闭全部效果以便对比。

收起区域
• 互动屏幕边缘的小箭头可以隐藏或显示顶部栏、底部面板和每个电平表，可以只保留互动屏幕。

撤销、预设与全部清除
• 第二行的箭头 ◀ ▶：撤销和重做（最多 100 步）。
• 全部清除：删除所有频段和滤波器。后悔了可以撤销。
• 预设：按类别提供的现成设置：滤波器、效果（电话、收音机、扩音喇叭…）、人声、鼓、
  弦乐与键盘、贝斯与合成器、管乐、音乐，以及去除嗡嗡声。
• 保存：用你喜欢的名字保存当前设置，之后会出现在“我的预设”中。

电平表
• 输入（左侧）：输入电平，左右声道。输出（右侧）：输出电平，左右声道。
• 条形显示平均电平，白色标记显示峰值。变为洋红色表示已到 0 dB，小心削波。

频段面板（右下）
• 类型：钟形提升或衰减一段频率；低架 / 高架提升或衰减该频率以下 / 以上的全部内容；
  陷波切除一个非常具体的频率（没有增益）。
• 启用：开关频段而不删除它。+ 频段 / 删除：添加或删除频段。
• 频率、增益和 Q 值：双击数值可输入精确数值，例如“2.5k”。

动态均衡
动态频段只有在该频率声音较大时才会起作用。请打开“动态”按钮。
• 范围：超过阈值后增益的变化量（负值 = 衰减，类似轻柔的去齿音）。
• 阈值：开始起作用的电平。启动 / 释放：反应和恢复的速度。

截止滤波器
• 高通（HPF）切除所选频率以下的低频。低通（LPF）切除所选频率以上的高频。
• 斜率（12、24、36 或 48 dB/oct）表示切除的陡峭程度。

窗口大小
• 拖动插件的右边缘、下边缘或右下角，即可缩小或放大。所有内容会随窗口一起缩放。
• 在“设置”中还可以分别调整顶部栏、两侧电平表和底部面板的大小。收起用的小箭头始终保留。

设置（左下角的小齿轮）
• 语言（English、Español、中文）、强调色、动画，以及各区域的选项：顶部栏、两侧、底部面板和
  中央屏幕（频段大小与不透明度、曲线粗细、频谱不透明度、分析器分辨率、网格不透明度……）。
  所有选择都会自动保存。
)TXT");
    }

    inline juce::String infoText()
    {
        switch (AppSettings::get().language)
        {
            case 1:  return infoSpanish();
            case 2:  return infoChinese();
            default: return infoEnglish();
        }
    }
}
