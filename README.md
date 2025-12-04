# 音乐播放器项目文档

## 1. 程序框架分析

本项目是一个基于Qt框架开发的音乐播放器，采用了C++与C混合编程的架构设计，将UI界面与底层逻辑分离，实现了良好的模块化结构。

### 1.1 总体架构

```
├── src/
│   ├── core/          # 程序入口
│   ├── lyrics/        # 歌词显示模块
│   ├── playlist/      # 播放列表管理模块
│   ├── search/        # 搜索功能模块
│   └── ui/            # 主界面模块
├── resources/         # 资源文件（图片、音效等）
├── ui/                # UI设计文件
└── build/             # 编译输出目录
```

### 1.2 模块依赖关系

```
core ──> ui ──> playlist_interface ──> playlist_manager ──> playlist_util
     └─> lyrics
     └─> search
```

- **core模块**：程序入口点，初始化整个应用
- **ui模块**：主界面，集成所有功能模块
- **playlist模块**：播放列表管理，采用C接口+C++封装的双层结构
- **lyrics模块**：歌词显示与解析
- **search模块**：音乐搜索功能

### 1.3 技术栈

- **框架**：Qt 6
- **语言**：C++（主要界面）、C（底层播放列表管理）
- **多媒体**：Qt Multimedia
- **UI设计**：Qt Designer
- **构建系统**：CMake

## 2. 开发者分工

### 2.1 尧铭（组长）
**负责范围**：所需数据文件读取和保存，歌词解析与显示

**主要职责**：
- 设计并实现播放列表数据的持久化存储机制，使用CSV格式管理播放列表文件
- 开发文件路径处理、文件名安全化等工具函数，确保跨平台兼容性
- 实现播放列表的加载、解析、保存等核心功能，包括SongInfo结构体和双链表数据结构的设计
- 负责playlist_manager.c/.h和playlist_util.c/.h文件的开发与维护
- 设计并实现歌词文件（LRC格式）的解析算法
- 维护lrcwidget.c/.h文件

**具体文件**：`src/playlist/playlist_manager.h`、`src/playlist/playlist_manager.c`、`src/playlist/playlist_util.h`、`src/playlist/playlist_util.c`、`src/lyrics/lrcwidget.h`、`src/lyrics/lrcwidget.cpp`

### 2.2 邓陈涵
**负责范围**：搜索功能、核心播放器

**主要职责**：
- 设计并实现音乐搜索功能，包括搜索界面、搜索逻辑和结果展示
- 开发搜索请求的网络通信、数据解析和结果处理机制
- 实现核心播放器功能，包括音频文件的加载、播放、暂停、停止、音量控制等
- 负责搜索结果与播放列表的交互逻辑，支持将搜索结果添加到播放列表
- 维护searchwidget.c/.h文件和mainwindow中与播放控制相关的代码

**具体文件**：`src/search/searchwidget.h`、`src/search/searchwidget.cpp`、`src/ui/mainwindow.h`、`src/ui/mainwindow.cpp`（播放控制部分）

### 2.3 邢展宾
**负责范围**：UI设计、图形化界面整体框架、串联模块（封装代码）

**主要职责**：
- 设计并实现音乐播放器的主界面，包括布局、控件选择和用户交互流程
- 开发界面框架，使用Qt Designer创建ui文件并生成对应的C++代码
- 实现模块间的串联，封装底层C接口为C++类，提供面向对象的API
- 负责主窗口与各功能模块（播放列表、歌词、搜索）的集成与通信
- 开发歌词同步显示功能，根据音频播放进度实时更新歌词
- 实现歌词窗口的界面设计和交互功能，支持歌词滚动、进度调整等
- 维护main.cpp、mainwindow.c/.h和playlist_interface.c/.h文件
- 封装底层接口，确保底层统一并并编译出完整程序

**具体文件**：`src/core/main.cpp`、`src/ui/mainwindow.h`、`src/ui/mainwindow.cpp`、`src/playlist/playlist_interface.h`、`src/playlist/playlist_interface.cpp`、`src/playlist/playlist_util.c`

