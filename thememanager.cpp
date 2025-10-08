#include "thememanager.h"
#include <QApplication>
#include <QSettings>
#include <QPalette>
#include <QString>

ThemeManager& ThemeManager::instance() {
    static ThemeManager inst;
    return inst;
}

ThemeManager::ThemeManager(QObject *p) : QObject(p) {}

void ThemeManager::applySavedTheme() {
    QSettings s;
    int val = s.value("ui/theme", static_cast<int>(Theme::Light)).toInt();
    m_accent = QColor(s.value("ui/accent", m_accent.name()).toString());
    m_radius = s.value("ui/cornerRadius", m_radius).toInt();
    setTheme(static_cast<Theme>(val));   // rigenera palette + stylesheet coi valori caricati
}

void ThemeManager::setTheme(Theme t) {
    // aggiorna palette e setting solo quando cambia tema,
    // ma rigenera SEMPRE lo stylesheet (accent/radius potrebbero essere cambiati)
    if (m_theme != t) {
        m_theme = t;
        applyPaletteFor(t);
        QSettings().setValue("ui/theme", static_cast<int>(t));
        emit themeChanged(t);
    }
    qApp->setStyleSheet(styleFor(m_theme));
    emit styleChanged();
}

void ThemeManager::setAccent(const QColor &c){
    if (!c.isValid()) return;
    m_accent = c;
    qApp->setStyleSheet(styleFor(m_theme));
    QSettings().setValue("ui/accent", m_accent.name());
    emit styleChanged();
}

void ThemeManager::setCornerRadius(int r){
    if (r < 0) r = 0; if (r > 20) r = 20;
    m_radius = r;
    qApp->setStyleSheet(styleFor(m_theme));
    QSettings().setValue("ui/cornerRadius", m_radius);
    emit styleChanged();
}

void ThemeManager::applyPaletteFor(Theme t) {
    QPalette p = qApp->palette();
    if (t == Theme::Dark) {
        p.setColor(QPalette::Window,            QColor("#121419"));
        p.setColor(QPalette::WindowText,        QColor("#e9edf3"));
        p.setColor(QPalette::Base,              QColor("#0f1318"));
        p.setColor(QPalette::AlternateBase,     QColor("#151a22"));
        p.setColor(QPalette::Text,              QColor("#e9edf3"));
        p.setColor(QPalette::Button,            QColor("#1e2530"));
        p.setColor(QPalette::ButtonText,        QColor("#e9edf3"));
        p.setColor(QPalette::Highlight,         QColor("#35507a"));
        p.setColor(QPalette::HighlightedText,   QColor("#ffffff"));
    } else if (t == Theme::HighContrast) {
        p = QPalette();
        p.setColor(QPalette::Window,            Qt::black);
        p.setColor(QPalette::WindowText,        Qt::white);
        p.setColor(QPalette::Base,              Qt::black);
        p.setColor(QPalette::Text,              Qt::white);
        p.setColor(QPalette::Button,            Qt::black);
        p.setColor(QPalette::ButtonText,        Qt::yellow);
        p.setColor(QPalette::Highlight,         QColor("#ffff00"));
        p.setColor(QPalette::HighlightedText,   Qt::black);
    } else {
        p = QPalette(); // reset “light” di default
    }
    qApp->setPalette(p);
}

QString ThemeManager::styleFor(Theme t) const {
    const QString acc = m_accent.name();
    const QString br  = QString::number(m_radius);

    if (t == Theme::Dark) {
        return QString(R"(
* { font-family:'Segoe UI','Noto Sans','Arial'; font-size:14px; }
QWidget { background:#121419; color:#e9edf3; }
QPushButton { background:#1e2530; border:1px solid %1; border-radius:%2px; padding:6px 12px; }
QPushButton:hover { background:%1; }
QLineEdit { background:#0f1318; color:#e9edf3; border:1px solid #2d3847; border-radius:8px; padding:4px 6px; }
QGroupBox{ border:1px solid #2d3847; border-radius:12px; margin-top:12px; }
QToolButton { border-radius:%2px; }
)").arg(acc, br);
    }

    if (t == Theme::HighContrast) {
        return QString(R"(
* { font-family:'Segoe UI','Noto Sans','Arial'; font-size:16px; }
QWidget { background:black; color:white; }
QPushButton { background:black; color:yellow; border:2px solid %1; border-radius:%2px; padding:8px 12px; }
QLineEdit { background:black; color:white; border:2px solid white; }
QToolButton { border-radius:%2px; }
)").arg(acc, br);
    }

    // Light
    return QString(R"(
* { font-family:'Segoe UI','Noto Sans','Arial'; font-size:14px; }
QWidget { background:#ffffff; color:#111; }
QPushButton { background:#e9eef5; border:1px solid %1; border-radius:%2px; padding:6px 12px; }
QPushButton:hover { background:%1; color:white; }
QLineEdit { background:#fff; color:#111; border:1px solid #c7d1e0; border-radius:8px; padding:4px 6px; }
QGroupBox{ border:1px solid #c7d1e0; border-radius:12px; margin-top:12px; }
QToolButton { border-radius:%2px; }
)").arg(acc, br);
}
