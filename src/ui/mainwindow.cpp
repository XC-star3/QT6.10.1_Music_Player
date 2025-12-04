#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QTime>
#include <QMenu>
#include <QAction>
#include "../lyrics/lrcwidget.h"
#include "../playlist/playlist_interface.h"

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_playlistInterface) {
        m_playlistInterface->savePlaylists();
    }
    QMainWindow::closeEvent(event);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    // 处理键盘按键事件
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Delete && watched == ui->listWidget) {
            delete ui->listWidget->takeItem(ui->listWidget->currentRow());
            return true;
        }
    }

    // 处理滑块点击事件
    if (event->type() == QEvent::MouseButtonPress && watched == ui->sliderPosition) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            int clickedValue = ui->sliderPosition->minimum() + ((ui->sliderPosition->maximum() - ui->sliderPosition->minimum()) * mouseEvent->pos().x() / ui->sliderPosition->width());
            ui->sliderPosition->setValue(clickedValue);
            player->setPosition(clickedValue);
            return true;
        }
    } else if (event->type() == QEvent::MouseMove && watched == ui->sliderPosition) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->buttons() & Qt::LeftButton) {
            int draggedValue = ui->sliderPosition->minimum() + ((ui->sliderPosition->maximum() - ui->sliderPosition->minimum()) * mouseEvent->pos().x() / ui->sliderPosition->width());
            ui->sliderPosition->setValue(draggedValue);
            player->setPosition(draggedValue);
            return true;
        }
    }

    return QMainWindow::eventFilter(watched, event);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    QPixmap pixmap(":/images/images/KK.jpg");
    ui->label->setPixmap(pixmap.scaled(ui->label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    
    isDragging = false;
    setWindowFlags(Qt::FramelessWindowHint);
    loadSavedMusic();
    ui->listWidget->installEventFilter(this);
    ui->sliderPosition->installEventFilter(this);
    
    // 启用右键菜单
    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listWidget, &QListWidget::customContextMenuRequested, this, &MainWindow::showContextMenu);
    
    player = new QMediaPlayer(this);
    lrcWidget = new lrcwidget(this);
    lrcWidget->hide();
    
    QAudioOutput *audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    
    // 初始化收藏夹和歌单功能
    m_playlistInterface = new PlaylistInterface(this);
    QString appDir = QCoreApplication::applicationDirPath();
    QString dataDir = appDir + "/data/playlists";
    m_playlistInterface->initialize(dataDir);
    updatePlaylistList();
    
    // 连接按钮信号
    connect(ui->btnCreatePlaylist, &QPushButton::clicked, this, &MainWindow::on_actionCreate_Playlist_triggered);
    connect(ui->btnAddToPlaylist, &QPushButton::clicked, this, &MainWindow::on_actionAdd_to_Playlist_triggered);
    connect(ui->btnLoadPlaylist, &QPushButton::clicked, this, &MainWindow::on_actionLoad_Playlist_triggered);
    connect(ui->btnShowFavorites, &QPushButton::clicked, this, &MainWindow::on_actionShow_Favorites_triggered);
    connect(ui->btnAddToFavorites, &QPushButton::clicked, this, &MainWindow::on_actionAdd_to_Favorites_triggered);

    searchWidget = new searchwidget(this);
    searchWidget->resize(760, 405);
    searchWidget->move(11, 52);
    searchWidget->hide();
    networkManager = new QNetworkAccessManager(this);

    // 连接播放器信号
    connect(player, &QMediaPlayer::positionChanged, this, &MainWindow::do_positionChanged);
    connect(player, &QMediaPlayer::durationChanged, this, &MainWindow::do_durationChanged);
    connect(player, &QMediaPlayer::sourceChanged, this, &MainWindow::do_sourceChanged);
    connect(player, &QMediaPlayer::playbackStateChanged, this, &MainWindow::do_playbackStateChanged);
    connect(player, &QMediaPlayer::metaDataChanged, this, &MainWindow::do_metaDataChanged);

    // 连接歌词界面
    connect(player, &QMediaPlayer::positionChanged, lrcWidget, &lrcwidget::updateLyrics);

    // 连接网络搜索
    connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::onSearchFinished);

    // 连接 lrcwidget 控件
    connect(lrcWidget->getSlider(), &QSlider::sliderMoved, this, &MainWindow::lrcWidget_sliderMoved);
    connect(lrcWidget->getPlayButton(), &QPushButton::clicked, this, &MainWindow::lrcWidget_playPauseToggled);
    connect(lrcWidget->getPrevButton(), &QPushButton::clicked, this, &MainWindow::lrcWidget_prevClicked);
    connect(lrcWidget->getNextButton(), &QPushButton::clicked, this, &MainWindow::lrcWidget_nextClicked);
    connect(lrcWidget->getSoundSlider(), &QSlider::valueChanged, this, &MainWindow::lrcWidget_volumeChanged);
    connect(lrcWidget->getSpeedSpinBox(), QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::lrcWidget_speedChanged);
    connect(lrcWidget->getModeButton(), &QPushButton::clicked, this, &MainWindow::lrcWidget_modeChanged);
    connect(lrcWidget, &lrcwidget::sliderMoved, this, &MainWindow::lrcWidget_sliderMoved);
    connect(lrcWidget, &lrcwidget::sliderPressed, this, &MainWindow::lrcWidget_sliderPressed);
    connect(lrcWidget, &lrcwidget::sliderReleased, this, &MainWindow::lrcWidget_sliderReleased);
}

