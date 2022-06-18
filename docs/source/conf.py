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
  'autodocsumm', # adds TOC for autosummary: https://autodocsumm.readthedocs.io/en/latest/index.html
]


# -- Options for the extension modules ---------------------------------------

autosummary_generate = True

#TODO if set here, the toc comes AFTER the __init__ docs :/
#autodoc_default_options = {
#  'autosummary': True,
#}



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
html_theme_options = {
    "collapse_navigation" : False
}


# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = []
#html_static_path = ['_static']


#https://stackoverflow.com/a/5599712/400948
def skip(app, what, name, obj, would_skip, options):
    if name == "__init__":
        return False
    # Include overloaded operators:
    if name in ["__add__", "__eq__", "__getstate__", "__iadd__", "__imul__",
            "__isub__", "__itruediv__", "__mul__", "__ne__", "__rmul__",
            "__sub__", "__truediv__", "__setstate__"]:
        return False
    return would_skip

def setup(app):
    app.connect("autodoc-skip-member", skip)

#autodoc_default_flags = ['members', 'private-members', 'special-members',
#                         #'undoc-members',
#                         'show-inheritance']

#def autodoc_skip_member(app, what, name, obj, skip, options):
#    # Ref: https://stackoverflow.com/a/21449475/
#    exclusions = ('__weakref__',  # special-members
#                  '__doc__', '__module__', '__dict__',  # undoc-members
#                  )
#    exclude = name in exclusions
#    print(f'what {what}::: name {name}::: obj {obj}::: skip {skip}')
#    # return True if (skip or exclude) else None  # Can interfere with subsequent skip functions.
#    return True if exclude else None
 
#def setup(app):
#    app.connect('autodoc-skip-member', autodoc_skip_member)


