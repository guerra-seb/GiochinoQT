#pragma once
#include <QWidget>

class OptionsPage : public QWidget {
    Q_OBJECT
public:
    explicit OptionsPage(QWidget *parent = nullptr);

signals:
    void homeRequested();
};
