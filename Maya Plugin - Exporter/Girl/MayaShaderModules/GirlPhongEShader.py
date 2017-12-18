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
from Girl.GirlNodes.GirlNode import ShaderEnumAttribute
from Girl.GirlNodes.GirlNode import ShaderBoolAttribute

class GirlPhongEShader(OpenMayaMPx.MPxNode, ShaderNode):
    """
    This class provides an interface to translate Maya phong shaders
    into Girl plastic materials.
    """
    strrMatteOpacityMode = {
			0: "Black Hole",
			1: "Solid Matte",
			2: "Opacity Gain"
			}
                        
    def __init__(self):
        """
        phongShader constructor. Calls the parent OpenMayaMPx.MPxNode constructor
        and defines the Maya attribute -> Girl material parameter mapping.
        """
        
        OpenMayaMPx.MPxNode.__init__(self)

        # translation table for shader
        self.attributes = {}
        self.GirlType = "GirlPhongE"
        self.attributes['color'] = ShaderColorAttribute('color')
        self.attributes['transparency'] = ShaderColorAttribute('transparency')
        self.attributes['ambientColor'] = ShaderColorAttribute('ambientColor')
        self.attributes['incandescence'] = ShaderColorAttribute('incandescence')

        self.attributes['diffuse'] = ShaderFloatAttribute('diffuse')
        self.attributes['translucence'] = ShaderFloatAttribute('translucence')
        self.attributes['translucenceDepth'] = ShaderFloatAttribute('translucenceDepth')
        self.attributes['translucenceFocus'] = ShaderFloatAttribute('translucenceFocus')

        self.attributes['glowIntensity'] = ShaderFloatAttribute('glowIntensity')

        self.attributes['matteOpacityMode'] = ShaderEnumAttribute('matteOpacityMode', self.strrMatteOpacityMode)
        self.attributes['matteOpacity'] = ShaderFloatAttribute('matteOpacity')

        self.attributes['refractions'] = ShaderBoolAttribute('refractions')
        self.attributes['chromaticAberration'] = ShaderBoolAttribute('chromaticAberration')
        self.attributes['hideSource'] = ShaderBoolAttribute('hideSource')

        self.attributes['refractiveIndex'] = ShaderFloatAttribute('refractiveIndex')
        self.attributes['refractionLimit'] = ShaderFloatAttribute('refractionLimit')
        self.attributes['lightAbsorbance'] = ShaderFloatAttribute('lightAbsorbance')
        self.attributes['surfaceThickness'] = ShaderFloatAttribute('surfaceThickness')
        self.attributes['shadowAttenuation'] = ShaderFloatAttribute('shadowAttenuation')

        self.attributes['specularColor'] = ShaderColorAttribute('specularColor')
        self.attributes['reflectedColor'] = ShaderColorAttribute('reflectedColor')

        self.attributes['reflectivity'] = ShaderFloatAttribute('reflectivity')

        self.attributes['reflectionLimit'] = ShaderFloatAttribute('reflectionLimit')
        self.attributes['reflectionSpecularity'] = ShaderFloatAttribute('reflectionSpecularity')

        self.attributes['roughness'] = ShaderFloatAttribute('roughness')
        self.attributes['highlightSize'] = ShaderFloatAttribute('highlightSize')
        self.attributes['whiteness'] = ShaderColorAttribute('whiteness')
        #Triangle Normal Camera


