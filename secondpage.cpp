#include "sudokupage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QIntValidator>
#include <QFont>
#include <QtGlobal>
#include <algorithm>
#include <QKeyEvent>

#if QT_VERSION >= QT_VERSION_CHECK(5,10,0)
  #include <QRandomGenerator>
  static int rndInt(int a, int bInclusive){ return (int)QRandomGenerator::global()->bounded(bInclusive - a + 1) + a; }
  template<typename It> static void shuffleRange(It f, It l){ std::shuffle(f, l, *QRandomGenerator::global()); }
#else
  #include <random>
  static std::mt19937& rng(){ static std::mt19937 r{ std::random_device{}() }; return r; }
  static int rndInt(int a, int bInclusive){ std::uniform_int_distribution<int> d(a,bInclusive); return d(rng()); }
  template<typename It> static void shuffleRange(It f, It l){ std::shuffle(f, l, rng()); }
#endif

SudokuPage::SudokuPage(QWidget *parent) : QWidget(parent) {
    setFocusPolicy(Qt::StrongFocus);
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16,16,16,16);
    root->setSpacing(10);

    m_title = new QLabel("Sudoku — genera una nuova partita");
    m_title->setAlignment(Qt::AlignCenter);
    QFont tf = m_title->font(); tf.setPointSize(16); tf.setBold(true);
    m_title->setFont(tf);
    root->addWidget(m_title);

    // Barra superiore: difficoltà + pulsanti
    auto *top = new QHBoxLayout;
    auto *lbl = new QLabel("Difficoltà:");
    m_diff = new QComboBox;
    m_diff->addItems({"Facile","Medio","Difficile"});
    auto *btnNew = new QPushButton("Nuova");
    auto *btnCheck = new QPushButton("Controlla");
    auto *btnSolve = new QPushButton("Rivela soluzione");
    auto *btnBack  = new QPushButton("Torna indietro");
    top->addWidget(lbl);
    top->addWidget(m_diff);
    top->addSpacing(12);
    top->addWidget(btnNew);
    top->addWidget(btnCheck);
    top->addWidget(btnSolve);
    top->addStretch();
    top->addWidget(btnBack);
    root->addLayout(top);

    m_info = new QLabel("Compila con numeri 1–9. Le celle grigie sono fisse.");
    m_info->setAlignment(Qt::AlignCenter);
    root->addWidget(m_info);

    m_grid = new QGridLayout;
    m_grid->setHorizontalSpacing(0);
    m_grid->setVerticalSpacing(0);
    m_grid->setContentsMargins(0,0,0,0);
    root->addLayout(m_grid);
    buildGrid();

    m_result = new QLabel;
    m_result->setAlignment(Qt::AlignCenter);
    root->addWidget(m_result);

    connect(btnBack,  &QPushButton::clicked, this, &SudokuPage::backRequested);
    connect(btnNew,   &QPushButton::clicked, this, &SudokuPage::newGame);
    connect(btnCheck, &QPushButton::clicked, this, &SudokuPage::checkBoard);
    connect(btnSolve, &QPushButton::clicked, this, &SudokuPage::revealSolution);

    // prima partita
    newGame();
}

void SudokuPage::buildGrid(){
    // 9x9 QLineEdit con validatore [1..9]; bordi 3x3 disegnati lato per lato
    QFont f; f.setPointSize(18);
    auto *validator = new QIntValidator(1,9,this);
    for (int r=0;r<9;r++){
        for (int c=0;c<9;c++){
            auto *e = new QLineEdit;
            e->setAlignment(Qt::AlignCenter);
            e->setMaxLength(1);
            e->setValidator(validator);
            e->setFont(f);
            e->setFrame(false);         // evita il bordo di default del QLineEdit
            e->setFixedSize(40, 40);
            applyCellBorder(e, r, c);   // imposta spessori top/right/bottom/left
            applyBaseColors(e, /*given=*/false); // testo nero su bianco
            m_cell[r][c] = e;
            m_grid->addWidget(e, r, c);
        }
    }
}

