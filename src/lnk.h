#include <stdint.h>

static_assert(true); // Suppress warning
#pragma pack(push, 1)
struct ShellLinkHeader {
  uint32_t size;

  /* The class identifier
   */
  uint8_t class_identifier[16];

  /* The flags
   */
  uint32_t flags;

  /* The file attributes
   */
  uint32_t file_attributes;

  /* The creation date and time
   * Contains a 64-bit filetime value
   */
  uint64_t creation_time;

  /* The last access date and time
   * Contains a 64-bit filetime value
   */
  uint64_t access_time;

  /* The last modification date and time
   * Contains a 64-bit filetime value
   */
  uint64_t modification_time;

  /* The file size
   */
  uint32_t file_size;

  /* The icon index
   */
  uint32_t icon_index;

  /* The show window
   */
  uint32_t show_window;

  /* The hot key
   */
  uint16_t hot_key;

  uint16_t reserved1;

  uint32_t reserved2;

  uint32_t reserved3;
};

struct RootFolderShellItem {
  uint16_t size;

  uint8_t type;

  uint8_t sort_index;

  uint8_t class_identifier[16];
};

struct VolumeShellItem {
  uint16_t size;

  uint8_t type;

  char volume[0];
};

struct FileEntryShellItem {
  uint16_t size;

  uint8_t type;

  uint8_t unknown1;

  uint32_t file_size;

  uint32_t modification_date;

  uint16_t file_attributes;

  /* 16-bit aligned, ascii depending on flag
   */
  char name[0];
};

struct FileEntryExtensionBlock1 {
  uint16_t size;

  uint16_t version;

  uint32_t signature;

  uint32_t creation_date;

  uint32_t access_date;

  uint16_t unknown1;
};

struct LinkInfo {
  uint32_t size;

  uint32_t header_size;

  uint32_t flags;

  uint32_t volume_id_offset;

  uint32_t local_base_path_offset;

  uint32_t common_network_relative_link_offset;

  uint32_t common_path_suffix_offset;

  /* Optional if header_size >= 0x24 uint32_t local_base_path_offset_unicode;

     Optional uint32_t if header_size >= 0x24 common_path_suffix_offset_unicode;
  */
};

struct VolumeId {
  uint32_t size;

  uint32_t drive_type;

  uint32_t drive_serial_number;

  uint32_t volume_label_offset;

  /* Optional if volume_label_offset > 16 uint32_t volume_label_offset_unicode;
   */
};
#pragma pack(pop)
