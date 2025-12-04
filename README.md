# Music - 简易音乐播放器
## 简介
在南邮程序设计实践任务需求下，基于Qt6开发的一款简易的音乐播放器，支持本地音乐播放、歌词显示、专辑封面展示、在线搜索以及播放列表管理等功能。该播放器采用模块化设计，界面美观，交互友好，支持跨平台运行。

## 主要功能
### 1、音乐管理
* 轻松添加和移除本地音乐文件，支持mp3、wav、wma和flac等多种音频格式。
* 自动识别音乐文件的元数据信息，如歌曲名、专辑封面等。
* 收藏夹功能，方便用户保存喜爱的歌曲。
* 多歌单管理，支持创建、加载和管理多个自定义歌单。

### 2、播放控制
* 支持播放、暂停、停止、上一曲、下一曲等基本操作。
* 实时显示歌曲播放进度和总时长。
* 可通过滑块拖拽实现精准的播放进度定位。
* 支持单曲循环播放模式。

### 3、音频设置
* 通过滑块调节音量大小，满足不同环境的需求。
* 支持倍速播放，可自由调整播放速度。
* 静音切换功能，一键静音/取消静音。

### 4、专辑封面与歌词
* 自动识别并显示歌曲内置的专辑封面。
* 独立的歌词窗口，支持歌词同步滚动显示。
* 歌词窗口背景使用专辑封面模糊效果，视觉体验更佳。
* 歌词窗口支持淡入淡出动画效果。

### 5、在线搜索
* 集成音乐搜索功能，支持在线搜索歌曲信息。
* 搜索结果以表格形式展示，包含歌曲名、歌手和时长等信息。

## 团队分工

### 开发者A（UI设计与整体框架搭建）
* **负责文件**：`src/ui/mainwindow.h`、`src/ui/mainwindow.cpp`
* **主要职责**：
  * 设计并实现主窗口界面，包括播放控制区、播放列表区和专辑封面显示区
  * 实现自定义窗口拖拽功能（`mousePressEvent`、`mouseMoveEvent`、`mouseReleaseEvent`）
  * 开发事件过滤器（`eventFilter`），处理键盘删除和滑块点击等特殊交互
  * 实现专辑封面自动加载和大小调整功能（`do_metaDataChanged`、`updateCoverArtSize`）
  * 负责主窗口的初始化、信号槽连接和组件布局管理
  * 开发窗口关闭时的资源保存逻辑（`closeEvent`）

### 开发者B（数据文件处理与持久化存储）
* **负责文件**：`src/playlist/playlist_interface.h`、`src/playlist/playlist_interface.cpp`、`src/playlist/playlist_manager.h`、`src/playlist/playlist_manager.c`
* **主要职责**：
  * 设计并实现播放列表管理的C语言底层数据结构（`PlaylistManager`、`Playlist`、`SongInfo`等）
  * 开发C++接口层（`PlaylistInterface`），提供面向对象的数据访问方式
  * 实现歌单数据的持久化存储，支持保存和加载播放列表
  * 开发收藏夹功能，包括添加、移除和查询收藏的歌曲
  * 设计歌曲信息（`SongInfo`）结构体，包含标题、艺术家、专辑、路径等元数据
  * 实现多歌单管理功能，支持创建、删除和切换不同歌单
  * 负责内存管理和资源释放，确保程序稳定运行

### 开发者C（核心播放器与拓展功能）
* **负责文件**：`src/core/main.cpp`、`src/lyrics/lrcwidget.h`、`src/lyrics/lrcwidget.cpp`、`src/search/searchwidget.h`、`src/search/searchwidget.cpp`
* **主要职责**：
  * 开发程序入口点（`main.cpp`），初始化应用程序环境
  * 设计并实现歌词显示窗口（`lrcwidget`），支持歌词解析和同步滚动
  * 开发歌词格式解析功能（`parseLyrics`），支持标准LRC格式
  * 实现歌词窗口的动画效果（`showLyric`、`hideLyric`）和背景模糊处理（`applyBlurToImage`）
  * 开发在线音乐搜索功能（`searchwidget`），支持网络请求和结果展示
  * 实现搜索结果的表格展示（`displaySearchResults`）
  * 负责歌词窗口与主窗口的信号通信，实现播放控制同步

## 项目结构与代码解析

### 整体架构
项目采用模块化设计，将不同功能分离到独立的类中，主要包含以下核心模块：
- 核心程序入口（main.cpp）
- 主窗口和播放器控制（MainWindow）
- 歌词显示和封面管理（lrcwidget）
- 播放列表管理（PlaylistInterface和playlist_manager）
- 在线搜索功能（searchwidget）

各模块之间通过信号槽机制实现松耦合通信，数据流向清晰，便于维护和扩展。

