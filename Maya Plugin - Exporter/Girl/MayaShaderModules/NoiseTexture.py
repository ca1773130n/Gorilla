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
class NoiseTexture(OpenMayaMPx.MPxNode, TextureNode):
    
    aNoiseTypes = [
              'Perlin',
              'Billow',
              'Wave',
              'Wispy',
              'SpaceTime'
              ]

    aFalloffTypes = [
              'Linear',
              'Smooth',
              'Fast',
              'Bubble'
              ]

    @staticmethod
    def nodeName():
        return "noise"
    
    def GirlName(self):
        return "GirlNoise"

    
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


        self.attributes['inflection']  = TextureBoolAttribute('inflection', self.addToOutput, self.prependToOutput)
        self.attributes['depthMax']  = TextureIntegerAttribute('depthMax', self.addToOutput, self.prependToOutput)
        self.attributes['numWaves']  = TextureIntegerAttribute('numWaves', self.addToOutput, self.prependToOutput)

        self.attributes['threshold']  = TextureFloatAttribute('threshold', self.addToOutput, self.prependToOutput)
        self.attributes['amplitude']  = TextureFloatAttribute('amplitude', self.addToOutput, self.prependToOutput)
        self.attributes['ratio']  = TextureFloatAttribute('ratio', self.addToOutput, self.prependToOutput)
        self.attributes['frequencyRatio']  = TextureFloatAttribute('frequencyRatio', self.addToOutput, self.prependToOutput)
        self.attributes['time']  = TextureFloatAttribute('time', self.addToOutput, self.prependToOutput)
        self.attributes['frequency']  = TextureFloatAttribute('frequency', self.addToOutput, self.prependToOutput)
        self.attributes['implode']  = TextureFloatAttribute('implode', self.addToOutput, self.prependToOutput)
        self.attributes['implodeCenterU']  = TextureFloatAttribute('implodeCenterU', self.addToOutput, self.prependToOutput)
        self.attributes['implodeCenterV']  = TextureFloatAttribute('implodeCenterV', self.addToOutput, self.prependToOutput)
        self.attributes['density']  = TextureFloatAttribute('density', self.addToOutput, self.prependToOutput)
        self.attributes['spottyness']  = TextureFloatAttribute('spottyness', self.addToOutput, self.prependToOutput)
        self.attributes['sizeRand']  = TextureFloatAttribute('sizeRand', self.addToOutput, self.prependToOutput)
        self.attributes['randomness']  = TextureFloatAttribute('randomness', self.addToOutput, self.prependToOutput)


        self.attributes['noiseType'] = TextureEnumAttribute('noiseType',  self.addToOutput, self.prependToOutput, asString = False, nameValues = self.aNoiseTypes)
        self.attributes['falloff'] = TextureEnumAttribute('falloff',  self.addToOutput, self.prependToOutput, asString = False, nameValues = self.aFalloffTypes)