MainWindow::~MainWindow()
{
    m_playlistInterface->savePlaylists();
    delete m_playlistInterface;
    delete ui;
}

void MainWindow::lrcWidget_sliderMoved(int value)
{
    player->setPosition(value);
}

void MainWindow::lrcWidget_playPauseToggled()
{
    if (player->playbackState() == QMediaPlayer::PlayingState) {
        player->pause();
    } else {
        player->play();
    }
}

void MainWindow::lrcWidget_sliderPressed()
{
    player->pause();
}

void MainWindow::lrcWidget_sliderReleased()
{
    player->play();
}

void MainWindow::lrcWidget_prevClicked()
{
    on_btnPrevious_clicked();
}

void MainWindow::lrcWidget_nextClicked()
{
    on_btnNext_clicked();
}

void MainWindow::lrcWidget_volumeChanged(int value)
{
    player->audioOutput()->setVolume(value / 100.0);
}

void MainWindow::lrcWidget_speedChanged(double value)
{
    player->setPlaybackRate(value);
}

void MainWindow::lrcWidget_modeChanged()
{
    on_btnLoop_clicked(!loopPay);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isDragging = true;
        lastMousePosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
    QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (isDragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPosition().toPoint() - lastMousePosition);
        event->accept();
    }
    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
        event->accept();
    }
    QMainWindow::mouseReleaseEvent(event);
}

QStringList MainWindow::getSavedMusicPaths()
{
    QString appDir = QCoreApplication::applicationDirPath();
    QString musicDirectory = appDir + "/sound";
    QDir directory(musicDirectory);
    QStringList musicFiles = directory.entryList(QStringList() << "*.mp3" << "*.wav" << "*.wma" << "*.flac", QDir::Files);
    
    QStringList musicPaths;
    foreach (const QString& fileName, musicFiles) {
        musicPaths.append(musicDirectory + "/" + fileName);
    }
    
    return musicPaths;
}

void MainWindow::loadSavedMusic()
{
    QStringList savedMusicPaths = getSavedMusicPaths();
    if(savedMusicPaths.isEmpty())
        return;
    
    foreach (const auto& item, savedMusicPaths) {
        QFileInfo fileInfo(item);
        QListWidgetItem *aItem = new QListWidgetItem(fileInfo.fileName());
        aItem->setIcon(QIcon(":/images/images/musicFile.png"));
        aItem->setData(Qt::UserRole, QUrl::fromLocalFile(item));
        ui->listWidget->addItem(aItem);
    }
}

void MainWindow::do_positionChanged(qint64 position)
{
    if(ui->sliderPosition->isSliderDown())
        return;
    
    ui->sliderPosition->setSliderPosition(position);
    int secs = position/1000;
    int mins = secs/60;
    secs %= 60;
    positionTime = QString::asprintf("%d:%02d", mins, secs);
    ui->labRatio->setText(positionTime + "/" + durationTime);
    
    // 更新歌词界面
    lrcWidget->getSlider()->setValue(position);
    
    // 更新进度显示
    QTime currentTime(0, 0);
    currentTime = currentTime.addMSecs(position);
    QString timeStr = currentTime.toString("mm:ss");
    
    int duration = player->duration();
    QTime totalTime(0, 0);
    totalTime = totalTime.addMSecs(duration);
    QString totalTimeStr = totalTime.toString("mm:ss");
    
    lrcWidget->updateLabProcess(timeStr + "/" + totalTimeStr);
}

