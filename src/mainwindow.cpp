#include "mainwindow.h"
#include "guid.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QString>

#define UNIX_EPOCH_IN_FILETIME 116444736000000000ULL

uint64_t fileTimeToEpochInSecs(uint64_t file_time) {
  return (file_time - UNIX_EPOCH_IN_FILETIME) / 10000000;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  ui->isRemovable->setAttribute(Qt::WA_TransparentForMouseEvents);

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

  if (header->validate() == false) {
    QMessageBox::critical(this, tr("Error"), tr("Not a valid lnk file."));
    return;
  }

  // TODO: Add class identifier validation

  this->updateTimeField(header);
  this->updateHotKeyField(header);
  this->updateFileSizeField(header);
  this->updateLinkFlagsField(header);
  this->updateFileAttributesField(header);
  this->updateShowTypeField(header);

  auto icon_index = header->icon_index; // TODO: Parse icons
  ui->iconIndex->setText(QString::number(icon_index));

  buffer = movePointer(buffer, header->size);

  if (header->flags & HAS_LINK_TARGET_ID_LIST) {
    buffer = movePointer(buffer, this->updateLinkTargetIdListField(buffer));
  }

  if (header->flags & HAS_LINK_INFO) {
    buffer = movePointer(buffer, this->updateLinkInfoField(buffer));
  }

  ui->description->setEnabled(header->flags & HAS_NAME);
  if (header->flags & HAS_NAME) {
    buffer = movePointer(buffer,
                         this->updateStringFieldUtf16(buffer, ui->description));
  }

  ui->relativePath->setEnabled(header->flags & HAS_RELATIVE_PATH);
  if (header->flags & HAS_RELATIVE_PATH) {
    buffer = movePointer(
        buffer, this->updateStringFieldUtf16(buffer, ui->relativePath));
  }

  ui->workingDir->setEnabled(header->flags & HAS_WORKING_DIR);
  if (header->flags & HAS_WORKING_DIR) {
    buffer = movePointer(buffer,
                         this->updateStringFieldUtf16(buffer, ui->workingDir));
  }

  ui->commandLine->setEnabled(header->flags & HAS_ARGUMENTS);
  if (header->flags & HAS_ARGUMENTS) {
    buffer = movePointer(buffer,
                         this->updateStringFieldUtf16(buffer, ui->commandLine));
  }

  ui->iconLocation->setEnabled(header->flags & HAS_ICON_LOCATION);
  if (header->flags & HAS_ICON_LOCATION) {
    buffer = movePointer(
        buffer, this->updateStringFieldUtf16(buffer, ui->iconLocation));
  }

  // TODO: Add extra blocks parsing

  return;
}

uint16_t MainWindow::updateLinkTargetIdListField(void *buffer) {
  auto id_list_size = *(uint16_t *)buffer;
  auto root = tr("Unknown");
  QString long_name;
  QString localized_name;
  QString short_name;
  QString volume;
  bool is_removable;

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
      auto [ident, sort_index] = parseRootFolderShellItem(buffer);
      root = ident;
      break;
    }
    case 0x20: {
      // Volume shell item
      auto [volume_, is_removable_] = parseVolumeShellItem(buffer);
      volume = volume_;
      is_removable = is_removable_;
      break;
    }
    case 0x30: {
      // File entry shell item
      auto [short_name_, long_name_, localized_name_, is_file] =
          parseFileEntryShellItem(buffer);
      short_name += this->pushPath(short_name_, is_file, false);
      long_name += this->pushPath(long_name_, is_file, false);
      localized_name += this->pushPath(localized_name_, is_file, true);
    }
    case 0x40: {
      // Network location shell item
      // TODO: Not implemented
    }
    }

    buffer = movePointer(buffer, item_id_size);
  }

  ui->rootIdentifier->setText(root);
  ui->longName->setText(long_name);
  ui->localizedName->setText(localized_name);
  ui->shortName->setText(short_name);
  ui->isRemovable->setChecked(is_removable);
  ui->volume->setText(volume);

  return id_list_size + sizeof(uint16_t);
}

