#pragma once
#include <QWidget>
#include <QString>
#include <QPair>

class QLineEdit;
class QLabel;
class QListWidget;

// Seconda pagina con tastiera numerica e gioco "indovina il codice"
class SecondPage : public QWidget {
    Q_OBJECT
public:
    explicit SecondPage(QWidget *parent = nullptr);

signals:
    void backRequested();
    void optionsRequested();

private:
    // UI
    QLineEdit   *m_display     = nullptr;   // mostra il codice digitato
    QLabel      *m_hintLabel   = nullptr;   // indizi (bulls/cows)
    QLabel      *m_resultLabel = nullptr;   // BRAVO! / SEI SCARSO!
    QListWidget *m_history     = nullptr;   // cronologia tentativi

    // Gioco
    QString m_secret;        // codice segreto
    int     m_codeLen = 4;   // lunghezza codice

    // Helpers
    void newSecret();
    void onDigitPressed(int d);
    void onBackspace();
    void onClear();
    void onSubmit();
    QPair<int,int> computeBullsCows(const QString &guess) const; // (bulls, cows)
};