void MainWindow::do_durationChanged(qint64 duration)
{
    ui->sliderPosition->setMaximum(duration);
    int secs = duration / 1000;
    int mins = secs/60;
    secs %= 60;
    durationTime = QString::asprintf("%d:%02d", mins, secs);
    ui->labRatio->setText(positionTime + "/" + durationTime);
    
    // 更新歌词界面进度条最大值
    lrcWidget->getSlider()->setMaximum(duration);
}

void MainWindow::do_sourceChanged(const QUrl &media)
{
    ui->labCurMedia->setText(media.fileName());
    
    // 重置封面和歌词
    lrcWidget->resetCoverImage();
    lrcWidget->clearLyrics();
    
    QString musicPath = media.toLocalFile();
    QFileInfo fileInfo(musicPath);
    QString lyricsPath = fileInfo.path() + "/" + fileInfo.completeBaseName() + ".lrc";
    
    lrcWidget->loadLyrics(lyricsPath);
}

void MainWindow::do_playbackStateChanged(QMediaPlayer::PlaybackState newState)
{
    ui->btnPlay->setEnabled(newState != QMediaPlayer::PlayingState);
    ui->btnPause->setEnabled(newState == QMediaPlayer::PlayingState);
    ui->btnStop->setEnabled(newState == QMediaPlayer::PlayingState);
    
    if((newState == QMediaPlayer::StoppedState) && loopPay) {
        int count = ui->listWidget->count();
        int curRow = ui->listWidget->currentRow();
        ++curRow;
        curRow = curRow >= count ? 0 : curRow;
        ui->listWidget->setCurrentRow(curRow);
        player->setSource(ui->listWidget->currentItem()->data(Qt::UserRole).value<QUrl>());
        player->play();
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    updateCoverArtSize();
}

void MainWindow::do_metaDataChanged()
{
    QMediaMetaData metaData = player->metaData();
    QVariant metaImg = metaData.value(QMediaMetaData::ThumbnailImage);
    
    if(metaImg.isValid()) {
        QImage img = metaImg.value<QImage>();
        QPixmap musicPixmp = QPixmap::fromImage(img);
        
        ui->labPic->setPixmap(musicPixmp);
        ui->btnCover->setIcon(musicPixmp);
        ui->btnCover->setIconSize(ui->btnCover->size());
        ui->btnCover->setFlat(true);
        ui->btnCover->setStyleSheet("border: none;");
        
        updateCoverArtSize();
    } else {
        // 使用默认封面
        QPixmap defaultCover(":/images/images/KK.jpg");
        ui->labPic->setPixmap(defaultCover);
        ui->btnCover->setIcon(defaultCover);
        ui->btnCover->setIconSize(ui->btnCover->size());
        ui->btnCover->setFlat(true);
        ui->btnCover->setStyleSheet("border: none;");
        
        updateCoverArtSize();
    }
}

void MainWindow::updateCoverArtSize()
{
    QSize newSize = ui->scrollArea->size();
    QPixmap pixmap = ui->labPic->pixmap(Qt::ReturnByValue);
    
    if (!pixmap.isNull()) {
        QSize scaledSize = newSize - QSize(30, 30);
        QPixmap scaledPixmap = pixmap.scaled(scaledSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->labPic->setPixmap(scaledPixmap);
    }
}

void MainWindow::on_btnAdd_clicked()
{
    QString curPath = QDir::homePath();
    QString dlgTitle = "选择音频文件";
    QString filter = "音频文件(*.mp3 *.wav *.wma);;所有文件(*.*)";
    
    QStringList fileList = QFileDialog::getOpenFileNames(this, dlgTitle, curPath, filter);
    if(fileList.isEmpty())
        return;
    
    foreach (const auto& item, fileList) {
        QFileInfo fileInfo(item);
        QListWidgetItem *aItem = new QListWidgetItem(fileInfo.fileName());
        aItem->setIcon(QIcon(":/images/images/musicFile.png"));
        aItem->setData(Qt::UserRole, QUrl::fromLocalFile(item));
        ui->listWidget->addItem(aItem);
    }
    
    // 如果现在没有正在播放，就开始播放第一个文件
    if(player->playbackState() != QMediaPlayer::PlayingState && ui->listWidget->count() > 0) {
        ui->listWidget->setCurrentRow(0);
        player->setSource(ui->listWidget->currentItem()->data(Qt::UserRole).value<QUrl>());
        player->play();
    }
}

void MainWindow::on_btnRemove_clicked()
{
    int index = ui->listWidget->currentRow();
    if(index < 0) return;
    
    delete ui->listWidget->takeItem(index);
    if(ui->listWidget->count() <= 0)
        loopPay = false;
}

void MainWindow::on_btnClear_clicked()
{
    loopPay = false;
    ui->listWidget->clear();
    player->stop();
}

void MainWindow::on_btnPlay_clicked()
{
    if(player->playbackState() == QMediaPlayer::PausedState) {
        player->play();
        return;
    }
    
    if(ui->listWidget->count() <= 0)
        return;
    
    if(ui->listWidget->currentRow() < 0)
        ui->listWidget->setCurrentRow(0);
        
    player->setSource(ui->listWidget->currentItem()->data(Qt::UserRole).value<QUrl>());
    player->play();
}

void MainWindow::on_btnPause_clicked()
{
    player->pause();
}

void MainWindow::on_btnStop_clicked()
{
    loopPay = false;
    player->stop();
}

void MainWindow::on_btnPrevious_clicked()
{
    int curRow = ui->listWidget->currentRow();
    --curRow;
    curRow = curRow < 0 ? ui->listWidget->count()-1 : curRow;
    ui->listWidget->setCurrentRow(curRow);
    
    loopPay = false;
    player->setSource(ui->listWidget->currentItem()->data(Qt::UserRole).value<QUrl>());
    player->play();
    loopPay = ui->btnLoop->isChecked();
}

void MainWindow::on_btnNext_clicked()
{
    int count = ui->listWidget->count();
    int curRow = ui->listWidget->currentRow();
    ++curRow;
    curRow = curRow >= count ? 0 : curRow;
    ui->listWidget->setCurrentRow(curRow);
    
    loopPay = false;
    player->setSource(ui->listWidget->currentItem()->data(Qt::UserRole).value<QUrl>());
    player->play();
    loopPay = ui->btnLoop->isChecked();
}

void MainWindow::on_doubleSpinBox_valueChanged(double arg1)
{
    bool wasPlaying = (player->playbackState() == QMediaPlayer::PlayingState);
    
    if (wasPlaying)
        player->pause();
    
    player->setPlaybackRate(arg1);
    
    if (wasPlaying)
        player->play();
}

void MainWindow::on_btnLoop_clicked(bool checked)
{
    loopPay = checked;
}

void MainWindow::on_btnSound_clicked()
{
    bool mute = player->audioOutput()->isMuted();
    player->audioOutput()->setMuted(!mute);
    ui->btnSound->setIcon(QIcon(mute ? ":/images/images/volumn.bmp" : ":/images/images/mute.bmp"));
}

void MainWindow::on_sliderVolumn_valueChanged(int value)
{
    player->audioOutput()->setVolume(value/100.0);
}

void MainWindow::on_sliderPosition_valueChanged(int value)
{
    player->setPosition(value);
}

void MainWindow::on_listWidget_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    loopPay = false;
    player->setSource(ui->listWidget->currentItem()->data(Qt::UserRole).value<QUrl>());
    player->play();
    loopPay = true;
}

void MainWindow::showContextMenu(const QPoint &pos)
{
    if (ui->listWidget->currentItem() == nullptr) {
        return; // 如果没有选中任何项，不显示菜单
    }
    
    QMenu menu(this);
    
    // 添加菜单项
    QAction *addToPlaylistAction = new QAction("添加到歌单", this);
    connect(addToPlaylistAction, &QAction::triggered, this, &MainWindow::on_actionAdd_to_Playlist_triggered);
    
    QAction *addToFavoritesAction = new QAction("添加到收藏夹", this);
    connect(addToFavoritesAction, &QAction::triggered, this, &MainWindow::on_actionAdd_to_Favorites_triggered);
    
    menu.addAction(addToPlaylistAction);
    menu.addAction(addToFavoritesAction);
    
    // 在鼠标位置显示菜单
    menu.exec(ui->listWidget->mapToGlobal(pos));
}

void MainWindow::on_btnCover_clicked()
{
    lrcWidget->setAttribute(Qt::WA_DeleteOnClose);
    lrcWidget->setWindowTitle("歌词窗口");
    
    QMediaMetaData metaData = player->metaData();
    QVariant metaImg = metaData.value(QMediaMetaData::ThumbnailImage);
    if(metaImg.isValid()) {
        QImage img = metaImg.value<QImage>();
        QPixmap musicPixmp = QPixmap::fromImage(img);
        lrcWidget->setCoverImage(musicPixmp);
    }
    
    lrcWidget->setGeometry(0, height(), width(), height());
    lrcWidget->show();
    lrcWidget->showLyric();
}

void MainWindow::on_btnSearch_clicked()
{
    QUrl url("http://mobilecdn.kugou.com/api/v3/search/song?format=json&keyword=" + ui->editSerch->text() + "&page=1&pagesize=20&showtype=1");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 ...");
    request.setRawHeader("Referer", "http://www.kuwo.cn/");
    networkManager->get(request);
}

void MainWindow::onSearchFinished(QNetworkReply *reply)
{
    if(reply->error() == QNetworkReply::NoError) {
        QByteArray response_data = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response_data);
        
        if (jsonDoc.isObject()) {
            QJsonObject jsonObj = jsonDoc.object();
            if (jsonObj.contains("data") && jsonObj["data"].isObject()) {
                QJsonObject dataObj = jsonObj["data"].toObject();
                if (dataObj.contains("info") && dataObj["info"].isArray()) {
                    QJsonArray results = dataObj["info"].toArray();
                    
                    QStringList songInfoList;
                    for(const QJsonValue &value : results) {
                        if (value.isObject()) {
                            QJsonObject songObj = value.toObject();
                            QString songInfo;
                            
                            if (songObj.contains("songname") && songObj["songname"].isString()) {
                                songInfo += songObj["songname"].toString() + ",";
                            }
                            if (songObj.contains("singername") && songObj["singername"].isString()) {
                                songInfo += songObj["singername"].toString() + ",";
                            }
                            if (songObj.contains("duration") && songObj["duration"].isDouble()) {
                                int duration = songObj["duration"].toInt();
                                songInfo += QString::number(duration / 60) + "分" + QString::number(duration % 60) + "秒";
                            }
                            
                            songInfoList.append(songInfo);
                        }
                    }
                    
                    if (searchWidget) {
                        searchWidget->displaySearchResults(songInfoList);
                        searchWidget->show();
                    }
                }
            }
        }
    }
    
    reply->deleteLater();
}

