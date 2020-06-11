#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "exitdialog.h"
#include "helpdialog.h"
#include "aboutdialog.h"
#include <sys/types.h>
#include <sys/param.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <QApplication>
#include <QTreeWidget>
#include <QProgressBar>
#include <QListView>
#include <QObject>
#include <QString>
#include <QList>
#include <QtCharts>
#include <QChartView>
#include <QPieSeries>
#include <QPieSlice>


//MainWindow display of the application
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{


     ui->setupUi(this);

    struct directory *rt=new directory;

    //set home directory path
    rt->dir="/home/yahya/";
    printf("%s\n", rt->dir.c_str());
    long int parent_dir_siz=listFilesRecursively(rt);

    printf("dir is %s, size is %ld \n", rt->dir.c_str(), parent_dir_siz);
    printf("..................................................\n");




    //create tree widget to display directories,thier sizes, and how much they represent from their parent directory in tree view (similar to c++ tree)
    ui->treeWidget->setColumnCount(3);

    //a QTreeWidgetItem represents a single item in the tree widget (think of a it as a tree node)
    QTreeWidgetItem* p_tree = new QTreeWidgetItem();

    //call function createTreeView() to build treeWidget from the backend tree
    createTreeView(rt, p_tree);

    //TraverseTree(p_tree);

    printf("%i", p_tree->childCount());

    // change name of root tree

    ui->treeWidget->addTopLevelItem(p_tree);
    ui->treeWidget->sortItems(1,Qt::DescendingOrder);


    //connect treeWidget to a QFrame to display a usage chart (SLOT buildchart) of a parent directory when that parent is DoubleClicked (SIGNAL)
    //think of SIGNAL as a function call and SLOT as the body of that function which runs on function call
    connect(ui->treeWidget, SIGNAL (itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(buildchart(QTreeWidgetItem *)));
}

//destructor
MainWindow::~MainWindow()
{
    delete ui;
}


//function createTreeView() builds a treeWidget from the backend tree
void MainWindow::createTreeView (struct directory *p_dir, struct QTreeWidgetItem *p_tree) {

    //if the backend node is null return
    if(p_dir==NULL) return;

    //if the backend node is doesn't have children create its corresponding node in treeWidget (file name in 1st col. and file size in 2nd col.) then return
    if (p_dir->numc == 0) {
        p_tree->setText(0, QString::fromStdString(p_dir->name));
        p_tree->setText(1, QString::number(p_dir->file_size));
        return;
    }

    /*if the backend node has children create its corresponding node in treeWidget (file name in 1st col. and file size in 2nd col.) then
     *create a new node for its child, add the new child node to the children of the current node and call createTreeView() recursively on this node.
     */
    for (int i=0;i< p_dir->numc;i++)
    {
        p_tree->setText(0, QString::fromStdString(p_dir->name));
        p_tree->setText(1, QString::number(p_dir->file_size));
        QTreeWidgetItem* child = new QTreeWidgetItem();

        p_tree->addChild(child);

        createTreeView(p_dir->children[i], child);

        //After creating the children of a parent node create the progress bar of those children in the 3rd col. in the treeWidget
        QProgressBar* bar = new QProgressBar();
        bar->setRange(0,100);
        bar->setMaximumHeight(15);

        double disk_ratio =  float(child->text(1).toLongLong()) / float(child->parent()->text(1).toULongLong());
        //std::string disk_ratio_display = std::to_string(disk_ratio);
        //child->setText(2, QString::fromStdString(disk_ratio_display));
        ui->treeWidget->setItemWidget(child, 2,bar);
        bar->setValue(disk_ratio*100);
    }

}

long long int MainWindow::getsize(std::string path)
{
       struct stat stats;
       // returns 0 on successful operation, otherwise  -1
       if (stat(path.c_str(), &stats) == 0)
       {
           return stats.st_size;
       }
       else
       {
           return 0;
       }

}


void MainWindow::TraverseTree (struct QTreeWidgetItem *p_tree)
{

    if(p_tree==NULL) return;
    else if(p_tree->childCount() == 0)
    {
        p_tree->setIcon(0, QIcon(":/resources/img/file.png"));
    }
    for (int i=0;i < p_tree->childCount(); i++)
    {
        p_tree->setIcon(0, QIcon(":/resources/img/directory-icon.jpg"));
        TraverseTree(p_tree->child(i));
    }

}


long long MainWindow::listFilesRecursively(struct directory *rt)
{
    std::string path;

    DIR *dir = opendir((rt->dir).c_str());

    if(dir)
    {
    struct dirent *dp;

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0 && dp->d_type != DT_LNK)
        {

          //  printf("%s\n",  strcat(path, dp->d_name));
              // create new node

            struct directory* temp= new directory;
          //  printf("%s\n", rt->dir);
           path =(rt->dir)+"/"+std::string(dp->d_name);

           temp->dir=path;

           rt->numc++;
           temp->name=dp->d_name;

           temp->file_size=getsize(path);
           rt->children.push_back(temp);

           if(dp->d_type==DT_REG)
           {
            rt->file_size +=getsize(path);
           }
           if(dp->d_type==DT_DIR) rt->file_size+=listFilesRecursively(temp);
        }
    }

    closedir(dir);
  }
 // printf("rt %s\n",rt->file_size);
    return rt->file_size;
}


//SLOT buildchart() serves as a function called when a directory is DoubleClicked to create its usage chart using QCharts
void MainWindow::buildchart(struct QTreeWidgetItem *p_tree)
{
    int no_children = p_tree->childCount();

    //QPieSeries seves as a table of values used to create charts in Excel
    QPieSeries *series = new QPieSeries();
    series->setHoleSize(0.35);

    for(int i=0; i<no_children; i++)
    {
        double disk_ratio =  float(p_tree->child(i)->text(1).toLongLong()) / p_tree->text(1).toULongLong();
        series->append(p_tree->child(i)->text(0) + " " + p_tree->child(i)->text(1) + " bytes", disk_ratio);
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setTitle(p_tree->text(0));
    chart->setTheme(QChart::ChartThemeBlueCerulean);

    QChartView *chartview = new QChartView(chart);
    chartview->setRenderHint(QPainter::Antialiasing);

    chartview->setParent(ui->horizontalFrame);
    chartview->show();
}


void MainWindow::on_actionExit_triggered()
{
    ExitDialog exitDialogue;
    exitDialogue.setModal(true);
    exitDialogue.exec();
}

void MainWindow::on_actionHelp_triggered()
{
    HelpDialog helpDialogue;
    helpDialogue.setModal(true);
    helpDialogue.exec();
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog aboutDialogue;
    aboutDialogue.setModal(true);
    aboutDialogue.exec();
}
