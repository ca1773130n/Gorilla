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
class LeatherTexture(OpenMayaMPx.MPxNode, TextureNode):
    
    @staticmethod
    def nodeName():
        return "leather"
    
    def GirlName(self):
        return "GirlLeather"

    
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


        self.attributes['creases']  = TextureBoolAttribute('creases', self.addToOutput, self.prependToOutput)

        self.attributes['cellColor']   = TextureColorAttribute('cellColor',  self.addToOutput, self.prependToOutput)
        self.attributes['creaseColor']   = TextureColorAttribute('creaseColor',  self.addToOutput, self.prependToOutput)

        self.attributes['cellSize']  = TextureFloatAttribute('cellSize', self.addToOutput, self.prependToOutput)
        self.attributes['density']  = TextureFloatAttribute('density', self.addToOutput, self.prependToOutput)
        self.attributes['spottyness']  = TextureFloatAttribute('spottyness', self.addToOutput, self.prependToOutput)
        self.attributes['randomness']  = TextureFloatAttribute('randomness', self.addToOutput, self.prependToOutput)
        self.attributes['threshold']  = TextureFloatAttribute('threshold', self.addToOutput, self.prependToOutput)