## 3. 模块函数详细说明
  
  ### 3.1 core模块

**文件**：`src/core/main.cpp`

- **功能**：程序入口点，初始化Qt应用和主窗口
- **主要流程**：
  1. 创建QApplication实例
  2. 初始化主窗口
  3. 启动事件循环

### 3.2 lyrics模块

**文件**：`src/lyrics/lrcwidget.h` / `src/lyrics/lrcwidget.cpp`

#### 类：lrcwidget

##### 公共方法：

| 方法签名 | 功能描述 |
|---------|---------|
| `QSlider* getSlider() const` | 获取进度条控件 |
| `QPushButton* getPlayButton() const` | 获取播放按钮 |
| `QPushButton* getPrevButton() const` | 获取上一曲按钮 |
| `QPushButton* getNextButton() const` | 获取下一曲按钮 |
| `QSlider* getSoundSlider() const` | 获取音量滑块 |
| `QDoubleSpinBox* getSpeedSpinBox() const` | 获取播放速度控件 |
| `void loadLyrics(const QString& filePath)` | 加载歌词文件 |
| `QMap<QTime, QString> parseLyrics(const QString& filePath)` | 解析歌词文件 |
| `void setCoverImage(const QPixmap &pixmap)` | 设置封面图片 |
| `void updateListHeight()` | 更新列表高度 |
| `void showLyric()` | 显示歌词 |
| `void hideLyric()` | 隐藏歌词 |
| `void resetCoverImage()` | 更新封面 |
| `void clearLyrics()` | 清空歌词 |
| `void updateLabProcess(const QString &text)` | 更新进度文本 |

##### 槽函数：

| 方法签名 | 功能描述 |
|---------|---------|
| `void updateLyrics(qint64 position)` | 更新当前播放位置的歌词 |

### 3.3 playlist模块

#### 3.3.1 playlist_interface模块

**文件**：`src/playlist/playlist_interface.h` / `src/playlist/playlist_interface.cpp`

#### 类：PlaylistInterface

##### 公共方法：

| 方法签名 | 功能描述 |
|---------|---------|
| `bool initialize(const QString &dataDirectory)` | 初始化播放列表管理器 |
| `bool addToFavorites(...)` | 添加歌曲到收藏夹 |
| `QStringList getFavoritesSongs()` | 获取收藏夹歌曲列表 |
| `bool removeFromFavorites(const QString &filePath)` | 从收藏夹移除歌曲 |
| `bool isInFavorites(const QString &filePath)` | 检查歌曲是否在收藏夹 |
| `bool savePlaylists()` | 保存所有播放列表 |
| `bool addCurrentSongToFavorites(...)` | 将当前播放歌曲添加到收藏夹 |
| `QStringList getAllPlaylistNames()` | 获取所有播放列表名称 |
| `bool createPlaylist(const QString &name)` | 创建新播放列表 |
| `bool deletePlaylist(const QString &name)` | 删除播放列表 |
| `bool addToPlaylist(...)` | 添加歌曲到指定播放列表 |
| `QStringList getPlaylistSongs(const QString &playlistName)` | 获取指定播放列表的歌曲 |
| `bool removeFromPlaylist(...)` | 从指定播放列表移除歌曲 |

#### 3.3.2 playlist_manager子模块

**文件**：`src/playlist/playlist_manager.h` / `src/playlist/playlist_manager.c`

##### 数据结构：

- `SongInfo`：歌曲信息结构体
- `PlaylistItem`：播放列表项
- `Playlist`：播放列表
- `PlaylistManager`：播放列表管理器

##### 函数：

