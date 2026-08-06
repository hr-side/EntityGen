#include <direct.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#define NOB_IMPLEMENTATION
#include "nob.h"

// TODO: We should build raylib our self to make sure we have it
// see tsoding/musilizer.

#ifdef _WIN32
#   define COMPILER "gcc.exe"
#   define HEADERS_DIR ".\\headers\\"
#   define THIREDPARTY_LIBS_BUILD_DIR   ".\\THIREDPARTY_LIBS_BUILD_DIR\\"
#   define BUILD_DIR       ".\\build\\"
#   define THIRDPARTY_DIR  ".\\thirdparty\\"
#   define build(cmd) build_win(cmd)
#   define SRC_DIR         ".\\src\\"
#else
#   define BUILD_DIR       "./build/"
#   define THIREDPARTY_LIBS_BUILD_DIR   ".\\THIREDPARTY_LIBS_BUILD_DIR\\"
#   define THIRDPARTY_DIR  "./thirdparty/"
#   define SRC_DIR         "./src/"
#   define COMPILER "cc"
#   define HEADERS_DIR "./headers"
#   define build(cmd) build_linux(cmd)
#endif

Cmd cmd = {0};

const char* thirdparty[] = {
    "tinyfiledialogs",
    "cJSON\\cJSON",
    "cJSON\\cJSON_Utils"
};
const char* src_files[] = {
    "main.c",
    "ui.c",
    "canvas.c", 
    "data.c",
    "Info_Panel.c",
    "Animations_Panel.c",
    "Animation_Panel.c",
    "parse.c"
};

int build_thirdparty(Cmd *cmd, size_t i)
{
    mkdir_if_not_exists(THIREDPARTY_LIBS_BUILD_DIR);
    mkdir_if_not_exists(THIREDPARTY_LIBS_BUILD_DIR"cJSON\\");

    cmd_append(cmd, COMPILER);
    cmd_append(cmd, "-w");
    cmd_append(cmd, "-c");

    char src_path[512];
    snprintf(src_path, sizeof(src_path), THIRDPARTY_DIR"%s.c", thirdparty[i]);
    cmd_append(cmd, strdup(src_path));

    cmd_append(cmd, "-o");

    char obj_path[512]; 
    snprintf(obj_path, sizeof(obj_path), THIREDPARTY_LIBS_BUILD_DIR"%s.o", thirdparty[i]);
    cmd_append(cmd, strdup(obj_path));

    cmd_append(cmd, "-I", HEADERS_DIR);

    if (!cmd_run(cmd)) return 1;
    return 0;
}

void build_parser(Cmd *cmd)
{
    cmd_append(cmd, COMPILER);
    cmd_append(cmd, "-Wall", "-Wextra");
    cmd_append(cmd, SRC_DIR"parse.c");
    cmd_append(cmd, THIREDPARTY_LIBS_BUILD_DIR"cJSON\\cJSON.o");
    cmd_append(cmd, THIREDPARTY_LIBS_BUILD_DIR"cJSON\\cJSON_Utils.o");
    cmd_append(cmd, "-o", BUILD_DIR"parse.exe");
    cmd_append(cmd, "-I", HEADERS_DIR, "-I", ".");
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
    cmd_append(cmd, THIREDPARTY_LIBS_BUILD_DIR"tinyfiledialogs.o");
    cmd_append(cmd, THIREDPARTY_LIBS_BUILD_DIR"cJSON/cJSON.o");
    cmd_append(cmd, THIREDPARTY_LIBS_BUILD_DIR"cJSON/cJSON_Utils.o");
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
    cmd_append(cmd, THIREDPARTY_LIBS_BUILD_DIR"tinyfiledialogs.o");
    cmd_append(cmd, THIREDPARTY_LIBS_BUILD_DIR"cJSON\\cJSON.o");
    cmd_append(cmd, THIREDPARTY_LIBS_BUILD_DIR"cJSON\\cJSON_Utils.o");
    cmd_append(cmd, "-I","C:\\Program Files (x86)\\raylib\\include");
    cmd_append(cmd, "-I",".\\headers");
    cmd_append(cmd, "-I",".");
    cmd_append(cmd, "-L",".\\libs");
    cmd_append(cmd, "-lraylib", "-lopengl32", "-lgdi32", "-lwinmm");
    cmd_append(cmd, "-lcomdlg32", "-lole32", "-luuid", "-lshlwapi");
}

bool delete_walk_entry(Nob_Walk_Entry entry)
{
    return nob_delete_file(entry.path);
}

bool delete_directory_recursively(const char *dir_path)
{
    return nob_walk_dir(dir_path, delete_walk_entry, .post_order = true);
}

int main(int argc, char **argv) {
    GO_REBUILD_URSELF(argc, argv);

    (void) argc;
    (void) argv;
    cmd.count = 0;

    mkdir_if_not_exists(THIREDPARTY_LIBS_BUILD_DIR);
    mkdir_if_not_exists(THIREDPARTY_LIBS_BUILD_DIR"cJSON\\");
    build_thirdparty(&cmd, 0);
    build_thirdparty(&cmd, 1);
    build_thirdparty(&cmd, 2);

    mkdir_if_not_exists("./build");

    build(&cmd);
    nob_copy_directory_recursively("./res", "./build/res");
    if (!cmd_run(&cmd)) return 1; 
    // Remove Uneeded Build Artifacts.
    if(!delete_directory_recursively(THIREDPARTY_LIBS_BUILD_DIR)) return 1;

    return 0;
}
