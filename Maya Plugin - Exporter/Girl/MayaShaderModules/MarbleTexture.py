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
class MarbleTexture(OpenMayaMPx.MPxNode, TextureNode):
    
    @staticmethod
    def nodeName():
        return "marble"
    
    def GirlName(self):
        return "GirlMarble"

    
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



        self.attributes['fillerColor']   = TextureColorAttribute('fillerColor',  self.addToOutput, self.prependToOutput)
        self.attributes['veinColor']   = TextureColorAttribute('veinColor',  self.addToOutput, self.prependToOutput)

        self.attributes['veinWidth']  = TextureFloatAttribute('veinWidth', self.addToOutput, self.prependToOutput)
        self.attributes['diffusion']  = TextureFloatAttribute('diffusion', self.addToOutput, self.prependToOutput)
        self.attributes['contrast']  = TextureFloatAttribute('contrast', self.addToOutput, self.prependToOutput)
        self.attributes['amplitude']  = TextureFloatAttribute('amplitude', self.addToOutput, self.prependToOutput)
        self.attributes['ratio']  = TextureFloatAttribute('ratio', self.addToOutput, self.prependToOutput)
        self.attributes['ripplesX']  = TextureFloatAttribute('ripplesX', self.addToOutput, self.prependToOutput)
        self.attributes['ripplesY']  = TextureFloatAttribute('ripplesY', self.addToOutput, self.prependToOutput)
        self.attributes['ripplesZ']  = TextureFloatAttribute('ripplesZ', self.addToOutput, self.prependToOutput)
        self.attributes['depthMin']  = TextureFloatAttribute('depthMin', self.addToOutput, self.prependToOutput)
        self.attributes['depthMax']  = TextureFloatAttribute('depthMax', self.addToOutput, self.prependToOutput)




