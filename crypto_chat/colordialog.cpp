/*
Window for changing color of text
*/

#include "colordialog.h"
#include "ui_colordialog.h"

#include <windows.h>
#include <QMessageBox>

ColorDialog::ColorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ColorDialog)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/images/hacker.ico"));
    this->setWindowTitle("Změna barvy");

    ui->lineEdit->setHidden(true);
    ui->pushButton_3->setHidden(true);

    ui->pushButton->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    ui->pushButton_2->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    ui->pushButton_3->setFocusPolicy(Qt::FocusPolicy::NoFocus);
}

ColorDialog::~ColorDialog()
{
    delete ui;
}


void ColorDialog::set_color(QString color){

    ColorDialog::user_color = color;

    QString convertedColor = convert_color(user_color);

    if (convertedColor == ColorDialog::user_color){
        ui->comboBox->setCurrentText("Vlastní...");
        ui->lineEdit->setHidden(false);
        ui->pushButton_3->setHidden(false);

        ui->lineEdit->setText(ColorDialog::user_color);

    } else{
        ui->comboBox->setCurrentText(convertedColor);
    }

    ColorDialog::on_comboBox_textActivated(ui->comboBox->currentText());
}

QString ColorDialog::convert_color(QString color){

    if(color == "black"){
        return "Černá";

    } else if(color == "red"){
        return "Červená";

    } else if(color == "green"){
        return "Zelená";

    } else if(color == "#00BFFF"){
        return "Modrá";

    } else if(color == "#800080"){
        return "Fialová";

    } else if(color == "#FF8C00"){
        return "Oranžová";

    } else if(color == "#8B4513"){
        return "Hnědá";

    } else if (color == "Černá"){
        return "black";

    } else if(color == "Červená"){
        return "red";

    } else if (color == "Zelená"){
        return "green";

    } else if(color == "Modrá"){
        return "#00BFFF";

    } else if(color == "Fialová"){
        return "#800080";

    } else if(color == "Oranžová"){
        return "#FF8C00";

    } else if(color == "Hnědá"){
        return "#8B4513";
    }

    return color;
}

void ColorDialog::on_comboBox_textActivated(const QString &arg1)
{

    QString color = arg1;

    if (color.contains("Vlastní")){

        ui->label->setStyleSheet("QLabel { background-color : }");

        QString hexColor = ui->lineEdit->text().replace(" ", "");

        ui->lineEdit->setHidden(false);
        ui->pushButton_3->setHidden(false);

        if(hexColor.length() == 7 && hexColor.startsWith("#")){
            ui->label->setStyleSheet(tr("QLabel { background-color : %1 }").arg(hexColor));

            ColorDialog::user_color = hexColor;
        }

    } else{
        ui->lineEdit->setHidden(true);
        ui->pushButton_3->setHidden(true);

        color = ColorDialog::convert_color(color);
        ColorDialog::user_color = color;
        ui->label->setStyleSheet(tr("QLabel { background-color : %1 }").arg(color));
    }

}


void ColorDialog::on_pushButton_2_clicked()
{
    // close window (old color)

    this->close();
    return;
}


void ColorDialog::on_pushButton_clicked()
{
    // close window (apply new color)

    if (ui->comboBox->currentText().contains("Vlastní")){

        QString hexColor = ui->lineEdit->text().replace(" ", "");

        if (hexColor.length() == 7 && hexColor.startsWith("#")){
            color_changed = true;

        } else {
            QMessageBox::critical(this, "Chyba", "Zadejte validní HEX hodnotu barvy! (např. #fffff, #cdff00)");
            return;
        }

    } else {
        color_changed = true;
    }


    this->close();
    return;
}


void ColorDialog::on_pushButton_3_clicked()
{
    ShellExecute(0, 0, L"https://rgbacolorpicker.com/hex-color-picker", 0, 0, SW_HIDE);
}


void ColorDialog::on_lineEdit_textEdited(const QString &arg1)
{
    if(arg1.length() == 7 && arg1.startsWith("#")){
        ColorDialog::user_color = arg1;
        ui->label->setStyleSheet(tr("QLabel { background-color : %1 }").arg(arg1));

        ColorDialog::user_color = arg1;

    } else{
        ui->label->setStyleSheet("QLabel { background-color : }");
    }
}


void ColorDialog::on_lineEdit_returnPressed()
{
    ColorDialog::on_pushButton_clicked();
}

