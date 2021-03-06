#include "choosedir.h"
#include "ui_choosedir.h"


choosedir::choosedir(QWidget *parent, std::string pathDir) :
    QDialog(parent),
    pathDir(pathDir), ui(new Ui::choosedir)
{
    ui->setupUi(this);
    setFixedSize(size());
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    std::string str=pathDir;
    int count=number_elements(str);

    ui->tree->setColumnCount(3);
    ui->tree->headerItem()->setText(0, "folder:");
    ui->tree->header()->setVisible(true);
    ui->tree->setColumnHidden(1, true);
    ui->tree->setColumnHidden(2, true);

    treeGenerate(str, count);
    ui->tree->header()->setStretchLastSection(false);
    ui->tree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    connect(ui->choose, SIGNAL(clicked()), parent, SLOT(change_text()));
    connect(ui->choose, SIGNAL(clicked()), parent, SLOT(enableButtonsAfter()));
    connect(ui->cancel, SIGNAL(clicked()), parent, SLOT(reset_text()));
    connect(ui->cancel, SIGNAL(clicked()), parent, SLOT(enableButtonsAfter()));
    connect(ui->cancel, SIGNAL(clicked()), this, SLOT(close()));
    setAttribute( Qt::WA_DeleteOnClose );
    ui->dirEmpty->hide();
}

choosedir::~choosedir()
{
    delete ui;
}

void choosedir::treeGenerate(std::string str, int count)
{
    std::string name;
    std::string path="./";
    std::string pathUser="/home/";
    std::string type;
    int indent=0;
    QTreeWidgetItem *fatherAbs=new QTreeWidgetItem();
    fatherAbs->setIcon(0, QIcon("://icon/folder.png"));
    fatherAbs->setText(0, "home");
    fatherAbs->setText(1, QString::fromStdString(path));
    fatherAbs->setText(2, QString::fromStdString(pathUser));
    ui->tree->addTopLevelItem(fatherAbs);
    QTreeWidgetItem *father=new QTreeWidgetItem();
    father=nullptr;
    std::stack<QTreeWidgetItem *> listOfFathers;

    for(int i=0; i<count; i++)
    {
        type=separate_word(str);
        id=separate_word(str);
        spaces=0;
        name=separate_word(str);
        for(size_t i=0;i<name.size();i++){
            if(name[i]==0x1F)
               name.replace(i,1,1,' ');
        }

        if(spaces==0)
        {
            listOfFathers=std::stack<QTreeWidgetItem *>();
            father=fatherAbs;
            if(type=="directory")
            {
                path="./"+id;
                pathUser="/home/"+name+"/";
            }
        }
        else if(spaces==indent+1)
        {
            father=listOfFathers.top();
            if(type=="directory")
            {
                path=path+"/"+id;
                pathUser=pathUser+name+"/";
            }
        }
        else if(spaces<indent)
        {
            for(int i=0; i<=indent-spaces; i++)
            {
                listOfFathers.pop();
            }
            father=listOfFathers.top();
            if(type=="directory")
            {
                for(int i=0; i<=indent-spaces; i++)
                {
                    std::size_t found = path.find_last_of("/");
                    path.erase(found, path.size());
                    found=pathUser.find_last_of("/");
                    pathUser.erase(found, pathUser.size());
                    found=pathUser.find_last_of("/");
                    pathUser.erase(found, pathUser.size());
                    pathUser=pathUser+"/";
                }
                path=path+"/"+id;
                pathUser=pathUser+name+"/";
            }
        }
        else
        {
            if(type=="directory")
            {
                listOfFathers.pop();
                std::size_t found = path.find_last_of("/");
                path.erase(found, path.size());
                found=pathUser.find_last_of("/");
                pathUser.erase(found, pathUser.size());
                found=pathUser.find_last_of("/");
                pathUser.erase(found, pathUser.size());
                pathUser=pathUser+"/";
                father=listOfFathers.top();
                path=path+"/"+id;
                pathUser=pathUser+name+"/";
            }
        }



        if(type=="directory")
        {
            QTreeWidgetItem *item=new QTreeWidgetItem();
            item->setIcon(0, QIcon("://icon/folder.png"));
            item->setText(0, QString::fromStdString(name));
            item->setText(1, QString::fromStdString(path));
            item->setText(2, QString::fromStdString(pathUser));
            father->addChild(item);
            listOfFathers.push(item);
            indent=spaces;
           }

        else if(type=="file")
        {
            QTreeWidgetItem *item=new QTreeWidgetItem();
            item->setIcon(0, QIcon("://icon/file.png"));
            item->setText(0, QString::fromStdString(name));
            item->setText(1, "");
            item->setText(2, "");
            father->addChild(item);
            std::string word=separate_word(str);
            indent=spaces;
        }
        else
        {
            QTreeWidgetItem *item=new QTreeWidgetItem();
            item->setIcon(0, QIcon("://icon/link.png"));
            item->setText(0, QString::fromStdString(name));
            item->setText(1, "");
            item->setText(2, "");
            father->addChild(item);
            std::string word=separate_word(str);
            indent=spaces;
        }
    }




}


void choosedir::on_tree_itemClicked(QTreeWidgetItem *item, int column)
{
    ui->dirEmpty->hide();
    QString pathUser;
    if(column==2)
        pathUser=item->text(column);
    else
        pathUser=item->text(2);
    nameOfDir=pathUser.toStdString();
    pathId=(item->text(1)).toStdString();
    ui->nameDir->setText(pathUser);
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
        return separate_word(string);
    }
    return separato;
}

int choosedir::number_elements(std::string& string)
{
    int count=0;
    for(size_t i = 0; i < string.size(); i++)
      if(string[i] == '\n')
      {count++; string[i]=' ';}
    return count;
}

void choosedir::on_choose_clicked()
{
    ui->dirEmpty->hide();
    if(nameOfDir=="")
        ui->dirEmpty->show();
    else
        this->close();
}

