#include <sstream>
#include <vector>

#include <bindings/binding_helpers.h>

//FIXME remove
//#include <werkzeugkiste/files/filesys.h>
//#include <werkzeugkiste/files/fileio.h>
#include <werkzeugkiste/strings/strings.h>

#include <helpers/logging.h>

namespace viren2d {
namespace bindings {
namespace examples {
std::vector<std::string> CodeExample(const std::string &snippet_name) {
  if (snippet_name.compare("colorization-scaled") == 0) {
    const char *code =
    #include <bindings/docstr-snippets/colorization-scaled.inc>
    ;
    return werkzeugkiste::strings::Split(code, '\n');
  } else if (snippet_name.compare("colorization-categorical") == 0) {
    const char *code =
    #include <bindings/docstr-snippets/colorization-categorical.inc>
    ;
    return werkzeugkiste::strings::Split(code, '\n');
  } else {
    return std::vector<std::string>();
  }
}
} // namespace examples


std::string DocstringCodeExample(
    const std::string &snippet_name,
    const std::string &block_prefix,
    std::size_t line_indentation,
    const std::string &line_prefix) {

  std::ostringstream code_block;
  if (!block_prefix.empty()) {
    code_block << block_prefix;
  }

  for (const auto &line : examples::CodeExample(snippet_name)) {
    for (std::size_t cnt = 0; cnt < line_indentation; ++cnt) {
      code_block << ' ';
    }

    if (!line_prefix.empty()) {
      code_block << line_prefix;
    }

    code_block << werkzeugkiste::strings::RTrim(line) << '\n';
  }
  return code_block.str();
}

} // namespace bindings
} // namespace viren2d

