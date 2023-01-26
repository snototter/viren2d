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
  'sphinx_copybutton',  # Copy button for code examples
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
#html_theme = "furo"


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
#html_static_path = []
html_static_path = ['_static']


# Include operators in autodoc
# Taken from https://stackoverflow.com/a/5599712/400948
def autodoc_skip_member(app, what, name, obj, would_skip, options):
    if name == "__init__":
        return False
    # Include overloaded operators:
    if name in ["__add__", "__call__", "__eq__", "__getstate__", "__iadd__", "__imul__",
            "__getitem__", "__setitem__", "__isub__", "__itruediv__", "__mul__", "__ne__", "__neg__", "__rmul__",
            "__sub__", "__truediv__", "__setstate__"]:
        return False
    return would_skip


# Provides a summary table with links to the most important
# interface functionality, i.e. Painter.draw_xxx methods.
from docutils import nodes
from docutils.parsers.rst import Directive, directives
from docutils.statemachine import StringList
from sphinx.util.nodes import nested_parse_with_titles
from sphinx.util.typing import OptionSpec
import inspect


def get_linenumber():
    cf = inspect.currentframe()
    return cf.f_back.f_lineno


class Viren2dDrawingSummary(Directive):
    has_content = True

    def run(self):
        tdraw = self._create_table_drawing_methods()
        return tdraw

    
    def _create_table_drawing_methods(self):
        """
        Creates a rst-formatted StringList, which is
        then parsed and returned as a `nodes.table` instance.
        I didn't find an easier way to resolve the code doc links.
        """
        def should_include(fx):
            return fx.startswith('draw_') or fx.startswith('set_clip') or (fx == 'reset_clip')

        draw_fxs = sorted(
            [f for f in dir(viren2d.Painter)
             if callable(getattr(viren2d.Painter, f))
             and should_include(f)])
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


class Viren2dColorNamesTable(Directive):
    required_arguments = 0
    optional_arguments = 0
    has_content = False

    def run(self):
        """Creates the color name table (name + hex + rgb + visualization)."""
        table = nodes.table()
        tgroup = nodes.tgroup(cols=4)
        for _ in range(4):
            colspec = nodes.colspec(colwidth=1)
            tgroup.append(colspec)
        table += tgroup

        # Header
        thead = nodes.thead()
        tgroup += thead
        row = nodes.row()
        entry = nodes.entry()
        entry += nodes.paragraph(text="Name")
        row += entry

        entry = nodes.entry()
        entry += nodes.paragraph(text="Hex code")
        row += entry

        entry = nodes.entry()
        entry += nodes.paragraph(text="RGB")
        row += entry

        entry = nodes.entry()
        entry += nodes.paragraph(text="rgb")
        row += entry
        thead.append(row)

        rows = list()
        for cname in viren2d.color_names():
            color = viren2d.Color(cname)

            row = nodes.row()
            rows.append(row)

            # First column, the name + link to online color lookup
            hex = color.to_hex(False)  # hex code without alpha
            url = f'https://www.google.com/search?q=%23{hex[1:]}'

            entry = nodes.entry()
            ref = nodes.reference(name=f'lookup-{hex[1:]}', refuri=url, target='_blank')
            ref += nodes.paragraph(text=cname)
            txt = nodes.inline()
            txt += ref
            entry += txt
            row += entry

            # Next column, hex code
            entry = nodes.entry()
            color_block = nodes.inline(classes=['colorblock', f'bgc{hex[1:]}'])
            entry += color_block
            # entry += nodes.literal(text=hex)
            entry += nodes.Text(hex)
            row += entry

            # Next column, RGB representation
            RGBa = color.to_RGB()
            txt_RGB = f'({RGBa[0]}, {RGBa[1]}, {RGBa[2]})'

            entry = nodes.entry()
            # entry += nodes.literal(text=txt_RGB)
            entry += nodes.Text(txt_RGB)
            row += entry

            # Next column, rgb representation
            txt_rgb = f'({color.red:3.2f}, {color.green:3.2f}, {color.blue:3.2f})'
            entry = nodes.entry()
            # entry += nodes.literal(text=txt_rgb)
            entry += nodes.Text(txt_rgb)
            row += entry

        tbody = nodes.tbody()
        tbody.extend(rows)
        tgroup += tbody

        return [table]


