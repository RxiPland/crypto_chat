/*
Window for changing color of text
*/

#include "colordialog.h"
#include "ui_colordialog.h"


ColorDialog::ColorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ColorDialog)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/images/hacker.ico"));
    this->setWindowTitle("Změna barvy");

    ui->pushButton->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    ui->pushButton_2->setFocusPolicy(Qt::FocusPolicy::NoFocus);
}

ColorDialog::~ColorDialog()
{
    delete ui;
}


void ColorDialog::set_color(QString color){

    ColorDialog::user_color = color;

    ui->comboBox->setCurrentText(convert_color(user_color));
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

    } else if(color == "#FFD700"){
        return "Žlutá";

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

    } else if(color == "Žlutá"){
        return "#FFD700";

    } else if(color == "Hnědá"){
        return "#8B4513";
    }

    return "";
}

void ColorDialog::on_comboBox_textActivated(const QString &arg1)
{

    QString color = ColorDialog::convert_color(arg1);
    ColorDialog::user_color = color;
    ui->label->setStyleSheet(tr("QLabel { background-color : %1 }").arg(color));
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
    change_color = true;

    this->close();
    return;
}

