#include "mainwindow.h"
#include "lnk.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QString>
#include <QUuid>
#include <bitset>
#include <ranges>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  connect(ui->browse, &QPushButton::clicked, this,
          [&] { this->onBrowseFile(ui->selectFileToParse); });
  connect(ui->selectFileToParse, &QLineEdit::textChanged, this,
          &MainWindow::updateParseTab);
}

void MainWindow::updateParseTab() {
  auto path = ui->selectFileToParse->text();
  auto file = QFile(path, this);

  if (file.open(QFile::ReadOnly) == false) {
    QMessageBox::critical(this, tr("Error"), tr("Can not open lnk file."));
    return;
  }

  auto buffer = file.readAll();

  updateAllField(buffer.data());
}

void MainWindow::onBrowseFile(QLineEdit *output) {
  auto path = QFileDialog::getOpenFileName(this, QString(), QString(),
                                           tr("Lnk Files (*.lnk)"));

  output->setText(path);
}

void MainWindow::updateAllField(void *buffer) {
  auto header = (ShellLinkHeader *)buffer;

  if (header->size != 76) {
    QMessageBox::critical(this, tr("Error"), tr("Not a valid lnk file."));
    return;
  }

  // TODO: Add class identifier validation

  const uint64_t UNIX_EPOCH_IN_FILETIME = 116444736000000000ULL;
  auto creation_date = QDateTime::fromSecsSinceEpoch(
      (header->creation_time - UNIX_EPOCH_IN_FILETIME) / 10000000,
      QTimeZone::UTC);
  auto access_date = QDateTime::fromSecsSinceEpoch(
      (header->access_time - UNIX_EPOCH_IN_FILETIME) / 10000000,
      QTimeZone::UTC);
  auto modification_date = QDateTime::fromSecsSinceEpoch(
      (header->modification_time - UNIX_EPOCH_IN_FILETIME) / 10000000,
      QTimeZone::UTC);
  auto target_file_size = (double)header->file_size / 1024;
  auto icon_index = header->icon_index;
  auto show_window = parseShowWindow(header->show_window);
  auto [hot_key_1, hot_key_2] = this->parseHotKey(header->hot_key);
  auto link_flag = std::bitset<32>(header->flags);
  auto file_attributes = std::bitset<32>(header->file_attributes);

  ui->creationDate->setText(creation_date.toString());
  ui->accessDate->setText(access_date.toString());
  ui->modificationDate->setText(modification_date.toString());
  ui->targetFileSize->setText(QString::number(target_file_size).append(" KB"));
  ui->iconIndex->setText(QString::number(icon_index));
  ui->showType->setText(show_window);
  ui->hotKey->setText(tr("%2 + %1").arg(hot_key_1).arg(hot_key_2));
  for (auto i = 0; i <= 26; i++) {
    ui->linkFlags->item(i)->setCheckState(link_flag.test(i) ? Qt::Checked
                                                            : Qt::Unchecked);
  }
  for (auto i = 0; i <= 14; i++) {
    ui->fileAttributes->item(i)->setCheckState(
        file_attributes.test(i) ? Qt::Checked : Qt::Unchecked);
  }

  buffer = movePointer(buffer, header->size);
  auto has_link_target_id_list = link_flag.test(0);
  auto has_link_info = link_flag.test(1);
  auto has_name = link_flag.test(2);
  auto has_relative_path = link_flag.test(3);
  auto has_working_dir = link_flag.test(4);
  auto has_arguments = link_flag.test(5);
  auto has_icon_location = link_flag.test(6);
  // auto is_unicode = link_flag.test(7);
  // auto force_no_link_info = link_flag.test(8);
  // auto has_exp_string = link_flag.test(9);
  // auto run_in_separate_process = link_flag.test(10);
  // auto unused1 = link_flag.test(11);
  // auto has_darwin_id = link_flag.test(12);
  // auto run_as_user = link_flag.test(13);
  // auto has_exp_icon = link_flag.test(14);
  // auto no_pidl_alias = link_flag.test(15);
  // auto unused2 = link_flag.test(16);
  // auto run_with_shim_layer = link_flag.test(17);
  // auto force_no_link_track = link_flag.test(18);
  // auto enable_target_metadata = link_flag.test(19);
  // auto disable_link_path_tracking = link_flag.test(20);
  // auto disable_known_folder_tracking = link_flag.test(21);
  // auto disable_known_folder_alias = link_flag.test(22);
  // auto allow_link_to_link = link_flag.test(23);
  // auto unalias_on_save = link_flag.test(24);
  // auto prefer_environment_path = link_flag.test(25);
  // auto keep_local_id_list_for_unc_target = link_flag.test(26);

  if (has_link_target_id_list == true) {
    buffer = movePointer(buffer, this->updateLinkTargetIdListField(buffer));
  }

  if (has_link_info == true) {
    buffer = movePointer(buffer, this->updateLinkInfoField(buffer));
  }

  ui->description->setEnabled(has_name);
  if (has_name == true) {
    buffer = movePointer(buffer,
                         this->updateStringFieldUtf16(buffer, ui->description));
  }

  ui->relativePath->setEnabled(has_relative_path);
  if (has_relative_path == true) {
    buffer = movePointer(
        buffer, this->updateStringFieldUtf16(buffer, ui->relativePath));
  }

  ui->workingDir->setEnabled(has_working_dir);
  if (has_working_dir == true) {
    buffer = movePointer(buffer,
                         this->updateStringFieldUtf16(buffer, ui->workingDir));
  }

  ui->commandLine->setEnabled(has_arguments);
  if (has_arguments == true) {
    buffer = movePointer(buffer,
                         this->updateStringFieldUtf16(buffer, ui->commandLine));
  }

  ui->iconLocation->setEnabled(has_icon_location);
  if (has_icon_location == true) {
    buffer = movePointer(
        buffer, this->updateStringFieldUtf16(buffer, ui->iconLocation));
  }

  return;
}

