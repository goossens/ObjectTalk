#define USE_STD_FILESYSTEM

#include <GLFW/glfw3.h>
#define USE_EXPLORATION_BY_KEYS
#define IGFD_KEY_UP GLFW_KEY_UP
#define IGFD_KEY_DOWN GLFW_KEY_DOWN
#define IGFD_KEY_ENTER GLFW_KEY_ENTER
#define IGFD_KEY_BACKSPACE GLFW_KEY_BACKSPACE

#define USE_DIALOG_EXIT_WITH_KEY
#define IGFD_EXIT_KEY GLFW_KEY_ESCAPE

#define searchString "Search:"
#define fileNameString "File name:"
#define dirNameString "Directory path:"

#define fileSizeBytes "bytes"
#define fileSizeKiloBytes "KB"
#define fileSizeMegaBytes "MB"
#define fileSizeGigaBytes "GB"

#define OverWriteDialogTitleString "Confirm overwrite..."
#define OverWriteDialogMessageString "The file already exists!\nWould you like to overwrite it?\n\n"
#define OverWriteDialogConfirmButtonString "OK"

#define ImGuiKey_KeypadEnter ImGuiKey_KeyPadEnter
