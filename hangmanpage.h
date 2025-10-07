#pragma once
#include <QWidget>
#include <QString>
#include <QVector>
#include <QPushButton>

class QLabel;
class QGridLayout;

class HangmanPage : public QWidget {
    Q_OBJECT
public:
    explicit HangmanPage(QWidget *parent = nullptr);

signals:
    void backRequested();

private:
    // UI
    QLabel *m_titleLabel   = nullptr;
    QLabel *m_wordLabel    = nullptr; // _ _ _ _
    QLabel *m_infoLabel    = nullptr; // tentativi rimasti
    QLabel *m_resultLabel  = nullptr; // BRAVO / SEI SCARSO
    QGridLayout *m_keys    = nullptr; // tastiera A-Z
    QVector<QPushButton*> m_letterBtns;

    // Gioco
    QString m_secret;
    QString m_progress; // lettere rivelate, '_' per nascoste
    int m_wrong = 0;
    int m_maxWrong = 6;

    // Helpers
    void newWord();
    void setSecret(const QString &w);
    void onLetter(QChar ch);
    void refreshWordLabel();
    void setLettersEnabled(bool en);
};
