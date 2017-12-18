# ------------------------------------------------------------------------------
# Girl material shader node for Maya
#
# by Doug Hammond 05/2008
#
# This file is licensed under the GPL
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# $Id: shinymetalShader.py,v 1.3 2008/06/14 23:29:42 dougal2 Exp $
#
# ------------------------------------------------------------------------------
#
# Girl material shader node for Maya ( shinymetal attributes )
#
# ------------------------------------------------------------------------------

from maya import OpenMaya
from maya import OpenMayaMPx

from Girl.GirlNodes.ShaderNode import ShaderNode
from Girl.GirlNodes.GirlNode import ShaderColorAttribute
from Girl.GirlNodes.GirlNode import ShaderFloatAttribute

class shinymetalShader(OpenMayaMPx.MPxNode, ShaderNode):
    """
    ShinyMetal fragment of GirlShader
    """
    
    # shinymetal
    kr            =    OpenMaya.MObject()    # color
    ks            =    OpenMaya.MObject()    # color
    uroughness    =    OpenMaya.MObject()    # float
    vroughness    =    OpenMaya.MObject()    # float

    def __init__(self):
        OpenMayaMPx.MPxNode.__init__(self)

        # Export attributes
        self.attributes = {}
        self.GirlType = "shinymetal"
        self.attributes['Kr']        = ShaderColorAttribute('shinymetalKr')
        self.attributes['Ks']        = ShaderColorAttribute('shinymetalKs')
        self.attributes['uroughness'] = ShaderFloatAttribute('shinymetalURoughness', reciprocal = True)
        self.attributes['vroughness'] = ShaderFloatAttribute('shinymetalVRoughness', reciprocal = True)

    @staticmethod
    def shaderInitializer():
        try:
            # specular reflection
            shinymetalShader.kr = shinymetalShader.makeColor("shinymetalKr", "smkr")

            # glossy reflection
            shinymetalShader.ks = shinymetalShader.makeColor("shinymetalKs", "smks")

            # surface roughness
            shinymetalShader.uroughness = shinymetalShader.makeFloat("shinymetalURoughness", "smur", 500.0)
            shinymetalShader.vroughness = shinymetalShader.makeFloat("shinymetalVRoughness", "smvr", 500.0)

        except:
            OpenMaya.MGlobal.displayError("Failed to create shinymetal attributes\n")
            raise
        