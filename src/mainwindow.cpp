#include "mainwindow.h"

#include "ui_mainwindow.h"

#include <QDebug>
#include <bitset>
#include <cstdint>
#include <functional>

#define ARRAY_LEN(array) (sizeof((array)) / sizeof((array)[0]))

// These enums define the bit index for each value.
enum CD { CD00 = 30, CD01 = 31, CD02 = 4, CD03 = 5, CD04 = 6, CD05 = 7 };
static constexpr int CDIterable[6] = {CD00, CD01, CD02, CD03, CD04, CD05};

enum A {
    A00 = 16,
    A01 = 17,
    A02 = 18,
    A03 = 19,
    A04 = 20,
    A05 = 21,
    A06 = 22,
    A07 = 23,
    A08 = 24,
    A09 = 25,
    A10 = 26,
    A11 = 27,
    A12 = 28,
    A13 = 29,
    A14 = 0,
    A15 = 1
};
static constexpr int AIterable[16] = {A00, A01, A02, A03, A04, A05, A06, A07,
                                      A08, A09, A10, A11, A12, A13, A14, A15};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Disallow resizing the window via statusbar size grip.
    this->statusBar()->setSizeGripEnabled(false);

    // Disallow resizing the window via window borders.
    setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);

    ui->infoLabel->setText("Click on a bit below to see its purpose.");
    ui->vdpRegCmdLine->setPlaceholderText("0x0000");
    ui->vdpRegEffLbl->setEnabled(false);

    // Add 32 buttons in a row to a vector of QPushButtons.
    // These buttons will be clickable binary digits.
    // On click, display information about what the clicked digit
    // represents in the 32-bit number that is a VDP control port command.
    // Tightly packed.
    ui->binaryholder->setSpacing(0);
    for (int i = 0; i < 32; i++) {
        mBinary.push_back(new QPushButton("0"));
        mBinary[i]->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
        // Look like labels, not buttons.
        mBinary[i]->setFlat(true);
        mBinary[i]->setFixedSize(QSize(20, 20));
        ui->binaryholder->addWidget(mBinary[i]);

        QObject::connect(mBinary[i], &QPushButton::clicked, this, &MainWindow::updateInfoLabel);
    }

    // CD (address mode) bits have red text.
    QString cdStyleSheet("QPushButton { color: red; }");
    for (int i : CDIterable) {
        mBinary[31 - i]->setStyleSheet(cdStyleSheet);
    }

    QString addrStyleSheet("QPushButton { background: none; color: blue; }");
    for (int i : AIterable) {
        mBinary[31 - i]->setStyleSheet(addrStyleSheet);
    }

    QObject::connect(ui->vdpEdit, &QLineEdit::textEdited, this, &MainWindow::on_vdpEdit_edited);

    QObject::connect(ui->vdpRegCmdLine, &QLineEdit::textEdited, this,
                     &MainWindow::on_vdpRegCmdEdit_edited);

    QObject::connect(ui->ramEdit, &QLineEdit::textEdited, this, &MainWindow::on_ramEdit_edited);
    QObject::connect(ui->ramResult, &QLineEdit::textEdited, this, &MainWindow::on_ramResult_edited);

    QObject::connect(ui->srEdit, &QLineEdit::textEdited, this, &MainWindow::on_srEdit_edited);
    QObject::connect(ui->srResult, &QLineEdit::textEdited, this, &MainWindow::on_srResult_edited);
}

MainWindow::~MainWindow()
{
    delete ui;
}

uint16_t get_u16_conversion(const QLineEdit *e, bool &error)
{
    bool conversionstatus = false;
    uint16_t n		  = e->text().toUShort(&conversionstatus, 16);
    if (!conversionstatus) {
        error = true;
    }

    return n;
}

// e: QLineEdit from which to pull input text.
// o: QLineEdit to which the result is output.
// successfunc: A lambda which defines how the input text is manipulated to get the output text.
void set_line(const QLineEdit *e,
              QLineEdit *o,
              const std::function<uint16_t(uint16_t input)> &successfunc)
{
    if (e->text().size() == 0) {
        o->clear();
        return;
    }

    bool error      = false;
    uint16_t result = get_u16_conversion(e, error);
    if (error) {
        o->setText("Invalid input.");
    }
    else {
        result = successfunc(result);
        o->setText(QString::number(result, 16).toUpper());
    }
}

