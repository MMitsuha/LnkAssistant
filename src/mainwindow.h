#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLineEdit>
#include <QMainWindow>
#include <tuple>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

public slots:
  void updateParseTab();

private:
  Ui::MainWindow *ui;

  void onBrowseFile(QLineEdit *output);
  void updateAllField(void *buffer);
  uint16_t updateLinkTargetIdListField(void *buffer);
  uint32_t updateLinkInfoField(void *buffer);
  uint32_t updateStringFieldUtf16(void *buffer, QLineEdit *output);
  QString parseVolumeId(void *buffer);
  QString parseLocalBasePath(void *buffer);
  QString parseLocalBasePathUnicode(void *buffer);
  std::tuple<QString, QUuid> parseRootFolderShellItem(void *buffer);
  QString parseVolumeShellItem(void *buffer);
  std::tuple<QString, bool> parseFileEntryShellItem(void *buffer);
  std::tuple<uint16_t, QString> parseFileEntryExtensionBlock1(void *buffer);
  static std::tuple<QString, QString> parseHotKey(uint16_t hot_key);
  static QString parseShowWindow(uint16_t show_window);
  static void *movePointer(void *pointer, size_t offset);
};

#endif // MAINWINDOW_H