uint16_t MainWindow::updateLinkTargetIdListField(void *buffer) {
  auto id_list_size = *(uint16_t *)buffer;
  auto root = tr("Unknown");
  auto path = QString();

  buffer = movePointer(buffer, sizeof(uint16_t));

  while (true) {
    auto item_id_size = *(uint16_t *)buffer;

    if (item_id_size == 0) {
      break;
    }

    auto type = *(uint8_t *)movePointer(buffer, sizeof(uint16_t));

    switch (type & 0x70) {
    case 0x10: {
      // Root folder shell item
      auto [ident, uuid] = parseRootFolderShellItem(buffer);
      root = ident;
      break;
    }
    case 0x20: {
      // Volume shell item
      auto volume = parseVolumeShellItem(buffer);
      path = volume;
      break;
    }
    case 0x30: {
      // File entry shell item
      auto [name, is_file] = parseFileEntryShellItem(buffer);
      path.append(name);
      if (is_file == false) {
        path.append('\\');
      }
    }
    case 0x40: {
      // Network location shell item
      // TODO: Not implemented
    }
    }

    buffer = movePointer(buffer, item_id_size);
  }

  ui->rootIdentifier->setText(root);
  ui->targetPath->setText(path);

  return id_list_size + sizeof(uint16_t);
}

uint32_t MainWindow::updateLinkInfoField(void *buffer) {
  auto link_info = (LinkInfo *)buffer;
  auto flags = std::bitset<32>(link_info->flags);

  if (flags.test(0) == true) {
    auto label =
        parseVolumeId(movePointer(buffer, link_info->volume_id_offset));

    auto local_base_path = parseLocalBasePath(
        movePointer(buffer, link_info->local_base_path_offset));

    if (link_info->header_size >= 0x24) {
      auto local_base_path_offset_unicode =
          *(uint32_t *)movePointer(buffer, sizeof(LinkInfo));

      local_base_path = parseLocalBasePathUnicode(
          movePointer(buffer, local_base_path_offset_unicode));
    }
  }

  if (flags.test(1) == true) {
    // TODO: Implement this
  }

  return link_info->size;
}

std::tuple<QString, QUuid> MainWindow::parseRootFolderShellItem(void *buffer) {
  auto item = (RootFolderShellItem *)buffer;
  auto uuid = QUuid::fromBytes(item->class_identifier, QSysInfo::LittleEndian);
  auto root = tr("Unknown");

  switch (item->sort_index) {
  case 0x00:
    root = tr("Internet Explorer");
    break;
  case 0x42:
    root = tr("Libraries");
    break;
  case 0x44:
    root = tr("Users");
    break;
  case 0x48:
    root = tr("My Documents");
    break;
  case 0x50:
    root = tr("My Computer");
    break;
  case 0x58:
    root = tr("My Network Places/Network");
    break;
  case 0x60:
    root = tr("Recycle Bin");
    break;
  case 0x68:
    root = tr("Internet Explorer");
    break;
  case 0x70:
    root = tr("Unknown");
    break;
  case 0x80:
    root = tr("My Games");
    break;
  }

  return std::make_tuple(root, uuid);
}

QString MainWindow::parseVolumeShellItem(void *buffer) {
  auto item = (VolumeShellItem *)buffer;
  auto volume = QString(item->volume);

  return volume;
}

std::tuple<QString, bool> MainWindow::parseFileEntryShellItem(void *buffer) {
  auto item = (FileEntryShellItem *)buffer;
  auto type = item->type & 0xF;
  auto short_name = QString();
  size_t name_size = 0;
  if (type & 0x4) {
    short_name = QString::fromUtf16((char16_t *)item->name);
    name_size = short_name.size() * sizeof(char16_t);
  } else {
    short_name = QString::fromUtf8(item->name);
    name_size = short_name.size();
  }
  if ((name_size + 1) % 2 != 0) {
    name_size += 1;
  }
  buffer = movePointer(buffer, sizeof(FileEntryShellItem) + name_size + 1);

  auto [size_of_block1, long_name] = parseFileEntryExtensionBlock1(buffer);

  // TODO: Parse more block

  // TODO: Only use 0x2 to detect whether it is a file
  return std::make_tuple(long_name, type & 0x2);
}

