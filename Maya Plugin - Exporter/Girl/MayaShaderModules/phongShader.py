# ------------------------------------------------------------------------------
# Girl material shader node for Maya
#
# by Doug Hammond 05/2008
#
# This file is licensed under the GPL
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# $Id: phongShader.py,v 1.3 2008/06/14 23:29:42 dougal2 Exp $
#
# ------------------------------------------------------------------------------
#
# Lambert shader node attributes translator
#
# ------------------------------------------------------------------------------

from maya import OpenMaya
from maya import OpenMayaMPx

from Girl.GirlNodes.ShaderNode import ShaderNode
from Girl.GirlNodes.GirlNode import ShaderColorAttribute
from Girl.GirlNodes.GirlNode import ShaderFloatAttribute

class phongShader(OpenMayaMPx.MPxNode, ShaderNode):
    """
    This class provides an interface to translate Maya phong shaders
    into Girl plastic materials.
    """
    
    def __init__(self):
        """
        phongShader constructor. Calls the parent OpenMayaMPx.MPxNode constructor
        and defines the Maya attribute -> Girl material parameter mapping.
        """
        
        OpenMayaMPx.MPxNode.__init__(self)

        # translation table for shader
        self.attributes = {}
        self.GirlType = "plastic"
        self.attributes['Kd']         = ShaderColorAttribute('color')
        self.attributes['Ks']         = ShaderColorAttribute('specularColor')
        self.attributes['uroughness'] = ShaderFloatAttribute('cosinePower', preScale = 0.01, invert=True, reciprocal = True, postScale = 0.1)
        self.attributes['vroughness'] = ShaderFloatAttribute('cosinePower', preScale = 0.01, invert=True, reciprocal = True, postScale = 0.1)
         