| 函数签名 | 功能描述 |
|---------|---------|
| `PlaylistManager *playlist_manager_init(...)` | 初始化播放列表管理器 |
| `void playlist_manager_free(PlaylistManager *manager)` | 释放播放列表管理器 |
| `bool add_to_favorites(...)` | 添加歌曲到收藏夹 |
| `bool remove_from_favorites(...)` | 从收藏夹移除歌曲 |
| `bool is_in_favorites(...)` | 检查歌曲是否在收藏夹 |
| `Playlist *get_favorites(...)` | 获取收藏夹 |
| `Playlist *create_playlist(...)` | 创建新播放列表 |
| `bool delete_playlist(...)` | 删除播放列表 |
| `Playlist *get_playlist(...)` | 获取指定播放列表 |
| `bool add_to_playlist(...)` | 添加歌曲到播放列表 |
| `bool remove_from_playlist(...)` | 从播放列表移除歌曲 |
| `bool save_playlists(...)` | 保存播放列表到文件 |
| `bool load_playlists(...)` | 从文件加载播放列表 |
| `SongInfo *create_song_info(...)` | 创建歌曲信息 |
| `void free_song_info(SongInfo *song)` | 释放歌曲信息 |
| `char **get_all_playlist_names(...)` | 获取所有播放列表名称 |
| `void free_playlist_names(char **names, int count)` | 释放播放列表名称数组 |

#### 3.3.3 playlist_util子模块

**文件**：`src/playlist/playlist_util.h` / `src/playlist/playlist_util.c`

##### 函数：

| 函数签名 | 功能描述 |
|---------|---------|
| `char *safe_strdup(const char *s)` | 安全的字符串复制 |
| `bool build_playlist_filepath(...)` | 构建播放列表文件路径 |
| `bool ensure_directory(const char *dir_path)` | 确保目录存在 |
| `bool sanitize_filename(...)` | 清理文件名，使其适合文件系统 |
| `int parse_csv_line(...)` | 解析CSV行 |
| `void free_parsed_fields(...)` | 释放解析的CSV字段 |
| `char *escape_csv_field(const char *field)` | 转义CSV字段 |
| `PlaylistItem *create_playlist_item(SongInfo *song)` | 创建播放列表项 |
| `Playlist *create_playlist(const char *name)` | 创建播放列表 |

### 3.4 search模块

**文件**：`src/search/searchwidget.h` / `src/search/searchwidget.cpp`

#### 类：searchwidget

##### 公共方法：

| 方法签名 | 功能描述 |
|---------|---------|
| `void displaySearchResults(const QStringList &results)` | 显示搜索结果 |

##### 信号：

| 信号名称 | 功能描述 |
|---------|---------|
| `void songDoubleClicked(const QString &songName)` | 当歌曲被双击时发出 |

### 3.5 ui模块

**文件**：`src/ui/mainwindow.h` / `src/ui/mainwindow.cpp`

#### 类：MainWindow

##### 公共方法：

| 方法签名 | 功能描述 |
|---------|---------|
| `MainWindow(QWidget *parent = nullptr)` | 构造函数 |
| `~MainWindow()` | 析构函数 |

##### 槽函数：

