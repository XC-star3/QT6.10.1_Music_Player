# Music - 简易音乐播放器
## 简介
在南邮程序设计实践任务需求下，基于Qt6开发的一款简易的音乐播放器，支持本地音乐播放、歌词显示、专辑封面展示、在线搜索以及播放列表管理等功能。

## 主要功能
### 1、音乐管理
* 轻松添加和移除本地音乐文件。
* 自动识别音乐文件的相关信息，如歌曲名、歌手等。
* 收藏夹和多歌单管理功能
### 2、播放控制
* 支持播放、暂停、停止操作。
* 歌曲进度显示
* 可自由调整播放进度，通过拖拽实现精准定位。
* 播放模式可选
- [ ]  单曲循环
- [ ]  顺序播放
- [ ]  随机播放
### 3、音频设置
* 调节音量大小，满足不同环境的需求。
* 支持倍速播放 

### 4、专辑封面与歌词
* 自动识别带有cover的歌曲的封面,并展示出来
* 歌词界面，支持歌词滚动播放
* 背景模糊效果和动画过渡

### 5、在线搜索
* 支持在线搜索歌曲

## 团队分工

### 开发者A（核心播放器功能）
* 负责MainWindow类的开发和维护
* 实现音乐播放、暂停、停止等基本控制功能
* 音乐列表管理（添加、移除、清空）
* 播放进度控制和播放模式设置
* 歌曲元数据获取与显示
* 系统音效设置集成

### 开发者B（歌词与UI展示）
* 负责lrcwidget类的开发和维护
* 实现歌词的加载、解析和滚动显示
* 专辑封面的显示和更新
* UI界面美化和交互体验优化
* 音量控制和播放速度调节功能
* 音乐播放动画效果实现

### 开发者C（在线功能与拓展）
* 负责searchwidget类的开发和维护
* 实现在线音乐搜索功能
* 网络API接口调用和数据解析
* 实现JSON数据处理
* 后续本地歌曲搜索功能开发
* 历史记录和收藏功能实现
* AI音效选择功能探索

## 项目结构与代码解析

### 整体架构
项目采用模块化设计，将不同功能分离到独立的类中，主要包含以下核心模块：
- 核心程序入口（main.cpp）
- 主窗口和播放器控制（MainWindow）
- 歌词显示和封面管理（lrcwidget）
- 播放列表管理（PlaylistInterface和playlist_manager）
- 在线搜索功能（searchwidget）

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
├── search/             # 在线搜索功能
│   ├── searchwidget.h  # 搜索窗口类头文件
│   └── searchwidget.cpp # 搜索窗口类实现
└── ui/                 # Qt设计文件
    ├── mainwindow.ui   # 主窗口UI设计
    └── lrcwidget.ui    # 歌词窗口UI设计
