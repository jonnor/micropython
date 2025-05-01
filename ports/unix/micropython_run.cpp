#include <pybind11/pybind11.h>

// From libmicropython.a
extern "C" {
    int micropython_unix_main(int argc, char **argv);
}

namespace py = pybind11;

// This is just a stub for now.
// Later, you can initialize MicroPython and run the code inside this function.
void run_file(const std::string &path) {
    // TODO: hook into MicroPython C API here
    // For now, just print it

    // FIXME: copy these things so they are safely mutable
    char *argv[2] = { "micropython", (char *)path.c_str() };
    micropython_unix_main(2, argv);

}

PYBIND11_MODULE(micropython_run, m) {
    m.doc() = "MicroPython runner module (skeleton using pybind11)";
    m.def("run_file", &run_file, "Run MicroPython file");
}