void SudokuPage::applyBaseColors(QLineEdit *e, bool given){
    QPalette p = e->palette();
    p.setColor(QPalette::Base, given ? QColor("#f0f3f7") : QColor("#ffffff"));
    p.setColor(QPalette::Text, QColor("#111111"));
    p.setColor(QPalette::Highlight, QColor("#cde1ff"));
    p.setColor(QPalette::HighlightedText, QColor("#000000"));
    e->setPalette(p);

    QFont f = e->font();
    f.setBold(given);
    e->setFont(f);

    e->setReadOnly(given);
}

void SudokuPage::styleGiven(QLineEdit *e, bool given){
    applyBaseColors(e, given);
}

void SudokuPage::setAllEditable(bool en){
    for (int r=0;r<9;r++)
        for (int c=0;c<9;c++)
            m_cell[r][c]->setReadOnly(!en && m_cell[r][c]->text().isEmpty()==false);
}

void SudokuPage::clearBoard(int b[9][9]){
    for (int r=0;r<9;r++) for (int c=0;c<9;c++) b[r][c]=0;
}
void SudokuPage::copyBoard(const int src[9][9], int dst[9][9]){
    for (int r=0;r<9;r++) for (int c=0;c<9;c++) dst[r][c]=src[r][c];
}

bool SudokuPage::findEmpty(const int b[9][9], int &r, int &c) const {
    for (r=0;r<9;r++) for (c=0;c<9;c++) if (b[r][c]==0) return true;
    return false;
}
bool SudokuPage::isSafe(const int b[9][9], int r, int c, int num) const {
    for (int x=0;x<9;x++) if (b[r][x]==num || b[x][c]==num) return false;
    int rs = boxStart(r), cs = boxStart(c);
    for (int i=0;i<3;i++) for (int j=0;j<3;j++) if (b[rs+i][cs+j]==num) return false;
    return true;
}

bool SudokuPage::solveRandom(int b[9][9]){
    int r,c; if (!findEmpty(b,r,c)) return true;
    int nums[9] = {1,2,3,4,5,6,7,8,9};
    shuffleRange(std::begin(nums), std::end(nums));
    for (int k=0;k<9;k++){
        int n = nums[k];
        if (isSafe(b,r,c,n)){
            b[r][c]=n;
            if (solveRandom(b)) return true;
            b[r][c]=0;
        }
    }
    return false;
}

int SudokuPage::countSolutionsLimited(int b[9][9], int limit){
    // backtracking: conta fino a 'limit' (early stop)
    int r,c; if (!findEmpty(b,r,c)) return 1;
    int count = 0;
    for (int n=1;n<=9;n++){
        if (isSafe(b,r,c,n)){
            b[r][c]=n;
            count += countSolutionsLimited(b, limit - count);
            if (count >= limit) { b[r][c]=0; return count; }
            b[r][c]=0;
        }
    }
    return count;
}

void SudokuPage::generatePuzzle(int removals){
    // Partendo da m_solution, rimuovi celle garantendo unicità (best-effort)
    copyBoard(m_solution, m_puzzle);
    int toRemove = std::min(removals, 81);
    // rimozione simmetrica
    QVector<QPair<int,int>> coords;
    coords.reserve(81/2);
    for (int r=0;r<9;r++) for (int c=0;c<9;c++){
        int r2 = 8 - r, c2 = 8 - c;
        if (r>r2 || (r==r2 && c>c2)) continue; // prendi solo la metà
        coords.push_back({r,c});
    }
    shuffleRange(coords.begin(), coords.end());
    for (auto rc : coords){
        if (toRemove<=0) break;
        int r=rc.first, c=rc.second, r2=8-r, c2=8-c;
        int backup1 = m_puzzle[r][c], backup2 = m_puzzle[r2][c2];
        m_puzzle[r][c]=0;
        m_puzzle[r2][c2]=0;
        int tmp[9][9]; copyBoard(m_puzzle, tmp);
        int sol = countSolutionsLimited(tmp, 2);
        if (sol != 1) {
            // ripristina se non unico
            m_puzzle[r][c]=backup1;
            m_puzzle[r2][c2]=backup2;
        } else {
            toRemove -= (r==r2 && c==c2) ? 1 : 2;
        }
    }
}

