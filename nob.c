#include <stddef.h>
#include <stdio.h>
#include <string.h>
#define NOB_IMPLEMENTATION
#include "nob.h"

// TODO: We should build raylib our self to make sure we have it
// see tsoding/musilizer.

#ifdef _WIN32
#   define COMPILER "gcc.exe"
#   define HEADERS_DIR ".\\headers"
#   define SRC_BUILD_DIR   ".\\src_build\\"
#   define BUILD_DIR       ".\\build\\"
#   define THIRDPARTY_DIR  ".\\thirdparty\\"
#   define build(cmd) build_win(cmd)
#   define SRC_DIR         ".\\src\\"
#else
#   define BUILD_DIR       "./build/"
#   define SRC_BUILD_DIR   "./src_build/"
#   define THIRDPARTY_DIR  "./thirdparty/"
#   define SRC_DIR         "./src/"
#   define COMPILER "cc"
#   define HEADERS_DIR "./headers"
#   define build(cmd) build_linux(cmd)
#endif

Cmd cmd = {0};

const char* thirdparty[] = {
    "tinyfiledialogs"
};
const char* src_files[] = {
    "main.c",
    "ui.c",
    "canvas.c", 
    "data.c",
    "Info_Panel.c"
};

void build_thirdparty(Cmd *cmd)
{
    mkdir_if_not_exists(SRC_BUILD_DIR);

    cmd_append(cmd, COMPILER);
    cmd_append(cmd, "-w");
    cmd_append(cmd, "-c");

    char src_path[512];
    snprintf(src_path, sizeof(src_path), THIRDPARTY_DIR"%s.c", thirdparty[0]);
    cmd_append(cmd, strdup(src_path));

    cmd_append(cmd, "-o");

    char obj_path[512]; 
    snprintf(obj_path, sizeof(obj_path), SRC_BUILD_DIR"%s.o", thirdparty[0]);
    cmd_append(cmd, strdup(obj_path));
    cmd_append(cmd, "-I", HEADERS_DIR);
}

void build_linux(Cmd *cmd)
{
    nob_log(NOB_INFO, "Build Target is : Posix");
    cmd_append(cmd, COMPILER);
    cmd_append(cmd, "-Wall", "-Wextra", "-ggdb");
    cmd_append(cmd, "-o", "./build/EntityGen");
    char srcs[512];
    for (size_t i = 0; i < ARRAY_LEN(src_files); ++i) {
        snprintf(srcs, sizeof(srcs), SRC_DIR"%s", src_files[i]);
        cmd_append(cmd, strdup(srcs));
    }
    cmd_append(cmd, SRC_BUILD_DIR"tinyfiledialogs.o");
    cmd_append(cmd, "-I","./headers");
    cmd_append(cmd, "-I","./");
    cmd_append(cmd, "-I","/usr/local/include");
    cmd_append(cmd, "-L","/usr/local/lib");
    cmd_append(cmd, "-lraylib", "-lGL", "-lm", "-lpthread", "-ldl", "-lrt", "-lX11");
}

void build_win(Cmd *cmd)
{
    nob_log(NOB_INFO, "Build Target is : win32");
    cmd_append(cmd, COMPILER);
    cmd_append(cmd, "-Wall", "-Wextra", "-ggdb");
    cmd_append(cmd, "-o", "./build/EntityGen.exe");
    char srcs[512];
    for (size_t i = 0; i < ARRAY_LEN(src_files); ++i) {
        snprintf(srcs, sizeof(srcs), SRC_DIR"%s", src_files[i]);
        cmd_append(cmd, strdup(srcs));
    }
    cmd_append(cmd, SRC_BUILD_DIR"tinyfiledialogs.o");
    cmd_append(cmd, "-I","C:\\Program Files (x86)\\raylib\\include");
    cmd_append(cmd, "-I",".\\headers");
    cmd_append(cmd, "-I",".");
    cmd_append(cmd, "-L",".\\libs");
    cmd_append(cmd, "-lraylib", "-lopengl32", "-lgdi32", "-lwinmm");
    cmd_append(cmd, "-lcomdlg32", "-lole32", "-luuid", "-lshlwapi");
}

int main(int argc, char **argv) {
    GO_REBUILD_URSELF(argc, argv);

    (void) argc;
    (void) argv;
    cmd.count = 0;
    build_thirdparty(&cmd);
    if (!cmd_run(&cmd)) return 1;

    mkdir_if_not_exists("./build");
    build(&cmd);

    nob_copy_directory_recursively("./res", "./build/res");
    if (!cmd_run(&cmd)) return 1; 
    return 0;
}