void MainWindow::on_pushButton_clicked()
{
    searchWidget->hide();
}

// 添加到收藏夹
void MainWindow::on_actionAdd_to_Favorites_triggered()
{
    QListWidgetItem *currentItem = ui->listWidget->currentItem();
    QString filePath;
    
    if (currentItem) {
        // 使用选中的歌曲
        filePath = currentItem->data(Qt::UserRole).value<QUrl>().toLocalFile();
    } else {
        // 使用当前播放的歌曲作为备份
        filePath = player->source().toLocalFile();
    }
    
    if (filePath.isEmpty()) {
        QMessageBox::information(this, "提示", "没有可添加的歌曲");
        return;
    }
    
    if (m_playlistInterface->isInFavorites(filePath)) {
        QMessageBox::information(this, "提示", "该歌曲已在收藏夹中");
        return;
    }
    
    if (m_playlistInterface->addCurrentSongToFavorites(filePath, player)) {
        QMessageBox::information(this, "成功", "歌曲已添加到收藏夹");
    } else {
        QMessageBox::warning(this, "失败", "添加到收藏夹失败");
    }
}

// 显示收藏夹
void MainWindow::on_actionShow_Favorites_triggered()
{
    QStringList songs = m_playlistInterface->getFavoritesSongs();
    
    ui->listWidget->clear();
    foreach (const QString &songInfo, songs) {
        QStringList parts = songInfo.split('|');
        if (parts.size() >= 4) {
            QString displayText = QString("%1 - %2").arg(parts[0]).arg(parts[1]);
            ui->listWidget->addItem(displayText);
            ui->listWidget->item(ui->listWidget->count() - 1)->setData(Qt::UserRole, parts[3]);
        }
    }
}

