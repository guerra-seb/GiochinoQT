#pragma once
#include <QWidget>
#include <QString>
#include <QVector>
#include <QPushButton>

class QLabel;
class QGridLayout;

class AnagramPage : public QWidget {
    Q_OBJECT
public:
    explicit AnagramPage(QWidget *parent = nullptr);

signals:
    void backRequested();

private:
    // UI
    QLabel *m_titleLabel   = nullptr;
    QLabel *m_wordLabel    = nullptr; // mostra la parola proposta (trattini e/o testo)
    QLabel *m_guessLabel   = nullptr; // costruzione corrente dell'utente
    QLabel *m_infoLabel    = nullptr; // messaggi/indizi
    QLabel *m_resultLabel  = nullptr; // BRAVO / SEI SCARSO
    QGridLayout *m_keys    = nullptr; // tastiera con lettere mescolate
    QVector<QPushButton*> m_letterBtns;
    QVector<QPushButton*> m_usedStack; // per riabilitare l'ultima lettera con Backspace

    // Gioco
    QString m_secret;      // parola da indovinare (MAIUSCOLA)
    QString m_guess;       // costruzione dell'utente

    // Helpers
    void newWord();
    void setSecret(const QString &w);
    void buildKeyboard(const QString &letters);
    void setLettersEnabled(bool en);
    void appendLetterFromButton(QPushButton *b);
    void onBackspace();
    void onSubmit();
    void onHint();
};
