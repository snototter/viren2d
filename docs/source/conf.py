# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))


# -- Project information -----------------------------------------------------

project = 'viren2d'
copyright = '2022, snototter'
author = 'snototter'

from multiprocessing import dummy
import viren2d
version = viren2d.__version__
release = version


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
  'sphinx.ext.autodoc',
  'sphinx.ext.autosummary',
  'sphinx.ext.napoleon',  # Parse Google and NumPy docstrings
  'sphinx.ext.mathjax',
  'sphinx.ext.intersphinx',
  'autodocsumm', # adds TOC for autosummary: https://autodocsumm.readthedocs.io/en/latest/index.html
]


# -- Options for the extension modules ---------------------------------------

autosummary_generate = True

# Don't enable autodoc by default --> need to check how this would change
# the enum class doc
#autodoc_default_options = {
#  'autosummary': True,
#}

intersphinx_mapping = {
  'python': ('https://docs.python.org/', None),
  'numpy': ('http://docs.scipy.org/doc/numpy/', None)
}


# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = []


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
#html_theme = 'alabaster'

html_theme = "sphinx_rtd_theme"


# Theme options are theme-specific and customize the look and feel of a theme
# further.  For a list of options available for each theme, see the
# documentation.
# Documentation of the sphinx_rtd_theme: https://sphinx-rtd-theme.readthedocs.io/en/stable/configuring.html
html_theme_options = {
    "collapse_navigation" : False,
    "prev_next_buttons_location" : "both"
}


# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = []
#html_static_path = ['_static']


# Include operators in autodoc
# Taken from https://stackoverflow.com/a/5599712/400948
def autodoc_skip_member(app, what, name, obj, would_skip, options):
    if name == "__init__":
        return False
    # Include overloaded operators:
    if name in ["__add__", "__eq__", "__getstate__", "__iadd__", "__imul__",
            "__getitem__", "__setitem__", "__isub__", "__itruediv__", "__mul__", "__ne__", "__neg__", "__rmul__",
            "__sub__", "__truediv__", "__setstate__"]:
        return False
    return would_skip


# Provides a summary table with links to the most important
# interface functionality, e.g. Painter.draw_xxx methods, maybe later
# on pseudocoloring, ...
from docutils import nodes
from docutils.parsers.rst import Directive, directives
from docutils.statemachine import StringList
from sphinx.util.nodes import nested_parse_with_titles
from inspect import currentframe

def get_linenumber():
    cf = currentframe()
    return cf.f_back.f_lineno

class Viren2dDrawingSummary(Directive):
    has_content = True

    def run(self):
        tdraw = self._create_table_drawing_methods()
        return tdraw

    
    def _create_table_drawing_methods(self):
        """
        Creates a rst-formatted StringList, which is
        then parsed and returned as a `nodes.table` instance
        I didn't find an easier way to resolve the code doc links.
        """
        draw_fxs = sorted(
            [f for f in dir(viren2d.Painter)
             if callable(getattr(viren2d.Painter, f))
             and f.startswith('draw_')])
        max_fname_len = max([len(f) for f in draw_fxs])

        docstr = []
        for f in draw_fxs:
            m = getattr(viren2d.Painter, f)
            # Find the first non-empty line after the method's signature
            lines = list(
                filter(len, [l.strip() for l in m.__doc__.split('\n')[1:10]]))
            if len(lines) > 0:
                docstr.append(lines[0])
            else:
                docstr.append('')
        max_summary_len = max([len(d) for d in docstr])
        # We add 25 chars in col 0 for the markup :meth:`~viren2d.Painter.`
        max_fname_len += 25
        row_fmt_str = f'{{name:{max_fname_len}s}}  {{summary:s}}'
        ruler = max_fname_len * '=' + '  ' + max_summary_len * '='

        lines = [ruler]
        for idx in range(len(draw_fxs)):
            frst = f':meth:`~viren2d.Painter.{draw_fxs[idx]}`'
            srst = docstr[idx]
            lines.append(row_fmt_str.format(name=frst, summary=srst))
        lines.append(ruler)

        self.content = StringList(lines)
        txt = '\n'.join(self.content)
        node = nodes.table(txt)
        self.add_name(node)
        self.state.nested_parse(self.content, self.content_offset, node)
        return node.children


def setup(app):
    # I want to include overloaded operators in autodoc
    app.connect("autodoc-skip-member", autodoc_skip_member)

    # Custom directives
    app.add_directive("viren2d-drawing-summary", Viren2dDrawingSummary)
