#ifdef _WIN32
#  define DELIMITER ";"
#else
#  define DELIMITER ":"
#endif

#include "pyxie.h"
#include "pythonStarter.h"
#include <string>
#include <Python.h>

#include "pyxieFios.h"
#include "pyxieSystemInfo.h"

const char showerror[] = 
    "import igeCore as core\n"\
    "import imgui\n"\
    "from igeCore.apputil.imguirenderer import ImgiIGERenderer\n"\
    "import os\n"\
    "def tail(f, lines=1, _buffer=4098):\n"\
    "  lines_found = []\n"\
    "  block_counter = -1\n"\
    "  while len(lines_found) < lines:\n"\
    "    try:\n"\
    "      f.seek(block_counter * _buffer, os.SEEK_END)\n"\
    "    except IOError:\n"\
    "      f.seek(0)\n"\
    "      lines_found = f.readlines()\n"\
    "      break\n"\
    "    lines_found = f.readlines()\n"\
    "    block_counter -= 1\n"\
    "  return lines_found[-lines:]\n"\
    "core.window(True, 480, 640, False)\n"\
    "imgui.create_context()\n"\
    "impl = ImgiIGERenderer()\n"\
    "while True:\n"\
    "  core.update()\n"\
    "  screenW, screenH = core.viewSize()\n"\
    "  impl.process_inputs()\n"\
    "  imgui.new_frame()\n"\
    "  imgui.set_next_window_size(screenW, screenH - 20)\n"\
    "  imgui.set_next_window_position(0, 20)\n"\
    "  imgui.begin('Main Window', False,\n"\
    "    flags = imgui.WINDOW_NO_TITLE_BAR +\n"\
    "    imgui.WINDOW_NO_RESIZE + imgui.WINDOW_NO_MOVE +\n"\
    "    imgui.WINDOW_NO_COLLAPSE)\n"\
    "  imgui.set_next_window_size(screenW*2, screenH - 20)\n"\
    "  imgui.begin_child('region', border=True,\n"\
    "    flags=imgui.WINDOW_ALWAYS_HORIZONTAL_SCROLLBAR +\n"\
    "    imgui.WINDOW_ALWAYS_VERTICAL_SCROLLBAR)\n"\
    "  f = open('error.log')\n"\
    "  lines = tail(f, 200)\n"\
    "  for line in lines:\n"\
    "    imgui.text(line)\n"\
    "  imgui.end_child()\n"\
    "  imgui.end()\n"\
    "  imgui.render()\n"\
    "  impl.render(imgui.get_draw_data())\n"\
    "  core.swap()";

using namespace pyxie;

PythonStarter::PythonStarter() { }

PythonStarter::~PythonStarter() {}

void PythonStarter::Run() {
    std::string root = pyxieFios::Instance().GetRoot();
    wchar_t pathw[1024];
    std::string path = root;
    mbstowcs(pathw, path.c_str(), 1024);
    Py_SetPythonHome(pathw);
    path.append(DELIMITER);
    path.append(root);
    path.append("PyLib");
    path.append(DELIMITER);
    path.append(root);
    path.append("PyLib/site-packages");
    mbstowcs(pathw, path.c_str(), 1024);
    Py_SetPath(pathw);

    path = root + "error.log";
    remove(path.c_str());
    pyxie_logg_start();

    Py_Initialize();
    FILE* exp_file = fopen("root.py", "r");
    PyRun_SimpleFile(exp_file, "root.py");
}

void PythonStarter::ShowError() {
    std::string root = pyxieFios::Instance().GetRoot();
    wchar_t pathw[1024];
    std::string path = root;
    mbstowcs(pathw, path.c_str(), 1024);
    Py_SetPythonHome(pathw);
    path.append(DELIMITER);
    path.append(root);
    path.append("PyLib");
    path.append(DELIMITER);
    path.append(root);
    path.append("PyLib/site-packages");
    mbstowcs(pathw, path.c_str(), 1024);
    Py_SetPath(pathw);

    pyxieSystemInfo::Instance().SetGemeScreenSize(960.0f);
    pyxieFios::Instance().SetRoot(root.c_str());
    PyRun_SimpleString(showerror);
    Py_Finalize();
}