void MainWindow::on_ramEdit_edited()
{
    set_line(ui->ramEdit, ui->ramResult, [](uint16_t in) { return (0x312u - in) & 0xFFFFu; });
}

void MainWindow::on_ramResult_edited()
{
    set_line(ui->ramResult, ui->ramEdit,
             [](uint16_t in) { return ((0x100000000u - in) + 0x312u) & 0xFFFFu; });
}

void MainWindow::on_srEdit_edited()
{
    set_line(ui->srEdit, ui->srResult, [](uint16_t in) { return 0x312u + in; });
}

void MainWindow::on_srResult_edited()
{
    set_line(ui->srResult, ui->srEdit, [](uint16_t in) { return (in - 0x312u) & 0xFFFFu; });
}

void MainWindow::updateInfoLabel()
{
    auto it   = std::find(mBinary.begin(), mBinary.end(), sender());
    int index = static_cast<int>(std::distance(mBinary.begin(), it));

    QString info;

    int bitnum = -1;

    // Set 'info' based on the index of the bit that was clicked.
    switch (index) {
    case 0: // 31
        bitnum = 1;
        info   = "CD";
        break;
    case 1: // 30
        bitnum = 0;
        info   = "CD";
        break;
    case 2:  // 29
    case 3:  // 28
    case 4:  // 27
    case 5:  // 26
    case 6:  // 25
    case 7:  // 24
    case 8:  // 23
    case 9:  // 22
    case 10: // 21
    case 11: // 20
    case 12: // 19
    case 13: // 18
    case 14: // 17
    case 15: // 16
        bitnum = 15 - index;
        info   = "A";
        break;
    case 16: // 15
    case 17: // 14
    case 18: // 13
    case 19: // 12
    case 20: // 11
    case 21: // 10
    case 22: // 09
    case 23: // 08
        info = "Unused";
        break;
    case 24: // 07
    case 25: // 06
    case 26:
    case 27:
        bitnum = 29 - index;
        info   = "CD";
        break;
    case 28:
    case 29:
        info = "Unused";
        break;
    case 30:
        bitnum = 15;
        info   = "A";
        break;
    case 31:
        bitnum = 14;
        info   = "A";
        break;
    default:
        break;
    }

    // Not an unused bit.
    // Add the 'bitnum' to the info string.
    // Width of 2, padded with 0's. Base 10.
    if (info[0] != 'U') {
        info += QString("%1").arg(bitnum, 2, 10, QChar('0'));
    }

    // Address mode (CD) bit.
    // Add the CD bitset string to the info string.
    // Add "[" and "]" around the clicked bit.
    if (info[0] == 'C') {
        info += ' ';
        for (int bitindex : CDIterable) {
            const QString &bit = mBinary[31 - bitindex]->text();
            if (bitindex == index) {
                info += bit;
            }
        }
    }
    // Address bit.
    // Same as with a CD bit, add bitset to string and [] around clicked bit.
    else if (info[0] == 'A') {
        info += ' ';
        std::vector<QPushButton *> buttons(16);
        buttons[0] = mBinary[30];
        buttons[1] = mBinary[31];
        for (int i = 2; i < 16; i++) {
            buttons[static_cast<unsigned>(i)] = mBinary[static_cast<unsigned>(i)];
        }
        for (int i = 0; i < 16; i++) {
            if (i == 15 - bitnum) {
                info += "[" + buttons[static_cast<unsigned>(i)]->text() + "]";
            }
            else {
                info += buttons[static_cast<unsigned>(i)]->text();
            }
        }
    }

    ui->infoLabel->setText(info);
}

