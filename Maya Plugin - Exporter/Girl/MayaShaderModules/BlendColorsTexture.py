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
class BlendColorsTexture(OpenMayaMPx.MPxNode, TextureNode):
    
    @staticmethod
    def nodeName():
        return "blendColors"
    
    def GirlName(self):
        return "GirlBlendColors"

    
    def __init__(self):
        OpenMayaMPx.MPxNode.__init__(self)

        # translation table for texture
        self.attributes = {}
        self.attributes['color1']   = TextureColorAttribute('color1',  self.addToOutput, self.prependToOutput)
        self.attributes['color2'] = TextureColorAttribute('color2',  self.addToOutput, self.prependToOutput)
        
        self.attributes['blender']  = TextureFloatAttribute('blender', self.addToOutput, self.prependToOutput)