uint32_t MainWindow::updateLinkInfoField(void *buffer) {
  auto link_info = (LinkInfo *)buffer;
  uint32_t serial_number = 0;
  QString drive_type;
  QString label;
  QString local_base_path;
  QString local_base_path_unicode;

  if (link_info->flags & VOLUME_ID_AND_LOCAL_BASE_PATH) {
    auto [label_, serial_number_, drive_type_] =
        parseVolumeId(movePointer(buffer, link_info->volume_id_offset));
    local_base_path = parseLocalBasePath(
        movePointer(buffer, link_info->local_base_path_offset));
    serial_number = serial_number_;
    label = label_;
    switch (drive_type_) {
    case DRIVE_UNKNOWN:
      drive_type = tr("Unknown");
      break;
    case DRIVE_NO_ROOT_DIR:
      drive_type = tr("No root directory");
      break;
    case DRIVE_REMOVABLE:
      drive_type = tr("Removable");
      break;
    case DRIVE_FIXED:
      drive_type = tr("Fixed");
      break;
    case DRIVE_REMOTE:
      drive_type = tr("Remote");
      break;
    case DRIVE_CDROM:
      drive_type = tr("CD-ROM");
      break;
    case DRIVE_RAMDISK:
      drive_type = tr("RAM disk");
      break;
    }

    if (link_info->header_size >= 0x24) {
      auto local_base_path_offset_unicode =
          *(uint32_t *)movePointer(buffer, sizeof(LinkInfo));

      local_base_path_unicode = parseLocalBasePathUnicode(
          movePointer(buffer, local_base_path_offset_unicode));
    }
  }

  if (link_info->flags & COMMON_NETWORK_RELATIVE_LINK_AND_PATH_SUFFIX) {
    // TODO: Implement this
  }

  ui->volumeLabel->setText(label);
  ui->serialNumber->setText(QString::number(serial_number));
  ui->driveType->setText(drive_type);
  ui->localBasePath->setText(local_base_path);
  ui->localBasePathUnicode->setText(local_base_path_unicode);

  return link_info->size;
}

std::tuple<QString, uint8_t>
MainWindow::parseRootFolderShellItem(void *buffer) {
  auto item = (RootFolderShellItem *)buffer;
  auto uuid = (GUID *)item->class_identifier;

  return std::make_tuple(QString::fromStdString(uuid->toClassName()),
                         item->sort_index);
}

std::tuple<QString, bool> MainWindow::parseVolumeShellItem(void *buffer) {
  auto item = (VolumeShellItem *)buffer;
  QString volume = "?:\\";

  if (item->type & 0x1) {
    volume = QString(item->volume);
  }

  // TODO: This is not accurate
  return std::make_tuple(volume, item->type & 0x8);
}

std::tuple<QString, QString, QString, bool>
MainWindow::parseFileEntryShellItem(void *buffer) {
  auto item = (FileEntryShellItem *)buffer;
  auto type = item->type & 0xF;
  QString short_name;
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

  auto [size_of_block1, long_name, localized_name] =
      parseFileEntryExtensionBlock04(buffer);

  // TODO: Parse more block

  // TODO: Only use 0x2 to detect whether it is a file
  return std::make_tuple(short_name, long_name, localized_name, type & 0x2);
}

std::tuple<uint16_t, QString, QString>
MainWindow::parseFileEntryExtensionBlock04(void *buffer) {
  auto block = (FileEntryExtensionBlock04 *)buffer;
  auto version = block->version;
  QString long_name;
  QString localized_name;
  uint16_t long_string_size = 0;
  buffer = movePointer(buffer, sizeof(FileEntryExtensionBlock04));

  if (version >= 7) {
    auto extension1 = (NTFSFileReference *)buffer;
    buffer = movePointer(buffer, sizeof(NTFSFileReference));
  }

  if (version >= 3) {
    long_string_size = *(uint16_t *)buffer;
    buffer = movePointer(buffer, 2);
  }

  if (version >= 9) {
    // Unknown
    buffer = movePointer(buffer, 4);
  }

  if (version >= 8) {
    // Unknown
    buffer = movePointer(buffer, 4);
  }

  if (version >= 3) {
    long_name = QString::fromUtf16((char16_t *)buffer);
    buffer = movePointer(buffer, (long_name.length() + 1) * sizeof(char16_t));
  }

  if (version >= 3 && version < 7 && long_string_size != 0) {
    localized_name = QString::fromUtf8((char *)buffer);
    buffer = movePointer(buffer, (localized_name.size() + 1) * sizeof(char));
  }

  if (version >= 7 && long_string_size != 0) {
    localized_name = QString::fromUtf16((char16_t *)buffer);
    buffer =
        movePointer(buffer, (localized_name.size() + 1) * sizeof(char16_t));
  }

  if (version >= 3) {
    auto offset = *(uint16_t *)buffer;
  }

  return std::make_tuple(block->size, long_name, localized_name);
}

