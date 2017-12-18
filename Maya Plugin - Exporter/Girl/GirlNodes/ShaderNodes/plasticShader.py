# ------------------------------------------------------------------------------
# Girl material shader node for Maya
#
# by Doug Hammond 05/2008
#
# This file is licensed under the GPL
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# $Id: plasticShader.py,v 1.3 2008/06/14 23:29:42 dougal2 Exp $
#
# ------------------------------------------------------------------------------
#
# Girl material shader node for Maya ( plastic attributes )
#
# ------------------------------------------------------------------------------

from maya import OpenMaya
from maya import OpenMayaMPx

from Girl.GirlNodes.ShaderNode import ShaderNode
from Girl.GirlNodes.GirlNode import ShaderColorAttribute
from Girl.GirlNodes.GirlNode import ShaderFloatAttribute

class plasticShader(OpenMayaMPx.MPxNode, ShaderNode):
    """
    Plastic fragment of GirlShader
    """
    
    # plastic
    kd          =    OpenMaya.MObject()    # color
    ks          =    OpenMaya.MObject()    # color
    uroughness  =    OpenMaya.MObject()    # float
    vroughness  =    OpenMaya.MObject()    # float

    def __init__(self):
        OpenMayaMPx.MPxNode.__init__(self)

        # Export attributes
        self.attributes = {}
        self.GirlType = "plastic"
        self.attributes['Kd']         = ShaderColorAttribute('plasticKd')
        self.attributes['Ks']         = ShaderColorAttribute('plasticKs')
        self.attributes['uroughness'] = ShaderFloatAttribute('plasticURoughness', reciprocal = True)
        self.attributes['vroughness'] = ShaderFloatAttribute('plasticVRoughness', reciprocal = True)

    @staticmethod
    def shaderInitializer():
        try:
            # diffuse reflectivity
            plasticShader.kd = plasticShader.makeColor("plasticKd", "pkd")

            # specular reflectivity
            plasticShader.ks = plasticShader.makeColor("plasticKs", "pks")

            # surface roughness
            plasticShader.uroughness = plasticShader.makeFloat("plasticURoughness", "pur", 500.0)
            plasticShader.vroughness = plasticShader.makeFloat("plasticVRoughness", "pvr", 500.0)

        except:
            OpenMaya.MGlobal.displayError("Failed to create plastic attributes\n")
            raise