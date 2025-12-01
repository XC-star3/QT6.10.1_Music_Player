#include "mainwindow.h"
#include "playlist_example.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // 可选：调用测试函数演示收藏夹和歌单功能（暂时注释掉以避免构建错误）
    // testPlaylistFunctions();
    
    MainWindow w;
    w.show();
    return a.exec();
}