// 创建新歌单
void MainWindow::on_actionCreate_Playlist_triggered()
{
    bool ok;
    QString playlistName = QInputDialog::getText(this, "新建歌单", "请输入歌单名称：", QLineEdit::Normal, "", &ok);
    if (!ok || playlistName.isEmpty()) {
        return;
    }
    
    if (m_playlistInterface->createPlaylist(playlistName)) {
        QMessageBox::information(this, "成功", "歌单创建成功");
        updatePlaylistList();
    } else {
        QMessageBox::warning(this, "失败", "歌单创建失败，可能已存在同名歌单");
    }
}

// 更新歌单列表
void MainWindow::updatePlaylistList()
{
    QStringList playlists = m_playlistInterface->getAllPlaylistNames();
}

// 添加到指定歌单
void MainWindow::on_actionAdd_to_Playlist_triggered()
{
    QListWidgetItem *currentItem = ui->listWidget->currentItem();
    QString filePath;
    
    if (currentItem) {
        // 使用选中的歌曲
        filePath = currentItem->data(Qt::UserRole).value<QUrl>().toLocalFile();
    } else {
        // 使用当前播放的歌曲作为备份
        filePath = player->source().toLocalFile();
    }
    
    if (filePath.isEmpty()) {
        QMessageBox::warning(this, "提示", "没有可添加的歌曲。");
        return;
    }
    
    QStringList playlistNames = m_playlistInterface->getAllPlaylistNames();
    if (playlistNames.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先创建歌单。");
        return;
    }
    
    bool ok;
    QString selectedPlaylist = QInputDialog::getItem(
        this, "选择歌单", "请选择要添加到的歌单:",
        playlistNames, 0, false, &ok);
    
    if (ok && !selectedPlaylist.isEmpty()) {
        QFileInfo fileInfo(filePath);
        QString title = fileInfo.baseName();
        int duration = player->duration() / 1000;
        
        if (m_playlistInterface->addToPlaylist(selectedPlaylist, title, "", "", filePath, "", "", duration)) {
            QMessageBox::information(this, "成功", "歌曲已成功添加到歌单！");
        } else {
            QMessageBox::warning(this, "失败", "添加到歌单失败！");
        }
    }
}

