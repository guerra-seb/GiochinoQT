#include "mainwindow.h"
#include <QStackedWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QAction>
#include <QKeySequence>

#include "homepage.h"
#include "secondpage.h"       // Becca il Codice
#include "optionspage.h"
#include "hangmanpage.h"      // Impiccato
#include "anagrampage.h"      // Anagrammi
#include "memoryconfigpage.h" // Memory – scelta dimensione
#include "memorypage.h"       // Memory – gioco
#include "sudokupage.h"       // Sudoku
#include "game2048page.h"     // 2048

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Benvenuto in sto' giochino");

    m_stack = new QStackedWidget(this);
    setCentralWidget(m_stack);

    // Pagine
    auto *home     = new HomePage;
    auto *code     = new SecondPage;     // Becca il Codice
    auto *opts     = new OptionsPage;
    auto *hangman  = new HangmanPage;    // Impiccato
    auto *anagram  = new AnagramPage;    // Anagrammi
    auto *memcfg   = new MemoryConfigPage; // Memory config
    auto *memory   = new MemoryPage;       // Memory game
    auto *sudoku   = new SudokuPage;       // Sudoku
    auto *page2048 = new Game2048Page;     // 2048

    // Aggiunta allo stack
    m_idxHome    = m_stack->addWidget(home);
    m_idxSecond  = m_stack->addWidget(code);
    m_idxOpts    = m_stack->addWidget(opts);
    m_idxHangman = m_stack->addWidget(hangman);
    m_idxAnagram = m_stack->addWidget(anagram);
    m_idxMemCfg  = m_stack->addWidget(memcfg);
    m_idxMemory  = m_stack->addWidget(memory);
    m_idxSudoku  = m_stack->addWidget(sudoku);
    m_idx2048    = m_stack->addWidget(page2048);

    // Navigazione pilotata dalla Home
    connect(home, &HomePage::startCodeClicked,    [this]{ m_stack->setCurrentIndex(m_idxSecond);  });
    connect(home, &HomePage::optionsRequested,    [this]{ m_stack->setCurrentIndex(m_idxOpts);   });
    connect(home, &HomePage::startHangmanClicked, [this]{ m_stack->setCurrentIndex(m_idxHangman); });
    connect(home, &HomePage::startAnagramClicked, [this]{ m_stack->setCurrentIndex(m_idxAnagram); });
    connect(home, &HomePage::startMemoryClicked,  [this]{ m_stack->setCurrentIndex(m_idxMemCfg);  });
    connect(home, &HomePage::startSudokuClicked,  [this, sudoku]{
        m_stack->setCurrentIndex(m_idxSudoku);
        sudoku->setFocus();
    });
    connect(home, &HomePage::start2048Clicked,    [this, page2048]{
        m_stack->setCurrentIndex(m_idx2048);
        page2048->setFocus(); // subito pronte le frecce
    });

    // Back/forward delle altre pagine
    connect(code,    &SecondPage::backRequested,     [this]{ m_stack->setCurrentIndex(m_idxHome);   });
    // connect(code,    &SecondPage::optionsRequested,  [this]{ m_stack->setCurrentIndex(m_idxOpts);   });

    connect(hangman, &HangmanPage::backRequested,    [this]{ m_stack->setCurrentIndex(m_idxHome);   });
    connect(anagram, &AnagramPage::backRequested,    [this]{ m_stack->setCurrentIndex(m_idxHome);   });
    connect(opts,    &OptionsPage::homeRequested,    [this]{ m_stack->setCurrentIndex(m_idxHome);   });

    connect(memcfg,  &MemoryConfigPage::backRequested, [this]{ m_stack->setCurrentIndex(m_idxHome);   });
    connect(memcfg,  &MemoryConfigPage::startRequested, [this, memory](int r, int c){
        memory->startNew(r, c);
        m_stack->setCurrentIndex(m_idxMemory);
    });
    connect(memory,  &MemoryPage::backRequested,        [this]{ m_stack->setCurrentIndex(m_idxHome);   });
    connect(memory,  &MemoryPage::changeSizeRequested,  [this]{ m_stack->setCurrentIndex(m_idxMemCfg); });
    connect(sudoku,  &SudokuPage::backRequested,        [this]{ m_stack->setCurrentIndex(m_idxHome); });

    // Back dalla pagina 2048 (bottone o tasto Esc)
    connect(page2048, &Game2048Page::backRequested, [this]{ m_stack->setCurrentIndex(m_idxHome); });

    // Menu + status bar
    setupMenus();
    statusBar()->showMessage("Pronto");
}

void MainWindow::setupMenus() {
    auto *mPagina  = menuBar()->addMenu("Pagina");
    auto *aHome    = mPagina->addAction("Home");
    auto *aCode    = mPagina->addAction("Becca il Codice");
    auto *aHangman = mPagina->addAction("Impiccato");
    auto *aAnagram = mPagina->addAction("Anagrammi");
    auto *aOpts    = mPagina->addAction("Opzioni");
    auto *aMemory  = mPagina->addAction("Memory (Coppie)");
    auto *aSudoku  = mPagina->addAction("Sudoku");
    auto *a2048    = mPagina->addAction("2048");

    aHome->setShortcut      (QKeySequence("Ctrl+1"));
    aCode->setShortcut      (QKeySequence("Ctrl+2"));
    aHangman->setShortcut   (QKeySequence("Ctrl+3"));
    aAnagram->setShortcut   (QKeySequence("Ctrl+4"));
    aOpts->setShortcut      (QKeySequence("Ctrl+5"));
    aMemory->setShortcut    (QKeySequence("Ctrl+6"));
    aSudoku->setShortcut    (QKeySequence("Ctrl+7"));
    a2048->setShortcut      (QKeySequence("Ctrl+8"));

    connect(aHome,    &QAction::triggered, [this]{ m_stack->setCurrentIndex(m_idxHome);    });
    connect(aCode,    &QAction::triggered, [this]{ m_stack->setCurrentIndex(m_idxSecond);  });
    connect(aHangman, &QAction::triggered, [this]{ m_stack->setCurrentIndex(m_idxHangman); });
    connect(aAnagram, &QAction::triggered, [this]{ m_stack->setCurrentIndex(m_idxAnagram); });
    connect(aOpts,    &QAction::triggered, [this]{ m_stack->setCurrentIndex(m_idxOpts);    });
    connect(aMemory,  &QAction::triggered, [this]{ m_stack->setCurrentIndex(m_idxMemCfg);  });
    connect(aSudoku,  &QAction::triggered, [this]{ m_stack->setCurrentIndex(m_idxSudoku);  });
    connect(a2048,    &QAction::triggered, [this]{
        m_stack->setCurrentIndex(m_idx2048);
        if (auto *w = qobject_cast<Game2048Page*>(m_stack->widget(m_idx2048)))
            w->setFocus(); // frecce subito attive anche da menu
    });
}