class Viren2dColorObjectCategoriesTable(Directive):
    required_arguments = 0
    optional_arguments = 0
    has_content = False

    # option_spec: OptionSpec = {
    #     'refertomethod': directives.flag
    # }

    def run(self):
        """Arranges the available category names in a table."""
        # Fixed number of columns
        num_cols = 4

        table = nodes.table()
        tgroup = nodes.tgroup(cols=num_cols)
        for _ in range(num_cols):
            colspec = nodes.colspec(colwidth=1)
            tgroup.append(colspec)
        table += tgroup

        rows = list()
        cnames = viren2d.object_category_names()
        row = None
        for idx in range(len(cnames)):
            if (idx % num_cols) == 0:
                # Create new row
                row = nodes.row()
                rows.append(row)
            # Place a fixed-sized span of the corresponding color before the
            # category name
            color = viren2d.Color.from_object_category(cnames[idx])
            hex = color.to_hex(False)  # hex code without alpha
            color_block = nodes.inline(classes=['colorblock', f'bgc{hex[1:]}'])

            entry = nodes.entry()
            entry += color_block
            entry += nodes.Text(cnames[idx])
            # entry += nodes.literal(text=cnames[idx])
            row += entry

        # Fill the last row with empty cells
        rem = num_cols - (len(cnames) % num_cols)
        for _ in range(rem):
            entry = nodes.entry()
            entry += nodes.Text('')
            row += entry

        tbody = nodes.tbody()
        tbody.extend(rows)
        tgroup += tbody

        return [table]


class Viren2dColorObjectIDsTable(Directive):
    required_arguments = 0
    optional_arguments = 0
    has_content = False

    def run(self):
        """Lists the first few colors for the object ID lookup."""
        # Fixed table size
        num_cols = 7
        num_rows = 6
        num_colors = num_rows * num_cols

        table = nodes.table()
        tgroup = nodes.tgroup(cols=num_cols)
        for _ in range(num_cols):
            colspec = nodes.colspec(colwidth=1)
            tgroup.append(colspec)
        table += tgroup

        rows = list()
        row = None
        for idx in range(num_colors):
            if (idx % num_cols) == 0:
                # Create new row
                row = nodes.row()
                rows.append(row)
            # Place a fixed-sized span of the corresponding color before the
            # category name
            color = viren2d.Color.from_object_id(idx)
            hex = color.to_hex()[:-2]  # hex code without alpha
            color_block = nodes.inline(classes=['colorblock', f'bgc{hex[1:]}'])

            entry = nodes.entry()
            entry += color_block
            entry += nodes.Text(str(idx))
            # entry += nodes.literal(text=str(idx))
            row += entry

        tbody = nodes.tbody()
        tbody.extend(rows)
        tgroup += tbody

        info = f"With the default color map, these are the colors for the first {num_colors} object IDs:"
        return [nodes.paragraph(text=info), table]

import sys
import os
sys.path.append(os.path.join(os.path.abspath(os.path.dirname(__file__)), '..', '..', 'examples', 'rtd-examples-python'))
from rtd_demo_images.constants import VIREN2D_COLORMAP_CATEGORIES

class Viren2dColorMapsTable(Directive):
    required_arguments = 0
    optional_arguments = 0
    has_content = False

    option_spec: OptionSpec = {
        k: directives.flag
        for k in VIREN2D_COLORMAP_CATEGORIES
    }

    def run(self):
        # Collect all color maps to show
        colormaps = list()
        for opt in self.options:
            if opt in VIREN2D_COLORMAP_CATEGORIES:
                colormaps.extend(VIREN2D_COLORMAP_CATEGORIES[opt])

        table = nodes.table()
        tgroup = nodes.tgroup(cols=2)
        for _ in range(2):
            colspec = nodes.colspec(colwidth=1)
            tgroup.append(colspec)
        table += tgroup

        # Header
        thead = nodes.thead()
        tgroup += thead
        row = nodes.row()
        thead.append(row)
        entry = nodes.entry()
        entry += nodes.paragraph(text="Name")
        row += entry

        entry = nodes.entry()
        entry += nodes.paragraph(text="Gradient")
        row += entry

        # Body
        env = self.state.document.settings.env
        rows = list()
        for cmap in colormaps:
            row = nodes.row()
            rows.append(row)

            entry = nodes.entry()
            entry += nodes.literal(text=cmap)
            row += entry

            entry = nodes.entry()
            image = nodes.image(
                uri=f'../images/colormap-gradient-{cmap}.png', 
                remote=False,
                size=(256, 40), align='center', alt='Rendered color gradient',
                show_caption=False)
            entry += image
            row += entry

            env.images.add_file('', image['uri'])


        tbody = nodes.tbody()
        tbody.extend(rows)
        tgroup += tbody

        return [table]


def setup(app):
    # I want to include overloaded operators in autodoc
    app.connect("autodoc-skip-member", autodoc_skip_member)
    app.add_css_file("dark.css")

    # Custom directives
    app.add_directive("viren2d-drawing-summary", Viren2dDrawingSummary)
    app.add_directive("viren2d-color-names-table", Viren2dColorNamesTable)
    app.add_directive("viren2d-object-category-names", Viren2dColorObjectCategoriesTable)
    app.add_directive("viren2d-color-by-id-defaults", Viren2dColorObjectIDsTable)
    app.add_directive("viren2d-color-map-table", Viren2dColorMapsTable)