### 详细文件结构
```
src/
├── core/               # 核心程序文件
│   └── main.cpp        # 程序入口文件
├── ui/                 # 用户界面相关
│   ├── mainwindow.h    # 主窗口类头文件
│   └── mainwindow.cpp  # 主窗口类实现文件
├── lyrics/             # 歌词显示相关
│   ├── lrcwidget.h     # 歌词窗口类头文件
│   └── lrcwidget.cpp   # 歌词窗口类实现文件
├── playlist/           # 播放列表管理
│   ├── playlist_interface.h    # 播放列表接口头文件
│   ├── playlist_interface.cpp  # 播放列表接口实现
│   ├── playlist_manager.h      # 播放列表C语言实现头文件
│   └── playlist_manager.c      # 播放列表C语言实现
└── search/             # 在线搜索功能
    ├── searchwidget.h  # 搜索窗口类头文件
    └── searchwidget.cpp # 搜索窗口类实现
```

### 核心代码模块详解

#### 1. 核心程序入口 (src/core/main.cpp)
```cpp
int main(int argc, char *argv[])
```
- **作用**：程序入口点，初始化应用程序环境
- **主要流程**：
  - 创建QApplication应用程序实例
  - 初始化并显示主窗口
  - 进入应用程序主事件循环
- **函数解析**：
  - `QApplication a(argc, argv)`: 创建Qt应用程序实例，初始化Qt环境
  - `MainWindow w`: 创建主窗口实例
  - `w.show()`: 显示主窗口
  - `a.exec()`: 启动应用程序事件循环，处理用户交互

#### 2. 主窗口模块 (src/ui/mainwindow.h/cpp)

##### MainWindow类核心功能
```cpp
class MainWindow : public QMainWindow
```
- **成员变量**：
  - `QMediaPlayer *player`: 媒体播放器核心组件
  - `lrcwidget *lrcWidget`: 歌词显示窗口
  - `PlaylistInterface *m_playlistInterface`: 播放列表管理接口
  - `searchwidget *searchWidget`: 搜索窗口
  - `QNetworkAccessManager *networkManager`: 网络请求管理器

- **主要方法解析**：

  1. **初始化相关方法**：
     - `MainWindow(QWidget *parent)`: 构造函数，初始化UI组件、连接信号槽
     - `loadSavedMusic()`: 加载应用目录下的音乐文件到播放列表
     - `getSavedMusicPaths()`: 获取保存的音乐文件路径列表

  2. **播放控制相关方法**：
     - `on_btnPlay_clicked()`: 处理播放按钮点击，开始或恢复播放
     - `on_btnPause_clicked()`: 处理暂停按钮点击，暂停当前播放
     - `on_btnStop_clicked()`: 处理停止按钮点击，停止播放并重置状态
     - `on_btnPrevious_clicked()`: 播放上一首歌曲
     - `on_btnNext_clicked()`: 播放下一首歌曲
     - `on_doubleSpinBox_valueChanged(double)`: 调整播放速度

  3. **播放列表管理方法**：
     - `on_btnAdd_clicked()`: 添加音乐文件到播放列表
     - `on_btnRemove_clicked()`: 从播放列表移除当前选中的歌曲
     - `on_btnClear_clicked()`: 清空播放列表
     - `on_listWidget_doubleClicked(const QModelIndex &)`: 双击播放列表项开始播放

  4. **播放器状态更新方法**：
     - `do_positionChanged(qint64)`: 处理播放位置变化，更新进度条和时间显示
     - `do_durationChanged(qint64)`: 处理媒体总时长变化，更新进度条最大值
     - `do_sourceChanged(const QUrl &)`: 处理媒体源变化，更新显示并加载歌词
     - `do_playbackStateChanged(QMediaPlayer::PlaybackState)`: 处理播放状态变化，更新按钮状态

  5. **歌单和收藏夹方法**：
     - `on_actionAdd_to_Favorites_triggered()`: 添加当前歌曲到收藏夹
     - `on_actionShow_Favorites_triggered()`: 显示收藏夹中的歌曲
     - `on_actionCreate_Playlist_triggered()`: 创建新的歌单
     - `on_actionAdd_to_Playlist_triggered()`: 添加当前歌曲到指定歌单
     - `on_actionLoad_Playlist_triggered()`: 加载并显示指定歌单

  6. **搜索相关方法**：
     - `on_btnSearch_clicked()`: 处理搜索按钮点击，发送网络请求
     - `onSearchFinished(QNetworkReply *)`: 处理搜索结果，解析JSON数据

  7. **歌词窗口交互方法**：
     - `on_btnCover_clicked()`: 点击封面显示歌词窗口
     - `lrcWidget_sliderMoved(int)`: 处理歌词窗口滑块移动
     - `lrcWidget_playPauseToggled()`: 处理歌词窗口播放/暂停切换

#### 3. 歌词窗口模块 (src/lyrics/lrcwidget.h/cpp)

