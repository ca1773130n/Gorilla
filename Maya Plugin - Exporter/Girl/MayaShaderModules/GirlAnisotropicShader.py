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

class GirlAnisotropicShader(OpenMayaMPx.MPxNode, ShaderNode):
    """
    This class provides an interface to translate Maya lambert shaders
    into Girl matte materials.
    """
    strrMatteOpacityMode = {
			0: "Black Hole",
			1: "Solid Matte",
			2: "Opacity Gain"
			}

    def __init__(self):
        """
        lambertShader constructor. Calls the parent OpenMayaMPx.MPxNode constructor
        and defines the Maya attribute -> Girl material parameter mapping.
        """
        
        OpenMayaMPx.MPxNode.__init__(self)

        # translation table for shader
        self.attributes = {}
        self.GirlType = "GirlAnisotropic"
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


        self.attributes['angle'] = ShaderFloatAttribute('angle')
        self.attributes['spreadX'] = ShaderFloatAttribute('spreadX')
        self.attributes['spreadY'] = ShaderFloatAttribute('spreadY')
        self.attributes['roughness'] = ShaderFloatAttribute('roughness')
        self.attributes['fresnelRefractiveIndex'] = ShaderFloatAttribute('fresnelRefractiveIndex')

        self.attributes['anisotropicReflectivity'] = ShaderBoolAttribute('anisotropicReflectivity')

         