| 方法签名 | 功能描述 |
|---------|---------|
| `void updateCoverArtSize()` | 更新封面艺术大小 |
| `QStringList getSavedMusicPaths()` | 获取保存的音乐路径 |
| `void loadSavedMusic()` | 加载保存的音乐 |
| `void do_positionChanged(qint64 position)` | 处理播放位置变化 |
| `void do_durationChanged(qint64 duration)` | 处理播放时长变化 |
| `void do_sourceChanged(const QUrl &media)` | 处理媒体源变化 |
| `void do_playbackStateChanged(...)` | 处理播放状态变化 |
| `void do_metaDataChanged()` | 处理元数据变化 |
| `void on_actionAdd_to_Favorites_triggered()` | 处理添加到收藏夹动作 |
| `void on_actionShow_Favorites_triggered()` | 处理显示收藏夹动作 |
| `void on_actionCreate_Playlist_triggered()` | 处理创建播放列表动作 |
| `void on_actionAdd_to_Playlist_triggered()` | 处理添加到播放列表动作 |
| `void on_actionLoad_Playlist_triggered()` | 处理加载播放列表动作 |
| `void updatePlaylistList()` | 更新播放列表列表 |
| `void on_btnAdd_clicked()` | 处理添加按钮点击 |
| `void on_btnRemove_clicked()` | 处理移除按钮点击 |
| `void on_btnClear_clicked()` | 处理清空按钮点击 |
| `void on_btnPlay_clicked()` | 处理播放按钮点击 |
| `void on_btnPause_clicked()` | 处理暂停按钮点击 |
| `void on_btnStop_clicked()` | 处理停止按钮点击 |
| `void on_btnPrevious_clicked()` | 处理上一曲按钮点击 |
| `void on_btnNext_clicked()` | 处理下一曲按钮点击 |
| `void on_doubleSpinBox_valueChanged(double arg1)` | 处理播放速度变化 |
| `void on_btnLoop_clicked(bool checked)` | 处理循环播放按钮点击 |
| `void on_btnSound_clicked()` | 处理声音按钮点击 |
| `void on_sliderVolumn_valueChanged(int value)` | 处理音量滑块变化 |
| `void on_sliderPosition_valueChanged(int value)` | 处理进度滑块变化 |
| `void on_listWidget_doubleClicked(...)` | 处理列表项双击 |
| `void on_btnCover_clicked()` | 处理封面按钮点击 |
| `void on_btnSearch_clicked()` | 处理搜索按钮点击 |
| `void onSearchFinished(QNetworkReply *reply)` | 处理搜索完成 |
| `void lrcWidget_sliderMoved(int value)` | 处理歌词窗口滑块移动 |
| `void lrcWidget_playPauseToggled()` | 处理歌词窗口播放/暂停切换 |
| `void lrcWidget_prevClicked()` | 处理歌词窗口上一曲点击 |
| `void lrcWidget_nextClicked()` | 处理歌词窗口下一曲点击 |
| `void lrcWidget_volumeChanged(int value)` | 处理歌词窗口音量变化 |
| `void lrcWidget_speedChanged(double value)` | 处理歌词窗口速度变化 |


## 4. 构建与运行

### 4.1 依赖安装

- Qt 6 SDK
- CMake
- Git

### 4.2 编译步骤

```bash
# 克隆仓库
git clone <repository-url>
cd <repository-dir>

# 创建构建目录
mkdir build
cd build

# 生成构建系统
cmake ..

# 编译
cmake --build . --config Release

# 运行
./bin/XC.exe
```

### 4.3 项目结构

```
├── src/              # 源代码
├── resources/        # 资源文件
├── ui/               # UI设计文件
├── build/            # 编译输出目录
├── CMakeLists.txt    # CMake构建脚本
├── LICENSE           # 许可证文件
└── README.md         # 项目文档
```

## 5. 使用说明

### 5.1 基本功能

1. **添加音乐**：点击"添加"按钮选择音乐文件
2. **播放控制**：使用播放、暂停、停止、上一曲、下一曲按钮控制播放
3. **音量调节**：使用音量滑块调整音量
4. **播放列表管理**：
   - 创建新播放列表
   - 将歌曲添加到播放列表
   - 加载和保存播放列表
5. **收藏功能**：将喜欢的歌曲添加到收藏夹
6. **歌词显示**：播放带歌词的歌曲时自动显示歌词
7. **搜索功能**：搜索在线音乐资源

### 5.2 快捷键

| 快捷键 | 功能 |
|-------|-----|
| Space | 播放/暂停 |
| Ctrl + A | 添加音乐 |
| Ctrl + R | 移除选中音乐 |
| Ctrl + L | 清空播放列表 |
| Ctrl + F | 添加到收藏夹 |
| Ctrl + N | 创建新播放列表 |

## 6. 未来计划

1. 增加在线音乐播放功能
2. 实现音乐标签编辑功能
3. 添加均衡器设置
4. 支持更多音频格式
5. 增加皮肤切换功能
6. 实现音乐分享功能

## 7. 许可证

本项目采用 [MIT License](LICENSE) 许可证。