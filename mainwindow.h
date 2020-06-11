#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    struct directory
    {
        std::string name;
        long long int file_size=0;
        std::string dir;
        long int numc=0; // num of children
        int depth=0;
        std::vector<struct directory *> children;// children vectors
    };

    long long int getsize(std::string path);
    void TraverseTree (struct QTreeWidgetItem *p_tree);
    long long  listFilesRecursively(struct directory *rt);
    void createTreeView (struct directory *p_dir, struct QTreeWidgetItem *p_tree);

    ~MainWindow();

protected slots:
    void buildchart(struct QTreeWidgetItem *p_tree);

private slots:
    void on_actionExit_triggered();

    void on_actionHelp_triggered();

    void on_actionAbout_triggered();


private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
