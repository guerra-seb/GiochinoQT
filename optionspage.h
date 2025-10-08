#pragma once
#include <QWidget>

class OptionsPage : public QWidget {
    Q_OBJECT
public:
    explicit OptionsPage(QWidget *parent = nullptr);

private:
    class QComboBox* m_themeCombo = nullptr;
    class QPushButton* m_accentBtn = nullptr;
    class QSlider*     m_radiusSlider = nullptr;

signals:
    void homeRequested();
};
