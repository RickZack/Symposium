#include "choosedir.h"
#include "ui_choosedir.h"


choosedir::choosedir(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::choosedir)
{
    ui->setupUi(this);

    std::string str="directory 1 directory& file 2  file owner& symlink 3  symlink modify&";

    int count=number_elements(str);


    listGenerate(str, count);

    connect(ui->choose, SIGNAL(clicked()), parent, SLOT(change_text()));
    connect(ui->cancel, SIGNAL(clicked()), parent, SLOT(reset_text()));
    connect(ui->cancel, SIGNAL(clicked()), this, SLOT(close()));
}

choosedir::~choosedir()
{
    delete ui;
}




void choosedir::listGenerate(std::string str, int count)
{
    std::string word;
    for(int i=0; i<count; i++)
    {
        word=separate_word(str);
        QVariant v;
        v.setValue(std::stoi(separate_word(str)));
        if(word=="directory")
        {
            word=separate_word(str);
            QListWidgetItem *item=new QListWidgetItem(QIcon("://icon/folder.png"), QString::fromStdString(word));
            item->setData(Qt::UserRole,v);
            ui->list->addItem(item);
        }

        else if(word=="file")
        {
            word=separate_word(str);
            QListWidgetItem *item=new QListWidgetItem(QIcon("://icon/file.png"), QString::fromStdString(word));
            item->setData(Qt::UserRole,v);
            ui->list->addItem(item);
            word=separate_word(str);
        }
        else
        {
            word=separate_word(str);
            QListWidgetItem *item=new QListWidgetItem(QIcon("://icon/link.png"), QString::fromStdString(word));
            item->setData(Qt::UserRole,v);
            ui->list->addItem(item);
            word=separate_word(str);
        }
    }
}


void choosedir::on_list_itemClicked(QListWidgetItem *item)
{
    QVariant v = item->data(Qt::UserRole);
    int id = v.value<int>();
    idDir=id;
    std::string name = item->text().toStdString();
    name.erase(std::remove(name.begin(), name.end(), ' '), name.end());
    nameOfDir=name;
    ui->nameDir->setText(QString::fromStdString(name));
}


std::string choosedir::separate_word(std::string& string)
{
    std::string separato;
    std::size_t found = string.find_first_of(' ');
    if(found==std::string::npos)
        return string;
    separato.append(string, 0, found);
    string.erase(0, found+1);
    if(separato=="")
    {
        return " "+separate_word(string);
    }
    return separato;
}

int choosedir::number_elements(std::string& string)
{
    int count=0;
    for(size_t i = 0; i < string.size(); i++)
      if(string[i] == '&')
          count++;
    string.erase(std::remove(string.begin(), string.end(), '&'), string.end());
    return count;
}

void choosedir::on_choose_clicked()
{
    if(nameOfDir=="")
        QMessageBox::information(parentWidget(),
                                 tr("Choose Directory"), tr("Your need to choose a directory"), QMessageBox::Ok);
    else
        this->hide();
}
