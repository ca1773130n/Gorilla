# ------------------------------------------------------------------------------
# Girl material shader node for Maya
#
# by Doug Hammond 05/2008
#
# This file is licensed under the GPL
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# $Id: mattetranslucentShader.py,v 1.1 2008/06/08 12:57:41 dougal2 Exp $
#
# ------------------------------------------------------------------------------
#
# Girl material shader node for Maya ( mattetranslucent attributes )
#
# ------------------------------------------------------------------------------

from maya import OpenMaya
from maya import OpenMayaMPx

from Girl.GirlNodes.ShaderNode import ShaderNode
from Girl.GirlNodes.GirlNode import ShaderColorAttribute
from Girl.GirlNodes.GirlNode import ShaderFloatAttribute

class mattetranslucentShader(OpenMayaMPx.MPxNode, ShaderNode):
    """
    MatteTranslucent fragment of GirlShader
    """
    
    # mattetranslucent
    kr            =    OpenMaya.MObject()    # color
    kt            =    OpenMaya.MObject()    # color
    sigma         =    OpenMaya.MObject()    # float

    def __init__(self):
        OpenMayaMPx.MPxNode.__init__(self)
        
        # Export attributes
        self.attributes = {}
        self.GirlType = "mattetranslucent"
        self.attributes['Kr']    = ShaderColorAttribute('mattetranslucentKr')
        self.attributes['Kt']    = ShaderColorAttribute('mattetranslucentKt')
        self.attributes['sigma'] = ShaderFloatAttribute('mattetranslucentSigma')

    @staticmethod
    def shaderInitializer():
        try:
            # diffuse reflectivity
            mattetranslucentShader.kr = mattetranslucentShader.makeColor("mattetranslucentKr", "mtkr")

            # diffuse transmissivity
            mattetranslucentShader.kt = mattetranslucentShader.makeColor("mattetranslucentKt", "mtkt")

            # Oren-Nayar sigma
            mattetranslucentShader.sigma = mattetranslucentShader.makeFloat("mattetranslucentSigma", "mts", 0.0)

        except:
            OpenMaya.MGlobal.displayError("Failed to create mattetranslucent attributes\n")
            raise