// 加载歌单
void MainWindow::on_actionLoad_Playlist_triggered()
{
    QStringList playlistNames = m_playlistInterface->getAllPlaylistNames();
    if (playlistNames.isEmpty()) {
        QMessageBox::warning(this, "提示", "没有找到任何歌单。");
        return;
    }
    
    bool ok;
    QString selectedPlaylist = QInputDialog::getItem(
        this, "选择歌单", "请选择要加载的歌单:",
        playlistNames, 0, false, &ok);
    
    if (ok && !selectedPlaylist.isEmpty()) {
        QStringList songs = m_playlistInterface->getPlaylistSongs(selectedPlaylist);
        
        if (songs.isEmpty()) {
            QMessageBox::information(this, "提示", "选中的歌单中没有歌曲。");
            return;
        }
        
        ui->listWidget->clear();
        
        foreach (const QString &songInfo, songs) {
            QStringList parts = songInfo.split('|');
            if (parts.size() >= 4) {
                QString displayText = QString("%1 - %2").arg(parts[0]).arg(parts[1]);
                QListWidgetItem *item = new QListWidgetItem(displayText);
                item->setIcon(QIcon(":/images/images/musicFile.png"));
                item->setData(Qt::UserRole, QUrl::fromLocalFile(parts[3]));
                ui->listWidget->addItem(item);
            }
        }
        
        QMessageBox::information(this, "成功", QString("已成功加载 '%1' 歌单，共 %2 首歌曲。").arg(selectedPlaylist).arg(songs.size()));
        
        if (ui->listWidget->count() > 0) {
            ui->listWidget->setCurrentRow(0);
            player->setSource(ui->listWidget->currentItem()->data(Qt::UserRole).value<QUrl>());
            player->play();
        }
    }
}