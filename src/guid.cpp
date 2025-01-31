#include "guid.h"
#include <unordered_map>

using namespace guid_parse::literals;

static const std::unordered_map<GUID, std::string> SHELL_FOLDER_GUID_MAP = {
    {"20d04fe0-3aea-1069-a2d8-08002b30309d"_guid, "My Computer"},
    {"450d8fba-ad25-11d0-98a8-0800361b1103"_guid, "My Documents"},
    {"645ff040-5081-101b-9f08-00aa002f954e"_guid, "Recycle Bin"},
    {"ff393560-c2a7-11cf-bff4-444553540000"_guid, "History"},
    {"208d2c60-3aea-1069-a2d7-08002b30309d"_guid, "Network"},
    {"d20ea4e1-3957-11d2-a40b-0c5020524153"_guid, "Administrative Tools"},
    {"7bd29e00-76c1-11cf-9dd0-00a0c9034933"_guid, "Fonts"},
    {"fd228cb7-ae11-4ae3-864c-16f3910ab8fe"_guid, "Libraries"},
    {"b4bfcc3a-db2c-424c-b029-7fe99a87c641"_guid, "Desktop"},
    {"f02c1a0d-be21-4350-88b0-7367fc96ef3c"_guid, "Network Connections"},
    {"374de290-123f-4565-9164-39c4925e467b"_guid, "Downloads"},
    {"b7bede81-df94-4682-a7d8-57a52620b86f"_guid, "Programs and Features"},
    {"d34df89c-7d98-4355-9ad1-5eb0927e5c4a"_guid, "Public"},
    {"1f4de370-d627-11d1-ba4f-00a0c91eedba"_guid, "Pictures"},
    {"fdd39ad0-238f-46af-adb4-6c85480369c7"_guid, "Documents"},
    {"374de290-123f-4565-9164-39c4925e467b"_guid, "Downloads"},
    {"3d1d4be3-4556-4f02-935a-24a9c1104a48"_guid, "Music"},
    {"7d1d3a04-debb-4115-95cf-2f29da2920da"_guid, "Videos"},
    {"0762d272-c50a-4bb0-a382-697dcd729b80"_guid, "Control Panel"},
    {"724ef170-a42d-4fef-9f26-b60e846fba4f"_guid, "Start Menu"},
    {"4c5c32ff-bfc0-4b15-8523-4a64f5a117f8"_guid, "Quick Access"},
    {"b97d20bb-f46a-4c97-ba10-5e3608430854"_guid, "Device Manager"},
    {"8e74d236-7f35-4720-b138-1fed0b85ea75"_guid, "Sync Center"},
    {"0ac0837c-bbf8-452a-850d-79d08e667ca7"_guid, "This PC"},
    {"31c0dd25-9439-4f12-bf41-7ff4eda38722"_guid, "Recent Items"},
    {"5fa96407-7e77-483c-ac93-691d05850de8"_guid, "Bluetooth Devices"},
    {"21ec2020-3aea-1069-a2dd-08002b30309d"_guid, "Control Panel"},
    {"59031a47-3f72-44a7-89c5-5595fe6b30ee"_guid, "Users"},
    {"93412589-74d4-4e4e-ad0e-e0cb621440fd"_guid, "OneDrive"},
    {"98ec0e18-2098-4d44-8644-66979315a281"_guid, "Saved Pictures"},
    {"b7534046-3ec3-4d57-a5df-3bd9c70b8c09"_guid, "Applications"},
    {"1cf1260c-4dd0-4ebb-811f-33c57269944c"_guid, "Music"},
    {"ae50c081-ebd2-438a-8655-8a092e34987a"_guid, "Videos"},
    {"f86fa3ab-70d2-4fc7-9c99-fcbf05467f3a"_guid, "Public Videos"},
    {"6d809377-6af0-444b-8957-a3773f02200e"_guid, "Program Files"},
    {"7c5a40ef-a0fb-4bfc-874a-c0f2e0b9fa8e"_guid, "Windows"},
    {"8983036c-27c0-404b-8f08-102d10dcfd74"_guid, "Roaming"},
    {"4badfc52-5034-46e4-a2ba-21cc987be871"_guid, "Startup"},
    {"df7266ac-9274-4867-8d55-3bd661de872d"_guid, "Games"},
    {"de61d971-5ebc-4f02-a3a9-6c82895e5c04"_guid, "Storage Devices"},
    {"fdd39ad0-238f-46af-adb4-6c85480369c7"_guid, "Documents"},
    {"1777f761-68ad-4d8a-87bd-30b759fa33dd"_guid, "Favorites"},
    {"bfb9d5e0-c6a9-404c-b2b2-ae6db6af4968"_guid, "Programs"},
    {"dfdf76a2-c82a-4d63-906a-5644ac457385"_guid, "Public Documents"},
    {"de92c1c7-837f-4f69-a3bb-86e631204a23"_guid, "Recorded TV"},
    {"18989b1d-99b5-455b-841c-ab7c74e4ddfc"_guid, "Saved Games"},
    {"0db7e03f-fc29-4dc6-9020-ff41b59e513a"_guid, "Recorded TV"},
    {"b2c5e279-7add-439f-b28c-c41fe1bbf672"_guid, "Camera Roll"},
    {"abcd1234-ef56-7890-abcd-1234567890ab"_guid, "Custom Folder"},

};

std::string GUID::toClassName() const {
  if (SHELL_FOLDER_GUID_MAP.contains(*this)) {
    return SHELL_FOLDER_GUID_MAP.at(*this);
  }

  return this->toString();
}

std::string GUID::toString() const {
  char buffer[40];
  snprintf(buffer, sizeof(buffer),
           "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", data1, data2,
           data3, data4[0], data4[1], data4[2], data4[3], data4[4], data4[5],
           data4[6], data4[7]);

  return buffer;
}
