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
#  'sphinx_rtd_dark_mode',
  'sphinx.ext.autodoc',
  'sphinx.ext.autosummary',
  'sphinx.ext.napoleon'  # Parse Google and NumPy docstrings
]


# -- Options for the extension modules ---------------------------------------
# First page visit should show the dark mode theme:
# https://pypi.org/project/sphinx-rtd-dark-mode/
default_dark_mode = True

autosummary_generate = True



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

#import sphinx_pdj_theme
#html_theme = 'sphinx_pdj_theme'
#htm_theme_path = [sphinx_pdj_theme.get_html_theme_path()]
## sets the darker appearence
#html_theme_options = {
#    'style': 'darker'
#}
html_theme = "sphinx_rtd_theme"

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']




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


