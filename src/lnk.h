#include <stdint.h>
#include <string>

#define HAS_LINK_TARGET_ID_LIST                                                \
  0x00000001 // The shell link is saved with an item ID list (IDList).
#define HAS_LINK_INFO                                                          \
  0x00000002                // The shell link is saved with link information.
#define HAS_NAME 0x00000004 // The shell link is saved with a name string.
#define HAS_RELATIVE_PATH                                                      \
  0x00000008 // The shell link is saved with a relative path string.
#define HAS_WORKING_DIR                                                        \
  0x00000010 // The shell link is saved with a working directory string.
#define HAS_ARGUMENTS                                                          \
  0x00000020 // The shell link is saved with command line arguments.
#define HAS_ICON_LOCATION                                                      \
  0x00000040 // The shell link is saved with an icon location string.
#define IS_UNICODE                                                             \
  0x00000080 // The shell link contains Unicode encoded strings. This bit SHOULD
             // be set.
#define FORCE_NO_LINK_INFO 0x00000100 // The LinkInfo structure is ignored.
#define HAS_EXP_STRING                                                         \
  0x00000200 // The shell link is saved with an EnvironmentVariableDataBlock.
#define RUN_IN_SEPARATE_PROCESS                                                \
  0x00000400 // The target is run in a separate virtual machine when launching a
             // link target that is a 16-bit application.
#define UNUSED_1 0x00000800
#define HAS_DARWIN_ID                                                          \
  0x00001000 // The shell link is saved with a DarwinDataBlock.
#define RUN_AS_USER                                                            \
  0x00002000 // The application is run as a different user when the target of
             // the shell link is activated.
#define HAS_EXP_ICON                                                           \
  0x00004000 // The shell link is saved with an IconEnvironmentDataBlock.
#define NO_PIDL_ALIAS                                                          \
  0x00008000 // The file system location is represented in the shell namespace
             // when the path to an item is parsed into an IDList.
#define UNUSED_2 0x00010000
#define RUN_WITH_SHIM_LAYER                                                    \
  0x00020000 // The shell link is saved with a ShimDataBlock.
#define FORCE_NO_LINK_TRACK 0x00040000 // The TrackerDataBlock is ignored.
#define ENABLE_TARGET_METADATA                                                 \
  0x00080000 // The shell link attempts to collect target properties and store
             // them in the PropertyStoreDataBlock when the link target is set.
#define DISABLE_LINK_PATH_TRACKING                                             \
  0x00100000 // The EnvironmentVariableDataBlock is ignored.
#define DISABLE_KNOWN_FOLDER_TRACKING                                          \
  0x00200000 // The SpecialFolderDataBlock and the KnownFolderDataBlock are
             // ignored when loading the shell link.
#define DISABLE_KNOWN_FOLDER_ALIAS                                             \
  0x00400000 // If the link has a KnownFolderDataBlock (section 2.5.6), the
             // unaliased form of the known folder IDList SHOULD be used when
             // translating the target IDList at the time that the link is
             // loaded.
#define ALLOW_LINK_TO_LINK                                                     \
  0x00800000 // Creating a link that references another link is enabled.
             // Otherwise, specifying a link as the target IDList SHOULD NOT be
             // allowed.
#define UNALIAS_ON_SAVE                                                        \
  0x01000000 // When saving a link for which the target IDList is under a known
             // folder, either the unaliased form of that known folder or the
             // target IDList SHOULD be used.
#define PREFER_ENVIRONMENT_PATH                                                \
  0x02000000 // The target IDList SHOULD NOT be stored; instead, the path
             // specified in the EnvironmentVariableDataBlock SHOULD be used to
             // refer to the target.
#define KEEP_LOCAL_ID_LIST_FOR_UNC_TARGET                                      \
  0x04000000 // When the target is a UNC name that refers to a location

#define VOLUME_ID_AND_LOCAL_BASE_PATH                                          \
  0x00000001 // If set, the VolumeID and LocalBasePath fields are present, and
             // their locations are specified by the values of the
             // VolumeIDOffset and LocalBasePathOffset fields, respectively. If
             // the value of the LinkInfoHeaderSize field is greater than or
             // equal to 0x00000024, the LocalBasePathUnicode field is present,
             // and its location is specified by the value of the
             // LocalBasePathOffsetUnicode field. If not set, the VolumeID,
             // LocalBasePath, and LocalBasePathUnicode fields are not present,
             // and the values of the VolumeIDOffset and LocalBasePathOffset
             // fields are zero. If the value of the LinkInfoHeaderSize field is
             // greater than or equal to 0x00000024, the value of the
             // LocalBasePathOffsetUnicode field is zero.
#define COMMON_NETWORK_RELATIVE_LINK_AND_PATH_SUFFIX                           \
  0x00000002 // If set, the CommonNetworkRelativeLink field is present, and its
             // location is specified by the value of the
             // CommonNetworkRelativeLinkOffset field.

#define DRIVE_UNKNOWN 0
#define DRIVE_NO_ROOT_DIR 1
#define DRIVE_REMOVABLE 2
#define DRIVE_FIXED 3
#define DRIVE_REMOTE 4
/* Win32 additions */
#define DRIVE_CDROM 5
#define DRIVE_RAMDISK 6

constexpr uint32_t shell_link_header_size = 0x4C;

// struct LinkFlags {
//   bool has_link_target_id_list;
//   bool has_link_info;
//   bool has_name;
//   bool has_relative_path;
//   bool has_working_dir;
//   bool has_arguments;
//   bool has_icon_location;
//   bool is_unicode;
//   bool force_no_link_info;
//   bool has_exp_string;
//   bool run_in_separate_process;
//   bool unused1;
//   bool has_darwin_id;
//   bool run_as_user;
//   bool has_exp_icon;
//   bool no_pidl_alias;
//   bool unused2;
//   bool run_with_shim_layer;
//   bool force_no_link_track;
//   bool enable_target_metadata;
//   bool disable_link_path_tracking;
//   bool disable_known_folder_tracking;
//   bool disable_known_folder_alias;
//   bool allow_link_to_link;
//   bool unalias_on_save;
//   bool prefer_environment_path;
//   bool keep_local_id_list_for_unc_target;
// };

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

  bool validate() const;
  std::tuple<std::string, std::string> parseHotKey() const;
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

struct FileEntryExtensionBlock04 {
  uint16_t size;

  uint16_t version;

  uint32_t signature;

  uint32_t creation_date;

  uint32_t access_date;

  uint16_t unknown1;
};

struct NTFSFileReference {
  uint16_t unknown1;

  uint64_t ntfs_reference;

  uint64_t unknown2;
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
