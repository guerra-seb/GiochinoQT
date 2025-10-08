#include "optionspage.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "thememanager.h"
#include <QComboBox>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QSettings>
#include <QColorDialog>
#include <QSlider>

OptionsPage::OptionsPage(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(16,16,16,16);
    layout->setSpacing(10);

    auto *title = new QLabel("Opzioni ⚙️");
    QFont tf = title->font(); tf.setPointSize(16); tf.setBold(true);
    title->setFont(tf);
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    // ---- Tema ----
    auto *rowTema = new QHBoxLayout;
    rowTema->addWidget(new QLabel("Tema:"));
    m_themeCombo = new QComboBox(this);
    m_themeCombo->addItems({"Chiaro", "Scuro", "Alto contrasto"});
    m_themeCombo->setCurrentIndex(static_cast<int>(ThemeManager::instance().theme()));
    rowTema->addWidget(m_themeCombo, 1);
    layout->addLayout(rowTema);

    // ---- Colore accento ----
    auto *rowAcc = new QHBoxLayout;
    rowAcc->addWidget(new QLabel("Colore accento:"));
    m_accentBtn = new QPushButton("Scegli…", this);
    auto refreshAccentBtn = [this](){
        QColor c = ThemeManager::instance().accent();
        const QString fg = (c.lightness() > 128) ? "#000" : "#fff";
        m_accentBtn->setStyleSheet(QString("QPushButton{background:%1; color:%2; border:1px solid #888; padding:6px 12px;}")
                                   .arg(c.name(), fg));
    };
    refreshAccentBtn();
    connect(m_accentBtn, &QPushButton::clicked, this, [this, refreshAccentBtn]{
        QColor c = QColorDialog::getColor(ThemeManager::instance().accent(), this, "Scegli colore accento");
        if (c.isValid()) { ThemeManager::instance().setAccent(c); refreshAccentBtn(); }
    });
    rowAcc->addWidget(m_accentBtn, 1);
    layout->addLayout(rowAcc);

    // ---- Raggio angoli ----
    auto *rowRad = new QHBoxLayout;
    rowRad->addWidget(new QLabel("Angoli arrotondati:"));
    m_radiusSlider = new QSlider(Qt::Horizontal, this);
    m_radiusSlider->setRange(0, 20);
    m_radiusSlider->setValue(ThemeManager::instance().cornerRadius());
    connect(m_radiusSlider, &QSlider::valueChanged, this, [](int v){
        ThemeManager::instance().setCornerRadius(v);
    });
    rowRad->addWidget(m_radiusSlider, 1);
    layout->addLayout(rowRad);

    layout->addStretch();
    auto *btnHome = new QPushButton("Torna alla Home");
    layout->addWidget(btnHome, 0, Qt::AlignHCenter);
    connect(btnHome, &QPushButton::clicked, this, &OptionsPage::homeRequested);

    // Cambia tema al volo
    connect(m_themeCombo,
            static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, [](int idx){ ThemeManager::instance().setTheme(static_cast<Theme>(idx)); });
}
