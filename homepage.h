#pragma once
#include <QWidget>

class HomePage : public QWidget {
    Q_OBJECT
public:
    explicit HomePage(QWidget *parent = nullptr);

signals:
    void startCodeClicked();    // Becca il Codice
    void startHangmanClicked(); // Impiccato
    void startAnagramClicked(); // Anagrammi
    void startMemoryClicked();  // Memory (Coppie)
    void startSudokuClicked();  // Sudoku
    void start2048Clicked();    // 2048
    void open_beccailcodice();
    void open_hangman();
    void open_anagram();
    void open_memory();
    void open_sudoku();
    void open_2048();

};
