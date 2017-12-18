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
class ContrastTexture(OpenMayaMPx.MPxNode, TextureNode):
    
    @staticmethod
    def nodeName():
        return "contrast"
    
    def GirlName(self):
        return "GirlContrast"

    
    def __init__(self):
        OpenMayaMPx.MPxNode.__init__(self)

        # translation table for texture
        self.attributes = {}       
        self.attributes['valueX']  = TextureFloatAttribute('valueX', self.addToOutput, self.prependToOutput)
        self.attributes['valueY']  = TextureFloatAttribute('valueY', self.addToOutput, self.prependToOutput)
        self.attributes['valueZ']  = TextureFloatAttribute('valueZ', self.addToOutput, self.prependToOutput)

        self.attributes['contrastX']  = TextureFloatAttribute('contrastX', self.addToOutput, self.prependToOutput)
        self.attributes['contrastY']  = TextureFloatAttribute('contrastY', self.addToOutput, self.prependToOutput)
        self.attributes['contrastZ']  = TextureFloatAttribute('contrastZ', self.addToOutput, self.prependToOutput)

        self.attributes['biasX']  = TextureFloatAttribute('biasX', self.addToOutput, self.prependToOutput)
        self.attributes['biasY']  = TextureFloatAttribute('biasY', self.addToOutput, self.prependToOutput)
        self.attributes['biasZ']  = TextureFloatAttribute('biasZ', self.addToOutput, self.prependToOutput)



