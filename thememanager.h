#pragma once
#include <QObject>
#include <QColor>

enum class Theme : int { Light=0, Dark=1, HighContrast=2 };

class ThemeManager : public QObject {
    Q_OBJECT
public:
    static ThemeManager& instance();

    Theme theme() const { return m_theme; }
    void applySavedTheme();           // legge da QSettings e applica
    void setTheme(Theme t);           // applica e salva

    QColor accent() const { return m_accent; }
    void setAccent(const QColor& c);
    int cornerRadius() const { return m_radius; }
    void setCornerRadius(int r);

signals:
    void themeChanged(Theme);
    void styleChanged();

private:
    explicit ThemeManager(QObject *parent=nullptr);
    void applyPaletteFor(Theme t);
    QString styleFor(Theme t) const;

    Theme m_theme = Theme::Light;
    QColor m_accent = QColor("#3b82f6"); // blu di default
    int m_radius = 10;                   // raggio arrotondamenti
};
