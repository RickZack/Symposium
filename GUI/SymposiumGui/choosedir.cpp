#include "choosedir.h"
#include "ui_choosedir.h"



choosedir::choosedir(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::choosedir)
{
    ui->setupUi(this);
    std::string str="directory 1 directory1\n file 9      file9 owner\n symlink 10      symlink10 modify\n directory 2  directory2\n directory 3   directory3\n directory 4  directory4\n directory 5 directory5\n directory 6  directory6\n directory 7   directory7\n directory 8 directory8\n";



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
    countDir=-1;
    path="/";
    pathForUser="/";
    int indent=0;
    spaces=0;
    for(int i=0; i<count; i++)
    {
        word=separate_word(str);
        QVariant v;
        id=separate_word(str);

        if(word=="directory")
        {
            spaces=0;
            word=separate_word(str);
            QListWidgetItem *item=new QListWidgetItem(QIcon("://icon/folder.png"), QString::fromStdString(word));
            countDir++;
            v.setValue(countDir);
            item->setData(Qt::UserRole,v);
            ui->list->addItem(item);


            if(spaces==indent)
            {
             std::size_t found2 = path.find_last_of("/");
             path.erase(found2, path.size());
             found2=pathForUser.find_last_of("/");
             pathForUser.erase(found2, pathForUser.size());
             }
             else if(spaces<indent)
             {
              for(int i=0; i<=indent-spaces; i++)
              {
               std::size_t found2 = path.find_last_of("/");
               path.erase(found2, path.size());
               found2=pathForUser.find_last_of("/");
               pathForUser.erase(found2, pathForUser.size());
              }
               indent=spaces;

              }
              else if(spaces>indent)
              {
               indent++;
               }
               path=path+"/"+id;
               pathForUser=pathForUser+"/"+word;
               paths.push_back(path);
               pathsForUser.push_back(pathForUser);



            //mettere anche le icone traslate, vedere nel link di Riccardo

            //NB riccordarsi di cambiare il codice di Symposium nella print!
        }

        else if(word=="file")
        {
            int spaces2=spaces;
            word=separate_word(str);
            QListWidgetItem *item=new QListWidgetItem(QIcon("://icon/file.png"), QString::fromStdString(word));
            v.setValue(-1);
            item->setData(Qt::UserRole,v);
            ui->list->addItem(item);
            word=separate_word(str);
            spaces=spaces2;
        }
        else
        {
            int spaces2=spaces;
            word=separate_word(str);
            QListWidgetItem *item=new QListWidgetItem(QIcon("://icon/link.png"), QString::fromStdString(word));
            v.setValue(-1);
            item->setData(Qt::UserRole,v);
            ui->list->addItem(item);
            word=separate_word(str);
            spaces=spaces2;
        }
    }
    path="";
}


void choosedir::on_list_itemClicked(QListWidgetItem *item)
{
    QVariant v = item->data(Qt::UserRole);
    int countDir = v.value<int>();

    if(countDir!=-1)
    {
        std::string name = pathsForUser.at(countDir);
        path=paths.at(countDir);
        name.erase(std::remove(name.begin(), name.end(), ' '), name.end());
        nameOfDir=name;
        ui->nameDir->setText(QString::fromStdString(name));
    }
    else {
        nameOfDir="";
        path="";
        ui->nameDir->setText(QString::fromStdString(nameOfDir));
    }
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
        spaces++;
        return " "+separate_word(string);
    }
    return separato;
}

int choosedir::number_elements(std::string& string)
{
    int count=0;
    for(size_t i = 0; i < string.size(); i++)
      if(string[i] == '\n')
          count++;
    string.erase(std::remove(string.begin(), string.end(), '\n'), string.end());
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