void MainWindow::on_vdpEdit_edited()
{
    auto str = ui->vdpEdit->text();

    // If nothing is in the text field, set some things to default values.
    if (str.size() == 0) {
        ui->infoLabel->setText("Click on a bit below to see its purpose.");
        for (auto &bit : mBinary) {
            bit->setText("0");
        }
        return;
    }

    bool conversionStatus = false;
    auto input		  = std::bitset<32>(str.toUInt(&conversionStatus, 16));
    if (!conversionStatus) {
        ui->infoLabel->setText("Invalid input.");
        return;
    }

    // Get address mode.
    std::bitset<6> cd;
    for (int i = 0; i < 6; i++) {
        cd[i] = input[CDIterable[i]];
    }

    // Get address.
    std::bitset<16> a;
    for (int i = 0; i < 16; i++) {
        a[i] = input[AIterable[i]];
    }

    QString address_str = QString::number(a.to_ulong(), 16).toUpper();
    ui->addressLabel->setText("0x" + address_str);

    for (int i = 0; i < ui->checkboxLayout->count(); i++) {
        qobject_cast<QRadioButton *>(ui->checkboxLayout->itemAt(i)->widget())->setChecked(false);
    }

    switch (cd.to_ulong()) {
    case 0b000000:
        ui->cbVramRead->setChecked(true);
        break;
    case 0b000001:
        ui->cbVramWrite->setChecked(true);
        break;
    case 0b001000:
        ui->cbCramRead->setChecked(true);
        break;
    case 0b000011:
        ui->cbCramWrite->setChecked(true);
        break;
    case 0b000100:
        ui->cbVsramRead->setChecked(true);
        break;
    case 0b000101:
        ui->cbVsramWrite->setChecked(true);
        break;
    case 0b001100:
        ui->cbVramRead8->setChecked(true);
        break;
    default:
        break;
    }

    for (unsigned int i = 0; i < 32; i++) {
        mBinary[31 - i]->setText(QString::number(static_cast<int>(input[i]), 2));
    }
}

