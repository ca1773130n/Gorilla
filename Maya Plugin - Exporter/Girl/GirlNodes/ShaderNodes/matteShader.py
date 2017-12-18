# ------------------------------------------------------------------------------
# Girl material shader node for Maya
#
# by Doug Hammond 05/2008
#
# This file is licensed under the GPL
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# $Id: matteShader.py,v 1.1 2008/06/08 12:57:41 dougal2 Exp $
#
# ------------------------------------------------------------------------------
#
# Girl material shader node for Maya ( matte attributes )
#
# ------------------------------------------------------------------------------

from maya import OpenMaya
from maya import OpenMayaMPx

from Girl.GirlNodes.ShaderNode import ShaderNode
from Girl.GirlNodes.GirlNode import ShaderColorAttribute
from Girl.GirlNodes.GirlNode import ShaderFloatAttribute

class matteShader(OpenMayaMPx.MPxNode, ShaderNode):
    """
    Matte fragment of GirlShader
    """
    
    # matte
    kd          =    OpenMaya.MObject()    # color
    sigma       =    OpenMaya.MObject()    # float
        
    def __init__(self):
        OpenMayaMPx.MPxNode.__init__(self)

        # Export attributes
        self.attributes = {}
        self.GirlType = "matte"
        self.attributes['Kd']    = ShaderColorAttribute('matteKd')
        self.attributes['sigma'] = ShaderFloatAttribute('matteSigma')

    @staticmethod
    def shaderInitializer():
        try:
            # diffuse reflectivity
            matteShader.kd = matteShader.makeColor("matteKd", "mkd")

            # Oren-Nayar sigma
            matteShader.sigma = matteShader.makeFloat("matteSigma", "ms", 0.0)

        except:
            OpenMaya.MGlobal.displayError("Failed to create matte attributes\n")
            raise