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

## 代码框架（简洁版）

项目的代码结构很简单，先看“框架”，再看每个人负责的内容：

- UI 层（界面与交互）：`src/ui` — 负责窗口、按钮、播放控制、列表展示和与用户的所有互动（开发者A）。
- 数据层（歌单与持久化）：`src/playlist` — 负责把歌单保存在磁盘（CSV）、加载、增删歌单和收藏夹（开发者B）。
- 辅助层（歌词、搜索、入口）：`src/lyrics`, `src/search`, `src/core` — 负责歌词显示、搜索界面和程序入口（开发者C）。

下面是更通俗易懂的职责分配：
- 开发者A（UI）做的事：负责主窗口（打开文件、播放/暂停、切歌、显示封面和歌词窗口），把用户的操作转换成播放器的动作。
- 开发者B（数据）做的事：负责歌单和收藏夹的保存/读取，处理 CSV 文件和文件夹创建，确保数据不丢失。
- 开发者C（扩展）做的事：负责歌词窗口（解析 .lrc 并同步显示）、在线搜索结果展示，以及程序的入口点。

---

## 各函数的通俗说明（按文件）

下面把每个重要函数用日常语言来解释，避免过多实现细节，方便快速阅读和维护。

### src/core/main.cpp
- main：程序开始的地方，会创建应用窗口并显示它。

### src/ui/mainwindow.h / src/ui/mainwindow.cpp（主窗口，开发者A）
- 构造函数 MainWindow：把界面组件创建好，连接按钮和播放器，让程序准备好响应用户操作。
- 析构函数 ~MainWindow：程序退出时保存歌单并释放占用的资源。
- closeEvent：窗口被关闭时会触发，保存工作在这里处理。
- eventFilter：用来拦截一些特殊操作（比如按 Delete 删除列表项，或点击进度条直接跳到某处）。
- mousePress/Move/Release：实现无边框窗口拖动的交互。
- getSavedMusicPaths：读取应用目录下的 `sound` 文件夹，找出支持的音频文件。
- loadSavedMusic：把找到的音频文件加入播放列表展示给用户。
- do_positionChanged：播放位置改变时更新进度条和歌词高亮位置。
- do_durationChanged：当媒体时长更新时刷新显示的总时间。
- do_sourceChanged：换歌时更新显示的歌名、封面、并尝试加载同名歌词文件。
- do_playbackStateChanged：播放/暂停/停止状态改变后更新按钮状态，若开启循环会自动下一首。
- do_metaDataChanged：读取歌曲内置的封面图并展示。
- updateCoverArtSize：调整封面图片尺寸以适配显示区域。

- on_btnAdd_clicked：弹出对话框让用户选歌并添加到列表，若当前没有播放则开始播放添加的第一首。
- on_btnRemove_clicked：从列表移除当前选中歌曲。
- on_btnClear_clicked：清空列表并停止播放。
- on_btnPlay/ Pause/ Stop：播放、暂停、停止的按钮行为。
- on_btnPrevious/Next：切换上一首或下一首。
- on_doubleSpinBox_valueChanged：设置播放速度（倍速）。
- on_btnLoop_clicked：切换是否循环播放。
- on_btnSound_clicked：静音或恢复声音。
- on_sliderVolumn_valueChanged：调整音量。
- on_sliderPosition_valueChanged：用户拖动进度条时跳到对应时间。
- on_listWidget_doubleClicked：双击列表项开始播放该歌。
- showContextMenu：右键菜单，支持“添加到歌单/收藏”等快捷操作。
- on_btnCover_clicked：打开歌词窗口并传入封面图。
- on_btnSearch_clicked / onSearchFinished：在线搜索歌曲并把结果交给搜索面板显示。
- on_actionCreate_Playlist / on_actionAdd_to_Playlist / on_actionLoad_Playlist / on_actionAdd_to_Favorites / on_actionShow_Favorites：歌单和收藏夹的常用操作（新建、添加、加载、显示收藏）。

### src/search/searchwidget.h / src/search/searchwidget.cpp（搜索面板，开发者C）
- 构造函数 searchwidget：创建一个表格用于显示搜索结果（歌名、作者、时长）。
- displaySearchResults：把网络返回的结果填进表格，用户双击可触发播放或其它操作。

### src/lyrics/lrcwidget.h / src/lyrics/lrcwidget.cpp（歌词窗口，开发者C）
- 构造函数 lrcwidget：初始化歌词界面、默认封面和动画。
- loadLyrics：读取 .lrc 文件并把每句歌词放进列表，如果没有歌词显示友好提示。
- parseLyrics：把 LRC 格式的时间标签和文本提取出来，方便同步。
- updateLyrics：根据播放器的当前进度高亮并滚动到当前歌词行。
- setCoverImage / resetCoverImage：设置或恢复封面图。
- showLyric / hideLyric：用动画把歌词窗口从屏幕底部弹出或收回。
- clearLyrics：清空歌词显示内容。

### src/playlist/*（歌单与收藏，开发者B）
- PlaylistInterface（C++ 层）
  - initialize：初始化歌单系统（指定数据目录），准备读写文件。
  - addToFavorites / addCurrentSongToFavorites：把一首歌加入收藏夹。
  - getFavoritesSongs / getAllPlaylistNames / getPlaylistSongs：获取收藏夹或歌单里的歌曲信息列表，用于 UI 展示。
  - createPlaylist / deletePlaylist：创建或删除歌单。
  - addToPlaylist / removeFromPlaylist：对指定歌单增加或删除歌曲。
  - savePlaylists：把当前的歌单和收藏夹写入磁盘。