uint32_t MainWindow::updateStringFieldUtf16(void *buffer, QLineEdit *output) {
  auto size = *(uint16_t *)buffer;
  auto description = QString::fromUtf16(
      (char16_t *)movePointer(buffer, sizeof(uint16_t)), size);

  output->setText(description);

  return size * sizeof(char16_t) + sizeof(uint16_t);
}

std::tuple<QString, uint32_t, uint32_t>
MainWindow::parseVolumeId(void *buffer) {
  auto id = (VolumeId *)buffer;
  QString label;
  if (id->volume_label_offset != 0x14) {
    label =
        QString::fromUtf8((char *)movePointer(buffer, id->volume_label_offset));
  } else {
    auto volume_label_offset_unicode =
        *(uint32_t *)movePointer(buffer, sizeof(VolumeId));
    QString::fromUtf16(
        (char16_t *)movePointer(buffer, volume_label_offset_unicode));
  }

  return std::make_tuple(label, id->drive_serial_number, id->drive_type);
}

QString MainWindow::parseLocalBasePath(void *buffer) {
  return QString::fromUtf8((char *)buffer);
}

QString MainWindow::parseLocalBasePathUnicode(void *buffer) {
  return QString::fromUtf16((char16_t *)buffer);
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

QString MainWindow::parseHotKey(std::tuple<std::string, std::string> hot_key) {
  auto [hot_key_1, hot_key_2] = hot_key;

  if (hot_key_2.empty() == true && hot_key_1.empty() == true) {
    return tr("None");
  } else if (hot_key_2.empty() == true && hot_key_1.empty() == false) {
    return QString::fromStdString(hot_key_1);
  } else if (hot_key_2.empty() == false && hot_key_1.empty() == false) {
    return QString::fromStdString(hot_key_1).append(" + ").append(
        QString::fromStdString(hot_key_2));
  } else {
    return QString::fromStdString(hot_key_2);
  }
}

void MainWindow::updateTimeField(const ShellLinkHeader *header) {
  auto creation_date = QDateTime::fromSecsSinceEpoch(
      fileTimeToEpochInSecs(header->creation_time), QTimeZone::UTC);
  auto access_date = QDateTime::fromSecsSinceEpoch(
      fileTimeToEpochInSecs(header->access_time), QTimeZone::UTC);
  auto modification_date = QDateTime::fromSecsSinceEpoch(
      fileTimeToEpochInSecs(header->modification_time), QTimeZone::UTC);

  ui->creationDate->setText(creation_date.toString());
  ui->accessDate->setText(access_date.toString());
  ui->modificationDate->setText(modification_date.toString());
}

void MainWindow::updateHotKeyField(const ShellLinkHeader *header) {
  auto hot_key = this->parseHotKey(header->parseHotKey());

  ui->hotKey->setText(hot_key);
}

void MainWindow::updateFileSizeField(const ShellLinkHeader *header) {
  auto target_file_size = (double)header->file_size / 1024;

  ui->targetFileSize->setText(QString::number(target_file_size).append(" KB"));
}

void MainWindow::updateLinkFlagsField(const ShellLinkHeader *header) {
  auto flag_bits = std::bitset<32>(header->flags);

  for (auto i = 0; i <= 26; i++) {
    ui->linkFlags->item(i)->setCheckState(flag_bits.test(i) ? Qt::Checked
                                                            : Qt::Unchecked);
  }
}

void MainWindow::updateFileAttributesField(const ShellLinkHeader *header) {
  auto attribute_bits = std::bitset<32>(header->file_attributes);

  for (auto i = 0; i <= 14; i++) {
    ui->fileAttributes->item(i)->setCheckState(
        attribute_bits.test(i) ? Qt::Checked : Qt::Unchecked);
  }
}

void MainWindow::updateShowTypeField(const ShellLinkHeader *header) {
  auto show_window = parseShowWindow(header->show_window);

  ui->showType->setText(show_window);
}

QString MainWindow::pushPath(QString &path, bool is_file, bool is_localize) {
  if (path.isEmpty() == true) {
    return QString();
  }

  if (is_file == true || is_localize == true) {
    return path;
  }

  return path.append("\\");
}

MainWindow::~MainWindow() { delete ui; }
