# ------------------------------------------------------------------------------
# Girl material shader node for Maya
#
# by Doug Hammond 05/2008
#
# This file is licensed under the GPL
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# $Id: glassShader.py,v 1.2 2008/06/16 15:24:54 dougal2 Exp $
#
# ------------------------------------------------------------------------------
#
# Girl material shader node for Maya ( glass attributes )
#
# ------------------------------------------------------------------------------

from maya import OpenMaya
from maya import OpenMayaMPx

from Girl.GirlNodes.ShaderNode import ShaderNode
from Girl.GirlNodes.GirlNode import ShaderColorAttribute
from Girl.GirlNodes.GirlNode import ShaderFloatAttribute
from Girl.GirlNodes.GirlNode import ShaderBoolAttribute

class glassShader(OpenMayaMPx.MPxNode, ShaderNode):
    """
    Glass fragment of GirlShader
    """
    
    # glass
    kr            = OpenMaya.MObject()    # color
    kt            = OpenMaya.MObject()    # color
    index         = OpenMaya.MObject()    # float
    cauchyb       = OpenMaya.MObject()    # float
    architectural = OpenMaya.MObject()    # bool

    def __init__(self):
        OpenMayaMPx.MPxNode.__init__(self)
        
        # Export attributes
        self.attributes = {}
        self.GirlType = "glass"
        self.attributes['Kr']            = ShaderColorAttribute('glassKr')
        self.attributes['Kt']            = ShaderColorAttribute('glassKt')
        self.attributes['index']         = ShaderFloatAttribute('glassIndex')
        self.attributes['cauchyb']       = ShaderFloatAttribute('glassCauchyB')
        self.attributes['architectural'] = ShaderBoolAttribute ('glassArchitectural')

    @staticmethod
    def shaderInitializer():
        try:
            # surface reflectivity
            glassShader.kr = glassShader.makeColor("glassKr", "gkr")

            # surface transmissivity
            glassShader.kt = glassShader.makeColor("glassKt", "gkt")

            # IOR
            glassShader.index = glassShader.makeFloat("glassIndex", "gi", 1.5)

            # cauchy b
            glassShader.cauchyb = glassShader.makeFloat("glassCauchyB", "gcb", 0.0)
            
            # architectural
            glassShader.architectural = glassShader.makeBoolean("glassArchitectural", "gar", default = False)

        except:
            OpenMaya.MGlobal.displayError("Failed to create glass attributes\n")
            raise
        