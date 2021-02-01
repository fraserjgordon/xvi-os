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

project = 'XVI'
copyright = '2018-2020, Fraser Gordon'
author = 'Fraser Gordon'
language = 'en'


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    'breathe',
    'sphinx.ext.autosectionlabel'
]

# Add any paths that contain templates here, relative to this directory.
templates_path = ['Resources/sphinx/templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

# Assume code is C++ unless marked otherwise.
primary_domain = 'cpp'
highlight_language = 'c++'

# Be strict about references; these docs should be self-contained.
nitpicky = True


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'sphinx_rtd_theme'

html_theme_options = {
    'style_external_links': True
}

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['Resources/sphinx/static']


# -- Options for Latex output ------------------------------------------------

# Unicode support is required.
latex_engine = 'xelatex'
latex_elements = {
    'papersize': 'a4paper',
    'preamble': r'\usepackage[math-style=literal]{unicode-math}'
}


# -- Breathe options ---------------------------------------------------------

# Doxygen project
breathe_default_project = 'XVI'


# -- Auto section label options ----------------------------------------------

# Use the document name as a label prefix to ensure uniqueness.
autosectionlabel_prefix_document = True
