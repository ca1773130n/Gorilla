# ------------------------------------------------------------------------------
# Girl material shader node for Maya
#
# by Doug Hammond 05/2008
#
# This file is licensed under the GPL
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# $Id: nullShader.py,v 1.1 2008/06/08 12:57:42 dougal2 Exp $
#
# ------------------------------------------------------------------------------
#
# Girl material shader node for Maya ( null attributes )
#
# ------------------------------------------------------------------------------

from maya import OpenMaya
from maya import OpenMayaMPx

from Girl.GirlNodes.ShaderNode import ShaderNode

class nullShader(OpenMayaMPx.MPxNode, ShaderNode):
    """
    Null fragment of GirlShader
    """
    
    def __init__(self):
        OpenMayaMPx.MPxNode.__init__(self)

        # Export attributes
        self.attributes = {}
        self.GirlType = "null"

    @staticmethod
    def shaderInitializer():
        # nothing to do... duh
        pass