/*  This file is part of asshdr.
 *
 *  asshdr is free software: you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation,
 *  either version 2 of the License,
 *  or (at your option) any later version.
 *
 *  asshdr is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public
 *  License along with assfonts. If not, see <https://www.gnu.org/licenses/>.
 *  
 *  written by wyzdwdz (https://github.com/wyzdwdz)
 */

#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <CLI/CLI.hpp>
#include <nowide/args.hpp>
#include <nowide/fstream.hpp>
#include <nowide/iostream.hpp>

#include "ass_recolorize.h"
#include "ver.h"

namespace fs = std::filesystem;

int main(int argc, char** argv) {
  nowide::args(argc, argv);

  std::vector<std::string> inputs;
  std::string output;
  unsigned int brightness = 203;
  bool is_help = false;

  CLI::App app{"Recolorize ASS subtitle for HDR contents."};
  auto* p_opt_i =
      app.add_option("-i,--input,input", inputs, "Input .ass files");
  auto* p_opt_o = app.add_option("-o,--output", output, "Output directory");
  auto* p_opt_b =
      app.add_option("-b,--brightness", brightness, "Target brightness");
  app.set_help_flag("");
  app.add_flag("-h,--help", is_help, "Get help info");
  p_opt_i->type_name("<files>");
  p_opt_o->type_name("<dir>");
  p_opt_b->type_name("<num>");
  app.failure_message(
      [=](const CLI::App* app, const CLI::Error& e) -> std::string {
        std::string str = CLI::FailureMessage::simple(app, e);
        str.pop_back();
        str.append(". See --help for more info.");
        nowide::cout << "[ERROR] " << str << '\n';
        return "";
      });
  CLI11_PARSE(app, argc, argv);

  if (is_help) {
    // clang-format off
    nowide::cout 
    << "asshdr v" << VERSION_MAJOR << '.' << VERSION_MINOR << '.' << VERSION_PATCH << '\n'
    << "Recolorize ASS subtitle for HDR contents.\n"
    << "Usage:     asshdr [options...] [<files>]\n"
    << "Examples:  assfonts <files>\n"
    << "           assfonts -i <files>\n"
    << "           assfonts -o <dir> -i <files>\n"
    << "           assfonts -b <num> -o <dir> -i <files>\n"
    << "Options:\n"
    << "  -i, --input,       <files>           Input .ass files\n"
    << "  -o, --output       <dir>             Output directory\n"
    << "                                      (Default: same directory as input)\n"
    << "  -b, --brightness   <num (0-1000)>    Target brightness for recoloring\n"  
    << "                                      (Default: 203)\n"
    << "  -h, --help                           Get help info\n\n";
    // clang-format on
  }

  if (!is_help && inputs.empty()) {
    nowide::cout << "[ERROR] --input is required. See --help for more info.\n";
    return 0;
  }

  std::error_code ec;
  fs::path output_path = fs::absolute(output, ec);
  if (!output.empty() && !fs::is_directory(output_path)) {
    nowide::cout
        << '\"' << output << '\"'
        << " is not a legal directory path. See --help for more info.\n";
    return 0;
  }

  for (const std::string& input : inputs) {
    fs::path input_path = fs::absolute(input, ec);
    if (!input.empty() && !fs::is_regular_file(input_path)) {
      nowide::cout << "[ERROR] \"" << input << '\"'
                   << " is not a file. See --help for more info.\n";
      return 0;
    }
    if (!fs::is_directory(output_path)) {
      output_path = input_path.parent_path();
    }
    nowide::ifstream is(input_path.u8string());
    std::stringstream sstream;
    sstream << is.rdbuf();
    std::string ass_text(sstream.str());
    unsigned int out_size = ass_text.size() * 2;
    std::unique_ptr<char> out_text(new char[out_size]);
    asshdr::AssRecolor(ass_text.c_str(), ass_text.size(), out_text.get(),
                       out_size, brightness);
    nowide::ofstream os(output_path.u8string() + fs::path::preferred_separator +
                        input_path.stem().u8string() + ".hdr" +
                        input_path.extension().u8string());
    os << std::string(out_text.get(), out_size);
  }

  return 0;
}