void SudokuPage::loadPuzzleToUI(){
    m_result->clear();
    m_result->setStyleSheet("");
    for (int r=0;r<9;r++){
        for (int c=0;c<9;c++){
            auto *e = m_cell[r][c];
            if (m_puzzle[r][c]==0){
                e->setText("");
                styleGiven(e, false);
            } else {
                e->setText(QString::number(m_puzzle[r][c]));
                styleGiven(e, true);
            }
        }
    }
}

void SudokuPage::newGame(){
    // 1) genera una soluzione casuale
    clearBoard(m_solution);
    solveRandom(m_solution);
    // 2) rimuovi celle secondo difficoltà (garantendo unicità)
    const QString d = m_diff->currentText();
    int removals = 50; // default Medio
    if (d == "Facile") removals = 40;
    else if (d == "Difficile") removals = 60;
    generatePuzzle(removals);
    // 3) UI
    loadPuzzleToUI();
    m_info->setText(QString("Celle rimosse: %1  —  Difficoltà: %2").arg(removals).arg(d));
}

void SudokuPage::checkBoard(){
    bool okAll = true;
    for (int r=0;r<9;r++){
        for (int c=0;c<9;c++){
            auto *e = m_cell[r][c];
            if (e->isReadOnly()) continue; // dato
            QString t = e->text().trimmed();
            if (t.isEmpty()){ okAll=false; continue; }
            bool okNum; int v = t.toInt(&okNum);
            bool right = okNum && v==m_solution[r][c];
            // colora feedback
            if (!right) {
                e->setStyleSheet(e->styleSheet() + " QLineEdit { background:#ffd6d6; }");
                okAll=false;
            } else {
                e->setStyleSheet(e->styleSheet() + " QLineEdit { background:#eaffea; }");
            }
        }
    }
    if (okAll){
        m_result->setText("BRAVO!");
        m_result->setStyleSheet("color: green; font-weight: bold; font-size: 18px;");
    } else {
        m_result->setText("SEI SCARSO!");
        m_result->setStyleSheet("color: red; font-weight: bold; font-size: 18px;");
    }
}

void SudokuPage::revealSolution(){
    for (int r=0;r<9;r++)
        for (int c=0;c<9;c++)
            m_cell[r][c]->setText(QString::number(m_solution[r][c]));
    setAllEditable(false);
    m_result->setText("Soluzione mostrata.");
    m_result->setStyleSheet("");
}

void SudokuPage::applyCellBorder(QLineEdit *e, int r, int c){
    // Disegna un solo bordo per lato condiviso:
    // - celle interne: SOLO right/bottom
    // - prima riga/colonna: aggiungi top/left (outer border)
    int right  = (c%3==2) ? 3 : 1;
    int bottom = (r%3==2) ? 3 : 1;
    int top    = (r==0) ? 4 : 0;   // bordo esterno superiore
    int left   = (c==0) ? 4 : 0;   // bordo esterno sinistro
    // chiusura del perimetro destro/inferiore
    if (c==8) right  = 4;
    if (r==8) bottom = 4;

    // azzera bordo di default e imposta solo i lati necessari
    QString css = "QLineEdit{border:none;}";
    css += "QLineEdit{";
    if (top>0)   css += QString("border-top:%1px solid #333;").arg(top);
    if (left>0)  css += QString("border-left:%1px solid #333;").arg(left);
    css += QString("border-right:%1px solid #333;").arg(right);
    css += QString("border-bottom:%1px solid #333;").arg(bottom);
    // colori di default (testo nero su bianco) + selezione visibile
    css += "color:#111; background:#ffffff;"
           "selection-background-color:#cde1ff;"
           "selection-color:#000;}";
    e->setStyleSheet(css);
}

void SudokuPage::keyPressEvent(QKeyEvent *e){
    if (e->key() == Qt::Key_Escape) {
        emit backRequested();
        return;
    }
    QWidget::keyPressEvent(e);
}

