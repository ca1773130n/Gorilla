# ------------------------------------------------------------------------------
# Girl material shader node for Maya
#
# by Doug Hammond 05/2008
#
# This file is licensed under the GPL
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# $Id: mirrorShader.py,v 1.1 2008/06/08 12:57:41 dougal2 Exp $
#
# ------------------------------------------------------------------------------
#
# Girl material shader node for Maya ( mirror attributes )
#
# ------------------------------------------------------------------------------

from maya import OpenMaya
from maya import OpenMayaMPx

from Girl.GirlNodes.ShaderNode import ShaderNode
from Girl.GirlNodes.GirlNode import ShaderColorAttribute

class mirrorShader(OpenMayaMPx.MPxNode, ShaderNode):
    """
    Mirror fragment of GirlShaderGirlShaderGirlShader
    """
    
    def __init__(self):
        OpenMayaMPx.MPxNode.__init__(self)

        # mirror
        kr            =    OpenMaya.MObject()    # color
        
        # Export attributes
        self.attributes = {}
        self.GirlType = "mirror"
        self.attributes['Kr'] = ShaderColorAttribute('mirrorKr')

    @staticmethod
    def shaderInitializer():
        try:
            # reflectiviy
            mirrorShader.kr = mirrorShader.makeColor("mirrorKr", "mikr")    # mkr is already in use ??!

        except:
            OpenMaya.MGlobal.displayError("Failed to create mirror attributes\n")
            raise
