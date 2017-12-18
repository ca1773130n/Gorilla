# ------------------------------------------------------------------------------
# Girl texture nodes for Maya
#
# by Doug Hammond 05/2008
#
# This file is licensed under the GPL
# http://www.gnu.org/licenses/gpl-3.0.txt
#
# $Id: bilerpTexture.py,v 1.2 2008/06/19 14:43:18 dougal2 Exp $
#
# ------------------------------------------------------------------------------
#
# Girl BiLerp Texture node for Maya
#
# ------------------------------------------------------------------------------

import math
from maya import OpenMaya
from maya import OpenMayaMPx

from Girl.GirlNodes.TextureNode import TextureNode
from Girl.GirlNodes.GirlNode import TextureIntegerAttribute
from Girl.GirlNodes.GirlNode import TextureFloatAttribute
from Girl.GirlNodes.GirlNode import TextureColorAttribute
from Girl.GirlNodes.GirlNode import TextureEnumAttribute
from Girl.GirlNodes.GirlNode import TextureBoolAttribute

# 3D Texture
class WaterTexture(OpenMayaMPx.MPxNode, TextureNode):
    
    @staticmethod
    def nodeName():
        return "water"
    
    def GirlName(self):
        return "GirlWater"

    
    def __init__(self):
        OpenMayaMPx.MPxNode.__init__(self)

        # translation table for texture
        self.attributes = {}
        self.attributes['defaultColor']   = TextureColorAttribute('defaultColor',  self.addToOutput, self.prependToOutput)
        self.attributes['colorGain'] = TextureColorAttribute('colorGain',  self.addToOutput, self.prependToOutput)
        self.attributes['colorOffset']   = TextureColorAttribute('colorOffset',  self.addToOutput, self.prependToOutput)
        
        self.attributes['alphaGain']  = TextureFloatAttribute('alphaGain', self.addToOutput, self.prependToOutput)
        self.attributes['alphaOffset']  = TextureFloatAttribute('alphaOffset', self.addToOutput, self.prependToOutput)
        self.attributes['alphaIsLuminance']  = TextureBoolAttribute('alphaIsLuminance', self.addToOutput, self.prependToOutput)


        self.attributes['numberOfWaves']  = TextureIntegerAttribute('numberOfWaves', self.addToOutput, self.prependToOutput)
        self.attributes['fast']  = TextureBoolAttribute('fast', self.addToOutput, self.prependToOutput)
        self.attributes['reflectionBox']  = TextureBoolAttribute('reflectionBox', self.addToOutput, self.prependToOutput)

        self.attributes['waveTime']  = TextureFloatAttribute('waveTime', self.addToOutput, self.prependToOutput)
        self.attributes['waveVelocity']  = TextureFloatAttribute('waveVelocity', self.addToOutput, self.prependToOutput)
        self.attributes['waveAmplitude']  = TextureFloatAttribute('waveAmplitude', self.addToOutput, self.prependToOutput)
        self.attributes['waveFrequency']  = TextureFloatAttribute('waveFrequency', self.addToOutput, self.prependToOutput)
        self.attributes['subWaveFrequency']  = TextureFloatAttribute('subWaveFrequency', self.addToOutput, self.prependToOutput)
        self.attributes['smoothness']  = TextureFloatAttribute('smoothness', self.addToOutput, self.prependToOutput)
        self.attributes['windU']  = TextureFloatAttribute('windU', self.addToOutput, self.prependToOutput)
        self.attributes['windV']  = TextureFloatAttribute('windV', self.addToOutput, self.prependToOutput)
        self.attributes['rippleTime']  = TextureFloatAttribute('rippleTime', self.addToOutput, self.prependToOutput)
        self.attributes['rippleFrequency']  = TextureFloatAttribute('rippleFrequency', self.addToOutput, self.prependToOutput)
        self.attributes['rippleAmplitude']  = TextureFloatAttribute('rippleAmplitude', self.addToOutput, self.prependToOutput)
        self.attributes['dropSize']  = TextureFloatAttribute('dropSize', self.addToOutput, self.prependToOutput)
        self.attributes['rippleOriginU']  = TextureFloatAttribute('rippleOriginU', self.addToOutput, self.prependToOutput)
        self.attributes['rippleOriginV']  = TextureFloatAttribute('rippleOriginV', self.addToOutput, self.prependToOutput)
        self.attributes['groupVelocity']  = TextureFloatAttribute('groupVelocity', self.addToOutput, self.prependToOutput)
        self.attributes['phaseVelocity']  = TextureFloatAttribute('phaseVelocity', self.addToOutput, self.prependToOutput)
        self.attributes['spreadStart']  = TextureFloatAttribute('spreadStart', self.addToOutput, self.prependToOutput)
        self.attributes['spreadRate']  = TextureFloatAttribute('spreadRate', self.addToOutput, self.prependToOutput)
        self.attributes['boxMinU']  = TextureFloatAttribute('boxMinU', self.addToOutput, self.prependToOutput)
        self.attributes['boxMinV']  = TextureFloatAttribute('boxMinV', self.addToOutput, self.prependToOutput)
        self.attributes['boxMaxU']  = TextureFloatAttribute('boxMaxU', self.addToOutput, self.prependToOutput)
        self.attributes['boxMaxV']  = TextureFloatAttribute('boxMaxV', self.addToOutput, self.prependToOutput)