- playlist_manager.c（C 层实现）
  - create_song_info / free_song_info：创建和释放歌曲信息结构体。
  - add_to_favorites / remove_from_favorites / is_in_favorites：管理收藏夹的增删查。
  - create_playlist / delete_playlist / add_to_playlist / remove_from_playlist：管理用户歌单。
  - save_playlists / load_playlists：把歌单保存到 CSV 文件或从 CSV 加载回内存。

- playlist_util.c（工具函数）
  - parse_csv_line / escape_csv_field：CSV 读写的辅助函数。
  - ensure_directory / sanitize_filename：保证数据目录存在并生成安全的文件名。

---

如果你想要，我可以：
- 把这份 README 按模块拆分到 docs/ 目录下；
- 或把每个函数的示例调用和常见问题加入到 README 中；
- 或立即把更改提交到 git（新分支并创建 PR）。

请选择下一步（拆分为 docs / 添加示例 / 创建 git 提交 / 结束）：
        - bool initialize(const QString &dataDirectory) — 创建并初始化底层 PlaylistManager
        - bool addToFavorites(const QString &title, const QString &artist, const QString &album, const QString &filePath, const QString &coverPath, const QString &lrcPath, int duration) — 添加歌曲到收藏夹
        - QStringList getFavoritesSongs() — 返回收藏夹中歌曲的字符串列表（由底层转换）
        - bool removeFromFavorites(const QString &filePath)
        - bool isInFavorites(const QString &filePath)
        - bool savePlaylists() — 强制持久化当前歌单/收藏夹
        - bool addCurrentSongToFavorites(const QString &filePath, QMediaPlayer *player) — 从播放器上下文创建 SongInfo 并添加
        - QStringList getAllPlaylistNames()
        - bool createPlaylist(const QString &name)
        - bool deletePlaylist(const QString &name)
        - bool addToPlaylist(const QString &playlistName, const QString &title, const QString &artist, const QString &album, const QString &filePath, const QString &coverPath, const QString &lrcPath, int duration)
        - QStringList getPlaylistSongs(const QString &playlistName)
        - bool removeFromPlaylist(const QString &playlistName, const QString &filePath)
        - 私有辅助：ensureManagerInitialized(), songInfoToString(void *song), createSongInfo(...), getSongsFromPlaylist(void *playlist)

      - playlist_manager.h / playlist_manager.c（C 层核心实现）
        - 数据结构：SongInfo, PlaylistItem, Playlist, PlaylistManager
        - PlaylistManager *playlist_manager_init(const char *data_directory)
        - void playlist_manager_free(PlaylistManager *manager)
        - SongInfo *create_song_info(const char *title, const char *artist, const char *album, const char *file_path, const char *cover_path, const char *lrc_path, int duration)
        - void free_song_info(SongInfo *song)
        - bool add_to_favorites(PlaylistManager *manager, SongInfo *song)
        - bool remove_from_favorites(PlaylistManager *manager, const char *file_path)
        - bool is_in_favorites(PlaylistManager *manager, const char *file_path)
        - Playlist *get_favorites(PlaylistManager *manager)
        - Playlist *create_playlist(PlaylistManager *manager, const char *name)
        - bool delete_playlist(PlaylistManager *manager, const char *name)
        - Playlist *get_playlist(PlaylistManager *manager, const char *name)
        - bool add_to_playlist(Playlist *playlist, SongInfo *song)
        - bool remove_from_playlist(Playlist *playlist, const char *file_path)
        - bool save_playlists(PlaylistManager *manager)
        - bool load_playlists(PlaylistManager *manager)

      - playlist_util.h / playlist_util.c（文件/CSV/路径工具）
        - char *safe_strdup(const char *s)
        - bool build_playlist_filepath(char *buf, size_t bufSize, const char *data_dir, const char *name)
        - bool ensure_directory(const char *dir_path)
        - bool sanitize_filename(const char *name, char *out, size_t outSize)
        - int parse_csv_line(const char *line, char **outFields, int maxFields)
        - void free_parsed_fields(char **fields, int count)
        - char *escape_csv_field(const char *field)
        - PlaylistItem *create_playlist_item(SongInfo *song)
        - Playlist *create_playlist_structure(const char *name)
        - void append_item_to_playlist(Playlist *playlist, PlaylistItem *item)
        - void free_playlist_items(Playlist *playlist)
        - bool write_playlist_file(const char *filename, Playlist *playlist)
        - bool is_song_in_playlist(Playlist *playlist, const char *file_path)
        - bool remove_item_from_playlist(Playlist *playlist, PlaylistItem *item)

    - 备注：C 层负责低级内存管理、CSV 解析/序列化、文件系统交互（Windows API 调用），C++ 层封装为 Qt 风格的接口供 UI 调用。

  ### 开发者C（歌词与搜索，应用入口）
  - 负责文件：
    - `src/core/main.cpp`
    - `src/lyrics/lrcwidget.h`
    - `src/lyrics/lrcwidget.cpp`
    - `src/search/searchwidget.h`
    - `src/search/searchwidget.cpp`

    - 主要函数与职责（按文件列出）：
      - core/main.cpp
        - int main(int argc, char *argv[]) — 程序入口（创建 QApplication，显示主窗口）

      - lyrics/lrcwidget.h / lyrics/lrcwidget.cpp
        - lrcwidget(QWidget *parent = nullptr) 构造函数 —— 初始化 UI、封面与动画
        - ~lrcwidget() 析构
        - QSlider* getSlider() const
        - QPushButton* getPlayButton() const
        - QPushButton* getPrevButton() const
        - QPushButton* getNextButton() const
        - QSlider* getSoundSlider() const
        - QDoubleSpinBox* getSpeedSpinBox() const
        - QPushButton* getModeButton() const
        - void loadLyrics(const QString &filePath) — 加载并解析 LRC 歌词，填充列表
        - QMap<QTime, QString> parseLyrics(const QString &filePath) — 解析 LRC 行为时间->文本映射
        - void updateLyrics(qint64 position) — 根据播放位置滚动并高亮当前歌词
        - void setCoverImage(const QPixmap &pixmap) — 设置并缩放封面
        - void resetCoverImage() — 恢复默认封面
        - void updateListHeight() — 计算并调整歌词列表高度
        - void showLyric() / void hideLyric() — 使用 QPropertyAnimation 做上下滑动动画展示/隐藏歌词窗口
        - void clearLyrics() — 清空歌词数据结构与 UI
        - void paintEvent(QPaintEvent *event) — 自定义绘制背景（封面模糊 + 遮罩）
        - QImage applyBlurToImage(QImage sourceImage, int radius) — 利用 QGraphicsBlurEffect 对图片模糊并返回新的 QImage
        - 插槽：on_horizontalSlider_sliderMoved/Pressed/Released、on_btnLrcClose_clicked

      - search/searchwidget.h / search/searchwidget.cpp
        - searchwidget(QWidget *parent = nullptr) 构造函数 — 构建表格并连接双击信号
        - void displaySearchResults(const QStringList &results) — 将解析后的搜索结果填入 QTableWidget
        - 信号：void songDoubleClicked(const QString &songName)

    - 备注：`lrcwidget::parseLyrics` 使用正则提取 [mm:ss:xx] 标签，`updateLyrics` 使用 QMap 上界/下界查找最近时间点并滚动列表以高亮。

  ---

  ## 按文件的函数清单（逐文件、逐函数 — 摘要说明）

  为便于阅读，下面按目录列出每个源码文件中公开/重要函数（签名与一句话说明）。这部分可直接用于开发者查阅或代码审计。

  ### src/core/main.cpp
  - int main(int argc, char *argv[]) — 应用入口；创建 QApplication 并显示 MainWindow。

  ### src/ui/mainwindow.h / src/ui/mainwindow.cpp
  - MainWindow(QWidget *parent = nullptr) — 构造，设置 UI、连接信号槽、初始化播放器/歌词/歌单接口。
  - ~MainWindow() — 析构并保存歌单。
  - void closeEvent(QCloseEvent *event) — 关闭时确保调用 m_playlistInterface->savePlaylists()。
  - bool eventFilter(QObject *watched, QEvent *event) — 处理键盘删除、滑块点击/拖拽行为。
  - void mousePressEvent(QMouseEvent *event) / mouseMoveEvent / mouseReleaseEvent — 无边框窗口拖拽实现。
  - QStringList getSavedMusicPaths() — 扫描应用目录下 `sound` 子目录并返回支持的音频文件路径列表。
  - void loadSavedMusic() — 把扫描结果添加到 `ui->listWidget`，设置图标与 UserRole 数据。
  - void do_positionChanged(qint64 position) — 更新滑块位置、时间标签，并通知 lrcWidget 更新歌词进度。
  - void do_durationChanged(qint64 duration) — 设置 slider 最大值并更新总时长显示。
  - void do_sourceChanged(const QUrl &media) — 更新当前媒体名、重置封面并尝试加载同名 .lrc 文件。
  - void do_playbackStateChanged(QMediaPlayer::PlaybackState newState) — 根据播放状态更新按钮可用性并在循环时自动下一曲。
  - void do_metaDataChanged() — 读取 QMediaMetaData 缩略图并更新封面按钮/显示。
  - void updateCoverArtSize() — 根据 scrollArea 大小缩放封面显示。
  - void on_btnAdd_clicked() — 弹出文件选择对话框并添加文件到列表；如无播放则开始播放第一个。
  - void on_btnRemove_clicked() — 删除当前选中项并在列表空时关闭循环标志。
  - void on_btnClear_clicked() — 清空列表并停止播放器。
  - void on_btnPlay_clicked() / on_btnPause_clicked() / on_btnStop_clicked() — 控制播放/暂停/停止。
  - void on_btnPrevious_clicked() / on_btnNext_clicked() — 切换上一首/下一首并开始播放。
  - void on_doubleSpinBox_valueChanged(double arg1) — 设置播放速率（暂停时修改以稳定体验）。
  - void on_btnLoop_clicked(bool checked) — 切换循环模式。
  - void on_btnSound_clicked() — 切换静音并更新图标。
  - void on_sliderVolumn_valueChanged(int value) — 调整 QAudioOutput 音量。
  - void on_sliderPosition_valueChanged(int value) — 跳转播放位置。
  - void on_listWidget_doubleClicked(const QModelIndex &index) — 双击项播放对应媒体。
  - void showContextMenu(const QPoint &pos) — 在播放列表项上显示“添加到歌单/收藏”菜单。
  - void on_btnCover_clicked() — 打开 lrcwidget，设置封面并显示歌词窗口。
  - void on_btnSearch_clicked() — 向在线 API 发起搜索请求（构造 QNetworkRequest）。
  - void onSearchFinished(QNetworkReply *reply) — 解析 JSON 响应并把结果交给 searchWidget 展示。
  - void on_pushButton_clicked() — 关闭搜索界面（UI 逻辑）。
  - void on_actionAdd_to_Favorites_triggered() / on_actionShow_Favorites_triggered() / on_actionCreate_Playlist_triggered() / on_actionAdd_to_Playlist_triggered() / on_actionLoad_Playlist_triggered() — 歌单和收藏操作的 UI 层实现（调用 PlaylistInterface 提供的 API）。

  ### src/search/searchwidget.h / src/search/searchwidget.cpp
  - searchwidget(QWidget *parent = nullptr) — 创建表格、设置列头、连接 cellDoubleClicked 到信号。
  - void displaySearchResults(const QStringList &results) — 将 "歌曲名,作者,时长" 格式的字符串切分并填充到 QTableWidget 对应单元格。
  - signal: void songDoubleClicked(const QString &songName) — 双击歌曲时发出。

  ### src/lyrics/lrcwidget.h / src/lyrics/lrcwidget.cpp
  - lrcwidget(QWidget *parent = nullptr) — 初始化 UI、封面、动画、stackedWidget 和无歌词提示。
  - ~lrcwidget() — 释放 UI 对象。
  - QSlider* getSlider() const — 返回歌词进度滑块，用于主窗口连接。
  - QPushButton* getPlayButton() const / getPrevButton / getNextButton / getModeButton — 返回对应控件指针。
  - QSlider* getSoundSlider() const / QDoubleSpinBox* getSpeedSpinBox() const — 返回控件以便主窗口连接。
  - void loadLyrics(const QString &filePath) — 调用 parseLyrics 并把解析结果填入 QListWidget；无歌词时显示提示页。
  - QMap<QTime, QString> parseLyrics(const QString &filePath) — 使用正则 (\[(\d{2}):(\d{2})(?::(\d{2}))?\](.*)) 解析 LRC 行并返回映射。
  - void updateLyrics(qint64 position) — 基于 position 在 lyricsMap 中定位最近的时间点，高亮并滚动到对应行；同时设置滑块值。
  - void setCoverImage(const QPixmap &pixmap) — 更新封面并触发重绘。
  - void resetCoverImage() — 恢复默认封面图像。
  - void updateListHeight() — 根据行高和显示行数调整 QListWidget 固定高度。
  - void showLyric() / hideLyric() — 使用 QPropertyAnimation 做上下滑动动画显示/隐藏。
  - void clearLyrics() — 清空 UI 歌词项和 lyricsMap。
  - void paintEvent(QPaintEvent *event) — 将封面模糊后绘制为背景并覆盖半透明遮罩。
  - QImage applyBlurToImage(QImage sourceImage, int radius) — 使用 QGraphicsBlurEffect 在离屏 QGraphicsScene 上渲染并返回模糊图像。
  - slots: on_horizontalSlider_sliderMoved(int), on_horizontalSlider_sliderPressed(), on_horizontalSlider_sliderReleased(), on_btnLrcClose_clicked()

  ### src/playlist/playlist_interface.h / .cpp
  - PlaylistInterface(QObject *parent = nullptr) / ~PlaylistInterface() — 管理 C 层 PlaylistManager 指针生命周期。
  - bool initialize(const QString &dataDirectory) — 调用 playlist_manager_init 并保存指针。
  - bool addToFavorites(...) — 创建 SongInfo 并调用 add_to_favorites
  - QStringList getFavoritesSongs() — 从 get_favorites 提取并转换为 QStringList
  - bool removeFromFavorites(const QString &filePath)
  - bool isInFavorites(const QString &filePath)
  - bool savePlaylists()
  - bool addCurrentSongToFavorites(const QString &filePath, QMediaPlayer *player)
  - QStringList getAllPlaylistNames()
  - bool createPlaylist(const QString &name)
  - bool deletePlaylist(const QString &name)
  - bool addToPlaylist(...)
  - QStringList getPlaylistSongs(const QString &playlistName)
  - bool removeFromPlaylist(const QString &playlistName, const QString &filePath)

  ### src/playlist/playlist_manager.h / .c
  - PlaylistManager *playlist_manager_init(const char *data_directory)
  - void playlist_manager_free(PlaylistManager *manager)
  - SongInfo *create_song_info(const char *title, const char *artist, const char *album, const char *file_path, const char *cover_path, const char *lrc_path, int duration)
  - void free_song_info(SongInfo *song)
  - bool add_to_favorites(PlaylistManager *manager, SongInfo *song)
  - bool remove_from_favorites(PlaylistManager *manager, const char *file_path)
  - bool is_in_favorites(PlaylistManager *manager, const char *file_path)
  - Playlist *get_favorites(PlaylistManager *manager)
  - Playlist *create_playlist(PlaylistManager *manager, const char *name)
  - bool delete_playlist(PlaylistManager *manager, const char *name)
  - Playlist *get_playlist(PlaylistManager *manager, const char *name)
  - bool add_to_playlist(Playlist *playlist, SongInfo *song)
  - bool remove_from_playlist(Playlist *playlist, const char *file_path)
  - bool save_playlists(PlaylistManager *manager)
  - bool load_playlists(PlaylistManager *manager)

  ### src/playlist/playlist_util.h / .c
  - char *safe_strdup(const char *s)
  - bool build_playlist_filepath(char *buf, size_t bufSize, const char *data_dir, const char *name)
  - bool ensure_directory(const char *dir_path)
  - bool sanitize_filename(const char *name, char *out, size_t outSize)
  - int parse_csv_line(const char *line, char **outFields, int maxFields)
  - void free_parsed_fields(char **fields, int count)
  - char *escape_csv_field(const char *field)
  - PlaylistItem *create_playlist_item(SongInfo *song)
  - Playlist *create_playlist_structure(const char *name)
  - void append_item_to_playlist(Playlist *playlist, PlaylistItem *item)
  - void free_playlist_items(Playlist *playlist)
  - bool write_playlist_file(const char *filename, Playlist *playlist)
  - bool is_song_in_playlist(Playlist *playlist, const char *file_path)
  - bool remove_item_from_playlist(Playlist *playlist, PlaylistItem *item)

  ---

  ## 代码框架与每人负责的部分

  整体架构概览（高阶）
  - 应用层（UI / 事件处理）：`src/ui`，负责与用户交互、信号槽绑定、视图更新与播放器控制。由开发者A 负责。
  - 服务层（播放列表接口）：`src/playlist`，C++/C 混合实现，负责持久化、CSV 解析、歌单与收藏夹逻辑。由开发者B 负责。
  - 辅助与扩展层（歌词、搜索、入口）：`src/lyrics`、`src/search`、`src/core`，负责歌词解析/展示、在线搜索与程序入口。由开发者C 负责。

  模块接口契约（简洁版）
  - MainWindow (UI)：输入 = 用户事件 / 网络与播放器回调；输出 = 更新 UI、调用 PlaylistInterface；错误模式 = 无效文件、网络失败（通过 QMessageBox 提示）。
  - PlaylistInterface / playlist_manager：输入 = 字符串路径 / 歌曲元数据；输出 = 成功/失败布尔、QStringList；错误模式 = 文件系统错误、CSV 解析错误（函数返回 false）。
  - lrcwidget：输入 = 播放位置（毫秒）、LRC 文件路径；输出 = 高亮当前行、发出 sliderMoved/Pressed/Released 信号。

  开发者职责（框架级别）
  - 开发者A：负责 UI 的所有窗口与交互逻辑（`MainWindow`），包括界面初始化、事件过滤、与 `lrcwidget`、`PlaylistInterface` 的连接。
  - 开发者B：负责歌单数据模型与持久化（`playlist_manager.c/h`、`playlist_util.*`、`playlist_interface.*`），确保文件 I/O 与内存安全。
  - 开发者C：负责歌词呈现、搜索界面与程序入口（`lrcwidget`、`searchwidget`、`main.cpp`），实现视觉效果和网络展示层。

  ---

  ## 每个函数的详细说明（输入/输出/副作用/错误模式）

  提示：下面逐文件列出关键函数；每条包含：签名、功能简介、主要输入、返回值/输出、副作用与常见错误模式。

  ### src/core/main.cpp
  - int main(int argc, char *argv[])
    - 功能：应用程序入口，初始化 Qt 环境并展示主窗口。
    - 输入：命令行参数。
    - 返回：应用退出码（a.exec() 返回值）。
    - 副作用：创建 QApplication、MainWindow 实例并进入事件循环。

  ### src/ui/mainwindow.h / src/ui/mainwindow.cpp
  - MainWindow(QWidget *parent = nullptr)
    - 功能：构造主窗口，初始化 UI、播放器、歌词窗口、网络管理器与歌单接口，并建立信号连接。
    - 输入：父窗口指针。
    - 返回：构造后对象可立即显示并响应用户操作。
    - 副作用：分配 QMediaPlayer、PlaylistInterface、searchwidget、lrcwidget 等对象；可能在初始化 `m_playlistInterface->initialize()` 时访问文件系统（data/playlists）。

  - ~MainWindow()
    - 功能：析构函数，保存歌单并释放资源。
    - 副作用：调用 m_playlistInterface->savePlaylists() 并 delete 已分配对象。

  - void closeEvent(QCloseEvent *event)
    - 功能：窗口关闭时执行的清理逻辑。
    - 输入：关闭事件。
    - 副作用：保存歌单；调用基类 closeEvent。

  - bool eventFilter(QObject *watched, QEvent *event)
    - 功能：拦截并特殊处理某些事件（Delete 键删除列表项、slider 点击/拖拽实现定位）。
    - 输入：被监视的对象与事件。
    - 返回：处理后返回 true（停止传播）或 false（继续默认处理）。
    - 副作用：修改 UI（删除 item、设置 slider 值、调用 player->setPosition）。

  - void mousePressEvent(QMouseEvent *event) / mouseMoveEvent / mouseReleaseEvent
    - 功能：实现无边框窗口的拖拽逻辑。
    - 输入：鼠标事件。
    - 副作用：调用 move() 改变窗口位置。

  - QStringList getSavedMusicPaths()
    - 功能：扫描应用目录下的 `sound` 文件夹，返回支持的音频文件路径列表（通过 QDir 过滤扩展名）。
    - 返回：绝对文件路径字符串列表（QUrl 用于 UI 存储）。
    - 错误模式：若目录不存在返回空列表。

  - void loadSavedMusic()
    - 功能：把 `getSavedMusicPaths()` 的结果转换成 QListWidgetItem 并添加到 UI 列表。
    - 副作用：修改 UI，设置图标并把本地文件 URL 存入 item 的 Qt::UserRole。

  - void do_positionChanged(qint64 position)
    - 功能：播放器位置更新回调，更新进度显示、歌词进度与时间标签。
    - 输入：position（毫秒）。
    - 副作用：设置 ui->sliderPosition、更新 lrcWidget 滑块和时间文本。

  - void do_durationChanged(qint64 duration)
    - 功能：媒体时长变化回调，设置滑块范围并更新总时长显示。
    - 输入：duration（毫秒）。

  - void do_sourceChanged(const QUrl &media)
    - 功能：媒体源变更处理，更新当前媒体名、重置封面并尝试加载同路径下同名 `.lrc` 歌词文件。
    - 副作用：lrcWidget->loadLyrics() 被调用（若存在 .lrc）。

  - void do_playbackStateChanged(QMediaPlayer::PlaybackState newState)
    - 功能：播放状态变更处理，更新播放/暂停/停止按钮状态并支持循环播放逻辑（到末尾自动下一曲）。
    - 输入：播放状态枚举。
    - 副作用：可能调用 player->setSource() 与 player->play() 以自动切歌。

  - void do_metaDataChanged()
    - 功能：当媒体元数据变更时读取缩略图并更新 UI 封面显示；若无缩略图则使用默认封面。
    - 副作用：调用 updateCoverArtSize() 并设置按钮 icon。

  - void updateCoverArtSize()
    - 功能：根据 scrollArea 可见大小缩放 labPic 的 pixmap 保持纵横比。
    - 副作用：更新 UI pixmap，避免拉伸变形。

  - void showContextMenu(const QPoint &pos)
    - 功能：在播放列表的某一项上弹出上下文菜单（添加到歌单/收藏），并连接相应槽。

  - UI 控件槽（on_ 前缀系列）
    - on_btnAdd_clicked()
      - 功能：弹出文件选择对话框并把选中文件加入播放列表，第一首文件将自动开始播放（如果当前未在播放）。
      - 副作用：修改 ui->listWidget 并可能调用 player->setSource()/play()

    - on_btnRemove_clicked()
      - 功能：删除当前列表项；若列表为空则关闭循环标志（loopPay=false）。

    - on_btnClear_clicked()
      - 功能：清空播放列表并停止播放器（调用 player->stop()）。

    - on_btnPlay_clicked() / on_btnPause_clicked() / on_btnStop_clicked()
      - 功能：播放/暂停/停止控制。
      - 错误模式：当无媒体时不会触发播放。

    - on_btnPrevious_clicked() / on_btnNext_clicked()
      - 功能：切换曲目指针并开始播放新曲目；短期会临时关闭循环标志以避免重复逻辑。

    - on_doubleSpinBox_valueChanged(double arg1)
      - 功能：设置播放速率（若正在播放则短暂停止以平滑切换）。

    - on_btnLoop_clicked(bool checked)
      - 功能：切换循环播放标志，影响 do_playbackStateChanged 的自动下一曲行为。

    - on_btnSound_clicked()
      - 功能：切换 mute 状态并替换按钮图标以反映当前静音状态。

    - on_sliderVolumn_valueChanged(int value)
      - 功能：把滑动条的值映射为 0.0-1.0 之间并设置 QAudioOutput::setVolume。

    - on_sliderPosition_valueChanged(int value)
      - 功能：在用户操作滑块时跳转播放位置（player->setPosition）。

    - on_listWidget_doubleClicked(const QModelIndex &index)
      - 功能：双击播放列表项，设置当前媒体并播放。

    - on_btnCover_clicked()
      - 功能：准备并显示歌词窗口（lrcwidget），将当前封面传入并调用 showLyric()。

    - on_btnSearch_clicked()
      - 功能：构造在线搜索的 URL 并通过 QNetworkAccessManager 发起 GET 请求。

    - onSearchFinished(QNetworkReply *reply)
      - 功能：解析 Kugou 风格的 JSON 响应（示例 URL），将解析出的 "歌曲,作者,时长" 字符串列表传给 searchWidget 展示。
      - 错误模式：网络错误或非 JSON 响应将被忽略并在 reply->error() 不为 NoError 时跳过解析。

    - on_actionAdd_to_Favorites_triggered()
      - 功能：把当前选中项或当前播放媒体加入收藏（通过 m_playlistInterface->addCurrentSongToFavorites 或 addToFavorites）。

    - on_actionShow_Favorites_triggered()
      - 功能：从 m_playlistInterface 获取 favorites 歌单并将其条目填充到 ui->listWidget。

    - on_actionCreate_Playlist_triggered()
      - 功能：弹出输入对话框获取歌单名称并调用 m_playlistInterface->createPlaylist。

    - on_actionAdd_to_Playlist_triggered()
      - 功能：询问用户选择目标歌单并将当前歌曲添加到该歌单（调用 addToPlaylist）。

    - on_actionLoad_Playlist_triggered()
      - 功能：从已存在歌单中选择并加载其条目到 ui->listWidget，若有条目则自动播放第一首。

  ### src/search/searchwidget.h / src/search/searchwidget.cpp
  - searchwidget(QWidget *parent = nullptr)
    - 功能：构造搜索结果表格（3 列：歌曲名、作者、时长），并连接 cellDoubleClicked 以发射 songDoubleClicked。
    - 副作用：初始化 QTableWidget 布局。

  - void displaySearchResults(const QStringList &results)
    - 功能：把传入的以逗号分隔的条目拆分为列并填充表格。
    - 输入：QStringList，每项格式为 "歌曲名,作者,时长"。
    - 错误模式：若某项列数少于 3，则只填充可用列。

  ### src/lyrics/lrcwidget.h / src/lyrics/lrcwidget.cpp
  - lrcwidget(QWidget *parent = nullptr)
    - 功能：初始化歌词窗口 UI、默认封面、动画对象与无歌词提示。

  - ~lrcwidget()
    - 功能：释放 UI 对象，断开信号连接（Qt 自动处理大部分 QObject 释放）。

  - QSlider* getSlider() const / QPushButton* getPlayButton() const / getPrevButton() / getNextButton() / getModeButton()
    - 功能：暴露内部控件指针供外部（MainWindow）连接信号槽。

  - QSlider* getSoundSlider() const / QDoubleSpinBox* getSpeedSpinBox() const
    - 功能：暴露声音与速率控件以便外部调整播放器设置。

  - void loadLyrics(const QString &filePath)
    - 功能：调用 parseLyrics 并根据结果填充 QListWidget；若无歌词则显示 noLyricsLabel。
    - 副作用：修改 ui->lrc_list、lyricsMap 与 stackedWidget 的显示状态。

  - QMap<QTime, QString> parseLyrics(const QString &filePath)
    - 功能：读取文本文件，使用正则表达式提取时间标签和文本，返回按时间排序的映射。
    - 输入：LRC 文件路径。
    - 返回：QMap，其中键为 QTime（mm:ss:xx），值为对应歌词字符串。
    - 错误模式：无法打开文件返回空映射；格式不匹配的行被忽略。

  - void updateLyrics(qint64 position)
    - 功能：根据 position（毫秒）定位 lyricsMap 中最近不晚于当前时间的条目，设置列表选择并滚动到中心位置。
    - 副作用：设置 ui->horizontalSlider 值与 ui->lrc_list 当前行。复杂度为 O(log n)（QMap 上届/下届查找）。

  - void setCoverImage(const QPixmap &pixmap) / void resetCoverImage()
    - 功能：设置或恢复封面图像并触发重绘。

  - void updateListHeight()
    - 功能：根据单行高度与预定显示行数调整 lrc_list 的高度，保持视觉一致。

  - void showLyric() / void hideLyric()
    - 功能：使用 QPropertyAnimation 在父窗口底部上下移动显示或隐藏歌词窗口；在 hide 完成后调用 QWidget::hide。
    - 错误模式：若 parentWidget 为 nullptr 则动画位置计算会失败（代码中已有调试输出）。

  - void clearLyrics()
    - 功能：清空 UI 列表与 lyricsMap，释放内存。

  - void paintEvent(QPaintEvent *event)
    - 功能：把 coverPixmap 模糊并绘制为背景，然后覆盖半透明遮罩以提高歌词可读性。
    - 副作用：在重绘期间可能较耗时（applyBlurToImage），但该实现使用 QGraphicsScene 离屏渲染以返回 QImage。

  - QImage applyBlurToImage(QImage sourceImage, int radius)
    - 功能：创建临时 QGraphicsScene，将带有 QGraphicsBlurEffect 的 QGraphicsPixmapItem 渲染为模糊图像并返回。
    - 错误模式：若渲染失败返回空或原图。

  ### src/playlist/playlist_interface.h / .cpp
  - PlaylistInterface::PlaylistInterface(QObject *parent = nullptr) / ~PlaylistInterface()
    - 功能：封装底层 C 实现（PlaylistManager），管理其生命周期。

  - bool initialize(const QString &dataDirectory)
    - 功能：初始化底层 manager（playlist_manager_init），并为后续操作准备数据目录。
    - 副作用：可能创建 dataDirectory 及读取现有 CSV 文件（load_playlists）。

  - bool addToFavorites(...)
    - 功能：把提供的元数据封装为 SongInfo 并调用 add_to_favorites。
    - 返回：是否成功（存在重复或写失败返回 false）。

  - QStringList getFavoritesSongs()
    - 功能：把底层 favorites 列表转换为 QStringList，每项为 "title|artist|album|file_path|duration"。

  - bool removeFromFavorites(const QString &filePath)
    - 功能：从 favorites 中移除指定路径的歌曲并持久化更改。

  - bool isInFavorites(const QString &filePath)
    - 功能：检查文件路径是否在 favorites 中存在（使用底层 is_in_favorites）。

  - bool savePlaylists()
    - 功能：调用底层 save_playlists 持久化所有歌单到 CSV 文件。

  - bool addCurrentSongToFavorites(const QString &filePath, QMediaPlayer *player)
    - 功能：基于播放器当前状态从文件名或元数据创建 SongInfo 并添加到收藏。

  - QStringList getAllPlaylistNames()
    - 功能：返回 manager->playlists 名称数组（从 C 层分配并由接口释放）。

  - bool createPlaylist(const QString &name) / bool deletePlaylist(const QString &name)
    - 功能：创建或删除用户歌单，调用底层并触发 save_playlists 以持久化。

  - bool addToPlaylist(...) / QStringList getPlaylistSongs(...) / bool removeFromPlaylist(...)
    - 功能：向指定歌单添加/查询/删除歌曲，返回操作结果并在必要时保存更改。

  ### src/playlist/playlist_manager.h / .c
  - PlaylistManager *playlist_manager_init(const char *data_directory)
    - 功能：分配并初始化 PlaylistManager，创建 favorites 结构并调用 load_playlists。
    - 错误模式：若目录无法创建或文件读取失败则返回 NULL。

  - void playlist_manager_free(PlaylistManager *manager)
    - 功能：释放所有歌单与歌曲内存并释放 manager 本身。

  - SongInfo *create_song_info(...)
    - 功能：基于 C 字符串创建并返回 SongInfo（为每个字符串分配副本）。

  - void free_song_info(SongInfo *song)
    - 功能：释放 SongInfo 内部分配的字符串与结构体。

  - bool add_to_favorites(PlaylistManager *manager, SongInfo *song)
    - 功能：把 SongInfo 包装成 PlaylistItem 并 append 到 favorites 链表，随后保存到文件。

  - bool remove_from_favorites(PlaylistManager *manager, const char *file_path)
    - 功能：在 favorites 链表中查找并移除匹配 file_path 的条目，释放其内存并保存。

  - bool is_in_favorites(PlaylistManager *manager, const char *file_path)
    - 功能：线性查找 favorites 链表以判断是否存在。

  - Playlist *create_playlist(PlaylistManager *manager, const char *name)
    - 功能：创建 Playlist 结构并插入 manager->playlists 链表；若已存在返回已存在对象。

  - bool delete_playlist(PlaylistManager *manager, const char *name)
    - 功能：在 manager->playlists 中查找并移除指定歌单，删除磁盘 csv 文件并释放内存。

  - bool add_to_playlist(Playlist *playlist, SongInfo *song)
    - 功能：为歌单创建 PlaylistItem 并 append；若文件路径重复则返回 false。

  - bool remove_from_playlist(Playlist *playlist, const char *file_path)
    - 功能：查找并通过 remove_item_from_playlist 删除条目并释放资源。

  - bool save_playlists(PlaylistManager *manager)
    - 功能：为 favorites 和每个用户歌单写入 CSV（使用 write_playlist_file）。

  - bool load_playlists(PlaylistManager *manager)
    - 功能：从 data_dir 读取 favorites.csv 和其他 .csv 文件，解析为 SongInfo 并恢复内存结构。
    - 错误模式：CSV 解析错误会跳过该行，无法打开文件则跳过该歌单。

  ### src/playlist/playlist_util.h / .c
  - char *safe_strdup(const char *s)
    - 功能：封装 strdup，并在 NULL 输入时返回 NULL。

  - bool build_playlist_filepath(char *buf, size_t bufSize, const char *data_dir, const char *name)
    - 功能：基于 data_dir 与 name 生成文件路径（替换非法字符，追加 .csv）。

  - bool ensure_directory(const char *dir_path)
    - 功能：递归创建目录（Windows API），处理驱动器和 UNC 路径。

  - bool sanitize_filename(const char *name, char *out, size_t outSize)
    - 功能：把任意用户输入转换为文件系统安全的文件名（替换非法字符为 '_'）。

  - int parse_csv_line(const char *line, char **outFields, int maxFields)
    - 功能：解析分号分隔 CSV 字段，支持双引号引用与转义；为每个字段分配缓冲区（调用者需 free）。

  - void free_parsed_fields(char **fields, int count)
    - 功能：释放 parse_csv_line 分配的字段字符串。

  - char *escape_csv_field(const char *field)
    - 功能：为输出 CSV 转义字段，包含必要时添加双引号并转义内部双引号。

  - PlaylistItem *create_playlist_item(SongInfo *song)
    - 功能：为 SongInfo 分配 PlaylistItem 节点并初始化指针。

  - Playlist *create_playlist_structure(const char *name)
    - 功能：分配 Playlist 结构并复制名字字符串。

  - void append_item_to_playlist(Playlist *playlist, PlaylistItem *item)
    - 功能：把节点追加到歌单双向链表尾部并维护计数器。

  - void free_playlist_items(Playlist *playlist)
    - 功能：遍历释放歌单中所有 PlaylistItem 与其 SongInfo。

  - bool write_playlist_file(const char *filename, Playlist *playlist)
    - 功能：以 CSV 形式把歌单写入文件，逐条输出经过 escape_csv_field 转义的字段，并写入持续时长整数。

  - bool is_song_in_playlist(Playlist *playlist, const char *file_path)
    - 功能：线性遍历歌单判断 file_path 是否已存在（避免重复添加）。

  - bool remove_item_from_playlist(Playlist *playlist, PlaylistItem *item)
    - 功能：维护链表指针并释放指定节点与其 SongInfo。

  ---

  许可证：MIT（详见 LICENSE 文件）
