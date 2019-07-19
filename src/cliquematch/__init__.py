"""
NIST Forensic Footwear Team
Project Folder: cliquematch
Filename: __init__.py
Author: gnv3 (Gautham Venkatasubramanian)
Created on: 11/2/18 2:07 PM

__init__.py used for:

Setting up the cliquematch package
"""

name = "cliquematch_v0.6.0"

from .core import Graph

try:
    from .core import psgraph
except Exception as e:
    from .psgraph import psgraph
from .A2AGraph import A2AGraph
from .A2LGraph import A2LGraph
from .L2LGraph import L2LGraph
from .L2AGraph import L2AGraph
from .IsoGraph import IsoGraph
