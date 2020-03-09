"""
NIST Forensic Footwear Team
Project Folder: cliquematch
Filename: __init__.py
Author: gnv3 (Gautham Venkatasubramanian)
Created on: 11/2/18 2:07 PM

__init__.py used for:

Setting up the cliquematch package
"""

VERSION = "0.7.0"

from cliquematch.core import (
    Graph,
    graph_from_file,
    graph_from_adjmat,
    graph_from_edgelist,
)

from cliquematch.psgraph import psgraph
from cliquematch.A2AGraph import A2AGraph
from cliquematch.AlignGraph import AlignGraph
from cliquematch.A2LGraph import A2LGraph
from cliquematch.L2LGraph import L2LGraph
from cliquematch.L2AGraph import L2AGraph
from cliquematch.IsoGraph import IsoGraph