```

### 核心代码模块详解

#### 1. 核心程序入口 (src/core/main.cpp)
```cpp
int main(int argc, char *argv[])
```
- **作用**：程序入口点，初始化应用程序环境
- **主要流程**：
  - 创建QApplication应用程序实例
  - 创建并显示MainWindow主窗口
  - 启动应用程序事件循环，处理用户交互和系统事件
  - 包含预留的测试收藏夹功能函数

#### 2. 主窗口模块 (src/ui/mainwindow.h/cpp)

MainWindow是整个应用的核心类，负责管理播放器状态和用户界面交互。

##### 初始化与核心功能
```cpp
MainWindow::MainWindow(QWidget *parent = nullptr)
```
- **详细功能**：
  - 配置窗口属性（无边框、透明效果）
  - 初始化UI组件和布局
  - 创建并配置QMediaPlayer音频引擎
  - 初始化歌词显示控件和播放列表管理器
  - 建立信号与槽的连接网络，确保各组件协同工作
  - 加载默认音乐文件到播放列表

##### 播放控制功能
```cpp
void MainWindow::on_btnPlay_clicked()
```
- **功能详解**：处理播放按钮点击事件，检查播放状态并开始播放
  - 如处于暂停状态则继续播放
  - 如无播放内容则选择并播放列表中的第一首歌曲
  - 设置播放源并启动播放

```cpp
void MainWindow::on_btnPause_clicked()
void MainWindow::on_btnStop_clicked()
void MainWindow::on_btnPrevious_clicked()
void MainWindow::on_btnNext_clicked()
```
- **功能组**：提供完整的播放控制功能集
- **实现细节**：
  - 暂停功能：直接调用播放器的暂停方法
  - 停止功能：停止播放并重置循环状态
  - 上一曲/下一曲：实现播放列表的循环切换机制

##### 进度和时间管理
```cpp
void MainWindow::do_positionChanged(qint64 position)
```
- **核心任务**：同步更新播放进度显示
- **具体实现**：
  - 实时更新进度滑块位置
  - 计算并格式化当前播放时间
  - 同步更新歌词显示窗口的进度
  - 管理时间显示格式（分:秒）

```cpp
void MainWindow::do_durationChanged(qint64 duration)
```
- **功能**：处理媒体总时长变化事件
- **实现**：设置进度条最大值和更新总时长显示

##### 播放列表操作
```cpp
void MainWindow::on_btnAdd_clicked()
```
- **详细流程**：
  - 打开文件选择对话框，支持多选
  - 过滤音频文件类型（mp3、wav、wma等）
  - 创建列表项并设置文件图标
  - 保存完整文件路径作为用户数据
  - 自动开始播放（如当前无播放内容）

```cpp
void MainWindow::on_btnRemove_clicked()
void MainWindow::on_btnClear_clicked()
```
- **列表管理**：提供从播放列表中移除单首歌曲或清空整个列表的功能

##### 收藏夹和歌单管理
```cpp
void MainWindow::on_actionAdd_to_Favorites_triggered()
```
- **实现流程**：
  - 获取当前播放歌曲的文件路径
  - 检查歌曲是否已在收藏夹中
  - 通过PlaylistInterface添加歌曲到收藏夹
  - 显示操作结果提示

```cpp
void MainWindow::on_actionCreate_Playlist_triggered()
void MainWindow::on_actionAdd_to_Playlist_triggered()
void MainWindow::on_actionLoad_Playlist_triggered()
```
- **功能集合**：提供完整的歌单管理功能
  - 创建自定义歌单
  - 将歌曲添加到指定歌单
  - 加载和切换不同歌单

##### 网络搜索功能
```cpp
void MainWindow::on_btnSearch_clicked()
```
- **搜索实现**：
  - 构建网络请求URL
  - 设置请求头和参数
  - 发送异步网络请求

```cpp
void MainWindow::onSearchFinished(QNetworkReply *reply)
```
- **结果处理**：
  - 检查网络请求状态和错误
  - 解析JSON格式的搜索结果
  - 提取歌曲名称、歌手、时长等关键信息
  - 在搜索窗口中显示结果列表

#### 3. 歌词窗口模块 (src/lyrics/lrcwidget.h/cpp)

##### 歌词解析与显示
```cpp
QMap<QTime, QString> lrcwidget::parseLyrics(const QString &filePath)
```
- **解析机制**：
  - 使用正则表达式匹配LRC格式的时间标签
  - 支持多种时间格式（分:秒和分:秒:毫秒）
  - 构建时间-歌词的映射关系，用于同步显示
  - 处理文件不存在或格式错误的情况

```cpp
void lrcwidget::updateLyrics(qint64 position)
```
- **同步显示实现**：
  - 将播放位置转换为QTime对象
  - 使用二分查找定位当前应显示的歌词
  - 自动滚动到当前歌词行并高亮显示
  - 更新进度条位置

##### 动画与界面效果
```cpp
void lrcwidget::showLyric()
void lrcwidget::hideLyric()
```
- **动画效果**：
  - 使用QPropertyAnimation实现平滑的滑入滑出动画
  - 精确计算动画起始和结束位置
  - 动画完成后自动处理窗口显示/隐藏状态

```cpp
void lrcwidget::paintEvent(QPaintEvent *event)
```
- **视觉效果**：
  - 自定义绘制背景，实现封面模糊效果
  - 添加暗色遮罩增强文字可读性
  - 保持界面视觉一致性

#### 4. 播放列表管理 (src/playlist/playlist_interface.h/cpp 和 playlist_manager.h/c)

##### C++接口层
```cpp
class PlaylistInterface : public QObject
```
- **设计目的**：为C++代码提供友好的QObject接口，封装底层C实现
- **核心功能**：
  - 初始化和配置播放列表管理器
  - 提供收藏夹和歌单操作的高级API
  - 处理Qt字符串和C字符串的转换
  - 管理数据持久化

```cpp
bool PlaylistInterface::initialize(const QString &dataDir)
```
- **初始化流程**：
  - 创建数据存储目录
  - 初始化C语言实现的管理器
  - 加载已保存的歌单数据

##### C语言实现层
```cpp
typedef struct {
    Playlist *favorites;    // 收藏夹
    Playlist *playlists;    // 所有歌单列表
    int playlist_count;     // 歌单数量
    char *data_dir;         // 数据保存目录
} PlaylistManager;
```
- **数据结构**：使用链表实现的高效数据管理
- **内存管理**：严格的资源分配和释放机制

```cpp
PlaylistManager *playlist_manager_init(const char *data_directory)
```
- **底层初始化**：分配内存并初始化管理器核心组件

```cpp
bool save_playlists(PlaylistManager *manager)
bool load_playlists(PlaylistManager *manager)
```
- **持久化实现**：实现歌单数据的文件存储和加载功能

#### 5. 搜索窗口模块 (src/search/searchwidget.h/cpp)

```cpp
void searchwidget::displaySearchResults(const QStringList &results)
```
- **结果展示**：
  - 清空现有搜索结果
  - 将搜索结果解析并填充到表格中
  - 设置表格格式和交互属性

## 技术特点

1. **跨平台兼容性**：基于Qt6框架，确保在Windows、macOS和Linux等平台上的一致体验

2. **模块化设计**：清晰的类层次结构和责任划分

3. **双语言混合编程**：C++/Qt提供界面和高级功能，C语言提供底层数据管理

4. **异步网络处理**：使用Qt的网络模块实现非阻塞的在线搜索

5. **动画效果**：使用Qt的动画框架实现流畅的界面过渡效果

## 后续开发计划
- [ ] 搜索本地歌曲
- [ ] 新增AI音效选择功能
- [ ] 历史记录
- [ ] 播放列表管理增强
- [ ] 主题切换功能
- [ ] 均衡器调节

## 贡献与交流
如果您对本项目感兴趣，欢迎提出宝贵的意见和建议，或者直接参与到项目的开发中来。

## 声明
本项目仅供学习参考，遵循相关开源协议，如有侵权联系删除，欢迎在遵循协议的前提下自由使用和修改。
