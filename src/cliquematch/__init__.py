"""
NIST Forensic Footwear Team
Project Folder: cliquematch
Filename: __init__.py
Author: gnv3 (Gautham Venkatasubramanian)
Created on: 11/2/18 2:07 PM

__init__.py used for:

Setting up the cliquematch package
"""

from cliquematch.core import (
    Graph,
    graph_from_file,
    graph_from_adjmat,
    graph_from_edgelist,
)
from .wrappers import *
