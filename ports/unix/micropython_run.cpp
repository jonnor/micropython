#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <emscripten/emscripten.h>

// From libmicropython.a
extern "C" {
    int micropython_unix_main(int argc, char **argv);
}

namespace py = pybind11;

void convert_to_argv(const std::vector<std::string>& args, int& argc, char*** argv) {
    argc = args.size();
    *argv = new char*[argc + 1];

    int i = 0;
    for (const auto& s : args) {
        (*argv)[i++] = strdup(s.c_str());
    }
    (*argv)[argc] = nullptr;
}

// Don’t forget to free argv later
void free_argv(int argc, char** argv) {
    for (int i = 0; i < argc; ++i) {
        free(argv[i]);
    }
    delete[] argv;
}


// This is just a stub for now.
// Later, you can initialize MicroPython and run the code inside this function.
void run_file(std::vector<std::string> args) {
    // TODO: hook into MicroPython C API here
    // For now, just print it

    int argc;
    char** argv;
    convert_to_argv(args, argc, &argv);

    printf("run_file: \n");
    for (int i = 0; i < argc; ++i) {
        printf("%s\n", argv[i]);
    }

    micropython_unix_main(argc, argv);

    free_argv(argc, argv);
}


int hello(const std::string &path) {

    printf("hello %s\n", path.c_str());
    fprintf(stderr, "stderr check %s\n", path.c_str());

    emscripten_log(EM_LOG_CONSOLE, "emscripted log test %s\n", path.c_str());


    return 1337;
}

PYBIND11_MODULE(micropython_run, m) {
    m.doc() = "MicroPython runner module (skeleton using pybind11)";
    m.def("run_file", &run_file, "Run MicroPython file");
    m.def("hello", &hello, "Check if we can run any code");
}