std::tuple<uint16_t, QString>
MainWindow::parseFileEntryExtensionBlock1(void *buffer) {
  auto block = (FileEntryExtensionBlock1 *)buffer;
  auto version = block->version;
  auto long_name = QString();
  auto localized_name = QString();
  uint16_t long_string_size = 0;
  buffer = movePointer(buffer, sizeof(FileEntryExtensionBlock1));

  if (version >= 7) {
    // TODO: NTFS lookup
    buffer = movePointer(buffer, 18);
  }

  if (version >= 3) {
    long_string_size = *(uint16_t *)buffer;
    buffer = movePointer(buffer, 2);
  }

  if (version >= 9) {
    buffer = movePointer(buffer, 4);
  }

  if (version >= 8) {
    buffer = movePointer(buffer, 4);
  }

  if (version >= 3) {
    long_name = QString::fromUtf16((char16_t *)buffer);
    buffer = movePointer(buffer, long_name.size() * sizeof(char16_t) + 3);
  }

  if (version >= 3 && long_string_size != 0) {
    localized_name = QString::fromUtf8((char *)buffer);
    buffer = movePointer(buffer, localized_name.size() * sizeof(char) + 2);
  }

  if (version >= 7 && long_string_size != 0) {
    localized_name = QString::fromUtf16((char16_t *)buffer);
    buffer = movePointer(buffer, localized_name.size() * sizeof(char16_t) + 3);
  }

  if (version >= 3) {
    auto offset = *(uint16_t *)buffer;
  }

  return std::make_tuple(block->size, long_name);
}

uint32_t MainWindow::updateStringFieldUtf16(void *buffer, QLineEdit *output) {
  auto size = *(uint16_t *)buffer;
  auto description = QString::fromUtf16(
      (char16_t *)movePointer(buffer, sizeof(uint16_t)), size);

  output->setText(description);

  return size * sizeof(char16_t) + sizeof(uint16_t);
}

QString MainWindow::parseVolumeId(void *buffer) {
  auto id = (VolumeId *)buffer;
  auto label = QString((char *)movePointer(buffer, id->volume_label_offset));

  if (id->volume_label_offset > 16) {
    auto volume_label_offset_unicode =
        *(uint32_t *)movePointer(buffer, sizeof(VolumeId));
    QString((char16_t *)movePointer(buffer, volume_label_offset_unicode));
  }

  return label;
}

QString MainWindow::parseLocalBasePath(void *buffer) {
  return QString((char *)buffer);
}

QString MainWindow::parseLocalBasePathUnicode(void *buffer) {
  return QString((char16_t *)buffer);
}

std::tuple<QString, QString> MainWindow::parseHotKey(uint16_t hot_key) {
  auto hot_key_low = hot_key & 0xFF;
  auto hot_key_high = (hot_key >> 8) & 0xFF;
  auto hot_key_1 = tr("Abnormal key value");
  auto hot_key_2 = QString();

  if (hot_key_low == 0) {
    hot_key_1 = tr("No key assigned");
  } else if ((0x30 <= hot_key_low && hot_key_low <= 0x39) ||
             (0x41 <= hot_key_low && hot_key_low <= 0x5A)) {
    hot_key_1 = QString((char)hot_key_low);
  } else if (0x70 <= hot_key_low && hot_key_low <= 0x87) {
    hot_key_1 = QString("F%1").arg(hot_key_low - 0x6F);
  } else if (hot_key_low == 0x90) {
    hot_key_1 = QString("NUM LOCK");
  } else if (hot_key_low == 0x91) {
    hot_key_1 = QString("SCROLL LOCK");
  }

  if (hot_key_high & 0x1) {
    hot_key_2 += QString("SHIFT ");
  }
  if (hot_key_high & 0x2) {
    hot_key_2 += QString("CTRL ");
  }
  if (hot_key_high & 0x4) {
    hot_key_2 += QString("ALT ");
  }
  if (hot_key_high == 0) {
    hot_key_2 = tr("No modifier used");
  } else if (hot_key_2.isEmpty() == true) {
    hot_key_2 = tr("Abnormal modifier value");
  }

  return std::make_tuple(hot_key_1, hot_key_2);
}

QString MainWindow::parseShowWindow(uint16_t show_window) {
  auto text = tr("Show normally");

  // TODO: Fix ambiguous value
  if (show_window == 3) {
    text = tr("Show maximally");
  } else if (show_window == 7) {
    text = tr("Show minimally");
  }

  return text;
}

void *MainWindow::movePointer(void *pointer, size_t offset) {
  return (uint8_t *)pointer + offset;
}

MainWindow::~MainWindow() { delete ui; }