##### lrcwidget类核心功能
```cpp
class lrcwidget : public QWidget
```
- **成员变量**：
  - `QMap<QTime, QString> lyricsMap`: 歌词时间映射表
  - `QPixmap coverPixmap`: 封面图片
  - `QPropertyAnimation *animation`: 动画效果控制器

- **主要方法解析**：

  1. **歌词加载与解析方法**：
     - `loadLyrics(const QString&)`: 加载歌词文件并解析
     - `parseLyrics(const QString&)`: 解析LRC格式歌词，提取时间标签和文本
     - `updateLyrics(qint64)`: 根据播放位置更新当前显示的歌词

  2. **界面控制方法**：
     - `showLyric()`: 显示歌词窗口，带动画效果
     - `hideLyric()`: 隐藏歌词窗口，带动画效果
     - `setCoverImage(const QPixmap &)`: 设置封面图片
     - `resetCoverImage()`: 重置为默认封面
     - `clearLyrics()`: 清空歌词数据

  3. **视觉效果方法**：
     - `paintEvent(QPaintEvent *)`: 绘制背景模糊效果和遮罩
     - `applyBlurToImage(QImage, int)`: 对图像应用模糊效果
     - `updateListHeight()`: 更新歌词列表高度

  4. **交互信号处理**：
     - `on_horizontalSlider_sliderMoved(int)`: 处理滑块移动信号
     - `on_horizontalSlider_sliderPressed()`: 处理滑块按下信号
     - `on_horizontalSlider_sliderReleased()`: 处理滑块释放信号
     - `on_btnLrcClose_clicked()`: 处理关闭按钮点击

#### 4. 播放列表管理模块 (src/playlist/)

##### PlaylistInterface类（C++接口层）
```cpp
class PlaylistInterface : public QObject
```
- **主要方法解析**：
  - `initialize(const QString &)`: 初始化播放列表管理器
  - `addToFavorites(...)`: 添加歌曲到收藏夹
  - `getFavoritesSongs()`: 获取收藏夹中的所有歌曲
  - `isInFavorites(const QString &)`: 检查歌曲是否在收藏夹中
  - `createPlaylist(const QString &)`: 创建新的歌单
  - `addToPlaylist(...)`: 添加歌曲到指定歌单
  - `getPlaylistSongs(const QString &)`: 获取指定歌单中的所有歌曲
  - `savePlaylists()`: 保存所有播放列表数据

##### 底层C语言实现（playlist_manager.h/c）
- **核心数据结构**：
  - `SongInfo`: 存储歌曲信息的结构体
  - `PlaylistItem`: 歌单项节点，用于链表实现
  - `Playlist`: 歌单结构体，管理歌曲列表
  - `PlaylistManager`: 播放列表管理器，管理所有歌单和收藏夹

- **主要功能**：
  - 提供完整的C语言API，实现歌单的增删改查操作
  - 支持播放列表数据的持久化存储
  - 实现高效的内存管理和资源释放

#### 5. 搜索窗口模块 (src/search/searchwidget.h/cpp)

##### searchwidget类核心功能
```cpp
class searchwidget : public QWidget
```
- **主要方法解析**：
  - `displaySearchResults(const QStringList &)`: 显示搜索结果列表
  - 设计表格界面，以结构化方式展示歌曲信息

## 技术特点

### 1. 跨平台兼容性
- 基于Qt6框架开发，支持Windows、macOS和Linux等多种操作系统
- 使用Qt提供的跨平台API，确保在不同系统上的一致体验

### 2. 模块化设计
- 采用清晰的模块划分，功能分离，便于维护和扩展
- 使用接口层隔离实现细节，提高代码可复用性

### 3. 双语言开发
- 核心数据结构使用C语言实现，确保性能和内存效率
- 界面层和业务逻辑使用C++实现，充分利用面向对象特性

### 4. 信号槽机制
- 大量使用Qt的信号槽机制，实现组件间的松耦合通信
- 减少了模块间的直接依赖，提高了代码的可维护性

### 5. 视觉效果优化
- 实现了歌词窗口的动画过渡效果
- 应用了封面图片模糊背景，提升用户体验

### 6. 网络功能集成
- 集成Qt Network模块，支持在线音乐搜索
- 实现了JSON数据解析，处理网络请求结果

## 后续开发计划

1. **完善播放模式**：增加随机播放和顺序播放等多种播放模式
2. **歌词编辑功能**：支持用户手动编辑和同步歌词
3. **均衡器设置**：添加音频均衡器，提供音效调节功能
4. **皮肤主题**：实现多种界面主题和皮肤切换
5. **音频可视化**：添加音频频谱可视化效果
6. **播放统计**：实现播放历史记录和统计功能

## 贡献声明

感谢所有参与本项目开发的同学，特别是：
- 开发者A：负责UI设计与整体框架搭建
- 开发者B：负责数据文件处理与持久化存储
- 开发者C：负责核心播放器与拓展功能

## 许可证

本项目采用MIT许可证。详情请参见LICENSE文件。