#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "lnk.h"
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
  std::tuple<QString, uint32_t, uint32_t> parseVolumeId(void *buffer);
  QString parseLocalBasePath(void *buffer);
  QString parseLocalBasePathUnicode(void *buffer);
  std::tuple<QString, uint8_t> parseRootFolderShellItem(void *buffer);
  std::tuple<QString, bool> parseVolumeShellItem(void *buffer);
  std::tuple<QString, QString, QString, bool>
  parseFileEntryShellItem(void *buffer);
  std::tuple<uint16_t, QString, QString>
  parseFileEntryExtensionBlock04(void *buffer);

  void updateTimeField(const ShellLinkHeader *header);
  void updateHotKeyField(const ShellLinkHeader *header);
  void updateFileSizeField(const ShellLinkHeader *header);
  void updateLinkFlagsField(const ShellLinkHeader *header);
  void updateFileAttributesField(const ShellLinkHeader *header);
  void updateShowTypeField(const ShellLinkHeader *header);

  static QString parseHotKey(std::tuple<std::string, std::string> hot_key);
  static QString parseShowWindow(uint16_t show_window);
  static void *movePointer(void *pointer, size_t offset);
  static QString pushPath(QString &path, bool is_file, bool is_localize);
};

#endif // MAINWINDOW_H
