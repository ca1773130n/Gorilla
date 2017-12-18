# ------------------------------------------------------------------------------
# Girl material shader node for Maya
#
# by Doug Hammond 05/2008
#
# This file is licensed under the GPL
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# $Id: substrateShader.py,v 1.2 2008/06/14 23:29:42 dougal2 Exp $
#
# ------------------------------------------------------------------------------
#
# Girl material shader node for Maya ( substrate attributes )
#
# ------------------------------------------------------------------------------

from maya import OpenMaya
from maya import OpenMayaMPx

from Girl.GirlNodes.ShaderNode import ShaderNode
from Girl.GirlNodes.GirlNode import ShaderColorAttribute
from Girl.GirlNodes.GirlNode import ShaderFloatAttribute

class substrateShader(OpenMayaMPx.MPxNode, ShaderNode):
    """
    Substrate fragment of GirlShader
    """
    
    # substrate
    kd            =    OpenMaya.MObject()    # color
    ks            =    OpenMaya.MObject()    # color
    uroughness    =    OpenMaya.MObject()    # float
    vroughness    =    OpenMaya.MObject()    # float

    def __init__(self):
        OpenMayaMPx.MPxNode.__init__(self)

        # Export attributes
        self.attributes = {}
        self.GirlType = "substrate"
        self.attributes['Kd']         = ShaderColorAttribute('substrateKd')
        self.attributes['Ks']         = ShaderColorAttribute('substrateKs')
        self.attributes['uroughness'] = ShaderFloatAttribute('substrateURoughness', reciprocal = True)
        self.attributes['vroughness'] = ShaderFloatAttribute('substrateVRoughness', reciprocal = True)

    @staticmethod
    def shaderInitializer():
        try:
            # diffuse reflectivity
            substrateShader.kd = substrateShader.makeColor("substrateKd", "skd", 0.5, 0.5, 0.5)

            # specular reflectivity
            substrateShader.ks = substrateShader.makeColor("substrateKs", "sks", 0.5, 0.5, 0.5)

            # U roughness
            substrateShader.uroughness = substrateShader.makeFloat("substrateURoughness", "sur", 500.0)

            # V roughness
            substrateShader.vroughness = substrateShader.makeFloat("substrateVRoughness", "svr", 500.0)

        except:
            OpenMaya.MGlobal.displayError("Failed to create substrate attributes\n")
            raise