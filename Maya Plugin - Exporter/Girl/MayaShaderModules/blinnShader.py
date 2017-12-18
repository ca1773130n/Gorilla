# ------------------------------------------------------------------------------
# Girl material shader node for Maya
#
# by Doug Hammond 05/2008
#
# This file is licensed under the GPL
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# $Id: lambertShader.py,v 1.1 2008/06/08 12:57:47 dougal2 Exp $
#
# ------------------------------------------------------------------------------
#
# blinn shader node attributes translator
#
# ------------------------------------------------------------------------------

from maya import OpenMaya
from maya import OpenMayaMPx

from Girl.GirlNodes.ShaderNode import ShaderNode
from Girl.GirlNodes.GirlNode import ShaderColorAttribute
from Girl.GirlNodes.GirlNode import ShaderFloatAttribute

class blinnShader(OpenMayaMPx.MPxNode, ShaderNode):
    """
    This class provides an interface to translate Maya lambert shaders
    into Girl matte materials.
    """

    def __init__(self):
        """
        blinnShader constructor. Calls the parent OpenMayaMPx.MPxNode constructor
        and defines the Maya attribute -> Girl material parameter mapping.
        """
        
        OpenMayaMPx.MPxNode.__init__(self)

        # translation table for shader
        self.attributes = {}
        self.GirlType = "substrate"
        self.attributes['Ks']         = ShaderColorAttribute('specularColor')
        self.attributes['Kd']         = ShaderColorAttribute('color')
        self.attributes['uroughness'] = ShaderFloatAttribute('eccentricity', reciprocal = True)
        self.attributes['vroughness'] = ShaderFloatAttribute('specularRollOff', reciprocal = True)
         