void MainWindow::on_vdpRegCmdEdit_edited()
{
    bool converror = false;
    uint16_t input = get_u16_conversion(ui->vdpRegCmdLine, converror);
    if (converror) {
        ui->vdpRegEffLbl->clear();
        ui->vdpRegEffLbl->setEnabled(false);
        return;
    }
    QString info;
    uint8_t reg = (input >> 8u) - 0x80u;

    ui->vdpRegEffLbl->setEnabled(true);

    info += "Register: ";
    if (reg > 0x17) {
        info += "invalid.";
        ui->vdpRegEffLbl->setText(info);
        return;
    }

    info += "0x" + QString::number(reg, 16);

    info += "\n";
    info += "Description: ";

    std::bitset<8> b(input & 0xFFu);

    if (reg == 0x0) {
        info += "Mode register 1\n\n";
        if (b[5]) {
            info += "Leftmost 8 pixels are blanked\n";
        }
        if (b[4]) {
            info += "Horizontal interrupt generated for every nth scanline\n";
        }
        if (b[1]) {
            info += "H/V value is latched to its current value\n";
        }
        if (b[0]) {
            info += "Display is enabled";
        }
        else {
            info += "Display is disabled";
        }
    }
    else if (reg == 0x1) {
        info += "Mode register 2\n\n";
        if (b[7]) {
            info += "Additional 64 KB external RAM enabled\n";
        }
        if (b[6]) {
            info += "Display is enabled\n";
        }
        else {
            info += "Display is disabled\n";
        }
        if (b[5]) {
            info += "VBlank interrupt is enabled\n";
        }
        if (b[4]) {
            info += "DMA is enabled\n";
        }
        else {
            info += "DMA is disabled\n";
        }
        if (b[3]) {
            info += "30 cell/240 pixel mode\n";
        }
        else {
            info += "28 cell/224 pixel mode\n";
        }
        if (b[2]) {
            info += "Mega Drive mode";
        }
        else {
            info += "Master System mode";
        }
    }
    else if (reg == 0x2) {
        info += "Plane A name table location\n\n";
        info += "Address: " + QString::number(b.to_ulong() * 0x400u, 16).toUpper();
    }
    else if (reg == 0x3) {
        info += "Window name table location\n\n";
        info += "Address: " + QString::number(b.to_ulong() * 0x400u, 16).toUpper();
    }
    else if (reg == 0x4) {
        info += "Plane B name table location\n\n";
        info += "Address: " + QString::number(b.to_ulong() * 0x2000u, 16).toUpper();
    }
    else if (reg == 0x5) {
        info += "Sprite table location\n\n";
        info += "Address: " + QString::number(b.to_ulong() * 0x200u, 16).toUpper();
    }
    else if (reg == 0x6) {
        info += "Sprite pattern generator base address\n\n";
        if (b[5]) {
            info += "Sprite attribute table address +0x10000\n";
        }
    }
    else if (reg == 0x7) {
        info += "Background colour\n\n";
        uint8_t pal = (b[5] << 1) | b[4];
        uint8_t col = b.to_ulong() & 0b1111;
        info += "Palette: " + QString::number(pal, 10);
        info += "\nColour: " + QString::number(col, 10);
    }
    else if (reg == 0x8) {
        info += "Unused";
    }
    else if (reg == 0x9) {
        info += "Unused";
    }
    else if (reg == 0xA) {
        info += "Horizontal interrupt counter\n\n";
        info += "Value: " + QString::number(b.to_ulong(), 10);
    }
    else if (reg == 0xB) {
        info += "Mode register 3\n\n";
        if (b[3]) {
            info += "IE2: Enabled\n";
        }
        if (b[2]) {
            info += "Vertical scroll two cells per VSRAM word";
        }
        else {
            info += "Entire screen scrolled by one long in VSRAM";
        }
    }
    else if (reg == 0xC) {
        info += "Mode register 4\n\n";
        if (b[7] || b[0]) {
            info += "40 tile wide display mode\n";
        }
        else {
            info += "32 tile wide display mode\n";
        }
        if (b[6]) {
            info += "VSync signal replaced with pixel clock signal\n";
        }
        if (b[4]) {
            info += "Enabled external pixel bus\n";
        }
        if (b[3]) {
            info += "Enabled shadow/highlight mode\n";
        }
        else {
            info += "Disabled shadow/highlight mode\n";
        }
        uint8_t lsm = (b[2] << 1) | b[1];
        if (lsm == 0b00) {
            info += "Interlace mode: none";
        }
        else if (lsm == 0b01) {
            info += "Interlace mode: enabled";
        }
        else if (lsm == 0b11) {
            info += "Interlace mode: double resolution";
        }
    }
    else if (reg == 0xD) {
        info += "Horizontal scroll data location\n\n";
        info += "Address: " + QString::number(b.to_ulong() * 0x400u, 16).toUpper();
    }
    else if (reg == 0xE) {
        info += "Nametable pattern generator base address\n\n";
    }
    else if (reg == 0xF) {
        info += "Auto-increment value\n\n";
        info += "Value: " + QString::number(b.to_ulong(), 10);
    }
    else if (reg == 0x10) {
        info += "Plane size\n\n";
        uint8_t vsz = (b[5] << 1) | b[4];
        uint8_t hsz = (b[1] << 1) | b[0];
        if (vsz == 0b00) {
            info += "VSZ: 32 tiles\n";
        }
        else if (vsz == 0b01) {
            info += "VSZ: 64 tiles\n";
        }
        else if (vsz == 0b11) {
            info += "VSZ: 128 tiles\n";
        }
        if (hsz == 0b00) {
            info += "HSZ: 32 tiles";
        }
        else if (hsz == 0b01) {
            info += "HSZ: 64 tiles";
        }
        else if (hsz == 0b11) {
            info += "HSZ: 128 tiles";
        }
    }
    else if (reg == 0x11) {
        info += "Window plane horizontal position\n\n";
        if (b[7]) {
            info += "Window plane moves right\n";
        }
        else {
            info += "Window plane moves left\n";
        }
        uint8_t whp = b.to_ulong() & 0b00011111;
        info += "Move amount: " + QString::number(whp, 10) + " cells";
    }
    else if (reg == 0x12) {
        info += "Window plane vertical position\n\n";
        if (b[7]) {
            info += "Window plane moves down\n";
        }
        else {
            info += "Window plane moves up\n";
        }
        uint8_t whp = b.to_ulong() & 0b00011111;
        info += "Move amount: " + QString::number(whp, 10) + " cells";
    }
    else if (reg == 0x13) {
        info += "DMA length low byte\n\n";
        info += "Value: " + QString::number(b.to_ulong(), 16).toUpper();
    }
    else if (reg == 0x14) {
        info += "DMA length high byte\n\n";
        info += "Value: " + QString::number(b.to_ulong(), 16).toUpper();
    }
    else if (reg == 0x15) {
        info += "DMA source low byte\n\n";
        info += "Value: " + QString::number(b.to_ulong(), 16).toUpper();
    }
    else if (reg == 0x16) {
        info += "DMA source mid byte\n\n";
        info += "Value: " + QString::number(b.to_ulong(), 16).toUpper();
    }
    else if (reg == 0x17) {
        info += "DMA source high byte\n\n";
        info += "Value: " + QString::number(b.to_ulong(), 16).toUpper();
    }

    ui->vdpRegEffLbl->setText(info);
}
